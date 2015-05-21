/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYCollector.h"

#include <glm/glm.hpp>

#include "libetonyek_utils.h"
#include "IWORKDocumentInterface.h"
#include "IWORKOutputElements.h"
#include "IWORKProperties.h"
#include "IWORKText.h"
#include "KEYDictionary.h"

namespace libetonyek
{

KEYCollector::KEYCollector(IWORKDocumentInterface *const document)
  : IWORKCollector(document)
  , m_size()
  , m_notes()
  , m_stickyNotes()
  , m_pageOpened(false)
  , m_layerOpened(false)
  , m_layerCount(0)
  , m_paint(false)
{
  assert(!m_paint);
}

KEYCollector::~KEYCollector()
{
  assert(!m_paint);
}

void KEYCollector::collectPresentationSize(const IWORKSize &size)
{
  m_size = size;
}

KEYLayerPtr_t KEYCollector::collectLayer()
{
  assert(m_layerOpened);

  KEYLayerPtr_t layer(new KEYLayer());

  layer->m_zoneId = getZoneManager().save();

  return layer;
}

void KEYCollector::insertLayer(const KEYLayerPtr_t &layer)
{
  assert(!m_layerOpened);

  if (bool(layer))
  {
    if (m_paint)
    {
      ++m_layerCount;

      librevenge::RVNGPropertyList props;
      props.insert("svg:id", m_layerCount);

      m_document->startLayer(props);
      if (layer->m_zoneId)
        getZoneManager().get(get(layer->m_zoneId)).write(m_document);
      m_document->endLayer();
    }
  }
  else
  {
    ETONYEK_DEBUG_MSG(("no layer\n"));
  }
}

void KEYCollector::collectPage()
{
  assert(m_pageOpened);

  if (m_paint)
  {
    drawNotes();
    drawStickyNotes();
  }
}

KEYPlaceholderPtr_t KEYCollector::collectTextPlaceholder(const IWORKStylePtr_t &style, const bool title)
{
  assert(bool(m_currentText));

  KEYPlaceholderPtr_t placeholder(new KEYPlaceholder());
  placeholder->m_title = title;
  placeholder->m_style = style;
  if (bool(placeholder->m_style))
  {
    m_styleStack.push();
    m_styleStack.set(placeholder->m_style);

    if (m_styleStack.has<property::Geometry>())
      placeholder->m_geometry = m_styleStack.get<property::Geometry>();

    m_styleStack.pop();
  }
  if (!m_currentText->empty())
    placeholder->m_text = m_currentText;

  m_currentText.reset();

  return placeholder;
}

void drawPlaceholder(const KEYPlaceholderPtr_t &placeholder, const glm::dmat3 &trafo, IWORKOutputElements &elements)
{
  if (bool(placeholder) && bool(placeholder->m_style) && bool(placeholder->m_text))
    (placeholder->m_text)->draw(trafo, placeholder->m_geometry, elements);
}

void KEYCollector::insertTextPlaceholder(const KEYPlaceholderPtr_t &placeholder)
{
  if (bool(placeholder))
  {
    glm::dmat3 trafo;
    if (bool(placeholder->m_geometry))
      trafo = makeTransformation(*placeholder->m_geometry);

    drawPlaceholder(placeholder, trafo * m_levelStack.top().m_trafo, getZoneManager().getCurrent());
  }
  else
  {
    ETONYEK_DEBUG_MSG(("no text placeholder found\n"));
  }
}

void KEYCollector::collectNote()
{
  m_currentText->draw(m_notes);
  m_currentText.reset();
}

void KEYCollector::collectStickyNote()
{
  assert(!m_levelStack.empty());

  librevenge::RVNGPropertyList props;

  if (bool(m_levelStack.top().m_geometry))
  {
    props.insert("svg:x", pt2in(m_levelStack.top().m_geometry->m_position.m_x));
    props.insert("svg:y", pt2in(m_levelStack.top().m_geometry->m_position.m_y));
    props.insert("svg:width", pt2in(m_levelStack.top().m_geometry->m_naturalSize.m_width));
    props.insert("svg:height", pt2in(m_levelStack.top().m_geometry->m_naturalSize.m_height));
  }

  if (bool(m_currentText))
  {
    m_stickyNotes.addOpenComment(props);
    m_currentText->draw(m_stickyNotes);
    m_stickyNotes.addCloseComment();
  }

  m_levelStack.top().m_geometry.reset();
  m_currentText.reset();
}

void KEYCollector::startSlides()
{
  m_paint = true;
}

void KEYCollector::endSlides()
{
  m_paint = false;
}

void KEYCollector::startThemes()
{
}

void KEYCollector::endThemes()
{
}

void KEYCollector::startPage()
{
  assert(!m_pageOpened);
  assert(!m_layerOpened);
  assert(m_notes.empty());
  assert(m_stickyNotes.empty());

  startLevel();

  m_pageOpened = true;

  if (m_paint)
  {
    librevenge::RVNGPropertyList props;
    props.insert("svg:width", pt2in(m_size.m_width));
    props.insert("svg:height", pt2in(m_size.m_height));

    m_document->startSlide(props);
  }
}

void KEYCollector::endPage()
{
  assert(m_pageOpened);

  endLevel();

  m_notes.clear();
  m_stickyNotes.clear();

  m_pageOpened = false;

  if (m_paint)
    m_document->endSlide();
}

void KEYCollector::startLayer()
{
  assert(m_pageOpened);
  assert(!m_layerOpened);

  getZoneManager().push();
  m_layerOpened = true;

  startLevel();
}

void KEYCollector::endLayer()
{
  assert(m_pageOpened);
  assert(m_layerOpened);

  endLevel();
  getZoneManager().pop();

  m_layerOpened = false;
}

void KEYCollector::drawNotes()
{
  if (m_notes.empty())
    return;

  m_document->startNotes(librevenge::RVNGPropertyList());
  m_notes.write(m_document);
  m_document->endNotes();
}

void KEYCollector::drawStickyNotes()
{
  if (!m_stickyNotes.empty())
    m_stickyNotes.write(m_document);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
