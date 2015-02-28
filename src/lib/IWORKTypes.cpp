/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTypes.h"

#include <cstring>

#include <boost/optional.hpp>

#include "IWORKTransformation.h"

namespace libetonyek
{

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

}

IWORKSize::IWORKSize()
  : width(0)
  , height(0)
{
}

IWORKSize::IWORKSize(const double w, const double h)
  : width(w)
  , height(h)
{
}

IWORKPosition::IWORKPosition()
  : x(0)
  , y(0)
{
}

IWORKPosition::IWORKPosition(const double x_, const double y_)
  : x(x_)
  , y(y_)
{
}

IWORKGeometry::IWORKGeometry()
  : naturalSize()
  , size()
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

IWORKColor::IWORKColor()
  : red()
  , green()
  , blue()
  , alpha()
{
}

IWORKColor::IWORKColor(const double r, const double g, const double b, const double a)
  : red(r)
  , green(g)
  , blue(b)
  , alpha(a)
{
}

IWORKPadding::IWORKPadding()
  : top()
  , right()
  , bottom()
  , left()
{
}

IWORKTabStop::IWORKTabStop(double pos_)
  : pos(pos_)
{
}

IWORKLine::IWORKLine()
  : geometry()
  , style()
  , x1()
  , y1()
  , x2()
  , y2()
{
}

IWORKData::IWORKData()
  : stream()
  , displayName()
  , type()
{
}

IWORKMediaContent::IWORKMediaContent()
  : size()
  , data()
{
}

IWORKBinary::IWORKBinary()
  : size()
  , path()
  , type()
  , dataSize()
{
}

IWORKImage::IWORKImage()
  : locked()
  , geometry()
  , binary()
{
}

IWORKMedia::IWORKMedia()
  : geometry()
  , style()
  , placeholder()
  , placeholderSize()
  , content()
{
}

IWORKWrap::IWORKWrap()
  : path()
  , geometry()
{
}

IWORKGroup::IWORKGroup()
  : objects()
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
  virtual void draw(librevenge::RVNGPresentationInterface *painter);

private:
  const IWORKGroupPtr_t m_group;
  const IWORKTransformation m_trafo;
};

GroupObject::GroupObject(const IWORKGroupPtr_t &group)
  : m_group(group)
{
}

void GroupObject::draw(librevenge::RVNGPresentationInterface *const painter)
{
  drawAll(m_group->objects, painter);
}

}

namespace
{

class ImageObject : public IWORKObject
{
public:
  ImageObject(const IWORKImagePtr_t &image, const IWORKTransformation &trafo);

private:
  virtual void draw(librevenge::RVNGPresentationInterface *painter);

private:
  const IWORKImagePtr_t m_image;
  const IWORKTransformation m_trafo;
};

ImageObject::ImageObject(const IWORKImagePtr_t &image, const IWORKTransformation &trafo)
  : m_image(image)
  , m_trafo(trafo)
{
}

void ImageObject::draw(librevenge::RVNGPresentationInterface *const painter)
{
  // TODO: implement me
  (void) painter;
}

}

namespace
{

class LineObject : public IWORKObject
{
public:
  LineObject(const IWORKLinePtr_t &line, const IWORKTransformation &trafo);

private:
  virtual void draw(librevenge::RVNGPresentationInterface *painter);

private:
  const IWORKLinePtr_t m_line;
  const IWORKTransformation m_trafo;
};

LineObject::LineObject(const IWORKLinePtr_t &line, const IWORKTransformation &trafo)
  : m_line(line)
  , m_trafo(trafo)
{
}

void LineObject::draw(librevenge::RVNGPresentationInterface *const painter)
{
  // TODO: transform the line

  if (m_line->x1 && m_line->y1 && m_line->x2 && m_line->y2)
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
    painter->setStyle(props);

    librevenge::RVNGPropertyListVector vertices;
    vertices.append(pointToWPG(get(m_line->x1), get(m_line->y1)));
    vertices.append(pointToWPG(get(m_line->x2), get(m_line->y2)));

    librevenge::RVNGPropertyList points;
    points.insert("svg:points", vertices);

    painter->drawPolyline(points);
  }
  else
  {
    ETONYEK_DEBUG_MSG(("line is missing head or tail point\n"));
  }
}

}

namespace
{

class MediaObject : public IWORKObject
{
public:
  MediaObject(const IWORKMediaPtr_t &media, const IWORKTransformation &trafo);

private:
  virtual void draw(librevenge::RVNGPresentationInterface *painter);

private:
  const IWORKMediaPtr_t m_media;
  const IWORKTransformation m_trafo;
};

MediaObject::MediaObject(const IWORKMediaPtr_t &media, const IWORKTransformation &trafo)
  : m_media(media)
  , m_trafo(trafo)
{
}

void MediaObject::draw(librevenge::RVNGPresentationInterface *const painter)
{
  if (bool(m_media)
      && bool(m_media->geometry)
      && bool(m_media->content)
      && bool(m_media->content->data)
      && bool(m_media->content->data->stream))
  {
    const RVNGInputStreamPtr_t input = m_media->content->data->stream;

    const optional<string> mimetype = getMimetype(m_media->content->data->type, input);

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

      double x = 0;
      double y = 0;
      m_trafo(x, y);
      props.insert("svg:x", pt2in(x));
      props.insert("svg:y", pt2in(y));

      double width = m_media->geometry->size.width;
      double height = m_media->geometry->size.height;
      m_trafo(width, height, true);
      props.insert("svg:width", pt2in(width));
      props.insert("svg:height", pt2in(height));

      painter->drawGraphicObject(props);
    }
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

IWORKObjectPtr_t makeObject(const IWORKMediaPtr_t &media, const IWORKTransformation &trafo)
{
  const IWORKObjectPtr_t object(new MediaObject(media, trafo));
  return object;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
