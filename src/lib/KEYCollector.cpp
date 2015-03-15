/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYCollector.h"

#include <algorithm>
#include <cassert>

#include <boost/bind.hpp>

#include "libetonyek_utils.h"
#include "IWORKDocumentInterface.h"
#include "IWORKOutputElements.h"
#include "IWORKShape.h"
#include "IWORKText.h"
#include "IWORKTransformation.h"
#include "KEYDictionary.h"
#include "KEYStyles.h"

namespace libetonyek
{

using boost::optional;

namespace
{

class OutputElementsObject : public IWORKObject
{
public:
  explicit OutputElementsObject(const IWORKOutputElements &elements);

private:
  virtual void draw(IWORKDocumentInterface *document);

private:
  const IWORKOutputElements m_elements;
};

OutputElementsObject::OutputElementsObject(const IWORKOutputElements &elements)
  : m_elements(elements)
{
}

void OutputElementsObject::draw(IWORKDocumentInterface *const document)
{
  m_elements.write(document);
}

}

KEYCollector::Level::Level()
  : m_geometry()
  , m_graphicStyle()
  , m_trafo()
{
}

KEYCollector::KEYCollector(IWORKDocumentInterface *const document)
  : m_document(document)
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
  , m_stickyNotes()
  , m_size()
  , m_paint(false)
  , m_pageOpened(false)
  , m_layerOpened(false)
  , m_groupLevel(0)
  , m_layerCount(0)
{
  m_document->startDocument(librevenge::RVNGPropertyList());
  m_document->setDocumentMetaData(librevenge::RVNGPropertyList());

  assert(!m_paint);
}

KEYCollector::~KEYCollector()
{
  assert(!m_paint);
  assert(m_objectsStack.empty());
  assert(m_levelStack.empty());
  assert(!m_pageOpened);
  assert(!m_layerOpened);
  assert(0 == m_groupLevel);

  assert(!m_currentPath);
  assert(!m_currentText);

  m_document->endDocument();
}

void KEYCollector::collectCellStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  // TODO: implement me
  (void) style;
  (void) anonymous;
}

void KEYCollector::collectCharacterStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  assert(m_currentStylesheet);

  if (bool(style))
  {
    if (style->getIdent() && !anonymous)
      m_currentStylesheet->m_styles[get(style->getIdent())] = style;
    m_newStyles.push_back(style);
  }
}

void KEYCollector::collectConnectionStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  // TODO: implement me
  (void) style;
  (void) anonymous;
}

void KEYCollector::collectGraphicStyle(const IWORKStylePtr_t &style, const bool anonymous)
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

void KEYCollector::collectLayoutStyle(const IWORKStylePtr_t &style, const bool anonymous)
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

void KEYCollector::collectListStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  // TODO: implement me
  (void) style;
  (void) anonymous;
}

void KEYCollector::collectParagraphStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  assert(m_currentStylesheet);

  if (bool(style))
  {
    if (style->getIdent() && !anonymous)
      m_currentStylesheet->m_styles[get(style->getIdent())] = style;
    m_newStyles.push_back(style);
  }
}

void KEYCollector::collectPlaceholderStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  assert(m_currentStylesheet);

  if (bool(style))
  {
    if (style->getIdent() && !anonymous)
      m_currentStylesheet->m_styles[get(style->getIdent())] = style;
    m_newStyles.push_back(style);
  }
}

void KEYCollector::collectSlideStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  // TODO: implement me
  (void) style;
  (void) anonymous;
}

void KEYCollector::collectTabularStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  // TODO: implement me
  (void) style;
  (void) anonymous;
}

void KEYCollector::collectVectorStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  // TODO: implement me
  (void) style;
  (void) anonymous;
}

void KEYCollector::collectGeometry(const IWORKGeometryPtr_t &geometry)
{
  assert(!m_levelStack.empty());

  m_levelStack.top().m_geometry = geometry;
  m_levelStack.top().m_trafo = makeTransformation(*geometry) * m_levelStack.top().m_trafo;
}

void KEYCollector::collectBezier(const IWORKPathPtr_t &path)
{
  m_currentPath = path;
}

void KEYCollector::collectGroup(const IWORKGroupPtr_t &group)
{
  assert(!m_objectsStack.empty());

  group->m_objects = m_objectsStack.top();
  m_objectsStack.pop();
  assert(!m_objectsStack.empty());
  m_objectsStack.top().push_back(makeObject(group));
}

