/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKCollector.h"

#include <algorithm>
#include <cassert>
#include <cstring>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

#include "IWORKDocumentInterface.h"
#include "IWORKOutputElements.h"
#include "IWORKPath.h"
#include "IWORKShape.h"
#include "IWORKText.h"

namespace libetonyek
{

using boost::make_shared;
using boost::optional;

using std::memcmp;
using std::string;

namespace
{

const unsigned char SIGNATURE_PDF[] = { '%', 'P', 'D', 'F' };
const unsigned char SIGNATURE_PNG[] = { 0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a };
const unsigned char SIGNATURE_JPEG[] = { 0xff, 0xd8 };
const unsigned char SIGNATURE_QUICKTIME[] = { 'm', 'o', 'o', 'v' };
const unsigned char SIGNATURE_TIFF_1[] = { 0x49, 0x49, 0x2a, 0x00 };
const unsigned char SIGNATURE_TIFF_2[] = { 0x4d, 0x4d, 0x00, 0x2a };

optional<string> detectMimetype(const RVNGInputStreamPtr_t &stream)
{
  stream->seek(0, librevenge::RVNG_SEEK_SET);

  unsigned long numBytesRead = 0;
  const unsigned char *const sig = stream->read(8, numBytesRead);

  if (8 != numBytesRead)
    // looks like the binary is broken anyway: just bail out
    return optional<string>();

  if (0 == memcmp(sig, SIGNATURE_PNG, ETONYEK_NUM_ELEMENTS(SIGNATURE_PNG)))
    return string("image/png");

  if (0 == memcmp(sig, SIGNATURE_PDF, ETONYEK_NUM_ELEMENTS(SIGNATURE_PDF)))
    return string("application/pdf");

  if ((0 == memcmp(sig, SIGNATURE_TIFF_1, ETONYEK_NUM_ELEMENTS(SIGNATURE_TIFF_1)))
      || (0 == memcmp(sig, SIGNATURE_TIFF_2, ETONYEK_NUM_ELEMENTS(SIGNATURE_TIFF_2))))
    return string("image/tiff");

  if (0 == memcmp(sig + 4, SIGNATURE_QUICKTIME, ETONYEK_NUM_ELEMENTS(SIGNATURE_QUICKTIME)))
    return string("video/quicktime");

  if (0 == memcmp(sig, SIGNATURE_JPEG, ETONYEK_NUM_ELEMENTS(SIGNATURE_JPEG)))
    return string("image/jpeg");

  return optional<string>();
}

optional<string> getMimetype(const optional<int> &type, const RVNGInputStreamPtr_t &stream)
{
  if (type)
  {
    switch (get(type))
    {
    case 1246774599 :
      return string("image/jpeg");
    case 1299148630 :
      return string("video/quicktime");
    case 1346651680 :
      return string("application/pdf");
    case 1347307366 :
      return string("image/png");
    case 1414088262 :
      return string("image/tiff");
    default :
      break;
    }
  }

  return detectMimetype(stream);
}

librevenge::RVNGPropertyList pointToWPG(const double x, const double y)
{
  librevenge::RVNGPropertyList props;

  props.insert("svg:x", pt2in(x));
  props.insert("svg:y", pt2in(y));

  return props;
}

void drawMedia(const IWORKMediaPtr_t &media, const glm::dmat3 &trafo, IWORKOutputElements &elements)
{
  if (bool(media)
      && bool(media->m_geometry)
      && bool(media->m_content)
      && bool(media->m_content->m_data)
      && bool(media->m_content->m_data->m_stream))
  {
    const RVNGInputStreamPtr_t input = media->m_content->m_data->m_stream;

    const optional<string> mimetype = getMimetype(media->m_content->m_data->m_type, input);

    if (mimetype)
    {
      input->seek(0, librevenge::RVNG_SEEK_END);
      const unsigned long size = input->tell();
      input->seek(0, librevenge::RVNG_SEEK_SET);

      unsigned long readBytes = 0;
      const unsigned char *const bytes = input->read(size, readBytes);
      if (readBytes != size)
        throw GenericException();

      librevenge::RVNGPropertyList props;

      props.insert("libwpg:mime-type", get(mimetype).c_str());
      props.insert("office:binary-data", librevenge::RVNGBinaryData(bytes, size));

      glm::dvec3 vec = trafo * glm::dvec3(0, 0, 1);
      props.insert("svg:x", pt2in(vec[0]));
      props.insert("svg:y", pt2in(vec[1]));

      double width = media->m_geometry->m_size.m_width;
      double height = media->m_geometry->m_size.m_height;
      vec = trafo * glm::dvec3(width, height, 0);
      props.insert("svg:width", pt2in(vec[0]));
      props.insert("svg:height", pt2in(vec[1]));

      elements.addDrawGraphicObject(props);
    }
  }
}

void drawImage(const IWORKImagePtr_t &image, const glm::dmat3 &trafo, IWORKOutputElements &elements)
{
  // TODO: implement me
  (void) image;
  (void) trafo;
  (void) elements;

}

void drawLine(const IWORKLinePtr_t &line, const glm::dmat3 &trafo, IWORKOutputElements &elements)
{
  // TODO: transform the line
  (void) trafo;

  if (line->m_x1 && line->m_y1 && line->m_x2 && line->m_y2)
  {
    librevenge::RVNGPropertyList props;
#if 0
    if (line->style)
    {
      // TODO: is it graphic style?
      const IWORKStyleMap_t::const_iterator styleIt = dict.graphicStyles.find(get(line->style));
      if (dict.graphicStyles.end() != styleIt)
      {
        IWORKStyle style = *styleIt->second;
        resolveStyle(style, dict.graphicStyles);
        props = makeLineStyle(style);
      }
    }
#endif
    elements.addSetStyle(props);

    librevenge::RVNGPropertyListVector vertices;
    vertices.append(pointToWPG(get(line->m_x1), get(line->m_y1)));
    vertices.append(pointToWPG(get(line->m_x2), get(line->m_y2)));

    librevenge::RVNGPropertyList points;
    points.insert("svg:points", vertices);

    elements.addDrawPolyline(points);
  }
  else
  {
    ETONYEK_DEBUG_MSG(("line is missing head or tail point\n"));
  }
}

void drawShape(const IWORKShapePtr_t &shape, const glm::dmat3 &trafo, IWORKOutputElements &elements)
{
  if (bool(shape) && bool(shape->m_path))
  {
    // TODO: make style

    const IWORKPath path = *shape->m_path * trafo;

    librevenge::RVNGPropertyList props;
    props.insert("svg:d", path.toWPG());

    elements.addSetStyle(librevenge::RVNGPropertyList());
    elements.addDrawPath(props);

    if (bool(shape->m_text))
      shape->m_text->draw(trafo, shape->m_geometry, elements);
  }
}

}

IWORKCollector::Level::Level()
  : m_geometry()
  , m_graphicStyle()
  , m_trafo()
{
}

IWORKCollector::IWORKCollector(IWORKDocumentInterface *const document)
  : m_document(document)
  , m_levelStack()
  , m_currentStylesheet(new IWORKStylesheet())
  , m_newStyles()
  , m_currentText()
  , m_currentTable()
  , m_currentPath()
  , m_groupLevel(0)
{
  m_document->startDocument(librevenge::RVNGPropertyList());
  m_document->setDocumentMetaData(librevenge::RVNGPropertyList());
}

IWORKCollector::~IWORKCollector()
{
  assert(m_levelStack.empty());
  assert(0 == m_groupLevel);

  assert(!m_currentPath);
  assert(!m_currentText);

  m_document->endDocument();
}

void IWORKCollector::collectStyle(const IWORKStylePtr_t &style, const bool anonymous)
{
  assert(m_currentStylesheet);

  if (bool(style))
  {
    if (style->getIdent() && !anonymous)
      m_currentStylesheet->m_styles[get(style->getIdent())] = style;
    m_newStyles.push_back(style);
  }
}

void IWORKCollector::setGraphicStyle(const IWORKStylePtr_t &style)
{
  if (!m_levelStack.empty())
  {
    m_levelStack.top().m_graphicStyle = style;
    m_styleStack.set(style);
  }
}

void IWORKCollector::collectGeometry(const IWORKGeometryPtr_t &geometry)
{
  assert(!m_levelStack.empty());

  m_levelStack.top().m_geometry = geometry;
  m_levelStack.top().m_trafo *= makeTransformation(*geometry);
}

void IWORKCollector::collectBezier(const IWORKPathPtr_t &path)
{
  m_currentPath = path;
}

void IWORKCollector::collectImage(const IWORKImagePtr_t &image)
{
  assert(!m_levelStack.empty());

  image->m_geometry = m_levelStack.top().m_geometry;
  m_levelStack.top().m_geometry.reset();

  drawImage(image, m_levelStack.top().m_trafo, m_outputManager.getCurrent());
}

void IWORKCollector::collectLine(const IWORKLinePtr_t &line)
{
  assert(!m_levelStack.empty());

  line->m_geometry = m_levelStack.top().m_geometry;
  m_levelStack.top().m_geometry.reset();

  drawLine(line, m_levelStack.top().m_trafo, m_outputManager.getCurrent());
}

void IWORKCollector::collectShape()
{
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
    shape->m_text = m_currentText;
    m_currentText.reset();
  }

