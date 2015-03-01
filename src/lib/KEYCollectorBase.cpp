/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYCollectorBase.h"

#include <algorithm>
#include <cassert>

#include <boost/bind.hpp>

#include "libetonyek_utils.h"
#include "IWORKShape.h"
#include "IWORKText.h"
#include "IWORKTransformation.h"
#include "KEYDictionary.h"
#include "KEYStyles.h"

using boost::optional;

namespace libetonyek
{

KEYCollectorBase::Level::Level()
  : m_geometry()
  , m_graphicStyle()
  , m_trafo()
{
}

KEYCollectorBase::KEYCollectorBase()
  : m_levelStack()
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

void KEYCollectorBase::collectCellStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  // TODO: implement me
  (void) style;
  (void) anonymous;
}

void KEYCollectorBase::collectCharacterStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  if (m_collecting)
  {
    assert(m_currentStylesheet);

    if (bool(style))
    {
      if (style->getIdent() && !anonymous)
        m_currentStylesheet->m_styles[get(style->getIdent())] = style;
      m_newStyles.push_back(style);
    }
  }
}

void KEYCollectorBase::collectConnectionStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  // TODO: implement me
  (void) style;
  (void) anonymous;
}

void KEYCollectorBase::collectGraphicStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  if (m_collecting)
  {
    assert(m_currentStylesheet);

    if (bool(style))
    {
      if (style->getIdent() && !anonymous)
        m_currentStylesheet->m_styles[get(style->getIdent())] = style;
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

void KEYCollectorBase::collectLayoutStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  if (m_collecting)
  {
    assert(m_currentStylesheet);

    if (bool(style))
    {
      if (style->getIdent() && !anonymous)
        m_currentStylesheet->m_styles[get(style->getIdent())] = style;
      m_newStyles.push_back(style);

      if (bool(m_currentText))
        m_currentText->setLayoutStyle(style);
    }
  }
}

void KEYCollectorBase::collectListStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  // TODO: implement me
  (void) style;
  (void) anonymous;
}

void KEYCollectorBase::collectParagraphStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  if (m_collecting)
  {
    assert(m_currentStylesheet);

    if (bool(style))
    {
      if (style->getIdent() && !anonymous)
        m_currentStylesheet->m_styles[get(style->getIdent())] = style;
      m_newStyles.push_back(style);
    }
  }
}

void KEYCollectorBase::collectPlaceholderStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  if (m_collecting)
  {
    assert(m_currentStylesheet);

    if (bool(style))
    {
      if (style->getIdent() && !anonymous)
        m_currentStylesheet->m_styles[get(style->getIdent())] = style;
      m_newStyles.push_back(style);
    }
  }
}

void KEYCollectorBase::collectSlideStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  // TODO: implement me
  (void) style;
  (void) anonymous;
}

void KEYCollectorBase::collectTabularStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  // TODO: implement me
  (void) style;
  (void) anonymous;
}

void KEYCollectorBase::collectVectorStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  // TODO: implement me
  (void) style;
  (void) anonymous;
}

void KEYCollectorBase::collectGeometry(const IWORKGeometryPtr_t &geometry)
{
  if (m_collecting)
  {
    assert(!m_levelStack.empty());

    m_levelStack.top().m_geometry = geometry;
    m_levelStack.top().m_trafo = makeTransformation(*geometry) * m_levelStack.top().m_trafo;
  }
}

void KEYCollectorBase::collectBezier(const IWORKPathPtr_t &path)
{
  if (m_collecting)
    m_currentPath = path;
}

void KEYCollectorBase::collectGroup(const IWORKGroupPtr_t &group)
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

void KEYCollectorBase::collectImage(const IWORKImagePtr_t &image)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());
    assert(!m_levelStack.empty());

    image->m_geometry = m_levelStack.top().m_geometry;
    m_levelStack.top().m_geometry.reset();
    m_objectsStack.top().push_back(makeObject(image, m_levelStack.top().m_trafo));
  }
}

void KEYCollectorBase::collectLine(const IWORKLinePtr_t &line)
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

void KEYCollectorBase::collectShape()
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

void KEYCollectorBase::collectBezierPath()
{
  // nothing needed
}

void KEYCollectorBase::collectPolygonPath(const IWORKSize &size, const unsigned edges)
{
  if (m_collecting)
    m_currentPath = makePolygonPath(size, edges);
}

void KEYCollectorBase::collectRoundedRectanglePath(const IWORKSize &size, const double radius)
{
  if (m_collecting)
    m_currentPath = makeRoundedRectanglePath(size, radius);
}

