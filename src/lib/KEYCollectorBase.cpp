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
#include "IWORKShape.h"
#include "IWORKText.h"
#include "IWORKTransformation.h"
#include "KEYCollectorBase.h"
#include "KEYDictionary.h"
#include "KEYStyles.h"

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
  : m_geometry()
  , m_graphicStyle()
  , m_trafo()
{
}

KEYCollectorBase::KEYCollectorBase(KEYDictionary &dict)
  : m_dict(dict)
  , m_currentLayer()
  , m_levelStack()
  , m_objectsStack()
  , m_currentPath()
  , m_currentText()
  , m_currentStylesheet(new IWORKStylesheet())
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

    IWORKStylePtr_t newStyle;
    // TODO: handle default style properties
    if (!ref && props)
      newStyle.reset(new IWORKStyle(get(props), ident, parentIdent));

    const IWORKStylePtr_t style = getValue(id, newStyle, ref, m_dict.m_characterStyles);
    if (bool(style))
    {
      if (ident && !anonymous)
        m_currentStylesheet->m_styles[get(ident)] = style;
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

    IWORKStylePtr_t newStyle;
    if (!ref && props)
      newStyle.reset(new IWORKStyle(get(props), ident, parentIdent));

    const IWORKStylePtr_t style = getValue(id, newStyle, ref, m_dict.m_graphicStyles);
    if (bool(style))
    {
      if (ident && !anonymous)
        m_currentStylesheet->m_styles[get(ident)] = style;
      if (!ref)
        m_newStyles.push_back(style);
    }

    if (m_layerOpened)
    {
      assert(!m_levelStack.empty());

      m_levelStack.top().m_graphicStyle = style;
      m_styleStack.set(style);
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

    IWORKStylePtr_t newStyle;
    if (!ref && props)
      newStyle.reset(new IWORKStyle(get(props), ident, parentIdent));

    const IWORKStylePtr_t style = getValue(id, newStyle, ref, m_dict.m_layoutStyles);
    if (bool(style))
    {
      if (ident && !anonymous)
        m_currentStylesheet->m_styles[get(ident)] = style;
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

    IWORKStylePtr_t newStyle;
    if (!ref && props)
      newStyle.reset(new IWORKStyle(get(props), ident, parentIdent));

    const IWORKStylePtr_t style = getValue(id, newStyle, ref, m_dict.m_paragraphStyles);
    if (bool(style))
    {
      if (ident && !anonymous)
        m_currentStylesheet->m_styles[get(ident)] = style;
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

    IWORKStylePtr_t newStyle;
    if (!ref && props)
      newStyle.reset(new IWORKStyle(get(props), ident, parentIdent));

    const IWORKStylePtr_t style = getValue(id, newStyle, ref, m_dict.m_placeholderStyles);
    if (bool(style))
    {
      if (ident && !anonymous)
        m_currentStylesheet->m_styles[get(ident)] = style;
      if (!ref)
        m_newStyles.push_back(style);
    }
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

void KEYCollectorBase::collectGeometry(const boost::optional<ID_t> &,
                                       boost::optional<IWORKSize> &naturalSize, boost::optional<IWORKSize> &size,
                                       boost::optional<IWORKPosition> &position, boost::optional<double> &angle,
                                       boost::optional<double> &shearXAngle, boost::optional<double> &shearYAngle,
                                       boost::optional<bool> &horizontalFlip, boost::optional<bool> &verticalFlip,
                                       boost::optional<bool> &aspectRatioLocked, boost::optional<bool> &sizesLocked)
{
  if (m_collecting)
  {
    assert(!m_levelStack.empty());

    assert(naturalSize && position);

    const IWORKGeometryPtr_t geometry(new IWORKGeometry);
    geometry->m_naturalSize = get(naturalSize);
    geometry->m_size = bool(size) ? get(size) : get(naturalSize);
    geometry->m_position = get(position);
    geometry->m_angle = angle;
    geometry->m_shearXAngle = shearXAngle;
    geometry->m_shearYAngle = shearYAngle;
    geometry->m_horizontalFlip = horizontalFlip;
    geometry->m_verticalFlip = verticalFlip;
    geometry->m_aspectRatioLocked = aspectRatioLocked;
    geometry->m_sizesLocked = sizesLocked;

    m_levelStack.top().m_geometry = geometry;
    m_levelStack.top().m_trafo = makeTransformation(*geometry) * m_levelStack.top().m_trafo;
  }
}

void KEYCollectorBase::collectBezier(const optional<ID_t> &id, const IWORKPathPtr_t &path, const bool ref)
{
  if (m_collecting)
    m_currentPath = getValue(id, path, ref, m_dict.m_beziers);
}

void KEYCollectorBase::collectGroup(const optional<ID_t> &, const IWORKGroupPtr_t &group)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());

    group->m_objects = m_objectsStack.top();
    m_objectsStack.pop();
    assert(!m_objectsStack.empty());
    m_objectsStack.top().push_back(makeObject(group));
  }
}

void KEYCollectorBase::collectImage(const optional<ID_t> &id, const IWORKImagePtr_t &image)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());
    assert(!m_levelStack.empty());

    image->m_geometry = m_levelStack.top().m_geometry;
    m_levelStack.top().m_geometry.reset();
    if (id)
      m_dict.m_images[get(id)] = image;
    m_objectsStack.top().push_back(makeObject(image, m_levelStack.top().m_trafo));
  }
}

