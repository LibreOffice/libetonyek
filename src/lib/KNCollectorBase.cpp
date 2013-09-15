/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include "libkeynote_utils.h"
#include "KNCollectorBase.h"
#include "KNDictionary.h"
#include "KNShape.h"
#include "KNText.h"

using boost::optional;

namespace libkeynote
{

namespace
{

/** Return value, either directly or by its ID.
  *
  * If the input is not a reference, save value to @c map for future use.
  *
  * @arg[in] id the value's ID
  * @arg[in] value a possible value
  * @arg[in] ref indicator whether the input should be handled as a
  *   value or a reference
  * @arg[in] map the map to use to retrieve the value by ID or to save a
  *   new value
  * @return the value
  */
template<class T>
T getValue(const optional<ID_t> &id, const T &value, const bool ref, boost::unordered_map<ID_t, T> &map)
{
  T retval;

  if (ref)
  {
    assert(id);

    const typename boost::unordered_map<ID_t, T>::const_iterator it = map.find(get(id));
    if (map.end() != it)
    {
      retval = it->second;
    }
    else
    {
      KN_DEBUG_MSG(("item with ID %s does not exist\n", get(id).c_str()));
    }
  }
  else
  {
    retval = value;
    if (id)
      map[get(id)] = value;
  }

  return retval;
}

/** Return value by its ID.
  *
  * @arg[in] id the value's ID
  * @arg[in] map the map to use to retrieve the value
  * @return the value
  */
template<class T>
T getValue(const optional<ID_t> &id, boost::unordered_map<ID_t, T> &map)
{
  return getValue(id, T(), true, map);
}

}

KNCollectorBase::KNCollectorBase(KNDictionary &dict)
  : m_dict(dict)
  , m_objectsStack()
  , m_currentGeometry()
  , m_currentText()
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

void KNCollectorBase::collectCharacterStyle(const optional<ID_t> &id, const KNCharacterStylePtr_t &style)
{
  if (m_collecting && id)
    m_dict.characterStyles[get(id)] = style;
}

void KNCollectorBase::collectGraphicStyle(const optional<ID_t> &id, const KNGraphicStylePtr_t &style)
{
  if (m_collecting && id)
    m_dict.graphicStyles[get(id)] = style;
}

void KNCollectorBase::collectLayoutStyle(const optional<ID_t> &id, const KNLayoutStylePtr_t &style)
{
  if (m_collecting && id)
    m_dict.layoutStyles[get(id)] = style;
}

void KNCollectorBase::collectParagraphStyle(const optional<ID_t> &id, const KNParagraphStylePtr_t &style)
{
  if (m_collecting && id)
    m_dict.paragraphStyles[get(id)] = style;
}

void KNCollectorBase::collectBezier(const optional<ID_t> &id, const KNPathPtr_t &path, const bool ref)
{
  if (m_collecting)
    m_currentPath = getValue(id, path, ref, m_dict.beziers);
}

void KNCollectorBase::collectGeometry(const optional<ID_t> &, const KNGeometryPtr_t &geometry)
{
  if (m_collecting)
    m_currentGeometry = geometry;
}

void KNCollectorBase::collectGroup(const optional<ID_t> &, const KNGroupPtr_t &group)
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

void KNCollectorBase::collectImage(const optional<ID_t> &id, const KNImagePtr_t &image)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());

    image->geometry = m_currentGeometry;
    m_currentGeometry.reset();
    if (id)
      m_dict.images[get(id)] = image;
    m_objectsStack.top().push_back(makeObject(image));
  }
}

void KNCollectorBase::collectLine(const optional<ID_t> &, const KNLinePtr_t &line)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());

    line->geometry = m_currentGeometry;
    m_currentGeometry.reset();
    m_objectsStack.top().push_back(makeObject(line));
  }
}

void KNCollectorBase::collectMedia(const optional<ID_t> &, const KNMediaPtr_t &media)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());

    media->geometry = m_currentGeometry;
    m_currentGeometry.reset();
    m_objectsStack.top().push_back(makeObject(media));
  }
}

void KNCollectorBase::collectShape(const optional<ID_t> &)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());

    const KNShapePtr_t shape(new KNShape());

    if (!m_currentPath)
    {
      KN_DEBUG_MSG(("the path is empty\n"));
    }
    shape->path = m_currentPath;
    m_currentPath.reset();

    // TODO: fill the other parts of shape as well

    m_objectsStack.top().push_back(makeObject(shape));
  }
}

void KNCollectorBase::collectBezierPath(const optional<ID_t> &)
{
  // nothing needed
}

void KNCollectorBase::collectPolygonPath(const optional<ID_t> &, const KNSize &size, const unsigned edges)
{
  if (m_collecting)
    m_currentPath = makePolygonPath(size, edges);
}

