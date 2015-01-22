/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>
#include <cassert>

#include <boost/bind.hpp>

#include "libetonyek_utils.h"
#include "KEYCollectorBase.h"
#include "KEYDefaults.h"
#include "KEYDictionary.h"
#include "KEYShape.h"
#include "KEYStyles.h"
#include "KEYText.h"

using boost::optional;

namespace libetonyek
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
    if (bool(id))
    {
      const typename boost::unordered_map<ID_t, T>::const_iterator it = map.find(get(id));
      if (map.end() != it)
      {
        retval = it->second;
      }
      else
      {
        ETONYEK_DEBUG_MSG(("item with ID %s does not exist\n", get(id).c_str()));
      }
    }
    else
    {
      ETONYEK_DEBUG_MSG(("reference without ID\n"));
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

KEYCollectorBase::Level::Level()
  : geometry()
  , graphicStyle()
{
}

KEYCollectorBase::KEYCollectorBase(KEYDictionary &dict, const KEYDefaults &defaults)
  : m_dict(dict)
  , m_defaults(defaults)
  , m_currentLayer()
  , m_levelStack()
  , m_objectsStack()
  , m_currentPath()
  , m_currentText()
  , m_currentPlaceholderStyle()
  , m_currentStylesheet(new KEYStylesheet())
  , m_newStyles()
  , m_currentData()
  , m_currentUnfiltered()
  , m_currentFiltered()
  , m_currentLeveled()
  , m_currentContent()
  , m_currentTable()
  , m_notes()
  , m_collecting(false)
  , m_layerOpened(false)
  , m_groupLevel(0)
{
}

KEYCollectorBase::~KEYCollectorBase()
{
  assert(!m_collecting);
  assert(m_objectsStack.empty());
  assert(m_levelStack.empty());
  assert(!m_layerOpened);
  assert(0 == m_groupLevel);

  assert(!m_currentPath);
  assert(!m_currentText);
  assert(!m_currentPlaceholderStyle);
}

void KEYCollectorBase::collectCellStyle(const boost::optional<ID_t> &id,
                                        const boost::optional<IWORKPropertyMap> &props,
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

void KEYCollectorBase::collectCharacterStyle(const optional<ID_t> &id,
                                             const boost::optional<IWORKPropertyMap> &props,
                                             const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                             const bool ref, const bool anonymous)
{
  if (m_collecting)
  {
    assert(m_currentStylesheet);

    KEYCharacterStylePtr_t newStyle;
    // TODO: handle default style properties
    if (!ref && props)
      newStyle.reset(new KEYCharacterStyle(get(props), ident, parentIdent));

    const KEYCharacterStylePtr_t style = getValue(id, newStyle, ref, m_dict.characterStyles);
    if (bool(style))
    {
      if (ident && !anonymous)
        m_currentStylesheet->characterStyles[get(ident)] = style;
      if (!ref)
        m_newStyles.push_back(style);
    }
  }
}

void KEYCollectorBase::collectConnectionStyle(const boost::optional<ID_t> &id,
                                              const boost::optional<IWORKPropertyMap> &props,
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

void KEYCollectorBase::collectGraphicStyle(const optional<ID_t> &id,
                                           const boost::optional<IWORKPropertyMap> &props,
                                           const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                           const bool ref, const bool anonymous)
{
  if (m_collecting)
  {
    assert(m_currentStylesheet);

    KEYGraphicStylePtr_t newStyle;
    if (!ref && props)
      newStyle.reset(new KEYGraphicStyle(get(props), ident, parentIdent));

    const KEYGraphicStylePtr_t style = getValue(id, newStyle, ref, m_dict.graphicStyles);
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

void KEYCollectorBase::collectLayoutStyle(const optional<ID_t> &id,
                                          const boost::optional<IWORKPropertyMap> &props,
                                          const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                          const bool ref, const bool anonymous)
{
  if (m_collecting)
  {
    assert(m_currentStylesheet);

    KEYLayoutStylePtr_t newStyle;
    if (!ref && props)
      newStyle.reset(new KEYLayoutStyle(get(props), ident, parentIdent));

    const KEYLayoutStylePtr_t style = getValue(id, newStyle, ref, m_dict.layoutStyles);
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

void KEYCollectorBase::collectListStyle(const boost::optional<ID_t> &id,
                                        const boost::optional<IWORKPropertyMap> &props,
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

void KEYCollectorBase::collectParagraphStyle(const optional<ID_t> &id,
                                             const boost::optional<IWORKPropertyMap> &props,
                                             const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                             const bool ref, const bool anonymous)
{
  if (m_collecting)
  {
    assert(m_currentStylesheet);

    KEYParagraphStylePtr_t newStyle;
    if (!ref && props)
      newStyle.reset(new KEYParagraphStyle(get(props), ident, parentIdent));

    const KEYParagraphStylePtr_t style = getValue(id, newStyle, ref, m_dict.paragraphStyles);
    if (bool(style))
    {
      if (ident && !anonymous)
        m_currentStylesheet->paragraphStyles[get(ident)] = style;
      if (!ref)
        m_newStyles.push_back(style);
    }
  }
}

void KEYCollectorBase::collectPlaceholderStyle(const boost::optional<ID_t> &id,
                                               const boost::optional<IWORKPropertyMap> &props,
                                               const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                               const bool ref, const bool anonymous)
{
  if (m_collecting)
  {
    assert(m_currentStylesheet);

    KEYPlaceholderStylePtr_t newStyle;
    if (!ref && props)
      newStyle.reset(new KEYPlaceholderStyle(get(props), ident, parentIdent));

    const KEYPlaceholderStylePtr_t style = getValue(id, newStyle, ref, m_dict.placeholderStyles);
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

void KEYCollectorBase::collectSlideStyle(const boost::optional<ID_t> &id,
                                         const boost::optional<IWORKPropertyMap> &props,
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

void KEYCollectorBase::collectTabularStyle(const boost::optional<ID_t> &id,
                                           const boost::optional<IWORKPropertyMap> &props,
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

void KEYCollectorBase::collectVectorStyle(const boost::optional<ID_t> &id,
                                          const boost::optional<IWORKPropertyMap> &props,
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

void KEYCollectorBase::collectGeometry(boost::optional<ID_t> &,
                                       boost::optional<IWORKSize> &naturalSize, boost::optional<IWORKSize> &size,
                                       boost::optional<IWORKPosition> &position, boost::optional<double> &angle,
                                       boost::optional<double> &shearXAngle, boost::optional<double> &shearYAngle,
                                       boost::optional<bool> &horizontalFlip, boost::optional<bool> &verticalFlip,
                                       boost::optional<bool> &aspectRatioLocked, boost::optional<bool> &sizesLocked)
{
  if (m_collecting)
  {
    assert(!m_levelStack.empty());

    m_defaults.applyGeometry(naturalSize, position);
    assert(naturalSize && position);

    const IWORKGeometryPtr_t geometry(new IWORKGeometry);
    geometry->naturalSize = get(naturalSize);
    geometry->size = bool(size) ? get(size) : get(naturalSize);
    geometry->position = get(position);
    geometry->angle = angle;
    geometry->shearXAngle = shearXAngle;
    geometry->shearYAngle = shearYAngle;
    geometry->horizontalFlip = horizontalFlip;
    geometry->verticalFlip = verticalFlip;
    geometry->aspectRatioLocked = aspectRatioLocked;
    geometry->sizesLocked = sizesLocked;

    m_levelStack.top().geometry = geometry;
  }
}

void KEYCollectorBase::collectBezier(const optional<ID_t> &id, const IWORKPathPtr_t &path, const bool ref)
{
  if (m_collecting)
    m_currentPath = getValue(id, path, ref, m_dict.beziers);
}

void KEYCollectorBase::collectGroup(const optional<ID_t> &, const KEYGroupPtr_t &group)
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

void KEYCollectorBase::collectImage(const optional<ID_t> &id, const KEYImagePtr_t &image)
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

void KEYCollectorBase::collectLine(const optional<ID_t> &, const KEYLinePtr_t &line)
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

void KEYCollectorBase::collectShape(const optional<ID_t> &)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());
    assert(!m_levelStack.empty());

    const KEYShapePtr_t shape(new KEYShape());

    if (!m_currentPath)
    {
      ETONYEK_DEBUG_MSG(("the path is empty\n"));
    }
    shape->path = m_currentPath;
    m_currentPath.reset();

    shape->geometry = m_levelStack.top().geometry;
    m_levelStack.top().geometry.reset();

    if (bool(m_currentText))
    {
      m_currentText->setBoundingBox(shape->geometry);
      shape->text = m_currentText;
      m_currentText.reset();
    }

    shape->style = m_levelStack.top().graphicStyle;
    m_levelStack.top().graphicStyle.reset();

    m_objectsStack.top().push_back(makeObject(shape));
  }
}

void KEYCollectorBase::collectBezierPath(const optional<ID_t> &)
{
  // nothing needed
}

void KEYCollectorBase::collectPolygonPath(const optional<ID_t> &, const IWORKSize &size, const unsigned edges)
{
  if (m_collecting)
    m_currentPath = makePolygonPath(size, edges);
}

void KEYCollectorBase::collectRoundedRectanglePath(const optional<ID_t> &, const IWORKSize &size, const double radius)
{
  if (m_collecting)
    m_currentPath = makeRoundedRectanglePath(size, radius);
}

void KEYCollectorBase::collectArrowPath(const optional<ID_t> &, const IWORKSize &size, const double headWidth, const double stemRelYPos, bool const doubleSided)
{
  if (m_collecting)
  {
    if (doubleSided)
      m_currentPath = makeDoubleArrowPath(size, headWidth, stemRelYPos);
    else
      m_currentPath = makeArrowPath(size, headWidth, stemRelYPos);
  }
}

void KEYCollectorBase::collectStarPath(const optional<ID_t> &, const IWORKSize &size, const unsigned points, const double innerRadius)
{
  if (m_collecting)
    m_currentPath = makeStarPath(size, points, innerRadius);
}

void KEYCollectorBase::collectConnectionPath(const optional<ID_t> &, const IWORKSize &size, const double middleX, const double middleY)
{
  if (m_collecting)
    m_currentPath = makeConnectionPath(size, middleX, middleY);
}

void KEYCollectorBase::collectCalloutPath(const optional<ID_t> &, const IWORKSize &size, const double radius, const double tailSize, const double tailX, const double tailY, bool quoteBubble)
{
  if (m_collecting)
  {
    if (quoteBubble)
      m_currentPath = makeQuoteBubblePath(size, radius, tailSize, tailX, tailY);
    else
      m_currentPath = makeCalloutPath(size, radius, tailSize, tailX, tailY);
  }
}

void KEYCollectorBase::collectData(const boost::optional<ID_t> &id, const RVNGInputStreamPtr_t &stream, const boost::optional<std::string> &displayName, const boost::optional<unsigned> &type, const bool ref)
{
  if (m_collecting)
  {
    KEYDataPtr_t newData;

    if (!ref)
    {
      newData.reset(new KEYData());
      newData->stream = stream;
      newData->displayName = displayName;
      newData->type = type;
    }

    assert(!m_currentData);
    m_currentData = getValue(id, newData, ref, m_dict.data);
  }
}

void KEYCollectorBase::collectUnfiltered(const boost::optional<ID_t> &id, const boost::optional<IWORKSize> &size, const bool ref)
{
  if (m_collecting)
  {
    KEYMediaContentPtr_t newUnfiltered;

    if (!ref)
    {
      newUnfiltered.reset(new KEYMediaContent());
      newUnfiltered->size = size;
      newUnfiltered->data = m_currentData;

      m_currentData.reset();
    }

    assert(!m_currentUnfiltered);
    m_currentUnfiltered = getValue(id, newUnfiltered, ref, m_dict.unfiltereds);
  }
}

void KEYCollectorBase::collectFiltered(const boost::optional<ID_t> &, const boost::optional<IWORKSize> &size)
{
  if (m_collecting)
  {
    const KEYMediaContentPtr_t newFiltered(new KEYMediaContent());
    newFiltered->size = size;
    newFiltered->data = m_currentData;

    m_currentData.reset();

    assert(!m_currentFiltered);
    m_currentFiltered = newFiltered;
  }
}

void KEYCollectorBase::collectLeveled(const boost::optional<ID_t> &, const boost::optional<IWORKSize> &size)
{
  if (m_collecting)
  {
    const KEYMediaContentPtr_t newLeveled(new KEYMediaContent());
    newLeveled->size = size;
    newLeveled->data = m_currentData;

    m_currentData.reset();

    assert(!m_currentLeveled);
    m_currentLeveled = newLeveled;
  }
}

void KEYCollectorBase::collectFilteredImage(const boost::optional<ID_t> &id, const bool ref)
{
  if (m_collecting)
  {
    KEYMediaContentPtr_t newFilteredImage;

    if (!ref)
    {
      // If a filter is applied to an image, the new image is saved next
      // to the original. So all we need is to pick the right one. We
      // can happily ignore the whole filter-properties section :-)
      // NOTE: Leveled is apparently used to save the result of using
      // the "Enhance" button.
      if (bool(m_currentFiltered))
        newFilteredImage = m_currentFiltered;
      else if (bool(m_currentLeveled))
        newFilteredImage = m_currentLeveled;
      else
        newFilteredImage = m_currentUnfiltered;

      m_currentFiltered.reset();
      m_currentLeveled.reset();
      m_currentUnfiltered.reset();
    }

    assert(!m_currentContent);
    m_currentContent = getValue(id, newFilteredImage, ref, m_dict.filteredImages);
  }
}

void KEYCollectorBase::collectMovieMedia(const boost::optional<ID_t> &)
{
  if (m_collecting)
  {
    assert(m_currentData);

    const KEYMediaContentPtr_t newContent(new KEYMediaContent());
    newContent->data = m_currentData;
    m_currentData.reset();

    assert(!m_currentContent);
    m_currentContent = newContent;
  }
}

void KEYCollectorBase::collectMedia(const optional<ID_t> &)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());
    assert(!m_levelStack.empty());

    const KEYMediaPtr_t media(new KEYMedia());
    media->geometry = m_levelStack.top().geometry;
    media->style = m_levelStack.top().graphicStyle;
    media->content = m_currentContent;

    m_currentContent.reset();
    m_levelStack.top().geometry.reset();
    m_levelStack.top().graphicStyle.reset();

    m_objectsStack.top().push_back(makeObject(media));
  }
}

void KEYCollectorBase::collectLayer(const optional<ID_t> &, bool)
{
  if (m_collecting)
  {
    assert(m_layerOpened);
    assert(!m_objectsStack.empty());

    m_currentLayer.reset(new KEYLayer());
    m_currentLayer->objects = m_objectsStack.top();
    m_objectsStack.pop();
  }
}

void KEYCollectorBase::collectStylesheet(const boost::optional<ID_t> &id, const boost::optional<ID_t> &parent)
{
  if (m_collecting)
  {
    assert(m_currentStylesheet);

    if (parent)
      m_currentStylesheet->parent = m_dict.stylesheets[get(parent)];

    if (id)
      m_dict.stylesheets[get(id)] = m_currentStylesheet;

    for_each(m_newStyles.begin(), m_newStyles.end(), boost::bind(&KEYStyle::link, _1, m_currentStylesheet));

    m_currentStylesheet.reset(new KEYStylesheet());
    m_newStyles.clear();
    m_currentPlaceholderStyle.reset();
  }
}

void KEYCollectorBase::collectText(const optional<ID_t> &style, const std::string &text)
{
  if (m_collecting)
  {
    assert(bool(m_currentText));

    m_currentText->insertText(text, getValue(style, m_dict.characterStyles));
  }
}

void KEYCollectorBase::collectTab()
{
  if (m_collecting)
  {
    assert(bool(m_currentText));

    m_currentText->insertTab();
  }
}

void KEYCollectorBase::collectLineBreak()
{
  if (m_collecting)
  {
    assert(bool(m_currentText));

    m_currentText->insertLineBreak();
  }
}

void KEYCollectorBase::collectTextPlaceholder(const optional<ID_t> &id, const bool title, const bool ref)
{
  if (m_collecting)
  {
    KEYPlaceholderPtr_t placeholder;
    KEYPlaceholderMap_t &placeholderMap = title ? m_dict.titlePlaceholders : m_dict.bodyPlaceholders;

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
        ETONYEK_DEBUG_MSG(("no text placeholder found\n"));
      }
    }
    else
    {
      assert(bool(m_currentText));

      placeholder.reset(new KEYPlaceholder());
      placeholder->title = title;
      if (bool(m_currentPlaceholderStyle))
        placeholder->geometry = m_currentPlaceholderStyle->getGeometry();
      if (!m_currentText->empty())
      {
        m_currentText->setBoundingBox(placeholder->geometry);
        placeholder->text = m_currentText;
      }
      placeholder->style = m_currentPlaceholderStyle;

      m_currentText.reset();
      m_currentPlaceholderStyle.reset();

      if (id)
        placeholderMap[get(id)] = placeholder;
    }
  }
}

void KEYCollectorBase::collectTableSizes(const KEYTable::RowSizes_t &rowSizes, const KEYTable::ColumnSizes_t &columnSizes)
{
  if (m_collecting)
    m_currentTable.setSizes(columnSizes, rowSizes);
}

void KEYCollectorBase::collectTableCell(const unsigned row, const unsigned column, const boost::optional<std::string> &content, const unsigned rowSpan, const unsigned columnSpan)
{
  if (m_collecting)
  {
    KEYObjectPtr_t textObject;

    if (bool(content))
    {
      assert(!m_currentText || m_currentText->empty());

      KEYTextPtr_t text(new KEYText());
      text->openParagraph(KEYParagraphStylePtr_t());
      text->insertText(get(content), KEYCharacterStylePtr_t());
      text->closeParagraph();

      textObject = makeObject(text);
    }
    else if (bool(m_currentText))
    {
      textObject = makeObject(m_currentText);
      m_currentText.reset();
    }

    m_currentTable.insertCell(column, row, textObject, columnSpan, rowSpan);
  }
}

void KEYCollectorBase::collectCoveredTableCell(const unsigned row, const unsigned column)
{
  if (m_collecting)
    m_currentTable.insertCoveredCell(column, row);
}

void KEYCollectorBase::collectTableRow()
{
  // nothing needed
}

void KEYCollectorBase::collectTable()
{
  if (m_collecting)
  {
    assert(!m_levelStack.empty());
    assert(!m_objectsStack.empty());

    m_currentTable.setGeometry(m_levelStack.top().geometry);
    m_levelStack.top().geometry.reset();

    m_objectsStack.top().push_back(makeObject(m_currentTable));
    m_currentTable = KEYTable();
  }
}

void KEYCollectorBase::collectNote()
{
  if (m_collecting)
  {
    m_notes.push_back(makeObject(m_currentText));
    m_currentText.reset();
  }
}

void KEYCollectorBase::collectStickyNote()
{
  if (m_collecting)
  {
    assert(!m_levelStack.empty());

    m_stickyNotes.push_back(KEYStickyNote(m_levelStack.top().geometry, m_currentText));
    m_levelStack.top().geometry.reset();
    m_currentText.reset();
  }
}

void KEYCollectorBase::startPage()
{
  assert(m_notes.empty());
  assert(m_stickyNotes.empty());
}

void KEYCollectorBase::endPage()
{
  m_notes.clear();
  m_stickyNotes.clear();
}

void KEYCollectorBase::startLayer()
{
  if (m_collecting)
  {
    assert(!m_layerOpened);
    assert(m_objectsStack.empty());
    assert(!m_currentLayer);

    m_objectsStack.push(KEYObjectList_t());
    m_layerOpened = true;

    startLevel();

    assert(!m_objectsStack.empty());
  }
}

void KEYCollectorBase::endLayer()
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

void KEYCollectorBase::startGroup()
{
  if (m_collecting)
  {
    assert(m_layerOpened);
    assert(!m_objectsStack.empty());

    m_objectsStack.push(KEYObjectList_t());
    ++m_groupLevel;
  }
}

void KEYCollectorBase::endGroup()
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

void KEYCollectorBase::startParagraph(const optional<ID_t> &style)
{
  if (m_collecting)
  {
    assert(bool(m_currentText));

    m_currentText->openParagraph(getValue(style, m_dict.paragraphStyles));
  }
}

void KEYCollectorBase::endParagraph()
{
  if (m_collecting)
  {
    assert(bool(m_currentText));

    m_currentText->closeParagraph();
  }
}

void KEYCollectorBase::startText(const bool object)
{
  if (m_collecting)
  {
    assert(!m_currentText);

    m_currentText.reset(new KEYText(object));

    assert(m_currentText->empty());
  }
}

void KEYCollectorBase::endText()
{
  if (m_collecting)
  {
    // text is reset at the place where it is used
    assert(!m_currentText || m_currentText->empty());

    m_currentText.reset();
  }
}

void KEYCollectorBase::startLevel()
{
  if (m_collecting)
    m_levelStack.push(Level());
}

void KEYCollectorBase::endLevel()
{
  if (m_collecting)
  {
    assert(!m_levelStack.empty());
    m_levelStack.pop();
  }
}

bool KEYCollectorBase::isCollecting() const
{
  return m_collecting;
}

void KEYCollectorBase::setCollecting(bool collecting)
{
  m_collecting = collecting;
}

const KEYDefaults &KEYCollectorBase::getDefaults() const
{
  return m_defaults;
}

const KEYLayerPtr_t &KEYCollectorBase::getLayer() const
{
  return m_currentLayer;
}

const KEYObjectList_t &KEYCollectorBase::getNotes() const
{
  return m_notes;
}

const KEYStickyNotes_t &KEYCollectorBase::getStickyNotes() const
{
  return m_stickyNotes;
}

} // namespace libetonyek

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
