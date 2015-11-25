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
#include <cmath>
#include <cstring>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

#include "IWORKDocumentInterface.h"
#include "IWORKOutputElements.h"
#include "IWORKPath.h"
#include "IWORKProperties.h"
#include "IWORKShape.h"
#include "IWORKTable.h"
#include "IWORKText.h"
#include "libetonyek_utils.h"

namespace libetonyek
{

using boost::make_shared;
using boost::optional;
using boost::shared_ptr;

using librevenge::RVNGPropertyList;

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

string detectMimetype(const RVNGInputStreamPtr_t &stream)
{
  stream->seek(0, librevenge::RVNG_SEEK_SET);

  unsigned long numBytesRead = 0;
  const unsigned char *const sig = stream->read(8, numBytesRead);

  if (8 != numBytesRead)
    // looks like the binary is broken anyway: just bail out
    return string();

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

  return string();
}

librevenge::RVNGPropertyList makePoint(const double x, const double y)
{
  librevenge::RVNGPropertyList props;

  props.insert("svg:x", pt2in(x));
  props.insert("svg:y", pt2in(y));

  return props;
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
    vertices.append(makePoint(get(line->m_x1), get(line->m_y1)));
    vertices.append(makePoint(get(line->m_x2), get(line->m_y2)));

    librevenge::RVNGPropertyList points;
    points.insert("svg:points", vertices);

    elements.addDrawPolyline(points);
  }
  else
  {
    ETONYEK_DEBUG_MSG(("line is missing head or tail point\n"));
  }
}

struct FillWriter : public boost::static_visitor<void>
{
  explicit FillWriter(RVNGPropertyList &props)
    : m_props(props)
  {
  }

  void operator()(const IWORKColor &color) const
  {
    m_props.insert("draw:fill", "solid");
    m_props.insert("draw:fill-color", makeColor(color));
  }

  void operator()(const IWORKGradient &gradient) const
  {
    if (gradient.m_stops.empty())
      return;
    m_props.insert("draw:fill", "gradient");
    switch (gradient.m_type)
    {
    case IWORK_GRADIENT_TYPE_LINEAR :
      m_props.insert("draw:style", "linear");
      break;
    case IWORK_GRADIENT_TYPE_RADIAL :
      m_props.insert("draw:style", "radial");
      break;
    }
    // TODO: use svg:linearGradient/svg:radialGradient?
    using librevenge::RVNG_PERCENT;
    m_props.insert("draw:start-color", makeColor(gradient.m_stops.front().m_color));
    m_props.insert("draw:start-intensity", gradient.m_stops.front().m_fraction, RVNG_PERCENT);
    m_props.insert("draw:end-color", makeColor(gradient.m_stops.back().m_color));
    m_props.insert("draw:end-intensity", gradient.m_stops.back().m_fraction, RVNG_PERCENT);
    m_props.insert("draw:angle", gradient.m_angle);
  }

  void operator()(const IWORKFillImage &bitmap) const
  {
    bool filled = false;

    if (bitmap.m_stream)
    {
      const unsigned long length = getLength(bitmap.m_stream);
      unsigned long readBytes = 0;
      const unsigned char *const bytes = bitmap.m_stream->read(length, readBytes);
      if (readBytes == length)
      {
        m_props.insert("draw:fill", "bitmap");
        m_props.insert("draw:fill-image", librevenge::RVNGBinaryData(bytes, length));
        m_props.insert("librevenge:mime-type", "jpg"); // TODO: fix
        switch (bitmap.m_type)
        {
        case IWORK_FILL_IMAGE_TYPE_ORIGINAL_SIZE :
          m_props.insert("style:repeat", "no-repeat");
          break;
        case IWORK_FILL_IMAGE_TYPE_STRETCH :
        case IWORK_FILL_IMAGE_TYPE_SCALE_TO_FILL :
        case IWORK_FILL_IMAGE_TYPE_SCALE_TO_FIT :
          m_props.insert("style:repeat", "stretch");
          break;
        case IWORK_FILL_IMAGE_TYPE_TILE :
          m_props.insert("style:repeat", "repeat");
          break;
        }
        m_props.insert("draw:fill-image-width", bitmap.m_size.m_width);
        m_props.insert("draw:fill-image-height", bitmap.m_size.m_height);
        filled = true;
      }
    }

    if (!filled && bitmap.m_color)
      (*this)(get(bitmap.m_color));
  }

private:
  RVNGPropertyList &m_props;
};

