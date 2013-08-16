/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KNContentCollector.h"

using std::string;

namespace libkeynote
{

KNContentCollector::KNContentCollector(::libwpg::WPGPaintInterface *const painter, const KNStyleSheet &masterStyles, const KNLayerMap_t &masterPages, const KNSize &size)
  : m_painter(painter)
  , m_masterStyles(masterStyles)
  , m_masterPages(masterPages)
  , m_size(size)
  , m_currentStyles()
  , m_currentLayer()
  , m_currentGeometries()
  , m_currentGroups()
  , m_currentImages()
  , m_currentLines()
  , m_currentMedia()
  , m_currentPaths()
  , m_collecting(false)
{
}

KNContentCollector::~KNContentCollector()
{
}

void KNContentCollector::collectCharacterStyle(const string &id, const KNStyle &style)
{
  if (m_collecting)
    m_currentStyles.characterStyles[id] = style;
}

void KNContentCollector::collectGraphicStyle(const string &id, const KNStyle &style)
{
  if (m_collecting)
    m_currentStyles.graphicStyles[id] = style;
}

void KNContentCollector::collectHeadlineStyle(const std::string &id, const KNStyle &style)
{
  if (m_collecting)
    m_currentStyles.headlineStyles[id] = style;
}

void KNContentCollector::collectLayoutStyle(const string &id, const KNStyle &style)
{
  if (m_collecting)
    m_currentStyles.layoutStyles[id] = style;
}

void KNContentCollector::collectParagraphStyle(const string &id, const KNStyle &style)
{
  if (m_collecting)
    m_currentStyles.paragraphStyles[id] = style;
}

void KNContentCollector::collectGeometry(const std::string &id, const KNGeometry &geometry)
{
  if (m_collecting)
    m_currentGeometries[id] = geometry;
}

void KNContentCollector::collectGroup(const std::string &id, const KNGroup &group)
{
  if (m_collecting)
    m_currentGroups[id] = group;
}

void KNContentCollector::collectImage(const std::string &id, const KNImage &image)
{
  if (m_collecting)
    m_currentImages[id] = image;
}

void KNContentCollector::collectLine(const std::string &id, const KNLine &line)
{
  if (m_collecting)
    m_currentLines[id] = line;
}

void KNContentCollector::collectMedia(const std::string &id, const KNMedia &media)
{
  if (m_collecting)
    m_currentMedia[id] = media;
}

void KNContentCollector::collectPath(const std::string &id, const KNPath &path)
{
  if (m_collecting)
    m_currentPaths[id] = path;
}

void KNContentCollector::collectSize(const KNSize &)
{
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

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
