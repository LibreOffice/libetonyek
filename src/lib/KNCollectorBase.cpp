/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>
#include <cassert>

#include <boost/bind.hpp>

#include "libkeynote_utils.h"
#include "KNCollectorBase.h"
#include "KNDefaults.h"
#include "KNDictionary.h"
#include "KNShape.h"
#include "KNStyles.h"
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

KNCollectorBase::Level::Level()
  : geometry()
  , graphicStyle()
{
}

KNCollectorBase::KNCollectorBase(KNDictionary &dict, const KNDefaults &defaults)
  : m_dict(dict)
  , m_defaults(defaults)
  , m_currentLayer()
  , m_levelStack()
  , m_objectsStack()
  , m_currentPath()
  , m_currentText()
  , m_currentStylesheet(new KNStylesheet())
  , m_newStyles()
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

void KNCollectorBase::collectCellStyle(const boost::optional<ID_t> &id,
                                       const boost::optional<KNPropertyMap> &props,
                                       const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                       const bool ref, const bool anonymous)
{
  // TODO: implement me
  (void) id;
  (void) props;
  (void) ident;
  (void) parentIdent;
  (void) ref;
  (void) anonymous;
}

void KNCollectorBase::collectCharacterStyle(const optional<ID_t> &id,
    const boost::optional<KNPropertyMap> &props,
    const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
    const bool ref, const bool anonymous)
{
  if (m_collecting)
  {
    assert(m_currentStylesheet);

    KNCharacterStylePtr_t newStyle;
    // TODO: handle default style properties
    if (!ref && props)
      newStyle.reset(new KNCharacterStyle(get(props), ident, parentIdent));

    const KNCharacterStylePtr_t style = getValue(id, newStyle, ref, m_dict.characterStyles);
    if (bool(style))
    {
      if (ident && !anonymous)
        m_currentStylesheet->characterStyles[get(ident)] = style;
      if (!ref)
        m_newStyles.push_back(style);
    }
  }
}

void KNCollectorBase::collectConnectionStyle(const boost::optional<ID_t> &id,
    const boost::optional<KNPropertyMap> &props,
    const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
    const bool ref, const bool anonymous)
{
  // TODO: implement me
  (void) id;
  (void) props;
  (void) ident;
  (void) parentIdent;
  (void) ref;
  (void) anonymous;
}

void KNCollectorBase::collectGraphicStyle(const optional<ID_t> &id,
    const boost::optional<KNPropertyMap> &props,
    const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
    const bool ref, const bool anonymous)
{
  if (m_collecting)
  {
    assert(m_currentStylesheet);

    KNGraphicStylePtr_t newStyle;
    if (!ref && props)
      newStyle.reset(new KNGraphicStyle(get(props), ident, parentIdent));

    const KNGraphicStylePtr_t style = getValue(id, newStyle, ref, m_dict.graphicStyles);
    if (bool(style))
    {
      if (ident && !anonymous)
        m_currentStylesheet->graphicStyles[get(ident)] = style;
      if (!ref)
        m_newStyles.push_back(style);
    }

    if (m_layerOpened)
    {
      assert(!m_levelStack.empty());

      m_levelStack.top().graphicStyle = style;
    }
  }
}

void KNCollectorBase::collectLayoutStyle(const optional<ID_t> &id,
    const boost::optional<KNPropertyMap> &props,
    const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
    const bool ref, const bool anonymous)
{
  if (m_collecting)
  {
    assert(m_currentStylesheet);

    KNLayoutStylePtr_t newStyle;
    if (!ref && props)
      newStyle.reset(new KNLayoutStyle(get(props), ident, parentIdent));

    const KNLayoutStylePtr_t style = getValue(id, newStyle, ref, m_dict.layoutStyles);
    if (bool(style))
    {
      if (ident && !anonymous)
        m_currentStylesheet->layoutStyles[get(ident)] = style;
      if (!ref)
        m_newStyles.push_back(style);

      if (bool(m_currentText))
        m_currentText->setLayoutStyle(style);
    }
  }
}

void KNCollectorBase::collectListStyle(const boost::optional<ID_t> &id,
                                       const boost::optional<KNPropertyMap> &props,
                                       const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                       const bool ref, const bool anonymous)
{
  // TODO: implement me
  (void) id;
  (void) props;
  (void) ident;
  (void) parentIdent;
  (void) ref;
  (void) anonymous;
}

