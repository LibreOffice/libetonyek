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
  , m_currentGeometry()
  , m_collecting(false)
  , m_layerOpened(false)
  , m_groupLevel(0)
{
}

KNCollectorBase::~KNCollectorBase()
{
  assert(!m_collecting);
  assert(m_objectsStack.empty());
}

void KNCollectorBase::collectCharacterStyle(const ID_t &id, const KNStylePtr_t &style)
{
  if (m_collecting)
    m_dict.characterStyles[id] = style;
}

void KNCollectorBase::collectGraphicStyle(const ID_t &id, const KNStylePtr_t &style)
{
  if (m_collecting)
    m_dict.graphicStyles[id] = style;
}

void KNCollectorBase::collectHeadlineStyle(const ID_t &id, const KNStylePtr_t &style)
{
  if (m_collecting)
    m_dict.headlineStyles[id] = style;
}

void KNCollectorBase::collectLayoutStyle(const ID_t &id, const KNStylePtr_t &style)
{
  if (m_collecting)
    m_dict.layoutStyles[id] = style;
}

void KNCollectorBase::collectParagraphStyle(const ID_t &id, const KNStylePtr_t &style)
{
  if (m_collecting)
    m_dict.paragraphStyles[id] = style;
}

void KNCollectorBase::collectGeometry(const ID_t &, const KNGeometryPtr_t &geometry)
{
  if (m_collecting)
    m_currentGeometry = geometry;
}

void KNCollectorBase::collectGroup(const ID_t &, const KNGroupPtr_t &group)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());

    group->objects = m_objectsStack.top();
    m_objectsStack.pop();
    assert(!m_objectsStack.empty());
    m_objectsStack.top().push_back(makeObject(group));
  }
}

void KNCollectorBase::collectImage(const ID_t &id, const KNImagePtr_t &image)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());

    image->geometry = m_currentGeometry;
    m_currentGeometry.reset();
    m_dict.images[id] = image;
    m_objectsStack.top().push_back(makeObject(image));
  }
}

void KNCollectorBase::collectLine(const ID_t &, const KNLinePtr_t &line)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());

    line->geometry = m_currentGeometry;
    m_currentGeometry.reset();
    m_objectsStack.top().push_back(makeObject(line));
  }
}

void KNCollectorBase::collectMedia(const ID_t &, const KNMediaPtr_t &media)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());

    media->geometry = m_currentGeometry;
    m_currentGeometry.reset();
    m_objectsStack.top().push_back(makeObject(media));
  }
}

void KNCollectorBase::collectPath(const ID_t &, const KNPathPtr_t &path)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());

    path->setGeometry(m_currentGeometry);
    m_currentGeometry.reset();
    m_objectsStack.top().push_back(makeObject(path));
  }
}

void KNCollectorBase::collectLayer(const ID_t &, bool)
{
  if (m_collecting)
  {
    assert(m_layerOpened);
    assert(!m_objectsStack.empty());
  }
}

void KNCollectorBase::startLayer()
{
  if (m_collecting)
  {
    assert(!m_layerOpened);
    assert(m_objectsStack.empty());

    m_objectsStack.push(KNObjectList_t());
    m_layerOpened = true;

    assert(!m_objectsStack.empty());
  }
}

void KNCollectorBase::endLayer()
{
  if (m_collecting)
  {
    assert(m_layerOpened);
    assert(!m_objectsStack.empty());

    m_objectsStack.pop();
    m_layerOpened = false;

    assert(m_objectsStack.empty());
  }
}

void KNCollectorBase::startGroup()
{
  if (m_collecting)
  {
    assert(m_layerOpened);
    assert(!m_objectsStack.empty());

    m_objectsStack.push(KNObjectList_t());
    ++m_groupLevel;
  }
}

void KNCollectorBase::endGroup()
{
  if (m_collecting)
  {
    assert(m_layerOpened);
    assert(!m_objectsStack.empty());
    assert(m_groupLevel > 0);

    --m_groupLevel;
    // stack is popped in collectGroup already
  }
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
  assert(m_collecting);
  assert(m_layerOpened);
  assert(!m_objectsStack.empty());

  return m_objectsStack.top();
}

} // namespace libkeynote

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
