/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstring>

#include <boost/optional.hpp>

#include <librevenge/librevenge.h>

#include "libetonyek_utils.h"
#include "KEYOutput.h"
#include "KEYStyles.h"
#include "KEYText.h"
#include "KEYTypes.h"

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

KEYLine::KEYLine()
  : geometry()
  , style()
  , x1()
  , y1()
  , x2()
  , y2()
{
}

KEYData::KEYData()
  : stream()
  , displayName()
  , type()
{
}

KEYMediaContent::KEYMediaContent()
  : size()
  , data()
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
  , geometry()
  , text()
{
}

KEYStickyNote::KEYStickyNote()
  : geometry()
  , text()
{
}

KEYStickyNote::KEYStickyNote(const IWORKGeometryPtr_t &geometry_, const KEYTextPtr_t &text_)
  : geometry(geometry_)
  , text(text_)
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
    librevenge::RVNGPropertyList props;
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
    output.getPainter()->setStyle(props);

    librevenge::RVNGPropertyListVector vertices;
    vertices.append(pointToWPG(get(m_line->x1), get(m_line->y1)));
    vertices.append(pointToWPG(get(m_line->x2), get(m_line->y2)));

    librevenge::RVNGPropertyList points;
    points.insert("svg:points", vertices);

    output.getPainter()->drawPolyline(points);
  }
  else
  {
    ETONYEK_DEBUG_MSG(("line is missing head or tail point\n"));
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

      double x = 1;
      double y = 1;
      const IWORKTransformation &tr = newOutput.getTransformation();
      tr(x, y);
      props.insert("svg:x", pt2in(x));
      props.insert("svg:y", pt2in(y));

      double width = m_media->geometry->size.width;
      double height = m_media->geometry->size.height;
      tr(width, height, true);
      props.insert("svg:width", pt2in(width));
      props.insert("svg:height", pt2in(height));

      newOutput.getPainter()->drawGraphicObject(props);
    }
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
    if (bool(m_body->geometry))
    {
      const KEYOutput newOutput(output, makeTransformation(*m_body->geometry), m_body->style);
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
