/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KNStylesCollector.h"

namespace libkeynote
{

KNStylesCollector::KNStylesCollector(KNStyleSheet &masterStyles, KNLayerMap_t &masterPages, KNSize &size)
  : m_currentStyles(masterStyles)
  , m_masterPages(masterPages)
  , m_size(size)
  , m_currentGeometries()
  , m_currentGroups()
  , m_currentImages()
  , m_currentLines()
  , m_currentMedia()
  , m_currentPaths()
  , m_currentLayer()
  , m_collecting(false)
{
}

KNStylesCollector::~KNStylesCollector()
{
}

void KNStylesCollector::collectCharacterStyle(const ID_t &id, const KNStyle &style)
{
  if (m_collecting)
    m_currentStyles.characterStyles[id] = style;
}

void KNStylesCollector::collectGraphicStyle(const ID_t &id, const KNStyle &style)
{
  if (m_collecting)
    m_currentStyles.graphicStyles[id] = style;
}

void KNStylesCollector::collectHeadlineStyle(const ID_t &id, const KNStyle &style)
{
  if (m_collecting)
    m_currentStyles.headlineStyles[id] = style;
}

void KNStylesCollector::collectLayoutStyle(const ID_t &id, const KNStyle &style)
{
  if (m_collecting)
    m_currentStyles.layoutStyles[id] = style;
}

void KNStylesCollector::collectParagraphStyle(const ID_t &id, const KNStyle &style)
{
  if (m_collecting)
    m_currentStyles.paragraphStyles[id] = style;
}

void KNStylesCollector::collectGeometry(const ID_t &id, const KNGeometry &geometry)
{
  if (m_collecting)
    m_currentGeometries[id] = geometry;
}

void KNStylesCollector::collectGroup(const ID_t &id, const KNGroup &group)
{
  if (m_collecting)
    m_currentGroups[id] = group;
}

void KNStylesCollector::collectImage(const ID_t &id, const KNImage &image)
{
  if (m_collecting)
    m_currentImages[id] = image;
}

void KNStylesCollector::collectLine(const ID_t &id, const KNLine &line)
{
  if (m_collecting)
    m_currentLines[id] = line;
}

void KNStylesCollector::collectMedia(const ID_t &id, const KNMedia &media)
{
  if (m_collecting)
    m_currentMedia[id] = media;
}

void KNStylesCollector::collectPath(const ID_t &id, const KNPath &path)
{
  if (m_collecting)
    m_currentPaths[id] = path;
}

void KNStylesCollector::collectSize(const KNSize &size)
{
  m_size = size;
}

void KNStylesCollector::collectLayer(const ID_t &id, const bool ref)
{
  // TODO: implement me
  (void) id;
  (void) ref;
}

void KNStylesCollector::collectPage(const ID_t &id)
{
  // TODO: implement me
  (void) id;
}

void KNStylesCollector::startSlides()
{
}

void KNStylesCollector::endSlides()
{
}

void KNStylesCollector::startThemes()
{
  m_collecting = true;
}

void KNStylesCollector::endThemes()
{
  m_collecting = false;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
