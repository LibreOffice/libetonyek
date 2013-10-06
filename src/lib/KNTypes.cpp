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
#include "KNOutput.h"
#include "KNStyles.h"
#include "KNText.h"
#include "KNTypes.h"

namespace libkeynote
{

KNSize::KNSize()
  : width(0)
  , height(0)
{
}

KNSize::KNSize(const double w, const double h)
  : width(w)
  , height(h)
{
}

KNPosition::KNPosition()
  : x(0)
  , y(0)
{
}

KNPosition::KNPosition(const double x_, const double y_)
  : x(x_)
  , y(y_)
{
}

KNGeometry::KNGeometry()
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

KNColor::KNColor()
  : red()
  , green()
  , blue()
  , alpha()
{
}

KNColor::KNColor(const double r, const double g, const double b, const double a)
  : red(r)
  , green(g)
  , blue(b)
  , alpha(a)
{
}

KNPadding::KNPadding()
  : top()
  , right()
  , bottom()
  , left()
{
}

KNLine::KNLine()
  : geometry()
  , style()
  , x1()
  , y1()
  , x2()
  , y2()
{
}

KNBinary::KNBinary()
  : size()
  , path()
  , type()
  , dataSize()
{
}

KNImage::KNImage()
  : locked()
  , geometry()
  , binary()
{
}

KNMedia::KNMedia()
  : geometry()
  , style()
  , placeholder()
  , placeholderSize()
  , data()
{
}

KNWrap::KNWrap()
  : path()
  , geometry()
{
}

KNGroup::KNGroup()
  : objects()
{
}

KNLayer::KNLayer()
  : type()
  , objects()
{
}

KNPlaceholder::KNPlaceholder()
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

  // TODO: unit conversion
  props.insert("svg:x", x);
  props.insert("svg:y", y);

  return props;
}

}

namespace
{

class GroupObject : public KNObject
{
public:
  explicit GroupObject(const KNGroupPtr_t &group);

private:
  virtual void draw(const KNOutput &output);

private:
  const KNGroupPtr_t m_group;
};

GroupObject::GroupObject(const KNGroupPtr_t &group)
  : m_group(group)
{
}

void GroupObject::draw(const KNOutput &output)
{
  drawAll(m_group->objects, output);
}

}

namespace
{

class ImageObject : public KNObject
{
public:
  explicit ImageObject(const KNImagePtr_t &image);

private:
  virtual void draw(const KNOutput &output);

private:
  const KNImagePtr_t m_image;
};

ImageObject::ImageObject(const KNImagePtr_t &image)
  : m_image(image)
{
}

void ImageObject::draw(const KNOutput &output)
{
  // TODO: implement me
  (void) output;
}

}

namespace
{

class LineObject : public KNObject
{
public:
  explicit LineObject(const KNLinePtr_t &line);

private:
  virtual void draw(const KNOutput &output);

private:
  const KNLinePtr_t m_line;
};

LineObject::LineObject(const KNLinePtr_t &line)
  : m_line(line)
{
}

void LineObject::draw(const KNOutput &output)
{
  // TODO: transform the line

  if (m_line->x1 && m_line->y1 && m_line->x2 && m_line->y2)
  {
    WPXPropertyList props;
#if 0
    if (line->style)
    {
      // TODO: is it graphic style?
      const KNStyleMap_t::const_iterator styleIt = dict.graphicStyles.find(get(line->style));
      if (dict.graphicStyles.end() != styleIt)
      {
        KNStyle style = *styleIt->second;
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
    KN_DEBUG_MSG(("line is missing head or tail point\n"));
  }
}

}

namespace
{

class MediaObject : public KNObject
{
public:
  explicit MediaObject(const KNMediaPtr_t &media);

private:
  virtual void draw(const KNOutput &output);

private:
  const KNMediaPtr_t m_media;
};

MediaObject::MediaObject(const KNMediaPtr_t &media)
  : m_media(media)
{
}

void MediaObject::draw(const KNOutput &output)
{
  // TODO: implement me
  (void) output;
}

}

namespace
{

class PlaceholderObject : public KNObject
{
public:
  explicit PlaceholderObject(const KNPlaceholderPtr_t &body);

private:
  virtual void draw(const KNOutput &output);

private:
  const KNPlaceholderPtr_t m_body;
};

PlaceholderObject::PlaceholderObject(const KNPlaceholderPtr_t &body)
  : m_body(body)
{
}

void PlaceholderObject::draw(const KNOutput &output)
{
  if (bool(m_body) && bool(m_body->style) && bool(m_body->text))
  {
    const KNGeometryPtr_t geometry = m_body->style->getGeometry(output.getStyleContext());
    if (bool(geometry))
    {
      const KNOutput newOutput(output, makeTransformation(*geometry), m_body->style);
      makeObject(m_body->text)->draw(newOutput);
    }
  }
}

}

KNObjectPtr_t makeObject(const KNGroupPtr_t &group)
{
  const KNObjectPtr_t object(new GroupObject(group));
  return object;
}

KNObjectPtr_t makeObject(const KNImagePtr_t &image)
{
  const KNObjectPtr_t object(new ImageObject(image));
  return object;
}

KNObjectPtr_t makeObject(const KNLinePtr_t &line)
{
  const KNObjectPtr_t object(new LineObject(line));
  return object;
}

KNObjectPtr_t makeObject(const KNMediaPtr_t &media)
{
  const KNObjectPtr_t object(new MediaObject(media));
  return object;
}

KNObjectPtr_t makeObject(const KNPlaceholderPtr_t &body)
{
  const KNObjectPtr_t object(new PlaceholderObject(body));
  return object;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
