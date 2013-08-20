/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include "KNCollectorBase.h"
#include "KNDictionary.h"

namespace libkeynote
{

KNCollectorBase::KNCollectorBase(KNDictionary &dict)
  : m_dict(dict)
  , m_collecting(false)
{
}

KNCollectorBase::~KNCollectorBase()
{
  assert(!m_collecting);
}

void KNCollectorBase::collectCharacterStyle(const ID_t &id, const KNStyle &style)
{
  if (m_collecting)
    m_dict.characterStyles[id] = style;
}

void KNCollectorBase::collectGraphicStyle(const ID_t &id, const KNStyle &style)
{
  if (m_collecting)
    m_dict.graphicStyles[id] = style;
}

void KNCollectorBase::collectHeadlineStyle(const ID_t &id, const KNStyle &style)
{
  if (m_collecting)
    m_dict.headlineStyles[id] = style;
}

void KNCollectorBase::collectLayoutStyle(const ID_t &id, const KNStyle &style)
{
  if (m_collecting)
    m_dict.layoutStyles[id] = style;
}

void KNCollectorBase::collectParagraphStyle(const ID_t &id, const KNStyle &style)
{
  if (m_collecting)
    m_dict.paragraphStyles[id] = style;
}

void KNCollectorBase::collectGeometry(const ID_t &id, const KNGeometry &geometry)
{
  if (m_collecting)
    m_dict.geometries[id] = geometry;
}

void KNCollectorBase::collectGroup(const ID_t &id, const KNGroup &group)
{
  if (m_collecting)
    m_dict.groups[id] = group;
}

void KNCollectorBase::collectImage(const ID_t &id, const KNImage &image)
{
  if (m_collecting)
    m_dict.images[id] = image;
}

void KNCollectorBase::collectLine(const ID_t &id, const KNLine &line)
{
  if (m_collecting)
    m_dict.lines[id] = line;
}

void KNCollectorBase::collectMedia(const ID_t &id, const KNMedia &media)
{
  if (m_collecting)
    m_dict.media[id] = media;
}

void KNCollectorBase::collectPath(const ID_t &id, const KNPath &path)
{
  if (m_collecting)
    m_dict.paths[id] = path;
}

void KNCollectorBase::collectLayer(const ID_t &id, bool ref)
{
  // TODO: implement me
  (void) id;
  (void) ref;
}

bool KNCollectorBase::getCollecting() const
{
  return m_collecting;
}

void KNCollectorBase::setCollecting(bool collecting)
{
  m_collecting = collecting;
}

void KNCollectorBase::fillLayer(KNLayer &layer)
{
  // TODO: implement me
  (void) layer;
}

} // namespace libkeynote

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
