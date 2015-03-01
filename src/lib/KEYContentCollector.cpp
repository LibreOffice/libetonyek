/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYContentCollector.h"

#include <cassert>

#include "libetonyek_utils.h"
#include "IWORKStyleStack.h"
#include "IWORKText.h"
#include "KEYDictionary.h"

using boost::optional;

namespace libetonyek
{

KEYContentCollector::KEYContentCollector(librevenge::RVNGPresentationInterface *const painter, const IWORKSize &size)
  : KEYCollectorBase()
  , m_painter(painter)
  , m_size(size)
  , m_pageOpened(false)
  , m_layerOpened(false)
  , m_layerCount(0)
{
  m_painter->startDocument(librevenge::RVNGPropertyList());
  m_painter->setDocumentMetaData(librevenge::RVNGPropertyList());
}

KEYContentCollector::~KEYContentCollector()
{
  assert(!m_pageOpened);
  assert(!m_layerOpened);

  m_painter->endDocument();
}

void KEYContentCollector::collectPresentation(const boost::optional<IWORKSize> &)
{
}

void KEYContentCollector::insertLayer(const KEYLayerPtr_t &layer)
{
  if (isCollecting())
  {
    assert(!m_layerOpened);

    if (bool(layer))
    {
      ++m_layerCount;

      librevenge::RVNGPropertyList props;
      props.insert("svg:id", m_layerCount);

      m_painter->startLayer(props);

      for (IWORKObjectList_t::const_iterator it = layer->m_objects.begin(); it != layer->m_objects.end(); ++it)
        (*it)->draw(m_painter);

      m_painter->endLayer();
    }
    else
    {
      ETONYEK_DEBUG_MSG(("no layer\n"));
    }
  }
}

void KEYContentCollector::collectPage()
{
  if (isCollecting())
  {
    assert(m_pageOpened);

    drawNotes(getNotes());
    drawStickyNotes(getStickyNotes());
  }
}

void KEYContentCollector::startSlides()
{
  setCollecting(true);
}

void KEYContentCollector::endSlides()
{
  setCollecting(false);
}

void KEYContentCollector::startThemes()
{
}

void KEYContentCollector::endThemes()
{
}

void KEYContentCollector::resolveStyle(IWORKStyle &style)
{
  // TODO: implement me
  (void) style;
}

void KEYContentCollector::startPage()
{
  if (isCollecting())
  {
    assert(!m_pageOpened);
    assert(!m_layerOpened);

    KEYCollectorBase::startPage();
    startLevel();

    librevenge::RVNGPropertyList props;
    props.insert("svg:width", pt2in(m_size.m_width));
    props.insert("svg:height", pt2in(m_size.m_height));

    m_pageOpened = true;
    m_painter->startSlide(props);
  }
}

void KEYContentCollector::endPage()
{
  if (isCollecting())
  {
    assert(m_pageOpened);

    endLevel();
    KEYCollectorBase::endPage();

    m_pageOpened = false;
    m_painter->endSlide();
  }
}

void KEYContentCollector::startLayer()
{
  if (isCollecting())
  {
    assert(m_pageOpened);
    assert(!m_layerOpened);

    m_layerOpened = true;

    KEYCollectorBase::startLayer();

  }
}

void KEYContentCollector::endLayer()
{
  if (isCollecting())
  {
    assert(m_pageOpened);
    assert(m_layerOpened);

    KEYCollectorBase::endLayer();

    m_layerOpened = false;
  }
}

void KEYContentCollector::drawNotes(const IWORKObjectList_t &notes)
{
  if (notes.empty())
    return;

  m_painter->startNotes(librevenge::RVNGPropertyList());
  for (IWORKObjectList_t::const_iterator it = notes.begin(); notes.end() != it; ++it)
    (*it)->draw(m_painter);
  m_painter->endNotes();
}

void KEYContentCollector::drawStickyNotes(const KEYStickyNotes_t &stickyNotes)
{
  if (stickyNotes.empty())
    return;

  for (KEYStickyNotes_t::const_iterator it = stickyNotes.begin(); stickyNotes.end() != it; ++it)
  {
    librevenge::RVNGPropertyList props;

    if (bool(it->m_geometry))
    {
      props.insert("svg:x", pt2in(it->m_geometry->m_position.m_x));
      props.insert("svg:y", pt2in(it->m_geometry->m_position.m_y));
      props.insert("svg:width", pt2in(it->m_geometry->m_naturalSize.m_width));
      props.insert("svg:height", pt2in(it->m_geometry->m_naturalSize.m_height));
    }

    m_painter->startComment(props);
    if (bool(it->m_text))
      makeObject(it->m_text, getTransformation())->draw(m_painter);
    m_painter->endComment();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