void fillGraphicProps(const IWORKStylePtr_t style, RVNGPropertyList &props)
{
  assert(bool(style));

  using namespace property;

  if (style->has<Fill>())
    apply_visitor(FillWriter(props), style->get<Fill>());

  if (style->has<Stroke>())
  {
    const IWORKStroke &stroke = style->get<Stroke>();

    // TODO: interpret pattern
    props.insert("draw:stroke", "solid");

    props.insert("svg:stroke-width", pt2in(stroke.m_width));
    props.insert("svg:stroke-color", makeColor(stroke.m_color));

    switch (stroke.m_cap)
    {
    default :
    case IWORK_LINE_CAP_BUTT :
      props.insert("svg:stroke-linecap", "butt");
      break;
    case IWORK_LINE_CAP_ROUND :
      props.insert("svg:stroke-linecap", "round");
      break;
    }

    switch (stroke.m_join)
    {
    case IWORK_LINE_JOIN_MITER :
      props.insert("svg:stroke-linejoin", "miter");
      break;
    default :
      props.insert("svg:stroke-linejoin", "none");
    }
  }

  if (style->has<Shadow>())
  {
    const IWORKShadow &shadow = style->get<Shadow>();

    props.insert("draw:shadow", "visible");
    props.insert("draw:shadow-color", makeColor(shadow.m_color));
    props.insert("draw:shadow-opacity", shadow.m_opacity);
    const double angle = deg2rad(shadow.m_angle);
    props.insert("draw:shadow-offset-x", shadow.m_offset * std::cos(angle));
    props.insert("draw:shadow-offset-y", shadow.m_offset * std::sin(angle));
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
  , m_stylesheetStack()
  , m_newStyles()
  , m_currentTable()
  , m_currentText()
  , m_headers()
  , m_footers()
  , m_currentPath()
  , m_groupLevel(0)
{
}

IWORKCollector::~IWORKCollector()
{
  assert(m_levelStack.empty());
  assert(m_stylesheetStack.empty());
  assert(0 == m_groupLevel);

  assert(!m_currentPath);
  assert(!m_currentText);
}

void IWORKCollector::collectStyle(const IWORKStylePtr_t &style)
{
  if (bool(style))
    m_newStyles.push_back(style);
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

  drawShape(shape);
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

  drawMedia(media);
}

void IWORKCollector::collectStylesheet(const IWORKStylesheetPtr_t &stylesheet)
{
  for_each(m_newStyles.begin(), m_newStyles.end(), boost::bind(&IWORKStyle::link, _1, stylesheet));
  m_newStyles.clear();
}

void IWORKCollector::collectMetadata(const IWORKMetadata &metadata)
{
  m_metadata = metadata;
}

void IWORKCollector::collectHeader(const std::string &name)
{
  collectHeaderFooter(name, m_headers);
}

void IWORKCollector::collectFooter(const std::string &name)
{
  collectHeaderFooter(name, m_footers);
}

void IWORKCollector::collectTable(const boost::shared_ptr<IWORKTable> &table)
{
  assert(!m_currentTable);
  m_currentTable = table;
  drawTable();
  m_currentTable.reset();
}

void IWORKCollector::collectText(const boost::shared_ptr<IWORKText> &text)
{
  assert(!m_currentText);
  m_currentText = text;
}

void IWORKCollector::startDocument()
{
  m_document->startDocument(librevenge::RVNGPropertyList());
}

void IWORKCollector::endDocument()
{
  assert(m_levelStack.empty());
  assert(0 == m_groupLevel);

  assert(!m_currentPath);
  assert(!m_currentText);

  m_document->endDocument();
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

boost::shared_ptr<IWORKTable> IWORKCollector::createTable(const IWORKTableNameMapPtr_t &tableNameMap) const
{
  return shared_ptr<IWORKTable>(new IWORKTable(tableNameMap));
}

boost::shared_ptr<IWORKText> IWORKCollector::createText(bool discardEmptyContent) const
{
  return make_shared<IWORKText>(discardEmptyContent);
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

void IWORKCollector::pushStylesheet(const IWORKStylesheetPtr_t &stylesheet)
{
  m_stylesheetStack.push(stylesheet);
}

void IWORKCollector::popStylesheet()
{
  assert(!m_stylesheetStack.empty());

  m_stylesheetStack.pop();
}

void IWORKCollector::resolveStyle(IWORKStyle &style)
{
  // TODO: implement me
  (void) style;
}

void IWORKCollector::collectHeaderFooter(const std::string &name, IWORKHeaderFooterMap_t &map)
{
  IWORKOutputElements &elements = map[name];
  if (!elements.empty())
  {
    ETONYEK_DEBUG_MSG(("header '%s' already exists, overwriting\n", name.c_str()));
    elements.clear();
  }
  if (bool(m_currentText))
  {
    m_currentText->draw(elements);
    m_currentText.reset();
  }
}

void IWORKCollector::fillMetadata(librevenge::RVNGPropertyList &props)
{
  if (!m_metadata.m_title.empty())
    props.insert("dc:subject", m_metadata.m_title.c_str());
  if (!m_metadata.m_author.empty())
    props.insert("meta:intial-creator", m_metadata.m_author.c_str());
  if (!m_metadata.m_keywords.empty())
    props.insert("meta:keyword", m_metadata.m_keywords.c_str());
  if (!m_metadata.m_comment.empty())
    props.insert("librevenge:comments", m_metadata.m_comment.c_str());
}

IWORKOutputManager &IWORKCollector::getOutputManager()
{
  return m_outputManager;
}

void IWORKCollector::drawMedia(const IWORKMediaPtr_t &media)
{
  if (bool(media)
      && bool(media->m_geometry)
      && bool(media->m_content)
      && bool(media->m_content->m_data)
      && bool(media->m_content->m_data->m_stream))
  {
    const glm::dmat3 trafo = m_levelStack.top().m_trafo;
    const RVNGInputStreamPtr_t input = media->m_content->m_data->m_stream;

    string mimetype(media->m_content->m_data->m_mimeType);
    if (mimetype.empty())
      mimetype = detectMimetype(input);

    if (!mimetype.empty())
    {
      input->seek(0, librevenge::RVNG_SEEK_END);
      const unsigned long size = input->tell();
      input->seek(0, librevenge::RVNG_SEEK_SET);

      unsigned long readBytes = 0;
      const unsigned char *const bytes = input->read(size, readBytes);
      if (readBytes != size)
        throw GenericException();

      const glm::dvec3 pos = trafo * glm::dvec3(0, 0, 1);
      const double width = media->m_geometry->m_size.m_width;
      const double height = media->m_geometry->m_size.m_height;
      const glm::dvec3 dim = trafo * glm::dvec3(width, height, 0);

      drawMedia(pos[0], pos[1], dim[0], dim[1], mimetype, librevenge::RVNGBinaryData(bytes, size));
    }
  }
}

void IWORKCollector::drawShape(const IWORKShapePtr_t &shape)
{
  if (bool(shape) && bool(shape->m_path))
  {
    const glm::dmat3 trafo = m_levelStack.top().m_trafo;
    IWORKOutputElements &elements = m_outputManager.getCurrent();


    const IWORKPath path = *shape->m_path * trafo;

    librevenge::RVNGPropertyList styleProps;

    if (bool(shape->m_style))
      fillGraphicProps(shape->m_style, styleProps);

    librevenge::RVNGPropertyList shapeProps;

    librevenge::RVNGPropertyListVector vec;
    path.write(vec);
    shapeProps.insert("svg:d", vec);
    fillShapeProperties(shapeProps);

    elements.addSetStyle(styleProps);
    elements.addDrawPath(shapeProps);

    drawTextBox(shape->m_text, trafo, shape->m_geometry);
  }
}

void IWORKCollector::writeFill(const IWORKFill &fill, librevenge::RVNGPropertyList &props)
{
  apply_visitor(FillWriter(props), fill);
}

} // namespace libetonyek

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