  shape->m_style = m_levelStack.top().m_graphicStyle;
  m_levelStack.top().m_graphicStyle.reset();

  drawShape(shape, m_levelStack.top().m_trafo, m_outputManager.getCurrent());
}

void IWORKCollector::collectBezierPath()
{
  // nothing needed
}

void IWORKCollector::collectPolygonPath(const IWORKSize &size, const unsigned edges)
{
  m_currentPath = makePolygonPath(size, edges);
}

void IWORKCollector::collectRoundedRectanglePath(const IWORKSize &size, const double radius)
{
  m_currentPath = makeRoundedRectanglePath(size, radius);
}

void IWORKCollector::collectArrowPath(const IWORKSize &size, const double headWidth, const double stemRelYPos, bool const doubleSided)
{
  if (doubleSided)
    m_currentPath = makeDoubleArrowPath(size, headWidth, stemRelYPos);
  else
    m_currentPath = makeArrowPath(size, headWidth, stemRelYPos);
}

void IWORKCollector::collectStarPath(const IWORKSize &size, const unsigned points, const double innerRadius)
{
  m_currentPath = makeStarPath(size, points, innerRadius);
}

void IWORKCollector::collectConnectionPath(const IWORKSize &size, const double middleX, const double middleY)
{
  m_currentPath = makeConnectionPath(size, middleX, middleY);
}