void KEYCollector::collectImage(const IWORKImagePtr_t &image)
{
  assert(!m_objectsStack.empty());
  assert(!m_levelStack.empty());

  image->m_geometry = m_levelStack.top().m_geometry;
  m_levelStack.top().m_geometry.reset();
  m_objectsStack.top().push_back(makeObject(image, m_levelStack.top().m_trafo));
}

void KEYCollector::collectLine(const IWORKLinePtr_t &line)
{
  assert(!m_objectsStack.empty());
  assert(!m_levelStack.empty());

  line->m_geometry = m_levelStack.top().m_geometry;
  m_levelStack.top().m_geometry.reset();
  m_objectsStack.top().push_back(makeObject(line, m_levelStack.top().m_trafo));
}

void KEYCollector::collectShape()
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

void KEYCollector::collectBezierPath()
{
  // nothing needed
}

void KEYCollector::collectPolygonPath(const IWORKSize &size, const unsigned edges)
{
  m_currentPath = makePolygonPath(size, edges);
}

void KEYCollector::collectRoundedRectanglePath(const IWORKSize &size, const double radius)
{
  m_currentPath = makeRoundedRectanglePath(size, radius);
}

void KEYCollector::collectArrowPath(const IWORKSize &size, const double headWidth, const double stemRelYPos, bool const doubleSided)
{
  if (doubleSided)
    m_currentPath = makeDoubleArrowPath(size, headWidth, stemRelYPos);
  else
    m_currentPath = makeArrowPath(size, headWidth, stemRelYPos);
}

void KEYCollector::collectStarPath(const IWORKSize &size, const unsigned points, const double innerRadius)
{
  m_currentPath = makeStarPath(size, points, innerRadius);
}

void KEYCollector::collectConnectionPath(const IWORKSize &size, const double middleX, const double middleY)
{
  m_currentPath = makeConnectionPath(size, middleX, middleY);
}

void KEYCollector::collectCalloutPath(const IWORKSize &size, const double radius, const double tailSize, const double tailX, const double tailY, bool quoteBubble)
{
  if (quoteBubble)
    m_currentPath = makeQuoteBubblePath(size, radius, tailSize, tailX, tailY);
  else
    m_currentPath = makeCalloutPath(size, radius, tailSize, tailX, tailY);
}

void KEYCollector::collectData(const IWORKDataPtr_t &data)
{
  m_currentData = data;
}

IWORKMediaContentPtr_t KEYCollector::collectUnfiltered(const boost::optional<IWORKSize> &size)
{
  IWORKMediaContentPtr_t newUnfiltered(new IWORKMediaContent());
  newUnfiltered->m_size = size;
  newUnfiltered->m_data = m_currentData;

  m_currentData.reset();

  return newUnfiltered;
}

void KEYCollector::insertUnfiltered(const IWORKMediaContentPtr_t &content)
{
  assert(!m_currentUnfiltered);

  m_currentUnfiltered = content;
}

void KEYCollector::collectFiltered(const boost::optional<IWORKSize> &size)
{
  const IWORKMediaContentPtr_t newFiltered(new IWORKMediaContent());
  newFiltered->m_size = size;
  newFiltered->m_data = m_currentData;

  m_currentData.reset();

  assert(!m_currentFiltered);
  m_currentFiltered = newFiltered;
}

void KEYCollector::collectLeveled(const boost::optional<IWORKSize> &size)
{
  const IWORKMediaContentPtr_t newLeveled(new IWORKMediaContent());
  newLeveled->m_size = size;
  newLeveled->m_data = m_currentData;

  m_currentData.reset();

  assert(!m_currentLeveled);
  m_currentLeveled = newLeveled;
}

IWORKMediaContentPtr_t KEYCollector::collectFilteredImage()
{
  IWORKMediaContentPtr_t newFilteredImage;

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

  return newFilteredImage;
}

void KEYCollector::insertFilteredImage(const IWORKMediaContentPtr_t &content)
{
  assert(!m_currentContent);

  m_currentContent = content;
}

void KEYCollector::collectMovieMedia()
{
  assert(m_currentData);

  const IWORKMediaContentPtr_t newContent(new IWORKMediaContent());
  newContent->m_data = m_currentData;
  m_currentData.reset();

  assert(!m_currentContent);
  m_currentContent = newContent;
}

