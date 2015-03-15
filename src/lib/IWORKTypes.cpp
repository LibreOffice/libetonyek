/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTypes.h"

#include "IWORKDocumentInterface.h"
#include "IWORKTransformation.h"

namespace libetonyek
{

IWORKSize::IWORKSize()
  : m_width(0)
  , m_height(0)
{
}

IWORKSize::IWORKSize(const double w, const double h)
  : m_width(w)
  , m_height(h)
{
}

IWORKPosition::IWORKPosition()
  : m_x(0)
  , m_y(0)
{
}

IWORKPosition::IWORKPosition(const double x_, const double y_)
  : m_x(x_)
  , m_y(y_)
{
}

IWORKGeometry::IWORKGeometry()
  : m_naturalSize()
  , m_size()
  , m_position()
  , m_angle()
  , m_shearXAngle()
  , m_shearYAngle()
  , m_horizontalFlip()
  , m_verticalFlip()
  , m_aspectRatioLocked()
  , m_sizesLocked()
{
}

IWORKColor::IWORKColor()
  : m_red()
  , m_green()
  , m_blue()
  , m_alpha()
{
}

IWORKColor::IWORKColor(const double r, const double g, const double b, const double a)
  : m_red(r)
  , m_green(g)
  , m_blue(b)
  , m_alpha(a)
{
}

IWORKPadding::IWORKPadding()
  : m_top()
  , m_right()
  , m_bottom()
  , m_left()
{
}

IWORKTabStop::IWORKTabStop(double pos_)
  : m_pos(pos_)
{
}

IWORKLine::IWORKLine()
  : m_geometry()
  , m_style()
  , m_x1()
  , m_y1()
  , m_x2()
  , m_y2()
{
}

IWORKData::IWORKData()
  : m_stream()
  , m_displayName()
  , m_type()
{
}

IWORKMediaContent::IWORKMediaContent()
  : m_size()
  , m_data()
{
}

IWORKBinary::IWORKBinary()
  : m_size()
  , m_path()
  , m_type()
  , m_dataSize()
{
}

IWORKImage::IWORKImage()
  : m_locked()
  , m_geometry()
  , m_binary()
{
}

IWORKMedia::IWORKMedia()
  : m_geometry()
  , m_style()
  , m_placeholder()
  , m_placeholderSize()
  , m_content()
{
}

IWORKWrap::IWORKWrap()
  : m_path()
  , m_geometry()
{
}

IWORKGroup::IWORKGroup()
  : m_objects()
{
}

namespace
{

librevenge::RVNGPropertyList pointToWPG(const double x, const double y)
{
  librevenge::RVNGPropertyList props;

  props.insert("svg:x", pt2in(x));
  props.insert("svg:y", pt2in(y));

  return props;
}

}

namespace
{

class GroupObject : public IWORKObject
{
public:
  explicit GroupObject(const IWORKGroupPtr_t &group);

private:
  virtual void draw(IWORKDocumentInterface *document);

private:
  const IWORKGroupPtr_t m_group;
  const IWORKTransformation m_trafo;
};

GroupObject::GroupObject(const IWORKGroupPtr_t &group)
  : m_group(group)
{
}

void GroupObject::draw(IWORKDocumentInterface *const document)
{
  drawAll(m_group->m_objects, document);
}

}

namespace
{

class ImageObject : public IWORKObject
{
public:
  ImageObject(const IWORKImagePtr_t &image, const IWORKTransformation &trafo);

private:
  virtual void draw(IWORKDocumentInterface *document);

private:
  const IWORKImagePtr_t m_image;
  const IWORKTransformation m_trafo;
};

ImageObject::ImageObject(const IWORKImagePtr_t &image, const IWORKTransformation &trafo)
  : m_image(image)
  , m_trafo(trafo)
{
}

void ImageObject::draw(IWORKDocumentInterface *const document)
{
  // TODO: implement me
  (void) document;
}

}

namespace
{

class LineObject : public IWORKObject
{
public:
  LineObject(const IWORKLinePtr_t &line, const IWORKTransformation &trafo);

private:
  virtual void draw(IWORKDocumentInterface *document);

private:
  const IWORKLinePtr_t m_line;
  const IWORKTransformation m_trafo;
};

LineObject::LineObject(const IWORKLinePtr_t &line, const IWORKTransformation &trafo)
  : m_line(line)
  , m_trafo(trafo)
{
}

void LineObject::draw(IWORKDocumentInterface *const document)
{
  // TODO: transform the line

  if (m_line->m_x1 && m_line->m_y1 && m_line->m_x2 && m_line->m_y2)
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
    document->setStyle(props);

    librevenge::RVNGPropertyListVector vertices;
    vertices.append(pointToWPG(get(m_line->m_x1), get(m_line->m_y1)));
    vertices.append(pointToWPG(get(m_line->m_x2), get(m_line->m_y2)));

    librevenge::RVNGPropertyList points;
    points.insert("svg:points", vertices);

    document->drawPolyline(points);
  }
  else
  {
    ETONYEK_DEBUG_MSG(("line is missing head or tail point\n"));
  }
}

}

IWORKObjectPtr_t makeObject(const IWORKGroupPtr_t &group)
{
  const IWORKObjectPtr_t object(new GroupObject(group));
  return object;
}

IWORKObjectPtr_t makeObject(const IWORKImagePtr_t &image, const IWORKTransformation &trafo)
{
  const IWORKObjectPtr_t object(new ImageObject(image, trafo));
  return object;
}

IWORKObjectPtr_t makeObject(const IWORKLinePtr_t &line, const IWORKTransformation &trafo)
{
  const IWORKObjectPtr_t object(new LineObject(line, trafo));
  return object;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