void IWORKCollector::collectCalloutPath(const IWORKSize &size, const double radius, const double tailSize, const double tailX, const double tailY, bool quoteBubble)
{
  if (quoteBubble)
    m_currentPath = makeQuoteBubblePath(size, radius, tailSize, tailX, tailY);
  else
    m_currentPath = makeCalloutPath(size, radius, tailSize, tailX, tailY);
}

void IWORKCollector::collectMedia(const IWORKMediaContentPtr_t &content)
{
  assert(!m_levelStack.empty());

  const IWORKMediaPtr_t media(new IWORKMedia());
  media->m_geometry = m_levelStack.top().m_geometry;
  media->m_style = m_levelStack.top().m_graphicStyle;
  media->m_content = content;

  m_levelStack.top().m_geometry.reset();
  m_levelStack.top().m_graphicStyle.reset();

  drawMedia(media, m_levelStack.top().m_trafo, m_outputManager.getCurrent());
}

IWORKStylesheetPtr_t IWORKCollector::collectStylesheet(const IWORKStylesheetPtr_t &parent)
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

void IWORKCollector::collectText(const std::string &text)
{
  assert(bool(m_currentText));

  m_currentText->insertText(text);
}

void IWORKCollector::collectTab()
{
  assert(bool(m_currentText));

  m_currentText->insertTab();
}

