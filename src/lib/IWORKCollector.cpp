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
#include <functional>
#include <memory>

#include "IWORKDocumentInterface.h"
#include "IWORKOutputElements.h"
#include "IWORKPath.h"
#include "IWORKProperties.h"
#include "IWORKRecorder.h"
#include "IWORKShape.h"
#include "IWORKTable.h"
#include "IWORKText.h"
#include "libetonyek_utils.h"

namespace libetonyek
{


using librevenge::RVNGPropertyList;
using librevenge::RVNG_PERCENT;
using librevenge::RVNG_POINT;

using namespace std::placeholders;

using std::make_shared;
using std::shared_ptr;

namespace
{
librevenge::RVNGPropertyList makePoint(const double x, const double y)
{
  librevenge::RVNGPropertyList props;

  props.insert("svg:x", pt2in(x));
  props.insert("svg:y", pt2in(y));

  return props;
}

struct FillWriter : public boost::static_visitor<void>
{
  explicit FillWriter(RVNGPropertyList &props)
    : m_props(props), m_opacity(1)
  {
  }

  double getOpacity() const
  {
    return m_opacity;
  }
  void operator()(const IWORKColor &color) const
  {
    m_props.insert("draw:fill", "solid");
    m_props.insert("draw:fill-color", makeColor(color));
    m_opacity=color.m_alpha;
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
      // TODO: store sf:start to retrieve the center position
      m_props.insert("draw:cx", 0.5, RVNG_PERCENT);
      m_props.insert("draw:cy", 0.5, RVNG_PERCENT);
      break;
    default:
      break;
    }
    // TODO: use svg:linearGradient/svg:radialGradient?
    if (gradient.m_stops.front().m_fraction<=0 && gradient.m_stops.back().m_fraction>=1)
    {
      IWORKGradientStop const &start= gradient.m_type==IWORK_GRADIENT_TYPE_LINEAR ? gradient.m_stops.front() : gradient.m_stops.back();
      IWORKGradientStop const &end= gradient.m_type==IWORK_GRADIENT_TYPE_LINEAR ? gradient.m_stops.back() : gradient.m_stops.front();
      m_props.insert("draw:start-color", makeColor(start.m_color));
      m_props.insert("draw:start-intensity", start.m_color.m_alpha, RVNG_PERCENT);
      m_props.insert("draw:end-color", makeColor(end.m_color));
      m_props.insert("draw:end-intensity", end.m_color.m_alpha, RVNG_PERCENT);
    }
    else
    {
      librevenge::RVNGPropertyListVector gradientVector;
      int firstVal=gradient.m_type==IWORK_GRADIENT_TYPE_LINEAR ? 1 : 0;
      for (int s=0; s < 2; ++s)
      {
        IWORKGradientStop const &stop= s==firstVal ? gradient.m_stops.front() : gradient.m_stops.back();
        librevenge::RVNGPropertyList grad;
        grad.insert("svg:offset", firstVal==0 ? stop.m_fraction : 1.-stop.m_fraction, librevenge::RVNG_PERCENT);
        grad.insert("svg:stop-color", makeColor(stop.m_color));
        grad.insert("svg:stop-opacity", stop.m_color.m_alpha, librevenge::RVNG_PERCENT);
        gradientVector.append(grad);
      }
      if (gradient.m_type==IWORK_GRADIENT_TYPE_RADIAL)
        m_props.insert("svg:radialGradient", gradientVector);
      else
        m_props.insert("svg:linearGradient", gradientVector);
    }
    // the axis of the gradient in Keynote is clockwise to the horizontal axis
    m_props.insert("draw:angle", rad2deg(/*etonyek_two_pi - */gradient.m_angle + etonyek_half_pi));
  }

  void operator()(const IWORKMediaContent &bitmap) const
  {
    if (bitmap.m_data && bitmap.m_data->m_stream)
    {
      const unsigned long length = getLength(bitmap.m_data->m_stream);
      unsigned long readBytes = 0;
      bitmap.m_data->m_stream->seek(0, librevenge::RVNG_SEEK_SET);
      const unsigned char *const bytes = bitmap.m_data->m_stream->read(length, readBytes);
      if (readBytes == length)
      {
        m_props.insert("draw:fill", "bitmap");
        m_props.insert("draw:fill-image", librevenge::RVNGBinaryData(bytes, length));
        m_props.insert("librevenge:mime-type", "jpg"); // TODO: fix
        switch (bitmap.m_type)
        {
        case IWORK_IMAGE_TYPE_ORIGINAL_SIZE :
          m_props.insert("style:repeat", "no-repeat");
          break;
        case IWORK_IMAGE_TYPE_STRETCH :
        case IWORK_IMAGE_TYPE_SCALE_TO_FILL :
        case IWORK_IMAGE_TYPE_SCALE_TO_FIT :
          m_props.insert("style:repeat", "stretch");
          break;
        case IWORK_IMAGE_TYPE_TILE :
          m_props.insert("style:repeat", "repeat");
          break;
        default:
          break;
        }
        if (bitmap.m_size)
        {
          m_props.insert("draw:fill-image-width", get(bitmap.m_size).m_width, RVNG_POINT);
          m_props.insert("draw:fill-image-height", get(bitmap.m_size).m_height, RVNG_POINT);
        }
        return;
      }
    }
    // can happen if data is a path to an Iwork's file
    if (bitmap.m_fillColor)
      (*this)(get(bitmap.m_fillColor));
    else
    {
      static bool first=true;
      if (first)
      {
        ETONYEK_DEBUG_MSG(("FillWriter::operator()(IWORKMediaContent)[IWORKCollector.cpp]: can not retrieve some pictures\n"));
        first=false;
      }
      m_props.insert("draw:fill", "none");
    }
  }

private:
  RVNGPropertyList &m_props;
  //! the opacity
  mutable double m_opacity;
};

}

