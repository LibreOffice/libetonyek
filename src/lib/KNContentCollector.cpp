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
#include "KNDictionary.h"
#include "KNTransformation.h"

namespace libkeynote
{

KNContentCollector::KNContentCollector(::libwpg::WPGPaintInterface *const painter, KNDictionary &dict, const KNLayerMap_t &masterPages, const KNSize &size)
  : KNCollectorBase(dict)
  , m_painter(painter)
  , m_dict(dict)
  , m_masterPages(masterPages)
  , m_size(size)
  , m_pageOpened(false)
  , m_layerOpened(false)
{
}

KNContentCollector::~KNContentCollector()
{
  assert(!m_pageOpened);
  assert(!m_layerOpened);
}

void KNContentCollector::collectSize(const KNSize &)
{
}

void KNContentCollector::collectLayer(const ID_t &id, const bool ref)
{
  assert(m_layerOpened);

  KNCollectorBase::collectLayer(id, ref);

  if (ref)
  {
    const KNLayerMap_t::const_iterator it = m_masterPages.find(id);
    if (m_masterPages.end() != it)
      drawLayer(it->second->objects);
    else
    {
      KN_DEBUG_MSG(("master page layer %s not found\n", id.c_str()));
    }
  }
  else
    drawLayer(getObjects());
}

void KNContentCollector::collectPage(const ID_t &)
{
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
  assert(!m_pageOpened);
  assert(!m_layerOpened);

  WPXPropertyList props;

  m_pageOpened = true;
  m_painter->startGraphics(props);
}

void KNContentCollector::endPage()
{
  assert(m_pageOpened);

  m_pageOpened = false;
  m_painter->endGraphics();
}

void KNContentCollector::startLayer()
{
  assert(m_pageOpened);
  assert(!m_layerOpened);

  KNCollectorBase::startLayer();

  WPXPropertyList props;

  m_layerOpened = true;
  m_painter->startLayer(props);
}

void KNContentCollector::endLayer()
{
  assert(m_pageOpened);
  assert(m_layerOpened);

  KNCollectorBase::endLayer();

  m_layerOpened = false;
  m_painter->endLayer();
}

void KNContentCollector::drawLayer(const KNObjectList_t &objects)
{
  KNTransformation tr;
  for (KNObjectList_t::const_iterator it = objects.begin(); it != objects.end(); ++it)
    (*it)->draw(m_painter, m_dict, tr);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