void KEYCollectorBase::collectLine(const optional<ID_t> &, const IWORKLinePtr_t &line)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());
    assert(!m_levelStack.empty());

    line->m_geometry = m_levelStack.top().m_geometry;
    m_levelStack.top().m_geometry.reset();
    m_objectsStack.top().push_back(makeObject(line, m_levelStack.top().m_trafo));
  }
}

void KEYCollectorBase::collectShape(const optional<ID_t> &)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());
    assert(!m_levelStack.empty());

    const IWORKShapePtr_t shape(new IWORKShape());

    if (!m_currentPath)
    {
      ETONYEK_DEBUG_MSG(("the path is empty\n"));
    }
    shape->m_path = m_currentPath;
    m_currentPath.reset();

    shape->m_geometry = m_levelStack.top().m_geometry;
    m_levelStack.top().m_geometry.reset();

    if (bool(m_currentText))
    {
      m_currentText->setBoundingBox(shape->m_geometry);
      shape->m_text = m_currentText;
      m_currentText.reset();
    }

    shape->m_style = m_levelStack.top().m_graphicStyle;
    m_levelStack.top().m_graphicStyle.reset();

    m_objectsStack.top().push_back(makeObject(shape, m_levelStack.top().m_trafo));
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
    IWORKDataPtr_t newData;

    if (!ref)
    {
      newData.reset(new IWORKData());
      newData->m_stream = stream;
      newData->m_displayName = displayName;
      newData->m_type = type;
    }

    assert(!m_currentData);
    m_currentData = getValue(id, newData, ref, m_dict.m_data);
  }
}

void KEYCollectorBase::collectUnfiltered(const boost::optional<ID_t> &id, const boost::optional<IWORKSize> &size, const bool ref)
{
  if (m_collecting)
  {
    IWORKMediaContentPtr_t newUnfiltered;

    if (!ref)
    {
      newUnfiltered.reset(new IWORKMediaContent());
      newUnfiltered->m_size = size;
      newUnfiltered->m_data = m_currentData;

      m_currentData.reset();
    }

    assert(!m_currentUnfiltered);
    m_currentUnfiltered = getValue(id, newUnfiltered, ref, m_dict.m_unfiltereds);
  }
}