IWORKCollector::Level::Level()
  : m_geometry()
  , m_graphicStyle()
  , m_trafo(1)
  , m_previousTrafo(1)
{
}

IWORKCollector::IWORKCollector(IWORKDocumentInterface *const document)
  : m_document(document)
  , m_recorder()
  , m_levelStack()
  , m_styleStack()
  , m_stylesheetStack()
  , m_outputManager()
  , m_newStyles()
  , m_currentTable()
  , m_currentText()
  , m_headers()
  , m_footers()
  , m_pathStack()
  , m_currentPath()
  , m_attachmentStack()
  , m_inAttachment(false)
  , m_inAttachments(false)
  , m_currentData()
  , m_currentUnfiltered()
  , m_currentFiltered()
  , m_currentLeveled()
  , m_currentContent()
  , m_metadata()
  , m_accumulateTransform(true)
  , m_groupLevel(0)
  , m_groupOpenLevel(0)
{
}

IWORKCollector::~IWORKCollector()
{
  assert(m_levelStack.empty());
  assert(m_stylesheetStack.empty());
  assert(0 == m_groupLevel && 0 == m_groupOpenLevel);

  assert(!m_currentPath);
  assert(!m_currentText);
}

void IWORKCollector::setRecorder(const std::shared_ptr<IWORKRecorder> &recorder)
{
  m_recorder = recorder;
}

void IWORKCollector::collectStyle(const IWORKStylePtr_t &style)
{
  if (bool(m_recorder))
  {
    m_recorder->collectStyle(style);
    return;
  }

  if (bool(style))
    m_newStyles.push_back(style);
}

void IWORKCollector::setGraphicStyle(const IWORKStylePtr_t &style)
{
  if (bool(m_recorder))
  {
    m_recorder->setGraphicStyle(style);
    return;
  }

  if (!m_levelStack.empty())
  {
    m_levelStack.top().m_graphicStyle = style;
    m_styleStack.set(style);
  }
}

void IWORKCollector::setAccumulateTransformTo(bool accumulate)
{
  m_accumulateTransform=accumulate;
}

void IWORKCollector::collectGeometry(const IWORKGeometryPtr_t &geometry)
{
  if (bool(m_recorder))
  {
    m_recorder->collectGeometry(geometry);
    return;
  }

  assert(!m_levelStack.empty());

  m_levelStack.top().m_geometry = geometry;
  m_levelStack.top().m_previousTrafo = m_levelStack.top().m_trafo;
  if (m_accumulateTransform)
    m_levelStack.top().m_trafo *= makeTransformation(*geometry);
  else
    m_levelStack.top().m_trafo = makeTransformation(*geometry);
}

void IWORKCollector::collectBezier(const IWORKPathPtr_t &path)
{
  if (bool(m_recorder))
    m_recorder->collectPath(path);
  else
  {
    m_currentPath = path;
    if (m_currentPath) m_currentPath->closePath(true);
  }
}

void IWORKCollector::collectImage(const IWORKMediaContentPtr_t &image, const IWORKGeometryPtr_t &cropGeometry, const boost::optional<int> &order, bool locked)
{
  if (bool(m_recorder))
  {
    m_recorder->collectImage(image, cropGeometry, order, locked);
    return;
  }

  assert(!m_levelStack.empty());

  const IWORKMediaPtr_t media(new IWORKMedia());
  media->m_geometry = m_levelStack.top().m_geometry;
  media->m_cropGeometry = cropGeometry;
  media->m_order = order;
  media->m_locked = locked;
  media->m_style = m_levelStack.top().m_graphicStyle;
  media->m_content = image;
  m_levelStack.top().m_geometry.reset();
  m_levelStack.top().m_graphicStyle.reset();

  drawMedia(media);
}

