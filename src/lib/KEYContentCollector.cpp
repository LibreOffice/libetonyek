/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include <libwpg/libwpg.h>

#include "libkeynote_utils.h"
#include "KEYContentCollector.h"
#include "KEYDefaults.h"
#include "KEYDictionary.h"
#include "KEYOutput.h"
#include "KEYStyleContext.h"

using boost::optional;

namespace libkeynote
{

KEYContentCollector::KEYContentCollector(::libwpg::WPGPaintInterface *const painter, KEYDictionary &dict, const KEYLayerMap_t &masterPages, const KEYSize &size, const KEYDefaults &defaults)
  : KEYCollectorBase(dict, defaults)
  , m_painter(painter)
  , m_masterPages(masterPages)
  , m_size(size)
  , m_pageOpened(false)
  , m_layerOpened(false)
  , m_layerCount(0)
{
}

KEYContentCollector::~KEYContentCollector()
{
  assert(!m_pageOpened);
  assert(!m_layerOpened);
}

void KEYContentCollector::collectPresentation(const boost::optional<KEYSize> &)
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
        KEY_DEBUG_MSG(("master page layer %s not found\n", get(id).c_str()));
      }
    }
    else
      drawLayer(getLayer());
  }
}

void KEYContentCollector::collectPage(const optional<ID_t> &)
{
  if (isCollecting())
    assert(m_pageOpened);
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

void KEYContentCollector::resolveStyle(KEYStyle &style)
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

    WPXPropertyList props;
    props.insert("svg:width", pt2in(m_size.width));
    props.insert("svg:height", pt2in(m_size.height));

    m_pageOpened = true;
    m_painter->startGraphics(props);
  }
}

void KEYContentCollector::endPage()
{
  if (isCollecting())
  {
    assert(m_pageOpened);

    m_pageOpened = false;
    m_painter->endGraphics();
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

    WPXPropertyList props;
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
    KEYStyleContext styleContext;
    const KEYOutput output(m_painter, styleContext);
    for (KEYObjectList_t::const_iterator it = layer->objects.begin(); it != layer->objects.end(); ++it)
      (*it)->draw(output);
  }
  else
  {
    KEY_DEBUG_MSG(("no layer\n"));
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
