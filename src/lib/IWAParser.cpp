/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWAParser.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <utility>

#include <boost/optional.hpp>

#include "IWAObjectType.h"
#include "IWASnappyStream.h"
#include "IWAText.h"
#include "IWORKCollector.h"
#include "IWORKNumberConverter.h"
#include "IWORKPath.h"
#include "IWORKProperties.h"
#include "IWORKTable.h"
#include "IWORKText.h"
#include "IWORKTransformation.h"
#include "IWORKTypes.h"

namespace libetonyek
{

using boost::none;
using boost::optional;

using namespace std::placeholders;

using std::bind;
using std::deque;
using std::make_pair;
using std::make_shared;
using std::map;
using std::shared_ptr;
using std::string;

namespace
{

bool samePoint(const optional<IWORKPosition> &point1, const optional<IWORKPosition> &point2)
{
  if (point1 && point2)
    return approxEqual(get(point1).m_x, get(point2).m_x) && approxEqual(get(point1).m_y, get(point2).m_y);
  return true;
}

const IWORKPosition &selectPoint(const optional<IWORKPosition> &point1, const optional<IWORKPosition> &point2, const optional<IWORKPosition> &point3)
{
  assert(point1 || point2 || point3);
  if (point1)
    return get(point1);
  else if (point2)
    return get(point2);
  return get(point3);
}

template<typename T>
optional<T> convert(const unsigned value)
{
  return IWORKNumberConverter<T>::convert(value);
}

template<typename P>
void putEnum(IWORKPropertyMap &props, const unsigned value)
{
  typedef typename IWORKPropertyInfo<P>::ValueType ValueType;
  const optional<ValueType> &converted = IWORKNumberConverter<ValueType>::convert(value);
  if (converted)
    props.put<P>(get(converted));
}

void parseColumnOffsets(const RVNGInputStreamPtr_t &input, const unsigned length, deque<optional<unsigned> > &offsets)
{
  try
  {
    while (!input->isEnd())
    {
      const unsigned offset = readU16(input);
      if (offset - 4 >= length)
        offsets.push_back(none);
      else
        offsets.push_back(offset);
    }
  }
  catch (...)
  {
    // ignore failure to read the last word: it can only happen in a broken file
  }
  // TODO: check that the valid offsets are sorted in ascending order
}

deque<double> makeSizes(const mdds::flat_segment_tree<unsigned, float> &sizes)
{
  deque<double> out(sizes.max_key(), sizes.default_value());
  for (mdds::flat_segment_tree<unsigned, float>::const_iterator it = sizes.begin(); it != sizes.end();)
  {
    const deque<double>::iterator start(out.begin() + it->first);
    const double size = it->second;
    ++it;
    const deque<double>::iterator end(it == sizes.end() ? out.end() : out.begin() + it->first);
    std::fill(start, end, size);
  }
  return out;
}

}

IWAParser::ObjectRecord::ObjectRecord()
  : m_stream()
  , m_type(0)
  , m_headerRange(0, 0)
  , m_dataRange(0, 0)
{
}

IWAParser::ObjectRecord::ObjectRecord(const RVNGInputStreamPtr_t &stream, const unsigned type,
                                      const long pos, const unsigned long headerLen, const unsigned long dataLen)
  : m_stream(stream)
  , m_type(type)
  , m_headerRange(pos, pos + long(headerLen))
  , m_dataRange(m_headerRange.second, m_headerRange.second + long(dataLen))
{
}

IWAParser::TableHeader::TableHeader(const unsigned count)
  : m_sizes(0, count, 0)
  , m_hidden(0, count, false)
{
}

IWAParser::TableInfo::TableInfo(const shared_ptr<IWORKTable> &table, const unsigned columns, const unsigned rows)
  : m_table(table)
  , m_columns(columns)
  , m_rows(rows)
  , m_style()
  , m_columnHeader(columns)
  , m_rowHeader(rows)
  , m_simpleTextList()
  , m_cellStyleList()
  , m_formattedTextList()
  , m_commentList()
{
}

IWAParser::IWAParser(const RVNGInputStreamPtr_t &fragments, const RVNGInputStreamPtr_t &package, IWORKCollector &collector)
  : m_langManager()
  , m_currentText()
  , m_fragments(fragments)
  , m_package(package)
  , m_collector(collector)
  , m_fragmentMap()
  , m_fragmentObjectMap()
  , m_fileMap()
  , m_visited()
  , m_charStyles()
  , m_paraStyles()
  , m_graphicStyles()
  , m_cellStyles()
  , m_tableStyles()
  , m_listStyles()
  , m_tableNameMap()
  , m_currentTable()
{
}

bool IWAParser::parse()
{
  parseObjectIndex();
  return parseDocument();
}

IWAParser::ObjectMessage::ObjectMessage(IWAParser &parser, const unsigned id, const unsigned type)
  : m_parser(parser)
  , m_message()
  , m_id(id)
  , m_type(0)
{
  std::deque<unsigned>::const_iterator it = find(m_parser.m_visited.begin(), m_parser.m_visited.end(), m_id);
  if (it == m_parser.m_visited.end())
  {
    optional<IWAMessage> msg;
    m_parser.queryObject(m_id, m_type, msg);
    if (msg)
    {
      if ((m_type == type) || (type == 0))
      {
        m_message = msg;
        m_parser.m_visited.push_back(m_id);
      }
      else
      {
        ETONYEK_DEBUG_MSG(("IWAParser::ObjectMessage::ObjectMessage: type mismatch for object %u: expected %u, got %u\n", id, type, m_type));
      }
    }
  }
}

IWAParser::ObjectMessage::~ObjectMessage()
{
  if (m_message)
  {
    assert(!m_parser.m_visited.empty());
    assert(m_parser.m_visited.back() == m_id);
    m_parser.m_visited.pop_back();
  }
}

IWAParser::ObjectMessage::operator bool() const
{
  return bool(m_message);
}

const IWAMessage &IWAParser::ObjectMessage::get() const
{
  return m_message.get();
}

unsigned IWAParser::ObjectMessage::getType() const
{
  return m_type;
}

void IWAParser::queryObject(const unsigned id, unsigned &type, boost::optional<IWAMessage> &msg) const
{
  const RecordMap_t::const_iterator recIt = m_fragmentObjectMap.find(id);
  if (recIt == m_fragmentObjectMap.end())
  {
    ETONYEK_DEBUG_MSG(("IWAParser::queryObject: object %u not found\n", id));
    return;
  }
  if (!recIt->second.second.m_stream)
    const_cast<IWAParser *>(this)->scanFragment(recIt->second.first);
  if (recIt->second.second.m_stream)
  {
    const ObjectRecord &objRecord = recIt->second.second;
    msg = IWAMessage(objRecord.m_stream, objRecord.m_dataRange.first, objRecord.m_dataRange.second);
    type = objRecord.m_type;
  }
}

const RVNGInputStreamPtr_t IWAParser::queryFile(const unsigned id) const
{
  const FileMap_t::iterator it = m_fileMap.find(id);

  if (it == m_fileMap.end())
  {
    ETONYEK_DEBUG_MSG(("IWAParser::queryFile: file %u not found\n", id));
    return RVNGInputStreamPtr_t();
  }

  if (!it->second.second)
  {
    assert(m_package->existsSubStream(it->second.first.c_str())); // we already checked for its presence
    it->second.second.reset(m_package->getSubStreamByName(it->second.first.c_str()));
  }

  assert(bool(it->second.second));
  return it->second.second;
}

boost::optional<unsigned> IWAParser::readRef(const IWAMessage &msg, const unsigned field)
{
  if (msg.message(field))
    return msg.message(field).uint32(1).optional();
  return boost::none;
}

std::deque<unsigned> IWAParser::readRefs(const IWAMessage &msg, const unsigned field)
{
  std::deque<unsigned> refs;
  if (msg.message(field))
  {
    const std::deque<IWAMessage> &objs = msg.message(field).repeated();
    for (const auto &obj : objs)
    {
      if (obj.uint32(1))
        refs.push_back(obj.uint32(1).get());
    }
  }
  return refs;
}

boost::optional<IWORKPosition> IWAParser::readPosition(const IWAMessage &msg, const unsigned field)
{
  if (msg.message(field))
  {
    const optional<float> &x = msg.message(field).float_(1).optional();
    const optional<float> &y = msg.message(field).float_(2).optional();
    return IWORKPosition(get_optional_value_or(x, 0), get_optional_value_or(y, 0));
  }
  return boost::none;
}

boost::optional<IWORKSize> IWAParser::readSize(const IWAMessage &msg, const unsigned field)
{
  if (msg.message(field))
  {
    const optional<float> &w = msg.message(field).float_(1).optional();
    const optional<float> &h = msg.message(field).float_(2).optional();
    return IWORKSize(get_optional_value_or(w, 0), get_optional_value_or(h, 0));
  }
  return boost::none;
}

boost::optional<IWORKColor> IWAParser::readColor(const IWAMessage &msg, const unsigned field)
{
  const IWAMessageField &color = msg.message(field);
  if (color)
  {
    if (color.float_(3) && color.float_(4) && color.float_(5))
      return IWORKColor(get(color.float_(3)), get(color.float_(4)), get(color.float_(5)), get_optional_value_or(color.float_(6), 0));
  }
  return boost::none;
}

void IWAParser::readStroke(const IWAMessage &msg, IWORKStroke &stroke)
{
  const optional<IWORKColor> &color = readColor(msg, 1);
  if (color)
    stroke.m_color = get(color);
  stroke.m_width = get(msg.float_(2));
  if (msg.uint32(3))
  {
    switch (get(msg.uint32(3)))
    {
    default :
      ETONYEK_DEBUG_MSG(("IWAParser::readStroke: unknown cap value: %u", get(msg.uint32(3))));
      ETONYEK_FALLTHROUGH;
    case 0 :
      stroke.m_cap = IWORK_LINE_CAP_BUTT;
      break;
    case 1 :
      stroke.m_cap = IWORK_LINE_CAP_ROUND;
      break;
    }
  }
  if (msg.uint32(4))
  {
    switch (get(msg.uint32(4)))
    {
    default :
      ETONYEK_DEBUG_MSG(("IWAParser::readStroke: unknown join value: %u", get(msg.uint32(4))));
      ETONYEK_FALLTHROUGH;
    case 0 :
      stroke.m_join = IWORK_LINE_JOIN_MITER;
      break;
    case 1 :
      stroke.m_join = IWORK_LINE_JOIN_ROUND;
      break;
    }
  }
  if (msg.message(6))
  {
    stroke.m_type = IWORK_STROKE_TYPE_SOLID;
    if (msg.message(6).uint32(1))
    {
      const optional<IWORKStrokeType> &strokeType = convert<IWORKStrokeType>(get(msg.message(6).uint32(1)));
      if (strokeType)
        stroke.m_type = get(strokeType);
    }
    unsigned remaining = 0;
    if (msg.message(6).uint32(3))
      remaining = get(msg.message(6).uint32(3));
    const deque<float> &elements = msg.message(6).float_(4).repeated();
    for (auto it = elements.begin(); it != elements.end() && remaining != 0; ++it)
      stroke.m_pattern.push_back(*it);
  }
}

bool IWAParser::readFill(const IWAMessage &msg, IWORKFill &fill)
{
  const optional<IWORKColor> &color = readColor(msg, 1);
  if (color)
  {
    fill = get(color);
    return true;
  }
  else if (msg.message(2))
  {
    IWORKGradient gradient;
    readGradient(get(msg.message(2)), gradient);
    fill = gradient;
    return true;
  }
  else if (msg.message(3))
  {
    IWORKFillImage bitmap;
    if (msg.message(3).uint32(2))
    {
      switch (get(msg.message(3).uint32(2)))
      {
      default :
        ETONYEK_DEBUG_MSG(("IWAParser::readFill: unknown bitmap fill type: %u", get(msg.message(3).uint32(2))));
        ETONYEK_FALLTHROUGH;
      case 0 :
        bitmap.m_type = IWORK_FILL_IMAGE_TYPE_ORIGINAL_SIZE;
        break;
      case 1 :
        bitmap.m_type = IWORK_FILL_IMAGE_TYPE_STRETCH;
        break;
      case 2 :
        bitmap.m_type = IWORK_FILL_IMAGE_TYPE_TILE;
        break;
      case 3 :
        bitmap.m_type = IWORK_FILL_IMAGE_TYPE_SCALE_TO_FILL;
        break;
      case 4 :
        bitmap.m_type = IWORK_FILL_IMAGE_TYPE_SCALE_TO_FIT;
        break;
      }
    }
    const optional<IWORKColor> &bitmapColor = readColor(get(msg.message(3)), 3);
    if (bitmapColor)
      bitmap.m_color = get(bitmapColor);
    const optional<IWORKSize> &size = readSize(get(msg.message(3)), 4);
    if (size)
      bitmap.m_size = get(size);
    const optional<unsigned> &fileRef = readRef(get(msg.message(3)), 6);
    if (fileRef)
      bitmap.m_stream = queryFile(get(fileRef));
    fill = bitmap;
    return true;
  }
  return false;
}

void IWAParser::readGradient(const IWAMessage &msg, IWORKGradient &gradient)
{
  if (msg.uint32(1))
  {
    switch (get(msg.uint32(1)))
    {
    default :
      ETONYEK_DEBUG_MSG(("IWAParser::readGradient: unknown gradient type: %u", get(msg.uint32(1))));
      ETONYEK_FALLTHROUGH;
    case 0 :
      gradient.m_type = IWORK_GRADIENT_TYPE_LINEAR;
      break;
    case 1 :
      gradient.m_type = IWORK_GRADIENT_TYPE_RADIAL;
      break;
    }
  }
  for (const auto &it : msg.message(2))
  {
    IWORKGradientStop stop;
    const optional<IWORKColor> &color = readColor(it, 1);
    if (color)
      stop.m_color = get(color);
    if (it.float_(2))
      stop.m_fraction = get(it.float_(2));
    if (it.float_(3))
      stop.m_inflection = get(it.float_(3));
    gradient.m_stops.push_back(stop);
  }
  if (msg.message(5) && msg.message(5).float_(2))
    gradient.m_angle = get(msg.message(5).float_(2));
}

void IWAParser::readShadow(const IWAMessage &msg, IWORKShadow &shadow)
{
  const optional<IWORKColor> &color = readColor(msg, 1);
  if (color)
    shadow.m_color = get(color);
  if (msg.float_(2))
    shadow.m_angle = get(msg.float_(2));
  if (msg.float_(3))
    shadow.m_offset = get(msg.float_(3));
  if (msg.float_(5))
    shadow.m_opacity = get(msg.float_(5));
}

void IWAParser::readPadding(const IWAMessage &msg, IWORKPadding &padding)
{
  padding.m_left = msg.float_(1).optional();
  padding.m_top = msg.float_(2).optional();
  padding.m_right = msg.float_(3).optional();
  padding.m_bottom = msg.float_(4).optional();
}

bool IWAParser::dispatchShape(const unsigned id)
{
  const ObjectMessage msg(*this, id);
  if (!msg)
    return false;

  switch (msg.getType())
  {
  case IWAObjectType::DrawableShape :
    return parseDrawableShape(get(msg));
  case IWAObjectType::Group :
    return parseGroup(get(msg));
  case IWAObjectType::Image :
    return parseImage(get(msg));
  case IWAObjectType::TabularInfo :
    return parseTabularInfo(get(msg));
  }

  return false;
}

bool IWAParser::parseText(const unsigned id)
{
  assert(bool(m_currentText));

  const ObjectMessage msg(*this, id, IWAObjectType::Text);
  if (!msg)
    return false;

  const IWAStringField &text = get(msg).string(3);
  if (text)
  {
    IWAText textParser(get(text), m_langManager);
    const size_t length = get(text).size();

    if (get(msg).message(5))
    {
      map<unsigned, IWORKStylePtr_t> paras;
      IWORKStylePtr_t style = make_shared<IWORKStyle>(IWORKPropertyMap(), none, none);
      for (const auto &it : get(msg).message(5).message(1))
      {
        if (it.uint32(1) && (get(it.uint32(1)) < length))
        {
          const optional<unsigned> &styleRef = readRef(it, 2);
          if (styleRef)
          {
            const IWORKStylePtr_t &newStyle = queryParagraphStyle(get(styleRef));
            if (bool(newStyle))
              style = newStyle;
          }
          paras.insert(paras.end(), make_pair(get(it.uint32(1)), style));
        }
      }
      textParser.setParagraphs(paras);
    }

    if (get(msg).message(6))
    {
      map<unsigned, unsigned> levels;
      for (const auto &it : get(msg).message(6).message(1))
      {
        if (it.uint32(1) && (get(it.uint32(1)) < length))
          levels.insert(levels.end(), make_pair(get(it.uint32(1)), get_optional_value_or(it.uint32(2), 0)));
      }
      textParser.setListLevels(levels);
    }

    if (get(msg).message(7))
    {
      map<unsigned, IWORKStylePtr_t> lists;
      for (const auto &it : get(msg).message(7).message(1))
      {
        if (it.uint32(1) && (get(it.uint32(1)) < length))
        {
          IWORKStylePtr_t style;
          const optional<unsigned> &styleRef = readRef(it, 2);
          if (styleRef)
            style = queryListStyle(get(styleRef));
          lists.insert(lists.end(), make_pair(get(it.uint32(1)), style));
        }
      }
      textParser.setLists(lists);
    }

    if (get(msg).message(8))
    {
      map<unsigned, IWORKStylePtr_t> spans;
      for (const auto &it : get(msg).message(8).message(1))
      {
        if (it.uint32(1) && (get(it.uint32(1)) < length))
        {
          IWORKStylePtr_t style;
          const optional<unsigned> &styleRef = readRef(it, 2);
          if (styleRef)
            style = queryCharacterStyle(get(styleRef));
          spans.insert(spans.end(), make_pair(get(it.uint32(1)), style));
        }
      }
      textParser.setSpans(spans);
    }

    if (get(msg).message(11))
    {
      map<unsigned, string> links;
      for (const auto &it : get(msg).message(11).message(1))
      {
        if (it.uint32(1))
        {
          string url;
          const optional<unsigned> &linkRef = readRef(it, 2);
          if (linkRef)
            parseLink(get(linkRef), url);
          links.insert(links.end(), make_pair(get(it.uint32(1)), url));
        }
      }
      textParser.setLinks(links);
    }

    if (get(msg).message(19))
    {
      map<unsigned, string> langs;
      for (const auto &it : get(msg).message(19).message(1))
      {
        if (it.uint32(1))
          langs.insert(langs.end(), make_pair(get(it.uint32(1)), get_optional_value_or(it.string(2), "")));
      }
      textParser.setLanguages(langs);
    }

    textParser.parse(*m_currentText);
  }

  return true;
}

const IWORKStylePtr_t IWAParser::queryStyle(const unsigned id, StyleMap_t &styleMap, StyleParseFun_t parseStyle) const
{
  StyleMap_t::const_iterator it = styleMap.find(id);
  if (it == styleMap.end())
  {
    IWORKStylePtr_t style;
    parseStyle(id, style);
    it = styleMap.insert(make_pair(id, style)).first;
  }
  assert(it != styleMap.end());
  return it->second;
}

const IWORKStylePtr_t IWAParser::queryCharacterStyle(const unsigned id) const
{
  return queryStyle(id, m_charStyles, bind(&IWAParser::parseCharacterStyle, const_cast<IWAParser *>(this), _1, _2));
}

const IWORKStylePtr_t IWAParser::queryParagraphStyle(const unsigned id) const
{
  return queryStyle(id, m_paraStyles, bind(&IWAParser::parseParagraphStyle, const_cast<IWAParser *>(this), _1, _2));
}

const IWORKStylePtr_t IWAParser::queryGraphicStyle(const unsigned id) const
{
  return queryStyle(id, m_graphicStyles, bind(&IWAParser::parseGraphicStyle, const_cast<IWAParser *>(this), _1, _2));
}

const IWORKStylePtr_t IWAParser::queryCellStyle(const unsigned id) const
{
  return queryStyle(id, m_cellStyles, bind(&IWAParser::parseCellStyle, const_cast<IWAParser *>(this), _1, _2));
}

const IWORKStylePtr_t IWAParser::queryTableStyle(const unsigned id) const
{
  return queryStyle(id, m_tableStyles, bind(&IWAParser::parseTableStyle, const_cast<IWAParser *>(this), _1, _2));
}

const IWORKStylePtr_t IWAParser::queryListStyle(const unsigned id) const
{
  return queryStyle(id, m_tableStyles, bind(&IWAParser::parseListStyle, const_cast<IWAParser *>(this), _1, _2));
}

bool IWAParser::parseDrawableShape(const IWAMessage &msg)
{
  m_collector.startLevel();

  const optional<IWAMessage> &shape = msg.message(1).optional();
  if (shape)
  {
    const optional<IWAMessage> &placement = get(shape).message(1).optional();
    if (placement)
      parseShapePlacement(get(placement));

    const optional<unsigned> styleRef = readRef(get(shape), 2);
    if (styleRef)
      m_collector.setGraphicStyle(queryGraphicStyle(get(styleRef)));

    const optional<IWAMessage> &path = get(shape).message(3).optional();
    if (path)
    {
      if (get(path).message(3)) // point path
      {
        const IWAMessage &pointPath = get(path).message(3).get();
        const optional<unsigned> &type = pointPath.uint32(1).optional();
        const optional<IWORKPosition> &point = readPosition(pointPath, 2);
        const optional<IWORKSize> &size = readSize(pointPath, 3);
        if (type && point && size)
        {
          switch (get(type))
          {
          case 1 :
          case 10 :
            m_collector.collectArrowPath(get(size), get(point).m_x, get(point).m_y, get(type) == 10);
            break;
          case 100 :
            m_collector.collectStarPath(get(size), get(point).m_x, get(point).m_y);
            break;
          default :
            ETONYEK_DEBUG_MSG(("IWAParser::parseDrawableShape: unknown point path type %u\n", get(type)));
            break;
          }
        }
      }
      else if (get(path).message(4)) // scalar path
      {
        const IWAMessage &scalarPath = get(path).message(4).get();
        const optional<unsigned> &type = scalarPath.uint32(1).optional();
        const optional<float> &value = scalarPath.float_(2).optional();
        const optional<IWORKSize> &size = readSize(scalarPath, 3);
        if (type && value && size)
        {
          switch (get(type))
          {
          case 0 :
            m_collector.collectRoundedRectanglePath(get(size), get(value));
            break;
          case 1 :
            m_collector.collectPolygonPath(get(size), get(value));
            break;
          default :
            ETONYEK_DEBUG_MSG(("IWAParser::parseDrawableShape: unknown scalar path type %u\n", get(type)));
            break;
          }
        }
      }
      else if (get(path).message(5)) // bezier path
      {
        const optional<IWAMessage> &bezier = get(path).message(5).get().message(3).optional();
        if (bezier)
        {
          const IWORKPathPtr_t bezierPath(new IWORKPath());
          const deque<IWAMessage> &elements = get(bezier).message(1).repeated();
          bool closed = false;
          bool closingMove = false;
          for (auto it = elements.begin(); it != elements.end() && !closed; ++it)
          {
            const optional<unsigned> &type = it->uint32(1).optional();
            if (type)
            {
              if (closed && closingMove)
              {
                ETONYEK_DEBUG_MSG(("IWAParser::parseDrawableShape: unexpected element %c after the closing move\n", get(type)));
                break;
              }
              switch (get(type))
              {
              case 1 :
                if (closed)
                {
                  closingMove = true;
                  break;
                }
                ETONYEK_FALLTHROUGH;
              case 2 :
              {
                const optional<IWORKPosition> &coords = readPosition(*it, 2);
                if (!coords)
                {
                  ETONYEK_DEBUG_MSG(("IWAParser::parseDrawableShape: missing coordinates for %c element\n", get(type) == 1 ? 'M' : 'L'));
                  break;
                }
                if (get(type) == 1)
                  bezierPath->appendMoveTo(get(coords).m_x, get(coords).m_y);
                else
                  bezierPath->appendLineTo(get(coords).m_x, get(coords).m_y);
                break;
              }
              case 4 :
              {
                if (it->message(2))
                {
                  const std::deque<IWAMessage> &positions = it->message(2).repeated();
                  if (positions.size() >= 3)
                  {
                    if (positions.size() > 3)
                    {
                      ETONYEK_DEBUG_MSG(("IWAParser::parseDrawableShape: a curve has got %u control coords\n", unsigned(positions.size())));
                    }
                    const optional<float> &x = positions[0].float_(1).optional();
                    const optional<float> &y = positions[0].float_(2).optional();
                    const optional<float> &x1 = positions[1].float_(1).optional();
                    const optional<float> &y1 = positions[1].float_(2).optional();
                    const optional<float> &x2 = positions[2].float_(1).optional();
                    const optional<float> &y2 = positions[2].float_(2).optional();
                    bezierPath->appendCurveTo(get_optional_value_or(x, 0), get_optional_value_or(y, 0),
                                              get_optional_value_or(x1, 0), get_optional_value_or(y1, 0),
                                              get_optional_value_or(x2, 0), get_optional_value_or(y2, 0));
                  }
                  else
                  {
                    ETONYEK_DEBUG_MSG(("IWAParser::parseDrawableShape: %u is not enough coords for a curve\n", unsigned(positions.size())));
                  }
                }
                break;
              }
              case 5 :
                bezierPath->appendClose();
                closed = true;
                break;
              default :
                ETONYEK_DEBUG_MSG(("IWAParser::parseDrawableShape: unknown bezier path element type %u\n", get(type)));
              }
            }
          }
          // the path is in unit area, scale it to the real size
          const optional<IWORKSize> &size = readSize(get(get(path).message(5)), 2);
          if (size)
            *bezierPath *= transformations::scale(get(size).m_width / 100, get(size).m_height / 100);
          m_collector.collectBezier(bezierPath);
          m_collector.collectBezierPath();
        }
      }
      else if (get(path).message(6)) // callout2 path
      {
        const IWAMessage &callout2Path = get(path).message(6).get();
        const optional<IWORKSize> &size = readSize(callout2Path, 1);
        const optional<IWORKPosition> &tailPos = readPosition(callout2Path, 2);
        const optional<float> &tailSize = callout2Path.float_(3).optional();
        if (size && tailPos && tailSize)
        {
          const optional<float> &cornerRadius = callout2Path.float_(4).optional();
          const optional<bool> &tailAtCenter = callout2Path.bool_(5).optional();
          m_collector.collectCalloutPath(get(size), get_optional_value_or(cornerRadius, 0),
                                         get(tailSize), get(tailPos).m_x, get(tailPos).m_y,
                                         get_optional_value_or(tailAtCenter, false));
        }
      }
      else if (get(path).message(7)) // connection path
      {
        ETONYEK_DEBUG_MSG(("IWAParser::parseDrawableShape: connection path is not supported yet\n"));
      }
      else if (get(path).message(8)) // editable path
      {
        const IWAMessageField &pathPoints = get(get(path).message(8)).message(1);
        if (pathPoints && !pathPoints.message(1).empty())
        {
          const IWORKPathPtr_t editablePath(new IWORKPath());
          const IWAMessageField &points = pathPoints.message(1);
          for (IWAMessageField::const_iterator it = points.begin(); it != points.end(); ++it)
          {
            const optional<IWORKPosition> &point1 = readPosition(*it, 1);
            const optional<IWORKPosition> &point2 = readPosition(*it, 2);
            const optional<IWORKPosition> &point3 = readPosition(*it, 3);
            if (!point1 && !point2 && !point3)
            {
              ETONYEK_DEBUG_MSG(("IWAParser::parseDrawableShape: no control points for point %u\n", unsigned(std::distance(points.begin(), it))));
              continue;
            }
            if (samePoint(point1, point2) && samePoint(point2, point3))
            {
              // insert a straight line
              const IWORKPosition &point = selectPoint(point1, point2, point3);
              if (it == points.begin())
                editablePath->appendMoveTo(point.m_x, point.m_y);
              else
                editablePath->appendLineTo(point.m_x, point.m_y);
            }
            else
            {
              // TODO: insert a curve
            }
          }
          if (pathPoints.bool_(2) && get(pathPoints.bool_(2)))
            editablePath->appendClose();
          m_collector.collectBezier(editablePath);
          m_collector.collectBezierPath();
        }
      }
    }
  }

  const optional<unsigned> &textRef = readRef(msg, 2);
  if (textRef)
  {
    m_currentText = m_collector.createText(m_langManager);
    parseText(get(textRef));
  }

  if (shape || textRef)
    m_collector.collectShape();
  m_currentText.reset();

  m_collector.endLevel();

  return true;
}

bool IWAParser::parseGroup(const IWAMessage &msg)
{
  if (msg.message(1))
    parseShapePlacement(get(msg.message(1)));
  if (!msg.message(2).empty())
  {
    m_collector.startLevel();
    m_collector.startGroup();
    const deque<unsigned> &shapeRefs = readRefs(msg, 2);
    std::for_each(shapeRefs.begin(), shapeRefs.end(), bind(&IWAParser::dispatchShape, this, _1));
    m_collector.endGroup();
    m_collector.endLevel();
  }

  return true;
}

bool IWAParser::parseShapePlacement(const IWAMessage &msg, IWORKGeometryPtr_t &geometry)
{
  geometry = make_shared<IWORKGeometry>();

  const optional<IWAMessage> &g = msg.message(1).optional();
  if (g)
  {
    const optional<IWORKPosition> &pos = readPosition(get(g), 1);
    if (pos)
      geometry->m_position = get(pos);
    const optional<IWORKSize> &size = readSize(get(g), 2);
    if (size)
    {
      geometry->m_naturalSize = get(size);
      geometry->m_size = get(size);
    }

    if (get(g).uint32(3))
    {
      switch (get(get(g).uint32(3)))
      {
      case 3 : // normal
        break;
      case 7 : // horizontal flip
        geometry->m_horizontalFlip = true;
        break;
      default :
        ETONYEK_DEBUG_MSG(("IWAParser::parseDrawableShape: unknown transformation %u\n", get(get(g).uint32(3))));
        break;
      }
    }
    if (get(g).float_(4))
      geometry->m_angle = deg2rad(get(get(g).float_(4)));
  }
  geometry->m_aspectRatioLocked = msg.bool_(7).optional();

  return true;
}

bool IWAParser::parseShapePlacement(const IWAMessage &msg)
{
  IWORKGeometryPtr_t geometry;
  const bool retval = parseShapePlacement(msg, geometry);
  m_collector.collectGeometry(geometry);
  return retval;
}

void IWAParser::parseObjectIndex()
{
  m_fragmentMap[2] = make_pair(string("Index/Metadata.iwa"), RVNGInputStreamPtr_t());
  m_fragmentObjectMap[2] = make_pair(2, ObjectRecord());
  scanFragment(2);
  const RecordMap_t::const_iterator indexIt = m_fragmentObjectMap.find(2);
  if (indexIt == m_fragmentObjectMap.end())
  {
    // TODO: scan all fragment files
    ETONYEK_DEBUG_MSG(("IWAParser::parseObjectIndex: object index is broken, nothing will be parsed\n"));
  }
  else
  {
    const ObjectRecord &rec = indexIt->second.second;
    assert(bool(rec.m_stream));
    const IWAMessage objectIndex(rec.m_stream, rec.m_dataRange.first, rec.m_dataRange.second);
    const deque<IWAMessage> &fragments = objectIndex.message(3).repeated();
    for (const auto &fragment : fragments)
    {
      if (fragment.uint32(1) && (fragment.string(2) || fragment.string(3)))
      {
        const unsigned pathIdx = fragment.string(3) ? 3 : 2;
        m_fragmentMap[fragment.uint32(1).get()] = make_pair("Index/" + fragment.string(pathIdx).get() + ".iwa", RVNGInputStreamPtr_t());
        m_fragmentObjectMap[fragment.uint32(1).get()] = make_pair(fragment.uint32(1).get(), ObjectRecord());
      }
      const deque<IWAMessage> &refs = fragment.message(6).repeated();
      for (const auto &ref : refs)
      {
        if (ref.uint32(1) && ref.uint32(2))
          m_fragmentObjectMap[ref.uint32(2).get()] = make_pair(ref.uint32(1).get(), ObjectRecord());
      }
    }
    const deque<IWAMessage> &files = objectIndex.message(4).repeated();
    for (const auto &file : files)
    {
      if (file.uint32(1))
      {
        const string virtualPath(file.string(3) ? ("Data/" + get(file.string(3))) : "");
        const string internalPath(file.string(4) ? ("Data/" + get(file.string(4))) : "");
        string path;
        if (!internalPath.empty() && m_package->existsSubStream(internalPath.c_str()))
          path = internalPath;
        else if (!virtualPath.empty() && m_package->existsSubStream(virtualPath.c_str()))
          path = virtualPath;
        if (!path.empty())
          m_fileMap[file.uint32(1).get()] = make_pair(path, RVNGInputStreamPtr_t());
      }
    }
  }
}

void IWAParser::scanFragment(const unsigned id)
{
  // scan the fragment file
  const FileMap_t::iterator fragmentIt = m_fragmentMap.find(id);
  if (fragmentIt != m_fragmentMap.end())
  {
    assert(!fragmentIt->second.second); // this could only happen if the fragment file had already been scanned
    if (m_fragments->existsSubStream(fragmentIt->second.first.c_str()))
    {
      const RVNGInputStreamPtr_t stream(m_fragments->getSubStreamByName(fragmentIt->second.first.c_str()));
      assert(bool(stream));
      fragmentIt->second.second = make_shared<IWASnappyStream>(stream);
      scanFragment(fragmentIt->first, fragmentIt->second.second);
    }
    else
    {
      ETONYEK_DEBUG_MSG(("IWAParser::scanFragment: file %s does not exist\n", fragmentIt->second.first.c_str()));
      m_fragmentMap.erase(fragmentIt); // avoid unnecessary repeats of the lookup
    }
  }
}

void IWAParser::scanFragment(const unsigned id, const RVNGInputStreamPtr_t &stream)
{
  try
  {
    while (!stream->isEnd())
    {
      // scan a single object
      const uint64_t headerLen = readUVar(stream);
      const long start = stream->tell();
      const IWAMessage header(stream, headerLen);
      if (!header.message(2) || !header.message(2).uint64(3))
        break;
      const uint64_t dataLen = header.message(2).uint64(3).get();
      if (header.uint32(1))
      {
        const optional<unsigned> type = header.message(2).uint32(1).optional();
        const ObjectRecord rec(stream, get_optional_value_or(type, 0), start, long(headerLen), long(dataLen));
        m_fragmentObjectMap[header.uint32(1).get()] = make_pair(id, rec);
      }
      if (stream->seek(start + long(headerLen) + long(dataLen), librevenge::RVNG_SEEK_SET) != 0)
        break;
    }
  }
  catch (...)
  {
    // just read as much as possible
  }

  // remove all objects from the fragment that have not been found
  auto it = m_fragmentObjectMap.begin();
  while (it != m_fragmentObjectMap.end())
  {
    const RecordMap_t::iterator curIt = it;
    ++it;
    if ((curIt->second.first == id) && !curIt->second.second.m_stream)
    {
      ETONYEK_DEBUG_MSG(("IWAParser::scanFragment: object with ID %u was not found\n", curIt->first));
      m_fragmentObjectMap.erase(curIt);
    }
  }
}

void IWAParser::parseCharacterStyle(const unsigned id, IWORKStylePtr_t &style)
{
  const ObjectMessage msg(*this, id, IWAObjectType::CharacterStyle);
  if (!msg)
    return;

  optional<string> name;
  IWORKStylePtr_t parent;
  const IWAMessageField &styleInfo = get(msg).message(1);
  if (styleInfo)
  {
    name = styleInfo.string(2).optional();
    const optional<unsigned> &parentRef = readRef(get(styleInfo), 3);
    if (parentRef)
      parent = queryCharacterStyle(get(parentRef));
  }

  IWORKPropertyMap props;
  if (get(msg).message(11))
    parseCharacterProperties(get(get(msg).message(11)), props);

  style.reset(new IWORKStyle(props, name, parent));
}

void IWAParser::parseParagraphStyle(const unsigned id, IWORKStylePtr_t &style)
{
  const ObjectMessage msg(*this, id, IWAObjectType::ParagraphStyle);
  if (!msg)
    return;

  optional<string> name;
  IWORKStylePtr_t parent;
  const IWAMessageField &styleInfo = get(msg).message(1);
  if (styleInfo)
  {
    name = styleInfo.string(2).optional();
    const optional<unsigned> &parentRef = readRef(get(styleInfo), 3);
    if (parentRef)
      parent = queryParagraphStyle(get(parentRef));
  }

  IWORKPropertyMap props;
  if (get(msg).message(11))
    parseCharacterProperties(get(get(msg).message(11)), props);
  if (get(msg).message(12))
  {
    const IWAMessage &paraProps = get(get(msg).message(12));
    using namespace property;

    if (paraProps.uint32(1))
      putEnum<Alignment>(props, get(paraProps.uint32(1)));
    const optional<IWORKColor> &fillColor = readColor(paraProps, 6);
    if (fillColor)
      props.put<ParagraphFill>(get(fillColor));
    if (paraProps.float_(7))
      props.put<FirstLineIndent>(get(paraProps.float_(7)));
    if (paraProps.bool_(8))
      props.put<Hyphenate>(get(paraProps.bool_(8)));
    if (paraProps.bool_(9))
      props.put<KeepLinesTogether>(get(paraProps.bool_(9)));
    if (paraProps.bool_(10))
      props.put<KeepWithNext>(get(paraProps.bool_(10)));
    if (paraProps.float_(11))
      props.put<LeftIndent>(get(paraProps.float_(11)));
    if (paraProps.message(13))
    {
      if (paraProps.message(13).float_(2))
        props.put<LineSpacing>(IWORKLineSpacing(get(paraProps.message(13).float_(2)), false));
    }
    if (paraProps.bool_(14))
      props.put<PageBreakBefore>(get(paraProps.bool_(14)));
    if (paraProps.uint32(15))
      putEnum<ParagraphBorderType>(props, get(paraProps.uint32(15)));
    if (paraProps.float_(19))
      props.put<RightIndent>(get(paraProps.float_(19)));
    if (paraProps.float_(20))
      props.put<SpaceAfter>(get(paraProps.float_(20)));
    if (paraProps.float_(21))
      props.put<SpaceBefore>(get(paraProps.float_(21)));
    if (paraProps.message(25))
    {
      IWORKTabStops_t tabs;
      const IWAMessageField &tabStops = paraProps.message(25).message(1);
      for (const auto &tabStop : tabStops)
      {
        if (tabStop.float_(1))
          tabs.push_back(IWORKTabStop(get(tabStop.float_(1))));
      }
    }
    if (paraProps.bool_(26))
      props.put<WidowControl>(get(paraProps.bool_(26)));
    if (paraProps.message(32))
    {
      IWORKStroke stroke;
      readStroke(get(paraProps.message(32)), stroke);
      props.put<ParagraphStroke>(stroke);
    }
  }

  style.reset(new IWORKStyle(props, name, parent));
}

void IWAParser::parseGraphicStyle(const unsigned id, IWORKStylePtr_t &style)
{
  const ObjectMessage msg(*this, id, IWAObjectType::GraphicStyle);
  if (!msg)
    return;

  optional<string> name;
  IWORKStylePtr_t parent;
  IWORKPropertyMap props;

  using namespace property;

  if (get(msg).message(1))
  {
    const IWAMessageField &styleInfo = get(msg).message(1).message(1);
    if (styleInfo)
    {
      name = styleInfo.string(2).optional();
      const optional<unsigned> &parentRef = readRef(get(styleInfo), 3);
      if (parentRef)
        parent = queryGraphicStyle(get(parentRef));
    }

    const IWAMessageField &styleProps = get(msg).message(1).message(11);
    if (styleProps)
    {
      if (styleProps.message(1))
      {
        IWORKFill fill;
        if (readFill(get(styleProps.message(1)), fill))
          props.put<Fill>(fill);
        else
          props.clear<Fill>();
      }
      if (styleProps.message(2))
      {
        IWORKStroke stroke;
        readStroke(get(styleProps.message(2)), stroke);
        props.put<Stroke>(stroke);
      }
      if (styleProps.float_(3))
        props.put<Opacity>(get(styleProps.float_(3)));
      if (styleProps.message(4))
      {
        IWORKShadow shadow;
        const optional<IWORKColor> &color = readColor(get(styleProps.message(4)), 1);
        if (color)
          shadow.m_color = get(color);
        if (styleProps.message(4).float_(2))
          shadow.m_angle = get(styleProps.message(4).float_(2));
        if (styleProps.message(4).float_(3))
          shadow.m_offset = get(styleProps.message(4).float_(3));
        if (styleProps.message(4).float_(5))
          shadow.m_opacity = get(styleProps.message(4).float_(5));
        props.put<Shadow>(shadow);
      }
    }
  }

  if (get(msg).message(11))
  {
    // TODO: layout props
  }

  style.reset(new IWORKStyle(props, name, parent));
}

void IWAParser::parseCellStyle(const unsigned id, IWORKStylePtr_t &style)
{
  const ObjectMessage msg(*this, id, IWAObjectType::CellStyle);
  if (!msg)
    return;

  optional<string> name;
  IWORKStylePtr_t parent;
  IWORKPropertyMap props;

  using namespace property;

  const IWAMessageField &styleInfo = get(msg).message(1);
  if (styleInfo)
  {
    name = styleInfo.string(2).optional();
    const optional<unsigned> &parentRef = readRef(get(styleInfo), 3);
    if (parentRef)
      parent = queryCellStyle(get(parentRef));
  }

  if (get(msg).message(11))
  {
    const IWAMessage &properties = get(get(msg).message(11));
    if (properties.message(1))
    {
      IWORKFill fill;
      if (readFill(get(properties.message(1)), fill))
        props.put<Fill>(fill);
      else
        props.clear<Fill>();
    }
    if (properties.message(9))
    {
      IWORKPadding padding;
      readPadding(get(properties.message(9)), padding);
      props.put<Padding>(padding);
    }
    if (properties.message(10))
    {
      IWORKStroke stroke;
      readStroke(get(properties.message(10)), stroke);
      props.put<TopBorder>(stroke);
    }
    if (properties.message(11))
    {
      IWORKStroke stroke;
      readStroke(get(properties.message(11)), stroke);
      props.put<RightBorder>(stroke);
    }
    if (properties.message(12))
    {
      IWORKStroke stroke;
      readStroke(get(properties.message(12)), stroke);
      props.put<BottomBorder>(stroke);
    }
    if (properties.message(13))
    {
      IWORKStroke stroke;
      readStroke(get(properties.message(13)), stroke);
      props.put<LeftBorder>(stroke);
    }
  }

  style.reset(new IWORKStyle(props, name, parent));
}

void IWAParser::parseTableStyle(const unsigned id, IWORKStylePtr_t &style)
{
  const ObjectMessage msg(*this, id, IWAObjectType::TableStyle);
  if (!msg)
    return;

  optional<string> name;
  IWORKStylePtr_t parent;
  IWORKPropertyMap props;

  using namespace property;

  const IWAMessageField &styleInfo = get(msg).message(1);
  if (styleInfo)
  {
    name = styleInfo.string(2).optional();
    const optional<unsigned> &parentRef = readRef(get(styleInfo), 3);
    if (parentRef)
      parent = queryTableStyle(get(parentRef));
  }

  if (get(msg).message(11))
  {
    const IWAMessage &properties = get(get(msg).message(11));

    if (properties.bool_(1))
      props.put<SFTTableBandedRowsProperty>(get(properties.bool_(1)));
    if (properties.message(2))
    {
      IWORKFill fill;
      if (readFill(get(properties.message(2)), fill))
        props.put<SFTTableBandedCellFillProperty>(fill);
      else
        props.clear<SFTTableBandedCellFillProperty>();
    }
    if (properties.bool_(22))
      props.put<SFTAutoResizeProperty>(get(properties.bool_(22)));
    if (properties.string(41))
      props.put<FontName>(get(properties.string(41)));
  }

  style.reset(new IWORKStyle(props, name, parent));
}

void IWAParser::parseListStyle(const unsigned id, IWORKStylePtr_t &style)
{
  const ObjectMessage msg(*this, id, IWAObjectType::ListStyle);
  if (!msg)
    return;

  optional<string> name;
  IWORKStylePtr_t parent;

  using namespace property;

  const IWAMessageField &styleInfo = get(msg).message(1);
  if (styleInfo)
  {
    name = styleInfo.string(2).optional();
    const optional<unsigned> &parentRef = readRef(get(styleInfo), 3);
    if (parentRef)
      parent = queryListStyle(get(parentRef));
  }

  unsigned level = 0;

  map<unsigned, IWORKPropertyMap> levelProps;

  const IWAUInt32Field &numberFormats = get(msg).uint32(15);
  const IWAStringField &bullets = get(msg).string(16);
  const IWABoolField &tiered = get(msg).bool_(25);
  for (IWAUInt32Field::const_iterator it = get(msg).uint32(11).begin(); it != get(msg).uint32(11).end(); ++it, ++level)
  {
    switch (*it)
    {
    default :
      ETONYEK_DEBUG_MSG(("parseListStyle: unknown label type %u\n", *it));
      ETONYEK_FALLTHROUGH;
    case 0 :
      // no label
      levelProps[level].put<ListLabelTypeInfo>(true);
      break;
    case 1 :
      // TODO: handle image
      break;
    case 2 :
      if (level < bullets.size())
        levelProps[level].put<ListLabelTypeInfo>(bullets[level]);
      break;
    case 3 :
    {
      IWORKTextLabel label;
      if (level < numberFormats.size())
      {
        switch (numberFormats[level] / 3)
        {
        case 0 :
          label.m_format.m_format = IWORK_LABEL_NUM_FORMAT_NUMERIC;
          break;
        case 1 :
          label.m_format.m_format = IWORK_LABEL_NUM_FORMAT_ROMAN;
          break;
        case 2 :
          label.m_format.m_format = IWORK_LABEL_NUM_FORMAT_ROMAN_LOWERCASE;
          break;
        case 3 :
          label.m_format.m_format = IWORK_LABEL_NUM_FORMAT_ALPHA;
          break;
        case 4 :
          label.m_format.m_format = IWORK_LABEL_NUM_FORMAT_ALPHA_LOWERCASE;
          break;
        default :
          ETONYEK_DEBUG_MSG(("parseListStyle: unknown label number format %u\n", numberFormats[level]));
          break;
        }
        switch (numberFormats[level] % 3)
        {
        case 0 :
          label.m_format.m_suffix = IWORK_LABEL_NUM_FORMAT_SURROUNDING_DOT;
          break;
        case 1 :
          label.m_format.m_prefix = IWORK_LABEL_NUM_FORMAT_SURROUNDING_PARENTHESIS;
          label.m_format.m_suffix = IWORK_LABEL_NUM_FORMAT_SURROUNDING_PARENTHESIS;
          break;
        case 2 :
          label.m_format.m_suffix = IWORK_LABEL_NUM_FORMAT_SURROUNDING_PARENTHESIS;
          break;
        }
      }
      if (level < tiered.size())
        label.m_tiered = tiered[level];
      levelProps[level].put<ListLabelTypeInfo>(label);
      break;
    }
    }
  }

  level = 0;
  for (IWAFloatField::const_iterator it = get(msg).float_(12).begin(); it != get(msg).float_(12).end(); ++it, ++level)
    levelProps[level].put<ListTextIndent>(*it);

  level = 0;
  for (IWAFloatField::const_iterator it = get(msg).float_(13).begin(); it != get(msg).float_(13).end(); ++it, ++level)
    levelProps[level].put<ListLabelIndent>(*it);

  level = 0;
  for (IWAMessageField::const_iterator it = get(msg).message(14).begin(); it != get(msg).message(14).end(); ++it, ++level)
  {
    IWORKListLabelGeometry geometry;
    if (it->float_(1))
      geometry.m_scale = get(it->float_(1));
    levelProps[level].put<ListLabelGeometry>(geometry);
  }

  if (bool(parent) && parent->has<ListLevelStyles>())
  {
    const IWORKListStyle_t &parentStyle = parent->get<ListLevelStyles>();
    for (const auto &it : parentStyle)
      levelProps[it.first].setParent(&it.second->getPropertyMap());
  }

  IWORKListStyle_t listStyle;
  for (map<unsigned, IWORKPropertyMap>::const_iterator it = levelProps.begin(); it != levelProps.end(); ++it)
    listStyle[it->first].reset(new IWORKStyle(it->second, none, none));

  IWORKPropertyMap props;
  props.put<ListLevelStyles>(listStyle);
  style.reset(new IWORKStyle(props, name, none));
}

void IWAParser::parseCharacterProperties(const IWAMessage &msg, IWORKPropertyMap &props)
{
  using namespace property;

  if (msg.bool_(1))
    props.put<Bold>(get(msg.bool_(1)));
  if (msg.bool_(2))
    props.put<Italic>(get(msg.bool_(2)));
  if (msg.float_(3))
    props.put<FontSize>(get(msg.float_(3)));
  if (msg.string(5))
    props.put<FontName>(get(msg.string(5)));
  const optional<IWORKColor> &fontColor = readColor(msg, 7);
  if (fontColor)
    props.put<FontColor>(get(fontColor));
  if (msg.uint32(10))
    putEnum<Baseline>(props, get(msg.uint32(10)));
  if (msg.bool_(11))
    props.put<Underline>(get(msg.bool_(11)));
  if (msg.bool_(12))
    props.put<Strikethru>(get(msg.bool_(12)));
  if (msg.uint32(13))
    putEnum<Capitalization>(props, get(msg.uint32(13)));
  if (msg.float_(14))
    props.put<BaselineShift>(get(msg.float_(14)));
  if (msg.float_(19))
    props.put<Outline>(get(msg.float_(19)));
  if (msg.float_(27))
    props.put<Tracking>(get(msg.float_(27)));
}

bool IWAParser::parseImage(const IWAMessage &msg)
{
  optional<unsigned> imageRef;

  const optional<unsigned> &filteredRef = readRef(msg, 15);
  if (filteredRef)
  {
    imageRef = filteredRef;
  }
  else
  {
    // FIXME: this is speculative
    const optional<unsigned> &fileRef1 = readRef(msg, 13);
    if (fileRef1)
      imageRef = fileRef1;
    else
      imageRef = readRef(msg, 11);
  }

  m_collector.startLevel();
  if (msg.message(1))
    parseShapePlacement(get(msg.message(1)));

  const IWORKMediaContentPtr_t content = make_shared<IWORKMediaContent>();
  if (imageRef)
  {
    const IWORKDataPtr_t data = make_shared<IWORKData>();
    data->m_stream = queryFile(get(imageRef));
    content->m_data = data;
  }
  content->m_size = readSize(msg, 9);
  if (!content->m_size)
    content->m_size = readSize(msg, 4);

  m_collector.collectMedia(content);
  m_collector.endLevel();

  return true;
}

void IWAParser::parseComment(const unsigned id)
{
  assert(bool(m_currentText));

  const ObjectMessage msg(*this, id, IWAObjectType::Comment);
  if (!msg)
    return;

  if (get(msg).string(1))
  {
    IWAText text(get(get(msg).string(1)), m_langManager);
    text.parse(*m_currentText);
  }
}

bool IWAParser::parseTabularInfo(const IWAMessage &msg)
{
  m_collector.startLevel();
  if (msg.message(1))
    parseShapePlacement(get(msg.message(1)));
  const optional<unsigned> &modelRef = readRef(msg, 2);
  if (modelRef)
    parseTabularModel(get(modelRef));
  m_collector.endLevel();
  return bool(modelRef);
}

void IWAParser::parseTabularModel(const unsigned id)
{
  const ObjectMessage msg(*this, id, IWAObjectType::TabularModel);
  if (!msg)
    return;

  const IWAUInt32Field &rows = get(msg).uint32(6);
  const IWAUInt32Field &columns = get(msg).uint32(7);
  if (!rows || !columns)
    return;

  m_currentTable.reset(new TableInfo(m_collector.createTable(m_tableNameMap, m_langManager), get(columns), get(rows)));
  m_currentTable->m_table->setSize(get(columns), get(rows));

  IWORKStylePtr_t tableStyle;
  const optional<unsigned> tableStyleRef = readRef(get(msg), 3);
  if (tableStyleRef)
    tableStyle = queryTableStyle(get(tableStyleRef));
  if (bool(tableStyle))
  {
    m_currentTable->m_style = tableStyle;
    m_currentTable->m_table->setStyle(tableStyle);

    if (tableStyle->has<property::SFTTableBandedCellFillProperty>())
    {
      IWORKPropertyMap props;
      props.put<property::Fill>(tableStyle->get<property::SFTTableBandedCellFillProperty>());
      m_currentTable->m_table->setDefaultCellStyle(IWORKTable::CELL_TYPE_ALTERNATE_BODY, make_shared<IWORKStyle>(props, none, none));
    }
  }

  optional<unsigned> tileRef;

  if (get(msg).message(4))
  {
    const IWAMessage &grid = get(get(msg).message(4));

    if (grid.message(1))
    {
      const optional<unsigned> &rowHeadersRef = readRef(get(grid.message(1)), 2);
      if (rowHeadersRef)
        parseHeaders(get(rowHeadersRef), m_currentTable->m_rowHeader);
    }
    const optional<unsigned> &columnHeadersRef = readRef(grid, 2);
    if (columnHeadersRef)
      parseHeaders(get(columnHeadersRef), m_currentTable->m_columnHeader);

    const optional<unsigned> &simpleTextListRef = readRef(grid, 4);
    if (simpleTextListRef)
      parseDataList(get(simpleTextListRef), m_currentTable->m_simpleTextList);
    const optional<unsigned> &cellStyleListRef = readRef(grid, 5);
    if (cellStyleListRef)
      parseDataList(get(cellStyleListRef), m_currentTable->m_cellStyleList);
    const optional<unsigned> &paraTextListRef = readRef(grid, 17);
    if (paraTextListRef)
      parseDataList(get(paraTextListRef), m_currentTable->m_formattedTextList);
    const optional<unsigned> &commentListRef = readRef(grid, 19);
    if (commentListRef)
      parseDataList(get(commentListRef), m_currentTable->m_commentList);

    m_currentTable->m_table->setSizes(makeSizes(m_currentTable->m_columnHeader.m_sizes), makeSizes(m_currentTable->m_rowHeader.m_sizes));

    if (grid.message(3) && grid.message(3).message(1))
      tileRef = readRef(get(grid.message(3).message(1)), 2);
  }

  m_currentTable->m_table->setHeaders(
    get_optional_value_or(get(msg).uint32(10).optional(), 0),
    get_optional_value_or(get(msg).uint32(9).optional(), 0),
    get_optional_value_or(get(msg).uint32(11).optional(), 0)
  );
  m_currentTable->m_table->setRepeated(
    get_optional_value_or(get(msg).bool_(13).optional(), false),
    get_optional_value_or(get(msg).bool_(12).optional(), false)
  );
  if (bool(tableStyle) && tableStyle->has<property::SFTTableBandedRowsProperty>())
    m_currentTable->m_table->setBandedRows(tableStyle->get<property::SFTTableBandedRowsProperty>());

  // default cell styles
  optional<unsigned> styleRef = readRef(get(msg), 18);
  if (styleRef)
    m_currentTable->m_table->setDefaultCellStyle(IWORKTable::CELL_TYPE_BODY, queryCellStyle(get(styleRef)));
  styleRef = readRef(get(msg), 19);
  if (styleRef)
    m_currentTable->m_table->setDefaultCellStyle(IWORKTable::CELL_TYPE_ROW_HEADER, queryCellStyle(get(styleRef)));
  styleRef = readRef(get(msg), 20);
  if (styleRef)
    m_currentTable->m_table->setDefaultCellStyle(IWORKTable::CELL_TYPE_COLUMN_HEADER, queryCellStyle(get(styleRef)));
  styleRef = readRef(get(msg), 21);
  if (styleRef)
    m_currentTable->m_table->setDefaultCellStyle(IWORKTable::CELL_TYPE_ROW_FOOTER, queryCellStyle(get(styleRef)));

  // default para styles
  styleRef = readRef(get(msg), 24);
  if (styleRef)
    m_currentTable->m_table->setDefaultParagraphStyle(IWORKTable::CELL_TYPE_BODY, queryParagraphStyle(get(styleRef)));
  styleRef = readRef(get(msg), 25);
  if (styleRef)
    m_currentTable->m_table->setDefaultParagraphStyle(IWORKTable::CELL_TYPE_ROW_HEADER, queryParagraphStyle(get(styleRef)));
  styleRef = readRef(get(msg), 26);
  if (styleRef)
    m_currentTable->m_table->setDefaultParagraphStyle(IWORKTable::CELL_TYPE_COLUMN_HEADER, queryParagraphStyle(get(styleRef)));
  styleRef = readRef(get(msg), 27);
  if (styleRef)
    m_currentTable->m_table->setDefaultParagraphStyle(IWORKTable::CELL_TYPE_ROW_FOOTER, queryParagraphStyle(get(styleRef)));

  // handle table
  if (tileRef)
    parseTile(get(tileRef));
  m_collector.collectTable(m_currentTable->m_table);
  m_currentTable.reset();
}

void IWAParser::parseDataList(const unsigned id, DataList_t &dataList)
{
  const ObjectMessage msg(*this, id, IWAObjectType::DataList);
  if (!msg)
    return;

  // TODO: it would likely to be more robust to parse everything.
  if (!get(msg).uint32(1))
    return;

  const unsigned type = get(get(msg).uint32(1));
  for (const auto &it : get(msg).message(3))
  {
    if (!it.uint32(1))
      continue;
    const unsigned index = get(it.uint32(1));
    switch (type)
    {
    case 1 :
      if (it.string(3))
        dataList[index] = get(it.string(3));
      break;
    case 4 :
      if (it.uint32(4))
        dataList[index] = get(it.uint32(4));
      break;
    case 9 :
      if (it.uint32(9))
        dataList[index] = get(it.uint32(9));
      break;
    case 10 :
      if (it.uint32(10))
        dataList[index] = get(it.uint32(10));
      break;
    default :
      ETONYEK_DEBUG_MSG(("IWAParser::parseDataList: unknown data list type %u\n", type));
      break;
    }
  }
}

void IWAParser::parseTile(const unsigned id)
{
  const ObjectMessage msg(*this, id, IWAObjectType::Tile);
  if (!msg)
    return;

  // rows must be fed to the collector in order
  typedef map<unsigned, const IWAMessage *> Rows_t;
  Rows_t rows;

  // save rows
  for (const auto &it : get(msg).message(5))
  {
    if (!it.uint32(1) || !it.bytes(3) || !it.bytes(4))
      continue;
    const unsigned row = get(it.uint32(1));
    if (row >= m_currentTable->m_rows)
    {
      ETONYEK_DEBUG_MSG(("IWAParser::parseTile: invalid row: %u\n", row));
      continue;
    }
    rows[row] = &it;
  }

  // process rows
  for (Rows_t::const_iterator it = rows.begin(); it != rows.end(); ++it)
  {
    const RVNGInputStreamPtr_t &input = get(it->second->bytes(3));
    auto length = unsigned(getLength(input));
    if (length >= 0xffff)
    {
      ETONYEK_DEBUG_MSG(("IWAParser::parseTile: invalid column data length: %u\n", length));
      length = 0xffff;
    }

    deque<optional<unsigned> > offsets;
    parseColumnOffsets(get(it->second->bytes(4)), length, offsets);

    for (deque<optional<unsigned> >::const_iterator offIt = offsets.begin(); offIt != offsets.end(); ++offIt)
    {
      if (!*offIt)
        continue;

      const unsigned column = offIt - offsets.begin();
      const unsigned row = it->first;

      IWORKCellType cellType = IWORK_CELL_TYPE_TEXT;
      IWORKStylePtr_t cellStyle;
      optional<string> text;
      optional<unsigned> textRef;

      // 1. Read the cell record
      // NOTE: The structure of the record is still not completely understood,
      // so we catch possible over-reading exceptions and continue.
      try
      {
        input->seek(get(*offIt) + 4, librevenge::RVNG_SEEK_SET);
        const unsigned flags = readU16(input);
        input->seek(6, librevenge::RVNG_SEEK_CUR);
        if (flags & 0x2) // cell style
        {
          const unsigned styleId = readU32(input);
          const DataList_t::const_iterator listIt = m_currentTable->m_cellStyleList.find(styleId);
          if (listIt != m_currentTable->m_cellStyleList.end())
          {
            if (const unsigned *const ref = boost::get<unsigned>(&listIt->second))
              cellStyle = queryCellStyle(*ref);
          }
        }
        if (flags & 0x4) // format
          readU32(input);
        if (flags & 0x8) // formula
          readU32(input);
        if (flags & 0x10) // simple text
        {
          const unsigned textId = readU32(input);
          const DataList_t::const_iterator listIt = m_currentTable->m_simpleTextList.find(textId);
          if (listIt != m_currentTable->m_simpleTextList.end())
          {
            if (const string *const s = boost::get<string>(&listIt->second))
              text = *s;
          }
        }
        if (flags & 0x1000) // comment
          readU32(input);
        if (flags & 0x20) // number
        {
          // TODO: parse value
          readU64(input);
        }
        if (flags & 0x40) // date
        {
          // TODO: parse value
          readU64(input);
        }
        if (flags & 0x200) // formatted text
        {
          const unsigned textId = readU32(input);
          const DataList_t::const_iterator listIt = m_currentTable->m_formattedTextList.find(textId);
          if (listIt != m_currentTable->m_formattedTextList.end())
          {
            if (const unsigned *const ref = boost::get<unsigned>(&listIt->second))
              textRef = *ref;
          }
        }

        assert(!m_currentText);
        m_currentText = m_collector.createText(m_langManager);

        if (bool(text))
        {
          m_currentText->insertText(get(text));
          m_currentText->flushSpan();
          m_currentText->flushParagraph();
        }
        else if (textRef)
        {
          parseText(get(textRef));
        }

        m_currentTable->m_table->insertCell(column, it->first, text, m_currentText, 1, 1, none, cellStyle, cellType);
        m_currentText.reset();
      }
      catch (...)
      {
        // ignore failure to read the last record
      }

      // 2. Create cell content
      m_currentText = m_collector.createText(m_langManager);

      // 2a. Get default font from table style
      if (m_currentTable->m_style)
      {
        IWORKPropertyMap defaultProps;
        if (m_currentTable->m_style->has<property::FontName>())
        {
          defaultProps.put<property::FontName>(m_currentTable->m_style->get<property::FontName>());
          m_currentText->pushBaseParagraphStyle(make_shared<IWORKStyle>(defaultProps, none, none));
        }
      }

      // 2b. Set default para and layout style
      m_currentText->pushBaseLayoutStyle(m_currentTable->m_table->getDefaultLayoutStyle(column, row));
      m_currentText->pushBaseParagraphStyle(m_currentTable->m_table->getDefaultParagraphStyle(column, row));

      // 2c. Insert text
      if (bool(text))
      {
        // TODO: handle embedded spaces and tabs (I assume line breaks are not allowed)
        m_currentText->insertText(get(text));
        m_currentText->flushSpan();
        m_currentText->flushParagraph();
      }
      else if (textRef)
      {
        parseText(get(textRef));
      }

      // 3. Insert the cell
      m_currentTable->m_table->insertCell(column, row, text, m_currentText, 1, 1, none, cellStyle, cellType);

      m_currentText.reset();
    }
  }
}

void IWAParser::parseHeaders(const unsigned id, TableHeader &header)
{
  const ObjectMessage msg(*this, id, IWAObjectType::Headers);
  if (!msg)
    return;

  for (const auto &it : get(msg).message(2))
  {
    if (it.uint32(1))
    {
      const unsigned index = get(it.uint32(1));
      if (index >= header.m_sizes.max_key())
      {
        ETONYEK_DEBUG_MSG(("IWAParser::parseHeaders: invalid row/column index %u\n", index));
        continue;
      }
      if (it.float_(2))
        header.m_sizes.insert_back(index, index + 1, get(it.float_(2)));
      if (it.bool_(3))
        header.m_hidden.insert_back(index, index + 1, get(it.bool_(3)));
    }
  }
}

void IWAParser::parseLink(const unsigned id, std::string &url)
{
  const ObjectMessage msg(*this, id, IWAObjectType::Link);
  if (!msg)
    return;

  if (get(msg).string(2))
    url = get(get(msg).string(2));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