void KEYCollectorBase::collectFiltered(const boost::optional<ID_t> &, const boost::optional<IWORKSize> &size)
{
  if (m_collecting)
  {
    const IWORKMediaContentPtr_t newFiltered(new IWORKMediaContent());
    newFiltered->m_size = size;
    newFiltered->m_data = m_currentData;

    m_currentData.reset();

    assert(!m_currentFiltered);
    m_currentFiltered = newFiltered;
  }
}

void KEYCollectorBase::collectLeveled(const boost::optional<ID_t> &, const boost::optional<IWORKSize> &size)
{
  if (m_collecting)
  {
    const IWORKMediaContentPtr_t newLeveled(new IWORKMediaContent());
    newLeveled->m_size = size;
    newLeveled->m_data = m_currentData;

    m_currentData.reset();

    assert(!m_currentLeveled);
    m_currentLeveled = newLeveled;
  }
}

void KEYCollectorBase::collectFilteredImage(const boost::optional<ID_t> &id, const bool ref)
{
  if (m_collecting)
  {
    IWORKMediaContentPtr_t newFilteredImage;

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
    m_currentContent = getValue(id, newFilteredImage, ref, m_dict.m_filteredImages);
  }
}

void KEYCollectorBase::collectMovieMedia(const boost::optional<ID_t> &)
{
  if (m_collecting)
  {
    assert(m_currentData);

    const IWORKMediaContentPtr_t newContent(new IWORKMediaContent());
    newContent->m_data = m_currentData;
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

    const IWORKMediaPtr_t media(new IWORKMedia());
    media->m_geometry = m_levelStack.top().m_geometry;
    media->m_style = m_levelStack.top().m_graphicStyle;
    media->m_content = m_currentContent;

    m_currentContent.reset();
    m_levelStack.top().m_geometry.reset();
    m_levelStack.top().m_graphicStyle.reset();

    m_objectsStack.top().push_back(makeObject(media, m_levelStack.top().m_trafo));
  }
}

void KEYCollectorBase::collectLayer(const optional<ID_t> &, bool)
{
  if (m_collecting)
  {
    assert(m_layerOpened);
    assert(!m_objectsStack.empty());

    m_currentLayer.reset(new KEYLayer());
    m_currentLayer->m_objects = m_objectsStack.top();
    m_objectsStack.pop();
  }
}

void KEYCollectorBase::collectStylesheet(const boost::optional<ID_t> &id, const boost::optional<ID_t> &parent)
{
  if (m_collecting)
  {
    assert(m_currentStylesheet);

    if (parent)
      m_currentStylesheet->parent = m_dict.m_stylesheets[get(parent)];

    if (id)
      m_dict.m_stylesheets[get(id)] = m_currentStylesheet;

    for_each(m_newStyles.begin(), m_newStyles.end(), boost::bind(&IWORKStyle::link, _1, m_currentStylesheet));

    m_currentStylesheet.reset(new IWORKStylesheet());
    m_newStyles.clear();
  }
}

