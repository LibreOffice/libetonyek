/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <libwpd/libwpd.h>

#include <libwpg/libwpg.h>

#include "libkeynote_utils.h"
#include "KEYOutput.h"
#include "KEYStyles.h"
#include "KEYText.h"
#include "KEYTypes.h"

namespace libkeynote
{

KEYSize::KEYSize()
  : width(0)
  , height(0)
{
}

KEYSize::KEYSize(const double w, const double h)
  : width(w)
  , height(h)
{
}

KEYPosition::KEYPosition()
  : x(0)
  , y(0)
{
}

KEYPosition::KEYPosition(const double x_, const double y_)
  : x(x_)
  , y(y_)
{
}

KEYGeometry::KEYGeometry()
  : naturalSize()
  , position()
  , angle()
  , shearXAngle()
  , shearYAngle()
  , horizontalFlip()
  , verticalFlip()
  , aspectRatioLocked()
  , sizesLocked()
{
}

KEYColor::KEYColor()
  : red()
  , green()
  , blue()
  , alpha()
{
}

KEYColor::KEYColor(const double r, const double g, const double b, const double a)
  : red(r)
  , green(g)
  , blue(b)
  , alpha(a)
{
}

KEYPadding::KEYPadding()
  : top()
  , right()
  , bottom()
  , left()
{
}

KEYLine::KEYLine()
  : geometry()
  , style()
  , x1()
  , y1()
  , x2()
  , y2()
{
}

KEYBinary::KEYBinary()
  : size()
  , path()
  , type()
  , dataSize()
{
}

KEYImage::KEYImage()
  : locked()
  , geometry()
  , binary()
{
}

KEYMedia::KEYMedia()
  : geometry()
  , style()
  , placeholder()
  , placeholderSize()
  , content()
{
}

KEYWrap::KEYWrap()
  : path()
  , geometry()
{
}

KEYGroup::KEYGroup()
  : objects()
{
}

KEYLayer::KEYLayer()
  : type()
  , objects()
{
}

KEYPlaceholder::KEYPlaceholder()
  : title()
  , empty()
  , style()
  , text()
{
}

namespace
{

WPXPropertyList pointToWPG(const double x, const double y)
{
  WPXPropertyList props;

  props.insert("svg:x", x, WPX_POINT);
  props.insert("svg:y", y, WPX_POINT);

  return props;
}

}

namespace
{

class GroupObject : public KEYObject
{
public:
  explicit GroupObject(const KEYGroupPtr_t &group);

private:
  virtual void draw(const KEYOutput &output);

private:
  const KEYGroupPtr_t m_group;
};

GroupObject::GroupObject(const KEYGroupPtr_t &group)
  : m_group(group)
{
}

void GroupObject::draw(const KEYOutput &output)
{
  drawAll(m_group->objects, output);
}

}

namespace
{

class ImageObject : public KEYObject
{
public:
  explicit ImageObject(const KEYImagePtr_t &image);

private:
  virtual void draw(const KEYOutput &output);

private:
  const KEYImagePtr_t m_image;
};

ImageObject::ImageObject(const KEYImagePtr_t &image)
  : m_image(image)
{
}

void ImageObject::draw(const KEYOutput &output)
{
  // TODO: implement me
  (void) output;
}

}

namespace
{

class LineObject : public KEYObject
{
public:
  explicit LineObject(const KEYLinePtr_t &line);

private:
  virtual void draw(const KEYOutput &output);

private:
  const KEYLinePtr_t m_line;
};

LineObject::LineObject(const KEYLinePtr_t &line)
  : m_line(line)
{
}

void LineObject::draw(const KEYOutput &output)
{
  // TODO: transform the line

  if (m_line->x1 && m_line->y1 && m_line->x2 && m_line->y2)
  {
    WPXPropertyList props;
#if 0
    if (line->style)
    {
      // TODO: is it graphic style?
      const KEYStyleMap_t::const_iterator styleIt = dict.graphicStyles.find(get(line->style));
      if (dict.graphicStyles.end() != styleIt)
      {
        KEYStyle style = *styleIt->second;
        resolveStyle(style, dict.graphicStyles);
        props = makeLineStyle(style);
      }
    }
#endif
    output.getPainter()->setStyle(props, WPXPropertyListVector());

    WPXPropertyListVector vertices;
    vertices.append(pointToWPG(get(m_line->x1), get(m_line->y1)));
    vertices.append(pointToWPG(get(m_line->x2), get(m_line->y2)));
    output.getPainter()->drawPolyline(vertices);
  }
  else
  {
    KEY_DEBUG_MSG(("line is missing head or tail point\n"));
  }
}

}

namespace
{

class MediaObject : public KEYObject
{
public:
  explicit MediaObject(const KEYMediaPtr_t &media);

private:
  virtual void draw(const KEYOutput &output);

private:
  const KEYMediaPtr_t m_media;
};

MediaObject::MediaObject(const KEYMediaPtr_t &media)
  : m_media(media)
{
}

void MediaObject::draw(const KEYOutput &output)
{
  if (bool(m_media)
      && bool(m_media->geometry)
      && bool(m_media->content)
      && bool(m_media->content->data)
      && bool(m_media->content->data->stream))
  {
    const KEYOutput newOutput(output, makeTransformation(*m_media->geometry));

    const WPXInputStreamPtr_t input = m_media->content->data->stream;

    input->seek(0, WPX_SEEK_END);
    const unsigned long size = input->tell();
    input->seek(0, WPX_SEEK_SET);

    unsigned long readBytes = 0;
    const unsigned char *const bytes = input->read(size, readBytes);
    if (readBytes != size)
      throw GenericException();

    WPXPropertyList props;

    double x = 1;
    double y = 1;
    const KEYTransformation &tr = newOutput.getTransformation();
    tr(x, y);
    props.insert("svx:x", x, WPX_POINT);
    props.insert("svx:y", y, WPX_POINT);

    newOutput.getPainter()->drawGraphicObject(props, WPXBinaryData(bytes, size));
  }
}

}

namespace
{

class PlaceholderObject : public KEYObject
{
public:
  explicit PlaceholderObject(const KEYPlaceholderPtr_t &body);

private:
  virtual void draw(const KEYOutput &output);

private:
  const KEYPlaceholderPtr_t m_body;
};

PlaceholderObject::PlaceholderObject(const KEYPlaceholderPtr_t &body)
  : m_body(body)
{
}

void PlaceholderObject::draw(const KEYOutput &output)
{
  if (bool(m_body) && bool(m_body->style) && bool(m_body->text))
  {
    const KEYGeometryPtr_t geometry = m_body->style->getGeometry(output.getStyleContext());
    if (bool(geometry))
    {
      const KEYOutput newOutput(output, makeTransformation(*geometry), m_body->style);
      makeObject(m_body->text)->draw(newOutput);
    }
  }
}

}

KEYObjectPtr_t makeObject(const KEYGroupPtr_t &group)
{
  const KEYObjectPtr_t object(new GroupObject(group));
  return object;
}

KEYObjectPtr_t makeObject(const KEYImagePtr_t &image)
{
  const KEYObjectPtr_t object(new ImageObject(image));
  return object;
}

KEYObjectPtr_t makeObject(const KEYLinePtr_t &line)
{
  const KEYObjectPtr_t object(new LineObject(line));
  return object;
}

KEYObjectPtr_t makeObject(const KEYMediaPtr_t &media)
{
  const KEYObjectPtr_t object(new MediaObject(media));
  return object;
}

KEYObjectPtr_t makeObject(const KEYPlaceholderPtr_t &body)
{
  const KEYObjectPtr_t object(new PlaceholderObject(body));
  return object;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