void IWORKCollector::collectLine(const IWORKLinePtr_t &line)
{
  if (bool(m_recorder))
  {
    m_recorder->collectLine(line);
    return;
  }

  assert(!m_levelStack.empty());

  line->m_geometry = m_levelStack.top().m_geometry;
  m_levelStack.top().m_geometry.reset();
  line->m_style = m_levelStack.top().m_graphicStyle;
  m_levelStack.top().m_graphicStyle.reset();

  drawLine(line);
}

void IWORKCollector::collectShape(const boost::optional<int> &order, const boost::optional<unsigned> &resizeFlags, bool locked)
{
  if (bool(m_recorder))
  {
    m_recorder->collectShape(order, resizeFlags, locked);
    return;
  }

  assert(!m_levelStack.empty());

  const IWORKShapePtr_t shape(new IWORKShape());

  if (!m_currentPath)
  {
    ETONYEK_DEBUG_MSG(("IWORKCollector::collectShape: the path is empty\n"));
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

  shape->m_order = order;
  shape->m_resizeFlags=resizeFlags;
  shape->m_locked = locked;
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
  const IWORKPathPtr_t path(makePolygonPath(size, edges));
  if (bool(m_recorder))
    m_recorder->collectPath(path);
  else
    m_currentPath = path;
}

void IWORKCollector::collectRoundedRectanglePath(const IWORKSize &size, const double radius)
{
  const IWORKPathPtr_t path(makeRoundedRectanglePath(size, radius));
  if (bool(m_recorder))
    m_recorder->collectPath(path);
  else
    m_currentPath = path;
}

void IWORKCollector::collectArrowPath(const IWORKSize &size, const double headWidth, const double stemRelYPos, bool const doubleSided)
{
  IWORKPathPtr_t path;
  if (doubleSided)
    path = makeDoubleArrowPath(size, headWidth, stemRelYPos);
  else
    path = makeArrowPath(size, headWidth, stemRelYPos);
  if (bool(m_recorder))
    m_recorder->collectPath(path);
  else
    m_currentPath = path;
}

void IWORKCollector::collectStarPath(const IWORKSize &size, const unsigned points, const double innerRadius)
{
  const IWORKPathPtr_t path(makeStarPath(size, points, innerRadius));
  if (bool(m_recorder))
    m_recorder->collectPath(path);
  else
    m_currentPath = path;
}

void IWORKCollector::collectConnectionPath(const IWORKConnectionPath &cPath)
{
  const IWORKPathPtr_t path=cPath.getPath();
  if (bool(m_recorder))
    m_recorder->collectPath(path);
  else
    m_currentPath = path;
}

void IWORKCollector::collectCalloutPath(const IWORKSize &size, const double radius, const double tailSize, const double tailX, const double tailY, bool quoteBubble)
{
  IWORKPathPtr_t path;
  if (quoteBubble)
    path = makeQuoteBubblePath(size, radius, tailSize, tailX, tailY);
  else
    path = makeCalloutPath(size, radius, tailSize, tailX, tailY);
  if (bool(m_recorder))
    m_recorder->collectPath(path);
  else
    m_currentPath = path;
}

void IWORKCollector::collectMedia(const IWORKMediaContentPtr_t &content, const IWORKGeometryPtr_t &cropGeometry, const boost::optional<int> &order)
{
  if (bool(m_recorder))
  {
    m_recorder->collectMedia(content, cropGeometry, order);
    return;
  }

  assert(!m_levelStack.empty());

  const IWORKMediaPtr_t media(new IWORKMedia());
  media->m_geometry = m_levelStack.top().m_geometry;
  media->m_cropGeometry = cropGeometry;
  media->m_style = m_levelStack.top().m_graphicStyle;
  media->m_order = order;
  media->m_content = content;

  m_levelStack.top().m_geometry.reset();
  m_levelStack.top().m_graphicStyle.reset();

  drawMedia(media);
}

void IWORKCollector::collectStylesheet(const IWORKStylesheetPtr_t &stylesheet)
{
  if (bool(m_recorder))
  {
    m_recorder->collectStylesheet(stylesheet);
    return;
  }

  for_each(m_newStyles.begin(), m_newStyles.end(), std::bind(&IWORKStyle::link, _1, stylesheet));
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

void IWORKCollector::collectTable(const std::shared_ptr<IWORKTable> &table)
{
  if (bool(m_recorder))
  {
    m_recorder->collectTable(table);
    return;
  }

  assert(!m_currentTable);
  m_currentTable = table;
  drawTable();
  m_currentTable.reset();
}

void IWORKCollector::collectText(const std::shared_ptr<IWORKText> &text)
{
  if (bool(m_recorder))
  {
    m_recorder->collectText(text);
    return;
  }

  assert(!m_currentText);
  m_currentText = text;
}

void IWORKCollector::collectStickyNote()
{
  ETONYEK_DEBUG_MSG(("IWORKCollector::collectStickyNote: not implemented\n"));
}

void IWORKCollector::startDocument(const librevenge::RVNGPropertyList &props)
{
  m_document->startDocument(props);
}

void IWORKCollector::endDocument()
{
  assert(m_levelStack.empty());
  assert(m_pathStack.empty());
  assert(0 == m_groupLevel && 0 == m_groupOpenLevel);

  assert(!m_currentPath);
  assert(!m_currentText);

  m_document->endDocument();
}

void IWORKCollector::startGroup()
{
  if (bool(m_recorder))
  {
    m_recorder->startGroup();
    return;
  }

  ++m_groupLevel;
}

void IWORKCollector::endGroup()
{
  if (bool(m_recorder))
  {
    m_recorder->endGroup();
    return;
  }

  assert(m_groupLevel > 0);

  --m_groupLevel;
}

void IWORKCollector::openGroup()
{
  if (bool(m_recorder))
  {
    m_recorder->openGroup();
    return;
  }

  assert(m_groupLevel > 0);
  m_outputManager.getCurrent().addOpenGroup(librevenge::RVNGPropertyList());
  ++m_groupOpenLevel;
}

void IWORKCollector::closeGroup()
{
  if (bool(m_recorder))
  {
    m_recorder->closeGroup();
    return;
  }
  assert(m_groupLevel > 0 && m_groupOpenLevel > 0);
  m_outputManager.getCurrent().addCloseGroup();
  --m_groupOpenLevel;
}

std::shared_ptr<IWORKTable> IWORKCollector::createTable(const IWORKTableNameMapPtr_t &tableNameMap, IWORKFormatNameMap &formatNameMap, const IWORKLanguageManager &langManager) const
{
  return shared_ptr<IWORKTable>(new IWORKTable(tableNameMap, formatNameMap, langManager));
}

std::shared_ptr<IWORKText> IWORKCollector::createText(const IWORKLanguageManager &langManager, bool discardEmptyContent, bool allowListInsertion) const
{
  return make_shared<IWORKText>(langManager, discardEmptyContent, allowListInsertion);
}

void IWORKCollector::startLevel()
{
  if (bool(m_recorder))
  {
    m_recorder->startLevel();
    return;
  }

  glm::dmat3 currentTrafo(1), prevTrafo(1);
  if (!m_levelStack.empty())
  {
    currentTrafo = m_levelStack.top().m_trafo;
    prevTrafo = m_levelStack.top().m_previousTrafo;
  }
  m_levelStack.push(Level());
  m_levelStack.top().m_trafo = currentTrafo;
  m_levelStack.top().m_previousTrafo = prevTrafo;

  pushStyle();
}

void IWORKCollector::endLevel()
{
  if (bool(m_recorder))
  {
    m_recorder->endLevel();
    return;
  }

  assert(!m_levelStack.empty());
  m_levelStack.pop();

  popStyle();
}

void IWORKCollector::startAttachment()
{
  if (bool(m_recorder))
  {
    m_recorder->startAttachment();
    return;
  }

  m_attachmentStack.push(m_inAttachment);
  m_inAttachment=true;

  m_pathStack.push(m_currentPath);
  m_currentPath.reset();
  startLevel();
}

void IWORKCollector::endAttachment()
{
  if (bool(m_recorder))
  {
    m_recorder->endAttachment();
    return;
  }

  assert(!m_attachmentStack.empty());
  if (!m_attachmentStack.empty())
  {
    m_inAttachment=m_attachmentStack.top();
    m_attachmentStack.pop();
  }
  assert(!m_pathStack.empty());
  if (!m_pathStack.empty())
  {
    m_currentPath=m_pathStack.top();
    m_pathStack.pop();
  }
  endLevel();
}

void IWORKCollector::startAttachments()
{
  assert(!m_inAttachments);
  m_inAttachments = true;
}

void IWORKCollector::endAttachments()
{
  assert(m_inAttachments);
  m_inAttachments = false;
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
  if (bool(m_recorder))
  {
    m_recorder->pushStylesheet(stylesheet);
    return;
  }

  m_stylesheetStack.push(stylesheet);
}

void IWORKCollector::popStylesheet()
{
  if (bool(m_recorder))
  {
    m_recorder->popStylesheet();
    return;
  }

  assert(!m_stylesheetStack.empty());

  m_stylesheetStack.pop();
}

void IWORKCollector::collectHeaderFooter(const std::string &name, IWORKHeaderFooterMap_t &map)
{
  IWORKOutputElements &elements = map[name];
  if (!elements.empty())
  {
    ETONYEK_DEBUG_MSG(("IWORKCollector::collectHeaderFooter: '%s' already exists, overwriting\n", name.c_str()));
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
    props.insert("meta:initial-creator", m_metadata.m_author.c_str());
  if (!m_metadata.m_keywords.empty())
    props.insert("meta:keyword", m_metadata.m_keywords.c_str());
  if (!m_metadata.m_comment.empty())
    props.insert("librevenge:comments", m_metadata.m_comment.c_str());
}

void IWORKCollector::fillGraphicProps(const IWORKStylePtr_t style, librevenge::RVNGPropertyList &props,
                                      bool isSurface, bool isFrame)
{
  assert(bool(style));

  using namespace property;

  double opacity=style->has<Opacity>() ? style->get<Opacity>() : 1.;
  if (isSurface && style->has<Fill>())
  {
    FillWriter fillWriter(props);
    apply_visitor(fillWriter, style->get<Fill>());
    opacity*=fillWriter.getOpacity();
  }
  else
    props.insert("draw:fill", "none");

  if (style->has<Stroke>())
  {
    const IWORKStroke &stroke = style->get<Stroke>();
    IWORKStrokeType type = stroke.m_pattern.m_type;
    if ((type == IWORK_STROKE_TYPE_DASHED) && stroke.m_pattern.m_values.size() < 2)
      type = IWORK_STROKE_TYPE_SOLID;

    if (isFrame)
    {
      std::string bType("none");
      switch (type)
      {
      case IWORK_STROKE_TYPE_NONE :
        break;
      case IWORK_STROKE_TYPE_SOLID :
        bType="solid";
        break;
      case IWORK_STROKE_TYPE_DASHED :
        bType=(stroke.m_pattern.m_values[0]<0.1 ? "dotted" : "dashed");
        break;
      case IWORK_STROKE_TYPE_AUTO :
        bType=style->has<Fill>() ? "none" : "solid";
        break;
      default:
        ETONYEK_DEBUG_MSG(("IWORKCollector::fillGraphicProps: unexpected stroke type\n"));
        break;
      }
      if (bType!="none")
      {
        std::stringstream s;
        s << stroke.m_width << "pt " << bType << " " << makeColor(stroke.m_color).cstr();
        props.insert("fo:border", s.str().c_str());
      }
      else
        props.insert("fo:border", "none");
    }
    else
    {
      switch (type)
      {
      case IWORK_STROKE_TYPE_NONE :
        props.insert("draw:stroke", "none");
        break;
      case IWORK_STROKE_TYPE_SOLID :
        props.insert("draw:stroke", "solid");
        break;
      case IWORK_STROKE_TYPE_DASHED :
        props.insert("draw:stroke", "dash");
        props.insert("draw:dots1", 1);
        props.insert("draw:dots1-length", stroke.m_pattern.m_values[0], RVNG_PERCENT);
        props.insert("draw:dots2", 1);
        props.insert("draw:dots2-length", stroke.m_pattern.m_values[0], RVNG_PERCENT);
        props.insert("draw:distance", stroke.m_pattern.m_values[1], RVNG_PERCENT);
        break;
      case IWORK_STROKE_TYPE_AUTO :
        if (style->has<Fill>())
          props.insert("draw:stroke", "none");
        else
          props.insert("draw:stroke", "solid");
        break;
      default:
        ETONYEK_DEBUG_MSG(("IWORKCollector::fillGraphicProps: unexpected stroke type\n"));
        break;
      }

      props.insert("svg:stroke-width", pt2in(stroke.m_width));
      props.insert("svg:stroke-color", makeColor(stroke.m_color));

      switch (stroke.m_cap)
      {
      default :
      case IWORK_LINE_CAP_NONE :

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
      case IWORK_LINE_JOIN_ROUND :
        props.insert("svg:stroke-linejoin", "round");
        break;
      case IWORK_LINE_JOIN_NONE :
      default :
        props.insert("svg:stroke-linejoin", "none");
      }
    }
  }

  for (int marker=0; marker<2; ++marker)
  {
    if ((marker==0 && !style->has<TailLineEnd>()) || (marker==1 && !style->has<HeadLineEnd>()))
      continue;
    const IWORKMarker &lineEnd=marker==0 ? style->get<TailLineEnd>() : style->get<HeadLineEnd>();
    try
    {
      if (lineEnd.m_path)
      {
        IWORKPathPtr_t path;
        path = std::make_shared<IWORKPath>(get(lineEnd.m_path));
        /*if (lineEnd.m_filled) path->closePath(false); */
        (*path)*=glm::dmat3(-1,0,0,0,-1,0,0,0,1);
        std::string finalStr=path->str();
        double bdbox[4];
        path->computeBoundingBox(bdbox[0],bdbox[1],bdbox[2],bdbox[3]);
        if (!finalStr.empty())
        {
          props.insert(marker==0 ? "draw:marker-start-path" : "draw:marker-end-path", finalStr.c_str());
          std::stringstream s;
          // viewbox's componant must be integer...
          s << std::floor(bdbox[0]) << " " << std::floor(bdbox[1]) << " " << std::ceil(bdbox[2]) << " " << std::ceil(bdbox[3]);
          props.insert(marker==0 ? "draw:marker-start-viewbox" : "draw:marker-end-viewbox", s.str().c_str());
          if (lineEnd.m_scale>0 && bdbox[2]>bdbox[0]) // unsure
            props.insert(marker==0 ? "draw:marker-start-width" : "draw:marker-end-width",
                         1.5*(bdbox[2]-bdbox[0])*lineEnd.m_scale, librevenge::RVNG_POINT);
          props.insert(marker==0 ? "draw:marker-start-center" : "draw:marker-end-center", true);
        }
      }
    }
    catch (const IWORKPath::InvalidException &)
    {
      ETONYEK_DEBUG_MSG(("IWORKCollector::fillGraphicProps: '%s' is not a valid path\n", get(lineEnd.m_path).c_str()));
    }
  }
  if (style->has<Shadow>())
  {
    const IWORKShadow &shadow = style->get<Shadow>();
    props.insert("draw:shadow", shadow.m_visible ? "visible" : "hidden");
    props.insert("draw:shadow-color", makeColor(shadow.m_color));
    props.insert("draw:shadow-opacity", shadow.m_opacity, RVNG_PERCENT);
    const double angle = deg2rad(shadow.m_angle);
    props.insert("draw:shadow-offset-x", shadow.m_offset * std::cos(angle), RVNG_POINT);
    props.insert("draw:shadow-offset-y", shadow.m_offset * std::sin(angle), RVNG_POINT);
  }

  if (opacity<1)
  {
    props.insert("draw:opacity", opacity, RVNG_PERCENT);
    props.insert("draw:image-opacity", opacity, RVNG_PERCENT);
  }
}

void IWORKCollector::fillLayoutProps(const IWORKStylePtr_t layoutStyle, librevenge::RVNGPropertyList &props)
{
  if (!layoutStyle) return;
  if (layoutStyle->has<property::VerticalAlignment>())
  {
    const IWORKVerticalAlignment align = layoutStyle->get<property::VerticalAlignment>();
    switch (align)
    {
    case IWORK_VERTICAL_ALIGNMENT_TOP:
      props.insert("draw:textarea-vertical-align", "top");
      break;
    case IWORK_VERTICAL_ALIGNMENT_MIDDLE:
      props.insert("draw:textarea-vertical-align", "middle");
      break;
    case IWORK_VERTICAL_ALIGNMENT_BOTTOM:
      props.insert("draw:textarea-vertical-align", "bottom");
      break;
    default:
      ETONYEK_DEBUG_MSG(("IWORKCollector::fillLayoutProps: find unknown vertical alignment\n"));
      break;
    }
  }
  if (layoutStyle->has<property::Padding>())
  {
    const IWORKPadding padding = layoutStyle->get<property::Padding>();
    if (padding.m_bottom)
      props.insert("fo:padding-bottom", get(padding.m_bottom)>0 ? get(padding.m_bottom) : 0, librevenge::RVNG_POINT);
    if (padding.m_left)
      props.insert("fo:padding-left", get(padding.m_left)>0 ? get(padding.m_left) : 0, librevenge::RVNG_POINT);
    if (padding.m_right)
      props.insert("fo:padding-right", get(padding.m_right)>0 ? get(padding.m_right) : 0, librevenge::RVNG_POINT);
    if (padding.m_top)
      props.insert("fo:padding-top", get(padding.m_top)>0 ? get(padding.m_top)>0 : 0, librevenge::RVNG_POINT);
  }
}

void IWORKCollector::fillTextAutoSizeProps(const boost::optional<unsigned> &resizeFlags, const IWORKGeometryPtr_t &boundingBox, librevenge::RVNGPropertyList &props)
{
  if (!resizeFlags) return;
  unsigned const flags=get(resizeFlags)&3;
  // checkme: set "draw:auto-grow-width" to true seems to create too many problems
  if ((flags&1)==1 && boundingBox && boundingBox->m_naturalSize.m_width>0)
    props.insert("draw:auto-grow-width",false);
  if ((flags&2)==0)
    props.insert("draw:auto-grow-height",true);
  else if (boundingBox && boundingBox->m_naturalSize.m_height>0)
  {
    props.insert("draw:auto-grow-height",false);
    // if ((flags&1)==0) props.insert("draw:auto-grow-width",true); checkme

    // the following seems only use in Impress
    props.insert("draw:fit-to-size",true);
    props.insert("style:shrink-to-fit",true);
  }
}

void IWORKCollector::fillWrapProps(const IWORKStylePtr_t style, librevenge::RVNGPropertyList &props,
                                   const boost::optional<int> &order)
{
  if (order)
  {
    if (get(order)>=0)
      props.insert("draw:z-index", get(order)+1);
    else   // background
    {
      props.insert("draw:z-index", -get(order));
      props.insert("style:wrap", "run-through");
      props.insert("style:run-through", "background");
      return;
    }
  }
  if (!bool(style) || !style->has<property::ExternalTextWrap>())
    return;
  const IWORKExternalTextWrap &wrap = style->get<property::ExternalTextWrap>();
  switch (wrap.m_floatingType)
  {
  case IWORK_WRAP_TYPE_DIRECTIONAL :
    switch (wrap.m_direction)
    {
    case IWORK_WRAP_DIRECTION_BOTH :
      if (wrap.m_aligned)
        props.insert("style:wrap", "parallel");
      else
        props.insert("style:wrap", "dynamic");
      break;
    case IWORK_WRAP_DIRECTION_LEFT :
      props.insert("style:wrap", "left");
      break;
    case IWORK_WRAP_DIRECTION_RIGHT :
      props.insert("style:wrap", "right");
      break;
    default:
      ETONYEK_DEBUG_MSG(("IWORKCollector::fillWrapProps: unknown direction\n"));
    }
    break;
  case IWORK_WRAP_TYPE_LARGEST :
    props.insert("style:wrap", "biggest");
    break;
  case IWORK_WRAP_TYPE_NEITHER :
    props.insert("style:wrap", "none");
    break;
  default:
    ETONYEK_DEBUG_MSG(("IWORKCollector::fillWrapProps: unknown wrap type\n"));
  }
}

IWORKOutputManager &IWORKCollector::getOutputManager()
{
  return m_outputManager;
}

void IWORKCollector::drawLine(const IWORKLinePtr_t &line)
{
  // TODO: transform the line
  IWORKOutputElements &elements = m_outputManager.getCurrent();
  double origPos[2], endPos[2];
  if (line->m_x1 && line->m_y1 && line->m_x2 && line->m_y2)
  {
    origPos[0]=get(line->m_x1);
    origPos[1]=get(line->m_y1);
    endPos[0]=get(line->m_x2);
    endPos[1]=get(line->m_y2);
  }
  else if (line->m_geometry && !line->m_x1 && !line->m_y1 && !line->m_x2 && !line->m_y2)
  {
    IWORKGeometry const &geometry=*line->m_geometry;
    origPos[0]=geometry.m_position.m_x;
    origPos[1]=geometry.m_position.m_y;
    double dir[2]= {geometry.m_size.m_width, geometry.m_size.m_height};
    double finalDir[2];
    if (geometry.m_angle)
    {
      const double angle = get(geometry.m_angle);
      double c = std::cos(angle);
      double s = std::sin(angle);
      finalDir[0]=dir[0]*c-dir[1]*s;
      finalDir[1]=dir[0]*s+dir[1]*c;
    }
    else
    {
      finalDir[0]=dir[0];
      finalDir[1]=dir[1];
    }
    // geometry.m_position is always the boundary top left point
    if (finalDir[0]<0)
    {
      endPos[0]=origPos[0];
      origPos[0]=endPos[0]-finalDir[0];
    }
    else
      endPos[0]=origPos[0]+finalDir[0];
    if (finalDir[1]<0)
    {
      endPos[1]=origPos[1];
      origPos[1]=endPos[1]-finalDir[1];
    }
    else
      endPos[1]=origPos[1]+finalDir[1];
    if (m_accumulateTransform)
    {
      const glm::dmat3 trafo = m_levelStack.top().m_previousTrafo;
      glm::dvec3 pos = trafo * glm::dvec3(origPos[0], origPos[1], 1);
      origPos[0]=pos[0];
      origPos[1]=pos[1];
      pos = trafo * glm::dvec3(endPos[0], endPos[1], 1);
      endPos[0]=pos[0];
      endPos[1]=pos[1];
    }
  }
  else
  {
    ETONYEK_DEBUG_MSG(("drawLine[IWORKCollector]: line is missing head or tail point\n"));
    return;
  }
  librevenge::RVNGPropertyList props;
  if (bool(line->m_style))
    fillGraphicProps(line->m_style, props, false);
  elements.addSetStyle(props);

  librevenge::RVNGPropertyListVector vertices;
  vertices.append(makePoint(origPos[0],origPos[1]));
  vertices.append(makePoint(endPos[0],endPos[1]));

  librevenge::RVNGPropertyList points;
  points.insert("svg:points", vertices);
  fillShapeProperties(points);

  elements.addDrawPolyline(points);
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

    std::string mimetype(media->m_content->m_data->m_mimeType);
    if (mimetype.empty())
      mimetype = detectMimetype(input);
    if (!mimetype.empty())
    {
      input->seek(0, librevenge::RVNG_SEEK_END);
      const auto size = (unsigned long) input->tell();
      input->seek(0, librevenge::RVNG_SEEK_SET);

      unsigned long readBytes = 0;
      const unsigned char *const bytes = input->read(size, readBytes);
      if (readBytes != size)
        throw GenericException();

      librevenge::RVNGPropertyList props;
      glm::dvec3 pos = trafo * glm::dvec3(0, 0, 1);
      glm::dvec3 dim = trafo * glm::dvec3(media->m_geometry->m_size.m_width, media->m_geometry->m_size.m_height, 0);
      if (media->m_cropGeometry)
      {
        /* cropping seems to pose problem to LibreOffice because
           sometimes it does not use the real picture size to clip
           the picture (or I make some mistakes).

           So for now, we only reset the origin and resize the picture to its final size */
        pos = glm::dvec3(media->m_cropGeometry->m_position.m_x, media->m_cropGeometry->m_position.m_y, 1);
        dim = glm::dvec3(media->m_cropGeometry->m_size.m_width, media->m_cropGeometry->m_size.m_height, 0);
        if (m_accumulateTransform)
        {
          pos = m_levelStack.top().m_previousTrafo * pos;
          dim = trafo * dim;
        }
      }

      // check if the image is flipped, ...
      if (dim[0]<0 && dim[1]<0)
      {
        props.insert("librevenge:rotate", 180, librevenge::RVNG_GENERIC);
        pos[0]+=dim[0];
        pos[1]+=dim[1];
        dim[0]*=-1;
        dim[1]*=-1;
      }
      else if (dim[0]<0)
      {
        props.insert("draw:mirror-horizontal", true);
        pos[0]+=dim[0];
        dim[0]*=-1;
      }
      else if (dim[1]<0)
      {
        props.insert("draw:mirror-vertical", true);
        pos[1]+=dim[1];
        dim[1]*=-1;
      }

      if (bool(media->m_style))
      {
        fillGraphicProps(media->m_style, props, false, false);
        fillWrapProps(media->m_style, props, media->m_order);
      }
      props.insert("librevenge:mime-type", mimetype.c_str());
      props.insert("office:binary-data", librevenge::RVNGBinaryData(bytes, size));
      props.insert("svg:width", pt2in(dim[0]));
      props.insert("svg:height", pt2in(dim[1]));
      drawMedia(pos[0], pos[1], props);
    }
  }
}

void IWORKCollector::drawShape(const IWORKShapePtr_t &shape)
{
  if (!bool(shape) || !bool(shape->m_path))
  {
    ETONYEK_DEBUG_MSG(("IWORKCollector::drawShape: can not find the shape\n"));
    return;
  }
  const glm::dmat3 trafo = m_levelStack.top().m_trafo;
  IWORKOutputElements &elements = m_outputManager.getCurrent();

  const IWORKPath path = *shape->m_path * trafo;
  bool isRectangle=path.isRectangle();
  bool hasText=bool(shape->m_text) && !shape->m_text->empty();
  bool createOnlyTextbox= hasText && isRectangle;
  librevenge::RVNGPropertyList styleProps;

  if (bool(shape->m_style))
  {
    fillGraphicProps(shape->m_style, styleProps, true, createOnlyTextbox && createFrameStylesForTextBox());
    fillWrapProps(shape->m_style, styleProps, shape->m_order);
  }
  if (shape->m_locked) // CHECKME: maybe also content
    styleProps.insert("style:protect", "position size");
  if (createOnlyTextbox)
  {
    IWORKStylePtr_t layoutStyle=shape->m_text->getLayoutStyle();
    if (!layoutStyle && bool(shape->m_style) && shape->m_style->has<property::LayoutStyle>())
      layoutStyle=shape->m_style->get<property::LayoutStyle>();
    fillLayoutProps(layoutStyle, styleProps);
    fillTextAutoSizeProps(shape->m_resizeFlags,shape->m_geometry,styleProps);
    return drawTextBox(shape->m_text, trafo, shape->m_geometry, styleProps);
  }

  librevenge::RVNGPropertyList shapeProps;
  librevenge::RVNGPropertyListVector vec;
  path.write(vec);
  shapeProps.insert("svg:d", vec);
  fillShapeProperties(shapeProps);

  elements.addSetStyle(styleProps);
  elements.addDrawPath(shapeProps);

  if (hasText)
  {
    librevenge::RVNGPropertyList props;
    IWORKStylePtr_t layoutStyle=shape->m_text->getLayoutStyle();
    if (!layoutStyle && bool(shape->m_style) && shape->m_style->has<property::LayoutStyle>())
      layoutStyle=shape->m_style->get<property::LayoutStyle>();
    fillLayoutProps(layoutStyle, props);
    fillTextAutoSizeProps(shape->m_resizeFlags,shape->m_geometry,props);
    drawTextBox(shape->m_text, trafo, shape->m_geometry, props);
  }
}

void IWORKCollector::writeFill(const IWORKFill &fill, librevenge::RVNGPropertyList &props)
{
  apply_visitor(FillWriter(props), fill);
}

} // namespace libetonyek

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