void KEYCollectorBase::collectArrowPath(const IWORKSize &size, const double headWidth, const double stemRelYPos, bool const doubleSided)
{
  if (m_collecting)
  {
    if (doubleSided)
      m_currentPath = makeDoubleArrowPath(size, headWidth, stemRelYPos);
    else
      m_currentPath = makeArrowPath(size, headWidth, stemRelYPos);
  }
}

void KEYCollectorBase::collectStarPath(const IWORKSize &size, const unsigned points, const double innerRadius)
{
  if (m_collecting)
    m_currentPath = makeStarPath(size, points, innerRadius);
}

void KEYCollectorBase::collectConnectionPath(const IWORKSize &size, const double middleX, const double middleY)
{
  if (m_collecting)
    m_currentPath = makeConnectionPath(size, middleX, middleY);
}

void KEYCollectorBase::collectCalloutPath(const IWORKSize &size, const double radius, const double tailSize, const double tailX, const double tailY, bool quoteBubble)
{
  if (m_collecting)
  {
    if (quoteBubble)
      m_currentPath = makeQuoteBubblePath(size, radius, tailSize, tailX, tailY);
    else
      m_currentPath = makeCalloutPath(size, radius, tailSize, tailX, tailY);
  }
}

void KEYCollectorBase::collectData(const IWORKDataPtr_t &data)
{
  if (m_collecting)
    m_currentData = data;
}

IWORKMediaContentPtr_t KEYCollectorBase::collectUnfiltered(const boost::optional<IWORKSize> &size)
{
  IWORKMediaContentPtr_t newUnfiltered;

  if (m_collecting)
  {
    newUnfiltered.reset(new IWORKMediaContent());
    newUnfiltered->m_size = size;
    newUnfiltered->m_data = m_currentData;

    m_currentData.reset();
  }

  return newUnfiltered;
}

void KEYCollectorBase::insertUnfiltered(const IWORKMediaContentPtr_t &content)
{
  if (m_collecting)
  {
    assert(!m_currentUnfiltered);

    m_currentUnfiltered = content;
  }
}

void KEYCollectorBase::collectFiltered(const boost::optional<IWORKSize> &size)
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

void KEYCollectorBase::collectLeveled(const boost::optional<IWORKSize> &size)
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

IWORKMediaContentPtr_t KEYCollectorBase::collectFilteredImage()
{
  IWORKMediaContentPtr_t newFilteredImage;

  if (m_collecting)
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

  return newFilteredImage;
}

void KEYCollectorBase::insertFilteredImage(const IWORKMediaContentPtr_t &content)
{
  if (m_collecting)
  {
    assert(!m_currentContent);

    m_currentContent = content;
  }
}

void KEYCollectorBase::collectMovieMedia()
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

void KEYCollectorBase::collectMedia()
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

KEYLayerPtr_t KEYCollectorBase::collectLayer()
{
  KEYLayerPtr_t layer;

  if (m_collecting)
  {
    assert(m_layerOpened);
    assert(!m_objectsStack.empty());

    layer.reset(new KEYLayer());
    layer->m_objects = m_objectsStack.top();
    m_objectsStack.pop();
  }

  return layer;
}

IWORKStylesheetPtr_t KEYCollectorBase::collectStylesheet(const IWORKStylesheetPtr_t &parent)
{
  IWORKStylesheetPtr_t stylesheet;

  if (m_collecting)
  {
    assert(m_currentStylesheet);
    assert(parent != m_currentStylesheet);

    m_currentStylesheet->parent = parent;

    for_each(m_newStyles.begin(), m_newStyles.end(), boost::bind(&IWORKStyle::link, _1, m_currentStylesheet));

    stylesheet = m_currentStylesheet;
    m_currentStylesheet.reset(new IWORKStylesheet());
    m_newStyles.clear();
  }

  return stylesheet;
}

void KEYCollectorBase::collectText(const IWORKStylePtr_t &style, const std::string &text)
{
  if (m_collecting)
  {
    assert(bool(m_currentText));

    m_currentText->insertText(text, style);
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

KEYPlaceholderPtr_t KEYCollectorBase::collectTextPlaceholder(const IWORKStylePtr_t &style, const bool title)
{
  KEYPlaceholderPtr_t placeholder;

  if (m_collecting)
  {
    assert(bool(m_currentText));

    placeholder.reset(new KEYPlaceholder());
    placeholder->m_title = title;
    placeholder->m_style = style;
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
  }

  return placeholder;
}

void KEYCollectorBase::insertTextPlaceholder(const KEYPlaceholderPtr_t &placeholder)
{
  if (m_collecting)
  {
    assert(!m_objectsStack.empty());

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

void KEYCollectorBase::startParagraph(const IWORKStylePtr_t &style)
{
  if (m_collecting)
  {
    assert(bool(m_currentText));

    m_currentText->openParagraph(style);
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
