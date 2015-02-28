/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include "libetonyek_utils.h"
#include "IWORKStyleContext.h"
#include "KEYContentCollector.h"
#include "KEYDefaults.h"
#include "KEYDictionary.h"
#include "KEYText.h"

using boost::optional;

namespace libetonyek
{

KEYContentCollector::KEYContentCollector(librevenge::RVNGPresentationInterface *const painter, KEYDictionary &dict, const KEYLayerMap_t &masterPages, const IWORKSize &size, const KEYDefaults &defaults)
  : KEYCollectorBase(dict, defaults)
  , m_painter(painter)
  , m_masterPages(masterPages)
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

void KEYContentCollector::collectLayer(const optional<ID_t> &id, const bool ref)
{
  if (isCollecting())
  {
    assert(m_layerOpened);

    KEYCollectorBase::collectLayer(id, ref);

    if (ref && id)
    {
      const KEYLayerMap_t::const_iterator it = m_masterPages.find(get(id));
      if (m_masterPages.end() != it)
        drawLayer(it->second);
      else
      {
        ETONYEK_DEBUG_MSG(("master page layer %s not found\n", get(id).c_str()));
      }
    }
    else
      drawLayer(getLayer());
  }
}

void KEYContentCollector::collectPage(const optional<ID_t> &)
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
    props.insert("svg:width", pt2in(m_size.width));
    props.insert("svg:height", pt2in(m_size.height));

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

    KEYCollectorBase::startLayer();

    ++m_layerCount;

    librevenge::RVNGPropertyList props;
    props.insert("svg:id", m_layerCount);

    m_layerOpened = true;
    m_painter->startLayer(props);
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
    m_painter->endLayer();
  }
}

void KEYContentCollector::drawLayer(const KEYLayerPtr_t &layer)
{
  if (bool(layer))
  {
    for (IWORKObjectList_t::const_iterator it = layer->objects.begin(); it != layer->objects.end(); ++it)
      (*it)->draw(m_painter);
  }
  else
  {
    ETONYEK_DEBUG_MSG(("no layer\n"));
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

    if (bool(it->geometry))
    {
      props.insert("svg:x", pt2in(it->geometry->position.x));
      props.insert("svg:y", pt2in(it->geometry->position.y));
      props.insert("svg:width", pt2in(it->geometry->naturalSize.width));
      props.insert("svg:height", pt2in(it->geometry->naturalSize.height));
    }

    m_painter->startComment(props);
    if (bool(it->text))
      makeObject(it->text, getTransformation())->draw(m_painter);
    m_painter->endComment();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