void KNCollectorBase::collectRoundedRectanglePath(const optional<ID_t> &, const KNSize &size, const double radius)
{
  if (m_collecting)
    m_currentPath = makeRoundedRectanglePath(size, radius);
}

void KNCollectorBase::collectArrowPath(const optional<ID_t> &, const KNSize &size, const double headWidth, const double stemRelYPos, bool const doubleSided)
{
  if (m_collecting)
  {
    if (doubleSided)
      m_currentPath = makeDoubleArrowPath(size, headWidth, stemRelYPos);
    else
      m_currentPath = makeArrowPath(size, headWidth, stemRelYPos);
  }
}

void KNCollectorBase::collectStarPath(const optional<ID_t> &, const KNSize &size, const unsigned points, const double innerRadius)
{
  if (m_collecting)
    m_currentPath = makeStarPath(size, points, innerRadius);
}

void KNCollectorBase::collectConnectionPath(const optional<ID_t> &, const KNSize &size, const double middleX, const double middleY)
{
  if (m_collecting)
    m_currentPath = makeConnectionPath(size, middleX, middleY);
}

void KNCollectorBase::collectCalloutPath(const optional<ID_t> &, const KNSize &size, const double radius, const double tailSize, const double tailX, const double tailY, bool quoteBubble)
{
  if (m_collecting)
  {
    if (quoteBubble)
      m_currentPath = makeQuoteBubblePath(size, radius, tailSize, tailX, tailY);
    else
      m_currentPath = makeCalloutPath(size, radius, tailSize, tailX, tailY);
  }
}

void KNCollectorBase::collectLayer(const optional<ID_t> &, bool)
{
  if (m_collecting)
  {
    assert(m_layerOpened);
    assert(!m_objectsStack.empty());

    m_currentLayer.reset(new KNLayer());
    m_currentLayer->objects = m_objectsStack.top();
    m_objectsStack.pop();
  }
}

void KNCollectorBase::collectText(const optional<ID_t> &style, const std::string &text)
{
  if (m_collecting)
  {
    assert(bool(m_currentText));

    m_currentText->insertText(text, getValue(style, m_dict.characterStyles));
  }
}

void KNCollectorBase::collectTab()
{
  if (m_collecting)
  {
    assert(bool(m_currentText));

    m_currentText->insertTab();
  }
}

void KNCollectorBase::collectLineBreak()
{
  if (m_collecting)
  {
    assert(bool(m_currentText));

    m_currentText->insertLineBreak();
  }
}

void KNCollectorBase::collectSlideText(const optional<ID_t> &id, const bool title)
{
  if (m_collecting)
  {
    // assert(!m_objectsStack.empty());

    KNTextBodyPtr_t textBody(new KNTextBody(title));
    textBody->geometry = m_currentGeometry;
    textBody->text = m_currentText;
    if (id)
    {
      if (title)
        m_dict.titlePlaceholders[get(id)] = textBody;
      else
        m_dict.bodyPlaceholders[get(id)] = textBody;
    }
    // m_objectsStack.top().push_back(makeObject(textBody));

    m_currentGeometry.reset();
    m_currentText.reset();
  }
}

void KNCollectorBase::startLayer()
{
  if (m_collecting)
  {
    assert(!m_layerOpened);
    assert(m_objectsStack.empty());
    assert(!m_currentLayer);

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
    // object stack is already cleared by collectLayer()
    assert(m_objectsStack.empty());

    m_currentLayer.reset();
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

void KNCollectorBase::startParagraph(const optional<ID_t> &style)
{
  if (m_collecting)
  {
    assert(bool(m_currentText));

    m_currentText->openParagraph(getValue(style, m_dict.paragraphStyles));
  }
}

void KNCollectorBase::endParagraph()
{
  if (m_collecting)
  {
    assert(bool(m_currentText));

    m_currentText->closeParagraph();
  }
}

void KNCollectorBase::startTextLayout(const optional<ID_t> &style)
{
  if (m_collecting)
  {
    assert(!m_currentText);

    m_currentText.reset(new KNText());
    m_currentText->setLayoutStyle(getValue(style, m_dict.layoutStyles));
  }
}

void KNCollectorBase::endTextLayout()
{
  if (m_collecting)
  {
    assert(bool(m_currentText));

    m_currentText.reset();
  }
}

bool KNCollectorBase::isCollecting() const
{
  return m_collecting;
}

void KNCollectorBase::setCollecting(bool collecting)
{
  m_collecting = collecting;
}

const KNLayerPtr_t &KNCollectorBase::getLayer() const
{
  return m_currentLayer;
}

} // namespace libkeynote

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
