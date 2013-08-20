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

#include "KNContentCollector.h"
#include "KNDictionary.h"

namespace libkeynote
{

KNContentCollector::KNContentCollector(::libwpg::WPGPaintInterface *const painter, KNDictionary &dict, const KNLayerMap_t &masterPages, const KNSize &size)
  : m_painter(painter)
  , m_dict(dict)
  , m_masterPages(masterPages)
  , m_size(size)
  , m_currentLayer()
  , m_collecting(false)
  , m_pageOpened(false)
  , m_layerOpened(false)
{
}

KNContentCollector::~KNContentCollector()
{
  assert(!m_collecting);
  assert(!m_pageOpened);
  assert(!m_layerOpened);
}

void KNContentCollector::collectCharacterStyle(const ID_t &id, const KNStyle &style)
{
  if (m_collecting)
    m_dict.characterStyles[id] = style;
}

void KNContentCollector::collectGraphicStyle(const ID_t &id, const KNStyle &style)
{
  if (m_collecting)
    m_dict.graphicStyles[id] = style;
}

void KNContentCollector::collectHeadlineStyle(const ID_t &id, const KNStyle &style)
{
  if (m_collecting)
    m_dict.headlineStyles[id] = style;
}

void KNContentCollector::collectLayoutStyle(const ID_t &id, const KNStyle &style)
{
  if (m_collecting)
    m_dict.layoutStyles[id] = style;
}

void KNContentCollector::collectParagraphStyle(const ID_t &id, const KNStyle &style)
{
  if (m_collecting)
    m_dict.paragraphStyles[id] = style;
}

void KNContentCollector::collectGeometry(const ID_t &id, const KNGeometry &geometry)
{
  if (m_collecting)
    m_dict.geometries[id] = geometry;
}

void KNContentCollector::collectGroup(const ID_t &id, const KNGroup &group)
{
  if (m_collecting)
    m_dict.groups[id] = group;
}

void KNContentCollector::collectImage(const ID_t &id, const KNImage &image)
{
  if (m_collecting)
    m_dict.images[id] = image;
}

void KNContentCollector::collectLine(const ID_t &id, const KNLine &line)
{
  if (m_collecting)
    m_dict.lines[id] = line;
}

void KNContentCollector::collectMedia(const ID_t &id, const KNMedia &media)
{
  if (m_collecting)
    m_dict.media[id] = media;
}

void KNContentCollector::collectPath(const ID_t &id, const KNPath &path)
{
  if (m_collecting)
    m_dict.paths[id] = path;
}

void KNContentCollector::collectSize(const KNSize &)
{
}

void KNContentCollector::collectLayer(const ID_t &id, const bool ref)
{
  // TODO: implement me
  (void) id;
  (void) ref;
  assert(m_layerOpened);
}

void KNContentCollector::collectPage(const ID_t &)
{
  assert(m_pageOpened);
}

void KNContentCollector::startSlides()
{
  m_collecting = true;
}

void KNContentCollector::endSlides()
{
  m_collecting = false;
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

  WPXPropertyList props;

  m_layerOpened = true;
  m_painter->startLayer(props);
}

void KNContentCollector::endLayer()
{
  assert(m_pageOpened);
  assert(m_layerOpened);

  m_layerOpened = false;
  m_painter->endLayer();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
