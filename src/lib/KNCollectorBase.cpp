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
  , m_objectsStack()
  , m_collecting(false)
  , m_groupLevel(0)
{
}

KNCollectorBase::~KNCollectorBase()
{
  assert(!m_collecting);
  assert(m_objectsStack.empty());
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
  assert(!m_objectsStack.empty());

  if (m_collecting)
  {
    m_dict.groups[id] = group;
    m_objectsStack.top().push_back(makeGroupObject(id));
  }
}

void KNCollectorBase::collectImage(const ID_t &id, const KNImage &image)
{
  assert(!m_objectsStack.empty());

  if (m_collecting)
  {
    m_dict.images[id] = image;
    m_objectsStack.top().push_back(makeImageObject(id));
  }
}

void KNCollectorBase::collectLine(const ID_t &id, const KNLine &line)
{
  assert(!m_objectsStack.empty());

  if (m_collecting)
  {
    m_dict.lines[id] = line;
    m_objectsStack.top().push_back(makeLineObject(id));
  }
}

void KNCollectorBase::collectMedia(const ID_t &id, const KNMedia &media)
{
  assert(!m_objectsStack.empty());

  if (m_collecting)
  {
    m_dict.media[id] = media;
    m_objectsStack.top().push_back(makeMediaObject(id));
  }
}

void KNCollectorBase::collectPath(const ID_t &id, const KNPath &path)
{
  assert(!m_objectsStack.empty());

  if (m_collecting)
  {
    m_dict.paths[id] = path;
    m_objectsStack.top().push_back(makePathObject(id));
  }
}

void KNCollectorBase::collectLayer(const ID_t &, bool)
{
  assert(m_layerOpened);
  assert(!m_objectsStack.empty());
}

void KNCollectorBase::startLayer()
{
  assert(!m_layerOpened);
  assert(m_objectsStack.empty());

  m_objectsStack.push(KNObjectList_t());
  m_layerOpened = true;

  assert(!m_objectsStack.empty());
}

void KNCollectorBase::endLayer()
{
  assert(m_layerOpened);
  assert(!m_objectsStack.empty());

  m_objectsStack.pop();
  m_layerOpened = false;

  assert(m_objectsStack.empty());
}

void KNCollectorBase::startGroup()
{
  assert(m_layerOpened);
  assert(!m_objectsStack.empty());

  m_objectsStack.push(KNObjectList_t());
  ++m_groupLevel;
}

void KNCollectorBase::endGroup()
{
  assert(m_layerOpened);
  assert(!m_objectsStack.empty());
  assert(m_groupLevel > 0);

  m_objectsStack.pop();
  --m_groupLevel;

  assert(!m_objectsStack.empty());
}

bool KNCollectorBase::getCollecting() const
{
  return m_collecting;
}

void KNCollectorBase::setCollecting(bool collecting)
{
  m_collecting = collecting;
}

const KNObjectList_t &KNCollectorBase::getObjects() const
{
  assert(m_layerOpened);
  assert(!m_objectsStack.empty());

  return m_objectsStack.top();
}

} // namespace libkeynote

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
