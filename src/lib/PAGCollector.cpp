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
  if ((it != hfMap.end()) && !it->second.empty())
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
  const string first((style->has<FirstPageMaster>()) ? pick(style->get<FirstPageMaster>()) : "");

  if (odd == even)
  {
    writeHeaderFooter(document, hfMap, odd, "both", open, close);
  }
  else
  {
    writeHeaderFooter(document, hfMap, odd, "odd", open, close);
    writeHeaderFooter(document, hfMap, even, "even", open, close);
  }
  writeHeaderFooter(document, hfMap, first, "first", open, close);
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
  , m_footnotes()
  , m_nextFootnote(m_footnotes.end())
  , m_pageGroups()
  , m_page(0)
{
}

void PAGCollector::collectPublicationInfo(const PAGPublicationInfo &pubInfo)
{
  m_pubInfo = pubInfo;
}

void PAGCollector::collectTextBody()
{
  // It seems that this is never used, as Pages always inserts all text
  // into a section. But better safe than sorry.
  flushPageSpan(false);
}

void PAGCollector::collectAttachment(const IWORKOutputID_t &id)
{
  assert(!m_textStack.empty());
  assert(bool(m_textStack.top()));

  m_textStack.top()->insertBlockContent(getOutputManager().get(id));
}

void PAGCollector::insertFootnote()
{
  assert(!m_textStack.empty());
  assert(bool(m_textStack.top()));

  if (m_nextFootnote != m_footnotes.end())
  {
    m_textStack.top()->insertInlineContent(*m_nextFootnote);
    ++m_nextFootnote;
  }
}

void PAGCollector::collectFootnote(const std::string &mark)
{
  assert(!m_textStack.empty());

  const bool firstFootnote = m_footnotes.empty();
  m_footnotes.push_back(IWORKOutputElements());
  if (bool(m_textStack.top()))
  {
    RVNGPropertyList props;
    if (!mark.empty())
      props.insert("text:label", mark.c_str());
    if (m_pubInfo.m_footnoteKind == PAG_FOOTNOTE_KIND_FOOTNOTE)
      m_footnotes.back().addOpenFootnote(props);
    else
      m_footnotes.back().addOpenEndnote(props);
    m_textStack.top()->draw(m_footnotes.back());
    if (m_pubInfo.m_footnoteKind == PAG_FOOTNOTE_KIND_FOOTNOTE)
      m_footnotes.back().addCloseFootnote();
    else
      m_footnotes.back().addCloseEndnote();
    m_textStack.top().reset(new IWORKText(false));
  }
  if (firstFootnote) // We can init. insertion iterator now
    m_nextFootnote = m_footnotes.begin();
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

void PAGCollector::openPageGroup(const boost::optional<int> &page)
{
  getOutputManager().push();
  if (page)
    m_page = get(page);
  else
    ++m_page;
}

void PAGCollector::closePageGroup()
{
  typedef std::pair<PageGroupsMap_t::const_iterator, bool> Result_t;
  const IWORKOutputID_t id = getOutputManager().save();
  const Result_t result = m_pageGroups.insert(PageGroupsMap_t::value_type(m_page, id));
  if (!result.second)
  {
    ETONYEK_DEBUG_MSG(("Page group for page %u already exists\n", m_page));
  }
  getOutputManager().pop();
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

void PAGCollector::drawMedia(
  const double x, const double y, const double w, const double h,
  const std::string &mimetype, const librevenge::RVNGBinaryData &data)
{
  RVNGPropertyList frameProps;
  frameProps.insert("text:anchor-type", "page"); // TODO: this needs to be more flexible
  frameProps.insert("text:anchor-page-number", m_page);
  frameProps.insert("svg:x", pt2in(x));
  frameProps.insert("svg:y", pt2in(y));
  frameProps.insert("svg:width", pt2in(w));
  frameProps.insert("svg:height", pt2in(h));

  RVNGPropertyList binaryObjectProps;
  binaryObjectProps.insert("librevenge:mime-type", mimetype.c_str());
  binaryObjectProps.insert("office:binary-data", data);

  getOutputManager().getCurrent().addOpenFrame(frameProps);
  getOutputManager().getCurrent().addInsertBinaryObject(binaryObjectProps);
  getOutputManager().getCurrent().addCloseFrame();
}

void PAGCollector::flushPageSpan(const bool writeEmpty)
{
  assert(!m_textStack.empty());

  if (m_firstPageSpan)
  {
    RVNGPropertyList metadata;
    fillMetadata(metadata);
    m_document->setDocumentMetaData(metadata);
    writePageGroupsObjects();
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

  if (bool(m_textStack.top()))
  {
    m_textStack.top()->draw(text);
    m_textStack.top().reset(new IWORKText(false));
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

void PAGCollector::writePageGroupsObjects()
{
  for (PageGroupsMap_t::const_iterator it = m_pageGroups.begin(); it != m_pageGroups.end(); ++it)
    getOutputManager().get(it->second).write(m_document);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