void KNCollectorBase::collectParagraphStyle(const optional<ID_t> &id,
    const boost::optional<KNPropertyMap> &props,
    const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
    const bool ref, const bool anonymous)
{
  if (m_collecting)
  {
    assert(m_currentStylesheet);

    KNParagraphStylePtr_t newStyle;
    if (!ref && props)
      newStyle.reset(new KNParagraphStyle(get(props), ident, parentIdent));

    const KNParagraphStylePtr_t style = getValue(id, newStyle, ref, m_dict.paragraphStyles);
    if (bool(style))
    {
      if (ident && !anonymous)
        m_currentStylesheet->paragraphStyles[get(ident)] = style;
      if (!ref)
        m_newStyles.push_back(style);
    }
  }
}

void KNCollectorBase::collectPlaceholderStyle(const boost::optional<ID_t> &id,
    const boost::optional<KNPropertyMap> &props,
    const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
    const bool ref, const bool anonymous)
{
  if (m_collecting)
  {
    assert(m_currentStylesheet);

    KNPlaceholderStylePtr_t newStyle;
    if (!ref && props)
      newStyle.reset(new KNPlaceholderStyle(get(props), ident, parentIdent));

    const KNPlaceholderStylePtr_t style = getValue(id, newStyle, ref, m_dict.placeholderStyles);
    if (bool(style))
    {
      if (ident && !anonymous)
        m_currentStylesheet->placeholderStyles[get(ident)] = style;
      if (!ref)
        m_newStyles.push_back(style);
    }

    m_currentPlaceholderStyle = style;
  }
}

void KNCollectorBase::collectSlideStyle(const boost::optional<ID_t> &id,
                                        const boost::optional<KNPropertyMap> &props,
                                        const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                        const bool ref, const bool anonymous)
{
  // TODO: implement me
  (void) id;
  (void) props;
  (void) ident;
  (void) parentIdent;
  (void) ref;
  (void) anonymous;
}

void KNCollectorBase::collectTabularStyle(const boost::optional<ID_t> &id,
    const boost::optional<KNPropertyMap> &props,
    const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
    const bool ref, const bool anonymous)
{
  // TODO: implement me
  (void) id;
  (void) props;
  (void) ident;
  (void) parentIdent;
  (void) ref;
  (void) anonymous;
}

void KNCollectorBase::collectVectorStyle(const boost::optional<ID_t> &id,
    const boost::optional<KNPropertyMap> &props,
    const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
    const bool ref, const bool anonymous)
{
  // TODO: implement me
  (void) id;
  (void) props;
  (void) ident;
  (void) parentIdent;
  (void) ref;
  (void) anonymous;
}

void KNCollectorBase::collectGeometry(boost::optional<ID_t> &,
                                      boost::optional<KNSize> &naturalSize, boost::optional<KNPosition> &position,
                                      boost::optional<double> &angle,
                                      boost::optional<double> &shearXAngle, boost::optional<double> &shearYAngle,
                                      boost::optional<bool> &horizontalFlip, boost::optional<bool> &verticalFlip,
                                      boost::optional<bool> &aspectRatioLocked, boost::optional<bool> &sizesLocked)
{
  if (m_collecting)
  {
    assert(!m_levelStack.empty());

    m_defaults.applyGeometry(naturalSize, position, angle, shearXAngle, shearYAngle, horizontalFlip, verticalFlip, aspectRatioLocked, sizesLocked);
    assert(naturalSize && position && angle && shearXAngle && shearYAngle && horizontalFlip && verticalFlip && aspectRatioLocked && sizesLocked);

    const KNGeometryPtr_t geometry(new KNGeometry);
    geometry->naturalSize = get(naturalSize);
    geometry->position = get(position);
    geometry->angle = get(angle);
    geometry->shearXAngle = get(shearXAngle);
    geometry->shearYAngle = get(shearYAngle);
    geometry->horizontalFlip = get(horizontalFlip);
    geometry->verticalFlip = get(verticalFlip);
    geometry->aspectRatioLocked = get(aspectRatioLocked);
    geometry->sizesLocked = get(sizesLocked);

    m_levelStack.top().geometry = geometry;
  }
}

void KNCollectorBase::collectBezier(const optional<ID_t> &id, const KNPathPtr_t &path, const bool ref)
{
  if (m_collecting)
    m_currentPath = getValue(id, path, ref, m_dict.beziers);
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
    assert(!m_levelStack.empty());

    image->geometry = m_levelStack.top().geometry;
    m_levelStack.top().geometry.reset();
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
    assert(!m_levelStack.empty());

    line->geometry = m_levelStack.top().geometry;
    m_levelStack.top().geometry.reset();
    m_objectsStack.top().push_back(makeObject(line));
  }
}