void KEYCollector::collectMedia()
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

void KEYCollector::collectPresentation(const boost::optional<IWORKSize> &size)
{
  if (size)
    m_size = get(size);
}

KEYLayerPtr_t KEYCollector::collectLayer()
{
  assert(m_layerOpened);
  assert(!m_objectsStack.empty());

  KEYLayerPtr_t layer(new KEYLayer());

  layer->m_objects = m_objectsStack.top();
  m_objectsStack.pop();

  return layer;
}

void KEYCollector::insertLayer(const KEYLayerPtr_t &layer)
{
  assert(!m_layerOpened);

  if (bool(layer))
  {
    if (m_paint)
    {
      ++m_layerCount;

      librevenge::RVNGPropertyList props;
      props.insert("svg:id", m_layerCount);

      m_document->startLayer(props);

      for (IWORKObjectList_t::const_iterator it = layer->m_objects.begin(); it != layer->m_objects.end(); ++it)
        (*it)->draw(m_document);

      m_document->endLayer();
    }
  }
  else
  {
    ETONYEK_DEBUG_MSG(("no layer\n"));
  }
}

void KEYCollector::collectPage()
{
  assert(m_pageOpened);

  if (m_paint)
  {
    drawNotes();
    drawStickyNotes();
  }
}

IWORKStylesheetPtr_t KEYCollector::collectStylesheet(const IWORKStylesheetPtr_t &parent)
{
  assert(m_currentStylesheet);
  assert(parent != m_currentStylesheet);

  m_currentStylesheet->parent = parent;

  for_each(m_newStyles.begin(), m_newStyles.end(), boost::bind(&IWORKStyle::link, _1, m_currentStylesheet));

  IWORKStylesheetPtr_t stylesheet(m_currentStylesheet);
  m_currentStylesheet.reset(new IWORKStylesheet());
  m_newStyles.clear();

  return stylesheet;
}

void KEYCollector::collectText(const IWORKStylePtr_t &style, const std::string &text)
{
  assert(bool(m_currentText));

  m_currentText->insertText(text, style);
}

void KEYCollector::collectTab()
{
  assert(bool(m_currentText));

  m_currentText->insertTab();
}

void KEYCollector::collectLineBreak()
{
  assert(bool(m_currentText));

  m_currentText->insertLineBreak();
}

KEYPlaceholderPtr_t KEYCollector::collectTextPlaceholder(const IWORKStylePtr_t &style, const bool title)
{
  assert(bool(m_currentText));

  KEYPlaceholderPtr_t placeholder(new KEYPlaceholder());
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

  return placeholder;
}

void KEYCollector::insertTextPlaceholder(const KEYPlaceholderPtr_t &placeholder)
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

void KEYCollector::collectTableSizes(const IWORKTable::RowSizes_t &rowSizes, const IWORKTable::ColumnSizes_t &columnSizes)
{
  m_currentTable.setSizes(columnSizes, rowSizes);
}

void KEYCollector::collectTableCell(const unsigned row, const unsigned column, const boost::optional<std::string> &content, const unsigned rowSpan, const unsigned columnSpan)
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

void KEYCollector::collectCoveredTableCell(const unsigned row, const unsigned column)
{
  m_currentTable.insertCoveredCell(column, row);
}

void KEYCollector::collectTableRow()
{
  // nothing needed
}

void KEYCollector::collectTable()
{
  assert(!m_levelStack.empty());
  assert(!m_objectsStack.empty());

  m_currentTable.setGeometry(m_levelStack.top().m_geometry);
  m_levelStack.top().m_geometry.reset();

  m_objectsStack.top().push_back(makeObject(m_currentTable, m_levelStack.top().m_trafo));
  m_currentTable = IWORKTable();
}

void KEYCollector::collectNote()
{
  m_notes.push_back(makeObject(m_currentText, m_levelStack.top().m_trafo));
  m_currentText.reset();
}

void KEYCollector::collectStickyNote()
{
  assert(!m_levelStack.empty());

  m_stickyNotes.push_back(KEYStickyNote(m_levelStack.top().m_geometry, m_currentText));
  m_levelStack.top().m_geometry.reset();
  m_currentText.reset();
}

void KEYCollector::startSlides()
{
  m_paint = true;
}

void KEYCollector::endSlides()
{
  m_paint = false;
}

