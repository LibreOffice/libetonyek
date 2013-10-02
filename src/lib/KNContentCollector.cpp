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
#include "KNContentCollector.h"
#include "KNDefaults.h"
#include "KNDictionary.h"
#include "KNOutput.h"
#include "KNStyleContext.h"

using boost::optional;

namespace libkeynote
{

KNContentCollector::KNContentCollector(::libwpg::WPGPaintInterface *const painter, KNDictionary &dict, const KNLayerMap_t &masterPages, const KNSize &, const KNDefaults &defaults)
  : KNCollectorBase(dict, defaults)
  , m_painter(painter)
  , m_masterPages(masterPages)
  , m_pageOpened(false)
  , m_layerOpened(false)
{
}

KNContentCollector::~KNContentCollector()
{
  assert(!m_pageOpened);
  assert(!m_layerOpened);
}

void KNContentCollector::collectPresentation(const boost::optional<KNSize> &)
{
}

void KNContentCollector::collectLayer(const optional<ID_t> &id, const bool ref)
{
  if (isCollecting())
  {
    assert(m_layerOpened);

    KNCollectorBase::collectLayer(id, ref);

    if (ref && id)
    {
      const KNLayerMap_t::const_iterator it = m_masterPages.find(get(id));
      if (m_masterPages.end() != it)
        drawLayer(it->second);
      else
      {
        KN_DEBUG_MSG(("master page layer %s not found\n", get(id).c_str()));
      }
    }
    else
      drawLayer(getLayer());
  }
}

void KNContentCollector::collectPage(const optional<ID_t> &)
{
  if (isCollecting())
    assert(m_pageOpened);
}

void KNContentCollector::startSlides()
{
  setCollecting(true);
}

void KNContentCollector::endSlides()
{
  setCollecting(false);
}

void KNContentCollector::startThemes()
{
}

void KNContentCollector::endThemes()
{
}

void KNContentCollector::resolveStyle(KNStyle &style)
{
  // TODO: implement me
  (void) style;
}

void KNContentCollector::startPage()
{
  KNCollectorBase::startPage();

  if (isCollecting())
  {
    assert(!m_pageOpened);
    assert(!m_layerOpened);

    WPXPropertyList props;

    m_pageOpened = true;
    m_painter->startGraphics(props);
  }
}

void KNContentCollector::endPage()
{
  KNCollectorBase::endPage();

  if (isCollecting())
  {
    assert(m_pageOpened);

    m_pageOpened = false;
    m_painter->endGraphics();
  }
}

void KNContentCollector::startLayer()
{
  if (isCollecting())
  {
    assert(m_pageOpened);
    assert(!m_layerOpened);

    KNCollectorBase::startLayer();

    WPXPropertyList props;

    m_layerOpened = true;
    m_painter->startLayer(props);
  }
}

void KNContentCollector::endLayer()
{
  if (isCollecting())
  {
    assert(m_pageOpened);
    assert(m_layerOpened);

    KNCollectorBase::endLayer();

    m_layerOpened = false;
    m_painter->endLayer();
  }
}

void KNContentCollector::drawLayer(const KNLayerPtr_t &layer)
{
  if (bool(layer))
  {
    KNStyleContext styleContext;
    const KNOutput output(m_painter, styleContext);
    for (KNObjectList_t::const_iterator it = layer->objects.begin(); it != layer->objects.end(); ++it)
      (*it)->draw(output);
  }
  else
  {
    KN_DEBUG_MSG(("no layer\n"));
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