void KNCollectorBase::collectShape(const optional<ID_t> &)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());
    assert(!m_levelStack.empty());

    const KNShapePtr_t shape(new KNShape());

    if (!m_currentPath)
    {
      KN_DEBUG_MSG(("the path is empty\n"));
    }
    shape->path = m_currentPath;
    m_currentPath.reset();

    shape->geometry = m_levelStack.top().geometry;
    m_levelStack.top().geometry.reset();

    shape->text = m_currentText;
    m_currentText.reset();

    shape->style = m_levelStack.top().graphicStyle;
    m_levelStack.top().graphicStyle.reset();

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

void KNCollectorBase::collectData(const boost::optional<ID_t> &id, const WPXInputStreamPtr_t &stream, const boost::optional<std::string> &displayName, const boost::optional<unsigned> &type, const bool ref)
{
  // TODO: implement me
  (void) id;
  (void) stream;
  (void) displayName;
  (void) type;
  (void) ref;
}

void KNCollectorBase::collectUnfiltered(const boost::optional<ID_t> &id, const boost::optional<KNSize> &size, const bool ref)
{
  // TODO: implement me
  (void) id;
  (void) size;
  (void) ref;
}

void KNCollectorBase::collectFilteredImage(const boost::optional<ID_t> &id, const bool ref)
{
  // TODO: implement me
  (void) id;
  (void) ref;
}

void KNCollectorBase::collectMedia(const optional<ID_t> &)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());

    // FIXME: build media from collected parts
    // media->geometry = m_currentGeometry;
    // m_currentGeometry.reset();
    // m_objectsStack.top().push_back(makeObject(media));
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

void KNCollectorBase::collectStylesheet(const boost::optional<ID_t> &id, const boost::optional<ID_t> &parent)
{
  if (m_collecting)
  {
    assert(m_currentStylesheet);

    if (parent)
      m_currentStylesheet->parent = m_dict.stylesheets[get(parent)];

    if (id)
      m_dict.stylesheets[get(id)] = m_currentStylesheet;

    for_each(m_newStyles.begin(), m_newStyles.end(), boost::bind(&KNStyle::link, _1, m_currentStylesheet));

    m_currentStylesheet.reset(new KNStylesheet());
    m_newStyles.clear();
    m_currentPlaceholderStyle.reset();
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

void KNCollectorBase::collectTextPlaceholder(const optional<ID_t> &id, const bool title, const bool ref)
{
  if (m_collecting)
  {
    KNPlaceholderPtr_t placeholder;
    KNPlaceholderMap_t &placeholderMap = title ? m_dict.titlePlaceholders : m_dict.bodyPlaceholders;

    if (ref)
    {
      assert(!m_objectsStack.empty());
      if (id)
        placeholder = placeholderMap[get(id)];

      if (bool(placeholder))
      {
        m_objectsStack.top().push_back(makeObject(placeholder));
      }
      else
      {
        KN_DEBUG_MSG(("no text placeholder found\n"));
      }
    }
    else
    {
      assert(bool(m_currentText));

      placeholder.reset(new KNPlaceholder());
      placeholder->title = title;
      if (!m_currentText->empty())
        placeholder->text = m_currentText;
      placeholder->style = m_currentPlaceholderStyle;

      m_currentText.reset();
      m_currentPlaceholderStyle.reset();

      if (id)
        placeholderMap[get(id)] = placeholder;
    }
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

    startLevel();

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

    endLevel();

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

void KNCollectorBase::startText()
{
  if (m_collecting)
  {
    assert(!m_currentText);

    m_currentText.reset(new KNText());

    assert(m_currentText->empty());
  }
}

void KNCollectorBase::endText()
{
  if (m_collecting)
  {
    // text is reset at the place where it is used
    assert(!m_currentText || m_currentText->empty());

    m_currentText.reset();
  }
}

void KNCollectorBase::startLevel()
{
  if (m_collecting)
    m_levelStack.push(Level());
}

void KNCollectorBase::endLevel()
{
  if (m_collecting)
  {
    assert(!m_levelStack.empty());
    m_levelStack.pop();
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

const KNDefaults &KNCollectorBase::getDefaults() const
{
  return m_defaults;
}

const KNLayerPtr_t &KNCollectorBase::getLayer() const
{
  return m_currentLayer;
}

} // namespace libkeynote

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