void KEYCollectorBase::collectText(const optional<ID_t> &style, const std::string &text)
{
  if (m_collecting)
  {
    assert(bool(m_currentText));

    m_currentText->insertText(text, getValue(style, m_dict.m_characterStyles));
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

void KEYCollectorBase::collectTextPlaceholder(const optional<ID_t> &id, const optional<ID_t> &style, const bool title, const bool ref)
{
  if (m_collecting)
  {
    KEYPlaceholderPtr_t placeholder;
    KEYPlaceholderMap_t &placeholderMap = title ? m_dict.m_titlePlaceholders : m_dict.m_bodyPlaceholders;

    if (ref)
    {
      assert(!m_objectsStack.empty());
      if (id)
        placeholder = placeholderMap[get(id)];

      if (bool(placeholder))
      {
        IWORKTransformation trafo;
        if (bool(placeholder->m_geometry))
          trafo = makeTransformation(*placeholder->m_geometry);
        m_objectsStack.top().push_back(makeObject(placeholder, trafo * m_levelStack.top().m_trafo));
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
      placeholder->m_title = title;
      placeholder->m_style = getValue(style, m_dict.m_placeholderStyles);
      if (bool(placeholder->m_style))
      {
        const KEYPlaceholderStyle placeholderStyle(placeholder->m_style, m_styleStack);
        placeholder->m_geometry = placeholderStyle.getGeometry();
      }
      if (!m_currentText->empty())
      {
        m_currentText->setBoundingBox(placeholder->m_geometry);
        placeholder->m_text = m_currentText;
      }

      m_currentText.reset();

      if (id)
        placeholderMap[get(id)] = placeholder;
    }
  }
}

void KEYCollectorBase::collectTableSizes(const IWORKTable::RowSizes_t &rowSizes, const IWORKTable::ColumnSizes_t &columnSizes)
{
  if (m_collecting)
    m_currentTable.setSizes(columnSizes, rowSizes);
}

void KEYCollectorBase::collectTableCell(const unsigned row, const unsigned column, const boost::optional<std::string> &content, const unsigned rowSpan, const unsigned columnSpan)
{
  if (m_collecting)
  {
    IWORKObjectPtr_t textObject;

    if (bool(content))
    {
      assert(!m_currentText || m_currentText->empty());

      IWORKTextPtr_t text(new IWORKText(false));
      text->openParagraph(IWORKStylePtr_t());
      text->insertText(get(content), IWORKStylePtr_t());
      text->closeParagraph();

      textObject = makeObject(text, m_levelStack.top().m_trafo);
    }
    else if (bool(m_currentText))
    {
      textObject = makeObject(m_currentText, m_levelStack.top().m_trafo);
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

    m_currentTable.setGeometry(m_levelStack.top().m_geometry);
    m_levelStack.top().m_geometry.reset();

    m_objectsStack.top().push_back(makeObject(m_currentTable, m_levelStack.top().m_trafo));
    m_currentTable = IWORKTable();
  }
}

void KEYCollectorBase::collectNote()
{
  if (m_collecting)
  {
    m_notes.push_back(makeObject(m_currentText, m_levelStack.top().m_trafo));
    m_currentText.reset();
  }
}

void KEYCollectorBase::collectStickyNote()
{
  if (m_collecting)
  {
    assert(!m_levelStack.empty());

    m_stickyNotes.push_back(KEYStickyNote(m_levelStack.top().m_geometry, m_currentText));
    m_levelStack.top().m_geometry.reset();
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

    m_objectsStack.push(IWORKObjectList_t());
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

    m_objectsStack.push(IWORKObjectList_t());
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

    m_currentText->openParagraph(getValue(style, m_dict.m_paragraphStyles));
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

    m_currentText.reset(new IWORKText(object));

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
  {
    IWORKTransformation currentTrafo;
    if (!m_levelStack.empty())
      currentTrafo = m_levelStack.top().m_trafo;
    m_levelStack.push(Level());
    m_levelStack.top().m_trafo = currentTrafo;

    pushStyle();
  }
}

void KEYCollectorBase::endLevel()
{
  if (m_collecting)
  {
    assert(!m_levelStack.empty());
    m_levelStack.pop();

    popStyle();
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

const KEYLayerPtr_t &KEYCollectorBase::getLayer() const
{
  return m_currentLayer;
}

const IWORKObjectList_t &KEYCollectorBase::getNotes() const
{
  return m_notes;
}

const KEYStickyNotes_t &KEYCollectorBase::getStickyNotes() const
{
  return m_stickyNotes;
}

const IWORKTransformation &KEYCollectorBase::getTransformation() const
{
  assert(!m_levelStack.empty());

  return m_levelStack.top().m_trafo;
}

void KEYCollectorBase::pushStyle()
{
  m_styleStack.push();
}

void KEYCollectorBase::popStyle()
{
  m_styleStack.pop();
}

} // namespace libetonyek

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
