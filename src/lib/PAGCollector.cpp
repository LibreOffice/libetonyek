/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAGCollector.h"

#include <boost/make_shared.hpp>

#include "IWORKDocumentInterface.h"
#include "IWORKOutputElements.h"
#include "IWORKText.h"
#include "PAGTypes.h"

namespace libetonyek
{

using librevenge::RVNGPropertyList;

using std::string;

PAGCollector::Section::Section()
  : m_width()
  , m_height()
  , m_horizontalMargin()
  , m_verticalMargin()
{
}

void PAGCollector::Section::clear()
{
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

void PAGCollector::openSection(const double width, const double height, const double horizontalMargin, const double verticalMargin)
{
  m_currentSection.m_width = width;
  m_currentSection.m_height = height;
  m_currentSection.m_horizontalMargin = horizontalMargin;
  m_currentSection.m_verticalMargin = verticalMargin;
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

  if (bool(m_currentText))
  {
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
    m_currentSection.clear();

    IWORKOutputElements text;

    m_currentText->draw(text);
    m_currentText.reset();

    if (!text.empty() || writeEmpty)
    {
      m_document->openPageSpan(props);
      text.write(m_document);
      m_document->closePageSpan();
    }
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