void KEYCollector::startThemes()
{
}

void KEYCollector::endThemes()
{
}

void KEYCollector::startPage()
{
  assert(!m_pageOpened);
  assert(!m_layerOpened);
  assert(m_notes.empty());
  assert(m_stickyNotes.empty());

  startLevel();

  m_pageOpened = true;

  if (m_paint)
  {
    librevenge::RVNGPropertyList props;
    props.insert("svg:width", pt2in(m_size.m_width));
    props.insert("svg:height", pt2in(m_size.m_height));

    m_document->startSlide(props);
  }
}

void KEYCollector::endPage()
{
  assert(m_pageOpened);

  endLevel();

  m_notes.clear();
  m_stickyNotes.clear();

  m_pageOpened = false;

  if (m_paint)
    m_document->endSlide();
}

void KEYCollector::startLayer()
{
  assert(m_pageOpened);
  assert(!m_layerOpened);
  assert(m_objectsStack.empty());

  m_objectsStack.push(IWORKObjectList_t());
  m_layerOpened = true;

  startLevel();

  assert(!m_objectsStack.empty());
}

void KEYCollector::endLayer()
{
  assert(m_pageOpened);
  assert(m_layerOpened);
  // object stack is already cleared by collectLayer()
  assert(m_objectsStack.empty());

  endLevel();

  m_layerOpened = false;

  assert(m_objectsStack.empty());
}

void KEYCollector::startGroup()
{
  assert(m_layerOpened);
  assert(!m_objectsStack.empty());

  m_objectsStack.push(IWORKObjectList_t());
  ++m_groupLevel;
}

void KEYCollector::endGroup()
{
  assert(m_layerOpened);
  assert(!m_objectsStack.empty());
  assert(m_groupLevel > 0);

  --m_groupLevel;
  // stack is popped in collectGroup already
}

void KEYCollector::startParagraph(const IWORKStylePtr_t &style)
{
  assert(bool(m_currentText));

  m_currentText->openParagraph(style);
}

void KEYCollector::endParagraph()
{
  assert(bool(m_currentText));

  m_currentText->closeParagraph();
}

void KEYCollector::startText(const bool object)
{
  assert(!m_currentText);

  m_currentText.reset(new IWORKText(object));

  assert(m_currentText->empty());
}

void KEYCollector::endText()
{
  // text is reset at the place where it is used
  assert(!m_currentText || m_currentText->empty());

  m_currentText.reset();
}

void KEYCollector::startLevel()
{
  IWORKTransformation currentTrafo;
  if (!m_levelStack.empty())
    currentTrafo = m_levelStack.top().m_trafo;
  m_levelStack.push(Level());
  m_levelStack.top().m_trafo = currentTrafo;

  pushStyle();
}

void KEYCollector::endLevel()
{
  assert(!m_levelStack.empty());
  m_levelStack.pop();

  popStyle();
}

void KEYCollector::pushStyle()
{
  m_styleStack.push();
}

void KEYCollector::popStyle()
{
  m_styleStack.pop();
}

void KEYCollector::resolveStyle(IWORKStyle &style)
{
  // TODO: implement me
  (void) style;
}

void KEYCollector::drawNotes()
{
  if (m_notes.empty())
    return;

  m_document->startNotes(librevenge::RVNGPropertyList());
  for (IWORKObjectList_t::const_iterator it = m_notes.begin(); m_notes.end() != it; ++it)
    (*it)->draw(m_document);
  m_document->endNotes();
}

void KEYCollector::drawStickyNotes()
{
  if (m_stickyNotes.empty())
    return;

  for (KEYStickyNotes_t::const_iterator it = m_stickyNotes.begin(); m_stickyNotes.end() != it; ++it)
  {
    librevenge::RVNGPropertyList props;

    if (bool(it->m_geometry))
    {
      props.insert("svg:x", pt2in(it->m_geometry->m_position.m_x));
      props.insert("svg:y", pt2in(it->m_geometry->m_position.m_y));
      props.insert("svg:width", pt2in(it->m_geometry->m_naturalSize.m_width));
      props.insert("svg:height", pt2in(it->m_geometry->m_naturalSize.m_height));
    }

    m_document->openComment(props);
    if (bool(it->m_text))
      makeObject(it->m_text, m_levelStack.top().m_trafo)->draw(m_document);
    m_document->closeComment();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
