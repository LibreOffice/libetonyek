/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KNStylesCollector.h"

using std::string;

namespace libkeynote
{

KNStylesCollector::KNStylesCollector(KNStyleSheet &masterStyles, KNLayerMap_t &masterPages)
  : m_currentStyles(masterStyles)
  , m_masterPages(masterPages)
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

void KNStylesCollector::collectCharacterStyle(const string &id, const KNStyle &style)
{
  if (m_collecting)
    m_currentStyles.characterStyles[id] = style;
}

void KNStylesCollector::collectGraphicStyle(const string &id, const KNStyle &style)
{
  if (m_collecting)
    m_currentStyles.graphicStyles[id] = style;
}

void KNStylesCollector::collectHeadlineStyle(const std::string &id, const KNStyle &style)
{
  if (m_collecting)
    m_currentStyles.headlineStyles[id] = style;
}

void KNStylesCollector::collectLayoutStyle(const string &id, const KNStyle &style)
{
  if (m_collecting)
    m_currentStyles.layoutStyles[id] = style;
}

void KNStylesCollector::collectParagraphStyle(const string &id, const KNStyle &style)
{
  if (m_collecting)
    m_currentStyles.paragraphStyles[id] = style;
}

void KNStylesCollector::collectGeometry(const std::string &id, const KNGeometry &geometry)
{
  if (m_collecting)
    m_currentGeometries[id] = geometry;
}

void KNStylesCollector::collectGroup(const std::string &id, const KNGroup &group)
{
  if (m_collecting)
    m_currentGroups[id] = group;
}

void KNStylesCollector::collectImage(const std::string &id, const KNImage &image)
{
  if (m_collecting)
    m_currentImages[id] = image;
}

void KNStylesCollector::collectLine(const std::string &id, const KNLine &line)
{
  if (m_collecting)
    m_currentLines[id] = line;
}

void KNStylesCollector::collectMedia(const std::string &id, const KNMedia &media)
{
  if (m_collecting)
    m_currentMedia[id] = media;
}

void KNStylesCollector::collectPath(const std::string &id, const KNPath &path)
{
  if (m_collecting)
    m_currentPaths[id] = path;
}

void KNStylesCollector::startSize()
{
  m_collecting = true;
}

void KNStylesCollector::endSize()
{
  m_collecting = false;
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
