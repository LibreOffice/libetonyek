/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAGCollector.h"

#include <cassert>

#include <boost/make_shared.hpp>

#include "IWORKDocumentInterface.h"
#include "IWORKOutputElements.h"
#include "IWORKText.h"
#include "PAGProperties.h"
#include "PAGTypes.h"

namespace libetonyek
{

using librevenge::RVNGPropertyList;

using std::string;

namespace
{

typedef void (IWORKDocumentInterface::*OpenFunction)(const RVNGPropertyList &);
typedef void (IWORKDocumentInterface::*CloseFunction)();
typedef const std::string &(*PickFunction)(const PAGPageMaster &);

const std::string &pickHeader(const PAGPageMaster &pageMaster)
{
  return pageMaster.m_header;
}

const std::string &pickFooter(const PAGPageMaster &pageMaster)
{
  return pageMaster.m_footer;
}

void writeHeaderFooter(
  IWORKDocumentInterface *const document, const IWORKHeaderFooterMap_t &hfMap,
  const string &name, const string &occurrence,
  const OpenFunction open, const CloseFunction close)
{
  assert(document);
  if (name.empty())
    return;

  const IWORKHeaderFooterMap_t::const_iterator it = hfMap.find(name);
  if (it != hfMap.end())
  {
    RVNGPropertyList props;
    props.insert("librevenge:occurrence", occurrence.c_str());
    (document->*open)(props);
    it->second.write(document);
    (document->*close)();
  }
}

void writeHeadersFooters(
  IWORKDocumentInterface *const document, const IWORKStylePtr_t &style, const IWORKHeaderFooterMap_t &hfMap,
  const PickFunction pick, const OpenFunction open, const CloseFunction close)
{
  assert(bool(style));

  using namespace property;
  const string odd((style->has<OddPageMaster>()) ? pick(style->get<OddPageMaster>()) : "");
  const string even((style->has<EvenPageMaster>()) ? pick(style->get<EvenPageMaster>()) : "");

  if (odd == even)
  {
    writeHeaderFooter(document, hfMap, odd, "both", open, close);
  }
  else
  {
    writeHeaderFooter(document, hfMap, odd, "odd", open, close);
    writeHeaderFooter(document, hfMap, even, "even", open, close);
  }
}

}

PAGCollector::Section::Section()
  : m_style()
  , m_width()
  , m_height()
  , m_horizontalMargin()
  , m_verticalMargin()
{
}

void PAGCollector::Section::clear()
{
  m_style.reset();
  m_width.reset();
  m_height.reset();
  m_horizontalMargin.reset();
  m_verticalMargin.reset();
}

PAGCollector::PAGCollector(IWORKDocumentInterface *const document)
  : IWORKCollector(document)
  , m_currentSection()
  , m_firstPageSpan(true)
{
}

void PAGCollector::collectPublicationInfo(const PAGPublicationInfo &/*pubInfo*/)
{
}

void PAGCollector::collectTextBody()
{
  // It seems that this is never used, as Pages always inserts all text
  // into a section. But better safe than sorry.
  flushPageSpan(false);
}

void PAGCollector::collectAttachment(const IWORKOutputID_t &id)
{
  assert(bool(m_currentText));
  m_currentText->insertBlockContent(getOutputManager().get(id));
}

void PAGCollector::openSection(const std::string &style, const double width, const double height, const double horizontalMargin, const double verticalMargin)
{
  m_currentSection.m_width = width;
  m_currentSection.m_height = height;
  m_currentSection.m_horizontalMargin = horizontalMargin;
  m_currentSection.m_verticalMargin = verticalMargin;

  if (!m_stylesheetStack.empty())
  {
    const IWORKStyleMap_t::iterator it = m_stylesheetStack.top()->m_styles.find(style);
    if (it != m_stylesheetStack.top()->m_styles.end())
    {
      m_currentSection.m_style = it->second;
    }
    else
    {
      ETONYEK_DEBUG_MSG(("style '%s' not found\n", style.c_str()));
    }
  }
  else
  {
    ETONYEK_DEBUG_MSG(("no stylesheet is available\n"));
  }
}

void PAGCollector::closeSection()
{
  flushPageSpan();
}

void PAGCollector::drawTable()
{
  assert(!m_levelStack.empty());

  librevenge::RVNGPropertyList props;

  // TODO: I am not sure this is the default for Pages...
  props.insert("table:align", "center");

  const IWORKGeometryPtr_t geometry(m_levelStack.top().m_geometry);
  if (geometry)
  {
    const glm::dvec3 dim(m_levelStack.top().m_trafo * glm::dvec3(geometry->m_naturalSize.m_width, 0, 0));
    props.insert("style:width", pt2in(dim[0]));
  }

  m_currentTable.draw(props, m_outputManager.getCurrent());
}

void PAGCollector::flushPageSpan(const bool writeEmpty)
{
  if (m_firstPageSpan)
  {
    RVNGPropertyList metadata;
    fillMetadata(metadata);
    m_document->setDocumentMetaData(metadata);
    m_firstPageSpan = false;
  }

  librevenge::RVNGPropertyList props;

  if (m_currentSection.m_width)
    props.insert("fo:page-width", get(m_currentSection.m_width));
  if (m_currentSection.m_height)
    props.insert("fo:page-height", get(m_currentSection.m_height));
  if (m_currentSection.m_horizontalMargin)
  {
    props.insert("fo:margin-left", get(m_currentSection.m_horizontalMargin));
    props.insert("fo:margin-right", get(m_currentSection.m_horizontalMargin));
  }
  if (m_currentSection.m_verticalMargin)
  {
    props.insert("fo:margin-top", get(m_currentSection.m_verticalMargin));
    props.insert("fo:margin-bottom", get(m_currentSection.m_verticalMargin));
  }

  IWORKOutputElements text;

  if (bool(m_currentText))
  {
    m_currentText->draw(text);
    m_currentText.reset();
  }

  if (!text.empty() || writeEmpty)
  {
    m_document->openPageSpan(props);
    if (m_currentSection.m_style)
    {
      writeHeadersFooters(m_document, m_currentSection.m_style, m_headers, pickHeader,
                          &IWORKDocumentInterface::openHeader, &IWORKDocumentInterface::closeHeader);
      writeHeadersFooters(m_document, m_currentSection.m_style, m_footers, pickFooter,
                          &IWORKDocumentInterface::openFooter, &IWORKDocumentInterface::closeFooter);
    }
    text.write(m_document);
    m_document->closePageSpan();
  }

  m_currentSection.clear();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