void IWORKCollector::collectLineBreak()
{
  assert(bool(m_currentText));

  m_currentText->insertLineBreak();
}

void IWORKCollector::collectTableSizes(const IWORKRowSizes_t &rowSizes, const IWORKColumnSizes_t &columnSizes)
{
  m_currentTable.setSizes(columnSizes, rowSizes);
}

void IWORKCollector::collectTableCell(const unsigned row, const unsigned column, const boost::optional<std::string> &content, const unsigned rowSpan, const unsigned columnSpan)
{
  IWORKOutputElements elements;

  if (bool(content))
  {
    assert(!m_currentText || m_currentText->empty());

    librevenge::RVNGPropertyList props;
    elements.addOpenParagraph(props);
    elements.addOpenSpan(props);
    elements.addInsertText(librevenge::RVNGString(get(content).c_str()));
    elements.addCloseSpan();
    elements.addCloseParagraph();
  }
  else if (bool(m_currentText))
  {
    m_currentText->draw(elements);
    m_currentText.reset();
  }

  m_currentTable.insertCell(column, row, elements, columnSpan, rowSpan);
}

void IWORKCollector::collectCoveredTableCell(const unsigned row, const unsigned column)
{
  m_currentTable.insertCoveredCell(column, row);
}

void IWORKCollector::collectTableRow()
{
  // nothing needed
}

void IWORKCollector::collectTable()
{
  drawTable();
}

void IWORKCollector::startGroup()
{
  ++m_groupLevel;
}

void IWORKCollector::endGroup()
{
  assert(m_groupLevel > 0);

  --m_groupLevel;
}

void IWORKCollector::startParagraph(const IWORKStylePtr_t &style)
{
  assert(bool(m_currentText));

  m_currentText->openParagraph(style);
}

void IWORKCollector::endParagraph()
{
  assert(bool(m_currentText));

  m_currentText->closeParagraph();
}

void IWORKCollector::openSpan(const IWORKStylePtr_t &style)
{
  assert(bool(m_currentText));

  m_currentText->openSpan(style);
}

void IWORKCollector::closeSpan()
{
  assert(bool(m_currentText));

  m_currentText->closeSpan();
}

void IWORKCollector::openLink(const std::string &url)
{
  assert(bool(m_currentText));

  m_currentText->openLink(url);
}

void IWORKCollector::closeLink()
{
  assert(bool(m_currentText));

  m_currentText->closeLink();
}

void IWORKCollector::startText()
{
  assert(!m_currentText);

  m_currentText.reset(new IWORKText());

  assert(m_currentText->empty());
}

void IWORKCollector::endText()
{
  // text is reset at the place where it is used
  assert(!m_currentText || m_currentText->empty());

  m_currentText.reset();
}

void IWORKCollector::startLevel()
{
  glm::dmat3 currentTrafo;
  if (!m_levelStack.empty())
    currentTrafo = m_levelStack.top().m_trafo;
  m_levelStack.push(Level());
  m_levelStack.top().m_trafo = currentTrafo;

  pushStyle();
}

void IWORKCollector::endLevel()
{
  assert(!m_levelStack.empty());
  m_levelStack.pop();

  popStyle();
}

void IWORKCollector::pushStyle()
{
  m_styleStack.push();
}

void IWORKCollector::popStyle()
{
  m_styleStack.pop();
}

void IWORKCollector::resolveStyle(IWORKStyle &style)
{
  // TODO: implement me
  (void) style;
}

IWORKOutputManager &IWORKCollector::getOutputManager()
{
  return m_outputManager;
}

} // namespace libetonyek

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
