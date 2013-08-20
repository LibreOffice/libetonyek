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
#include "KNDictionary.h"
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
  , size()
  , position()
  , angle(0)
  , shearXAngle(0)
  , shearYAngle(0)
  , horizontalFlip(false)
  , verticalFlip(false)
  , aspectRatioLocked(false)
  , sizesLocked(false)
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

KNPoint::KNPoint()
  : x(0)
  , y(0)
{
}

KNLine::KNLine()
  : geometry()
  , head()
  , tail()
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
  , cropGeometry()
  , data()
{
}

KNWrap::KNWrap()
  : path()
  , geometry()
{
}

KNGroup::KNGroup()
  : geometries()
  , groups()
  , images()
  , lines()
  , media()
  , shapes()
  , wraps()
{
}

KNLayer::KNLayer(const KNObjectList_t &objectList)
  : type()
  , objects(objectList)
{
}

namespace
{

WPXPropertyList toWPG(const KNPoint &point)
{
  WPXPropertyList props;

  // TODO: unit conversion
  props.insert("svg:x", point.x);
  props.insert("svg:y", point.y);

  return props;
}

}

namespace
{

class GroupObject : public KNObject
{
public:
  explicit GroupObject(const ID_t &id);

private:
  virtual void draw(libwpg::WPGPaintInterface *painter, const KNDictionary &dict, const KNTransformation &tr);

private:
  const ID_t m_id;
};

GroupObject::GroupObject(const ID_t &id)
  : m_id(id)
{
}

void GroupObject::draw(libwpg::WPGPaintInterface *const painter, const KNDictionary &dict, const KNTransformation &tr)
{
  // TODO: implement me
  (void) painter;
  (void) dict;
  (void) tr;
}

}

namespace
{

class ImageObject : public KNObject
{
public:
  explicit ImageObject(const ID_t &id);

private:
  virtual void draw(libwpg::WPGPaintInterface *painter, const KNDictionary &dict, const KNTransformation &tr);

private:
  const ID_t m_id;
};

ImageObject::ImageObject(const ID_t &id)
  : m_id(id)
{
}

void ImageObject::draw(libwpg::WPGPaintInterface *const painter, const KNDictionary &dict, const KNTransformation &tr)
{
  // TODO: implement me
  (void) painter;
  (void) dict;
  (void) tr;
}

}

namespace
{

class LineObject : public KNObject
{
public:
  explicit LineObject(const ID_t &id);

private:
  virtual void draw(libwpg::WPGPaintInterface *painter, const KNDictionary &dict, const KNTransformation &tr);

private:
  const ID_t m_id;
};

LineObject::LineObject(const ID_t &id)
  : m_id(id)
{
}

void LineObject::draw(libwpg::WPGPaintInterface *const painter, const KNDictionary &dict, const KNTransformation &tr)
{
  // TODO: transform the line
  (void) tr;

  const KNLineMap_t::const_iterator it = dict.lines.find(m_id);
  if (dict.lines.end() == it)
  {
    KN_DEBUG_MSG(("line %s not found\n", m_id.c_str()));
  }
  else
  {
    const KNLine &line = it->second;
    if (line.head && line.tail)
    {
      WPXPropertyListVector vertices;
      vertices.append(toWPG(get(line.head)));
      vertices.append(toWPG(get(line.tail)));
      painter->drawPolyline(vertices);
    }
    else
    {
      KN_DEBUG_MSG(("line %s is missing head or tail point\n", m_id.c_str()));
    }
  }
}

}

namespace
{

class MediaObject : public KNObject
{
public:
  explicit MediaObject(const ID_t &id);

private:
  virtual void draw(libwpg::WPGPaintInterface *painter, const KNDictionary &dict, const KNTransformation &tr);

private:
  const ID_t m_id;
};

MediaObject::MediaObject(const ID_t &id)
  : m_id(id)
{
}

void MediaObject::draw(libwpg::WPGPaintInterface *const painter, const KNDictionary &dict, const KNTransformation &tr)
{
  // TODO: implement me
  (void) painter;
  (void) dict;
  (void) tr;
}

}

KNObjectPtr_t makeGroupObject(const ID_t &id)
{
  const KNObjectPtr_t object(new GroupObject(id));
  return object;
}

KNObjectPtr_t makeImageObject(const ID_t &id)
{
  const KNObjectPtr_t object(new ImageObject(id));
  return object;
}

KNObjectPtr_t makeLineObject(const ID_t &id)
{
  const KNObjectPtr_t object(new LineObject(id));
  return object;
}

KNObjectPtr_t makeMediaObject(const ID_t &id)
{
  const KNObjectPtr_t object(new MediaObject(id));
  return object;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
