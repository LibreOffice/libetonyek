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
#include <map>
#include <utility>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>

#include "IWAObjectType.h"
#include "IWASnappyStream.h"
#include "IWORKCollector.h"
#include "IWORKNumberConverter.h"
#include "IWORKPath.h"
#include "IWORKProperties.h"
#include "IWORKTable.h"
#include "IWORKText.h"
#include "IWORKTypes.h"

namespace libetonyek
{

using boost::bind;
using boost::make_shared;
using boost::none;
using boost::optional;
using boost::shared_ptr;

using std::deque;
using std::make_pair;
using std::map;
using std::pair;
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

void mergeTextSpans(const map<unsigned, IWORKStylePtr_t> &paras,
                    const map<unsigned, IWORKStylePtr_t> &spans,
                    const map<unsigned, string> &langs,
                    map<unsigned, pair<IWORKStylePtr_t, IWORKStylePtr_t> > &merged)
{
  merged[0] = make_pair(IWORKStylePtr_t(), IWORKStylePtr_t());
  for (map<unsigned, IWORKStylePtr_t>::const_iterator it = paras.begin(); it != paras.end(); ++it)
    merged[it->first].first = it->second;
  for (map<unsigned, IWORKStylePtr_t>::const_iterator it = spans.begin(); it != spans.end(); ++it)
    merged[it->first].second = it->second;
  for (map<unsigned, string>::const_iterator it = langs.begin(); it != langs.end(); ++it)
  {
    map<unsigned, pair<IWORKStylePtr_t, IWORKStylePtr_t> >::iterator mergedIt = merged.lower_bound(it->first);
    if (mergedIt == merged.end())
      mergedIt = merged.insert(merged.end(), make_pair(it->first, make_pair(IWORKStylePtr_t(), IWORKStylePtr_t())));
    // TODO: create a new char. style with the language and make the
    // current char. style parent of it
    IWORKStylePtr_t langStyle;
    mergedIt->second.second = langStyle;
  }
}

void flushText(string &text, IWORKText &collector)
{
  if (!text.empty())
  {
    collector.insertText(text);
    text.clear();
  }
}

void writeText(const string &text, const unsigned start, const unsigned end, const bool endPara, IWORKText &collector)
{
  assert(end <= text.size());

  string buf;
  for (unsigned i = start; i < end; ++i)
  {
    switch (text[i])
    {
    case '\t' :
      flushText(buf, collector);
      collector.insertTab();
      break;
    case '\r' :
      flushText(buf, collector);
      collector.insertLineBreak();
      break;
    case '\n' :
      flushText(buf, collector);
      if (endPara && i != end - 1) // ignore the newline that ends the paragraph
        collector.insertLineBreak();
      break;
    default :
      buf.push_back(text[i]);
      break;
    }
  }

  flushText(buf, collector);
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
  : m_fragments(fragments)
  , m_package(package)
  , m_collector(collector)
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
    for (std::deque<IWAMessage>::const_iterator it = objs.begin(); it != objs.end(); ++it)
    {
      if (it->uint32(1))
        refs.push_back(it->uint32(1).get());
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
    // fall-through intended
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
    // fall-through intended
    case 0 :
      stroke.m_join = IWORK_LINE_JOIN_MITER;
      break;
    }
  }
  if (msg.message(6))
  {
    const deque<float> &elements = msg.message(6).float_(4).repeated();
    for (deque<float>::const_iterator it = elements.begin(); it != elements.end(); ++it)
      stroke.m_pattern.push_back(*it);
  }
}

void IWAParser::readFill(const IWAMessage &msg, IWORKFill &fill)
{
  const optional<IWORKColor> &color = readColor(msg, 1);
  if (color)
  {
    fill = get(color);
  }
  else if (msg.message(2))
  {
    IWORKGradient gradient;
    readGradient(get(msg.message(2)), gradient);
    fill = gradient;
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
      // fall-through intended
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
  }
}

void IWAParser::readGradient(const IWAMessage &msg, IWORKGradient &gradient)
{
  if (msg.uint32(1))
  {
    switch (get(msg.uint32(1)))
    {
    default :
      ETONYEK_DEBUG_MSG(("IWAParser::readGradient: unknown gradient type: %u", get(msg.uint32(1))));
    // fall-through intended
    case 0 :
      gradient.m_type = IWORK_GRADIENT_TYPE_LINEAR;
      break;
    case 1 :
      gradient.m_type = IWORK_GRADIENT_TYPE_RADIAL;
      break;
    }
  }
  for (IWAMessageField::const_iterator it = msg.message(2).begin(); it != msg.message(2).end(); ++it)
  {
    IWORKGradientStop stop;
    const optional<IWORKColor> &color = readColor(*it, 1);
    if (color)
      stop.m_color = get(color);
    if (it->float_(2))
      stop.m_fraction = get(it->float_(2));
    if (it->float_(3))
      stop.m_inflection = get(it->float_(3));
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
    const size_t length = get(text).size();
    map<unsigned, IWORKStylePtr_t> paras;
    if (get(msg).message(5))
    {
      IWORKStylePtr_t style = make_shared<IWORKStyle>(IWORKPropertyMap(), none, none);
      for (IWAMessageField::const_iterator it = get(msg).message(5).message(1).begin(); it != get(msg).message(5).message(1).end(); ++it)
      {
        if (it->uint32(1) && (get(it->uint32(1)) < length))
        {
          const optional<unsigned> &styleRef = readRef(*it, 2);
          if (styleRef)
          {
            const IWORKStylePtr_t &newStyle = queryParagraphStyle(get(styleRef));
            if (bool(newStyle))
              style = newStyle;
          }
          paras.insert(paras.end(), make_pair(get(it->uint32(1)), style));
        }
      }
    }

    map<unsigned, IWORKStylePtr_t> spans;
    if (get(msg).message(8))
    {
      for (IWAMessageField::const_iterator it = get(msg).message(8).message(1).begin(); it != get(msg).message(8).message(1).end(); ++it)
      {
        if (it->uint32(1) && (get(it->uint32(1)) < length))
        {
          IWORKStylePtr_t style;
          const optional<unsigned> &styleRef = readRef(*it, 2);
          if (styleRef)
            style = queryCharacterStyle(get(styleRef));
          spans.insert(spans.end(), make_pair(get(it->uint32(1)), style));
        }
      }
    }

    map<unsigned, string> langs;
    if (get(msg).message(19))
    {
      for (IWAMessageField::const_iterator it = get(msg).message(19).message(1).begin(); it != get(msg).message(19).message(1).end(); ++it)
      {
        if (it->uint32(1))
          langs.insert(langs.end(), make_pair(get(it->uint32(1)), string()));
      }
    }

    map<unsigned, pair<IWORKStylePtr_t, IWORKStylePtr_t> > textSpans;
    mergeTextSpans(paras, spans, langs, textSpans);

    for (map<unsigned, pair<IWORKStylePtr_t, IWORKStylePtr_t> >::const_iterator it = textSpans.begin(); it != textSpans.end();)
    {
      if (bool(it->second.first))
        m_currentText->openParagraph(it->second.first);
      m_currentText->openSpan(it->second.second);
      const unsigned start = it->first;
      ++it;
      if (it == textSpans.end())
      {
        writeText(get(text), start, length, true, *m_currentText);
        m_currentText->closeSpan();
        m_currentText->closeParagraph();
      }
      else
      {
        writeText(get(text), start, it->first, bool(it->second.first), *m_currentText);
        m_currentText->closeSpan();
        if (bool(it->second.first))
          m_currentText->closeParagraph();
      }
    }
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

bool IWAParser::parseDrawableShape(const IWAMessage &msg)
{
  m_collector.startLevel();

  const optional<IWAMessage> &shape = msg.message(1).optional();
  if (shape)
  {
    const optional<IWAMessage> &placement = get(shape).message(1).optional();
    if (placement)
      parseShapePlacement(get(placement));

    // const optional<unsigned> styleRef = readRef(get(shape), 2);

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
          for (deque<IWAMessage>::const_iterator it = elements.begin(); it != elements.end() && !closed; ++it)
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
              // fall-through intended
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
    m_currentText = m_collector.createText();
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
    for (deque<IWAMessage>::const_iterator it = fragments.begin(); it != fragments.end(); ++it)
    {
      if (it->uint32(1) && (it->string(2) || it->string(3)))
      {
        const unsigned pathIdx = it->string(3) ? 3 : 2;
        m_fragmentMap[it->uint32(1).get()] = make_pair("Index/" + it->string(pathIdx).get() + ".iwa", RVNGInputStreamPtr_t());
        m_fragmentObjectMap[it->uint32(1).get()] = make_pair(it->uint32(1).get(), ObjectRecord());
      }
      const deque<IWAMessage> &refs = it->message(6).repeated();
      for (deque<IWAMessage>::const_iterator refIt = refs.begin(); refIt != refs.end(); ++refIt)
      {
        if (refIt->uint32(1) && refIt->uint32(2))
          m_fragmentObjectMap[refIt->uint32(2).get()] = make_pair(refIt->uint32(1).get(), ObjectRecord());
      }
    }
    const deque<IWAMessage> &files = objectIndex.message(4).repeated();
    for (deque<IWAMessage>::const_iterator it = files.begin(); it != files.end(); ++it)
    {
      if (it->uint32(1))
      {
        const string virtualPath(it->string(3) ? ("Data/" + get(it->string(3))) : "");
        const string internalPath(it->string(4) ? ("Data/" + get(it->string(4))) : "");
        string path;
        if (!internalPath.empty() && m_package->existsSubStream(internalPath.c_str()))
          path = internalPath;
        else if (!virtualPath.empty() && m_package->existsSubStream(virtualPath.c_str()))
          path = virtualPath;
        if (!path.empty())
          m_fileMap[it->uint32(1).get()] = make_pair(path, RVNGInputStreamPtr_t());
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
  RecordMap_t::iterator it = m_fragmentObjectMap.begin();
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
      for (IWAMessageField::const_iterator it = tabStops.begin(); it != tabStops.message(1).end(); ++it)
      {
        if (it->float_(1))
          tabs.push_back(IWORKTabStop(get(it->float_(1))));
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
        readFill(get(styleProps.message(1)), fill);
        props.put<Fill>(fill);
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
      readFill(get(properties.message(1)), fill);
      props.put<Fill>(fill);
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
      readFill(get(properties.message(2)), fill);
      props.put<SFTTableBandedCellFillProperty>(fill);
    }
    if (properties.bool_(22))
      props.put<SFTAutoResizeProperty>(get(properties.bool_(22)));
    if (properties.string(41))
      props.put<FontName>(get(properties.string(41)));
  }

  style.reset(new IWORKStyle(props, name, parent));
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
    m_currentText->openParagraph(make_shared<IWORKStyle>(IWORKPropertyMap(), none, none));
    m_currentText->openSpan(IWORKStylePtr_t());
    const string &text = get(get(msg).string(1));
    writeText(text, 0, text.size(), false, *m_currentText);
    m_currentText->closeSpan();
    m_currentText->closeParagraph();
  }
}

bool IWAParser::parseTabularInfo(const IWAMessage &msg)
{
  if (msg.message(1))
    parseShapePlacement(get(msg.message(1)));
  const optional<unsigned> &modelRef = readRef(msg, 2);
  if (modelRef)
    parseTabularModel(get(modelRef));
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

  m_currentTable.reset(new TableInfo(m_collector.createTable(m_tableNameMap), get(columns), get(rows)));
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
  for (IWAMessageField::const_iterator it = get(msg).message(3).begin(); it != get(msg).message(3).end(); ++it)
  {
    if (!it->uint32(1))
      continue;
    const unsigned index = get(it->uint32(1));
    switch (type)
    {
    case 1 :
      if (it->string(3))
        dataList[index] = get(it->string(3));
      break;
    case 4 :
      if (it->uint32(4))
        dataList[index] = get(it->uint32(4));
      break;
    case 9 :
      if (it->uint32(9))
        dataList[index] = get(it->uint32(9));
      break;
    case 10 :
      if (it->uint32(10))
        dataList[index] = get(it->uint32(10));
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
  for (IWAMessageField::const_iterator it = get(msg).message(5).begin(); it != get(msg).message(5).end(); ++it)
  {
    if (!it->uint32(1) || !it->bytes(3) || !it->bytes(4))
      continue;
    const unsigned row = get(it->uint32(1));
    if (row >= m_currentTable->m_rows)
    {
      ETONYEK_DEBUG_MSG(("IWAParser::parseTile: invalid row: %u\n", row));
      continue;
    }
    rows[row] = &*it;
  }

  // process rows
  for (Rows_t::const_iterator it = rows.begin(); it != rows.end(); ++it)
  {
    const RVNGInputStreamPtr_t &input = get(it->second->bytes(3));
    unsigned length = unsigned(getLength(input));
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
        m_currentText = m_collector.createText();

        if (bool(text))
        {
          m_currentText->openParagraph();
          m_currentText->openSpan();
          m_currentText->insertText(get(text));
          m_currentText->closeSpan();
          m_currentText->closeParagraph();
        }
        else if (textRef)
        {
          parseText(get(textRef));
        }

        IWORKOutputElements elements;
        m_currentText->draw(elements);
        m_currentText.reset();

        m_currentTable->m_table->insertCell(column, it->first, text, elements, 1, 1, none, cellStyle, cellType);
      }
      catch (...)
      {
        // ignore failure to read the last record
      }

      // 2. Create cell content
      m_currentText = m_collector.createText();

      // 2a. Get default font from table style
      if (m_currentTable->m_style)
      {
        IWORKPropertyMap defaultProps;
        if (m_currentTable->m_style->has<property::FontName>())
        {
          defaultProps.put<property::FontName>(m_currentTable->m_style->get<property::FontName>());
          m_currentText->pushParagraphStyle(make_shared<IWORKStyle>(defaultProps, none, none));
        }
      }

      // 2b. Set default para and layout style
      m_currentText->pushLayoutStyle(m_currentTable->m_table->getDefaultLayoutStyle(column, row));
      m_currentText->pushParagraphStyle(m_currentTable->m_table->getDefaultParagraphStyle(column, row));

      // 2c. Insert text
      if (bool(text))
      {
        m_currentText->openParagraph();
        m_currentText->openSpan();
        // TODO: handle embedded spaces and tabs (I assume line breaks are not allowed)
        m_currentText->insertText(get(text));
        m_currentText->closeSpan();
        m_currentText->closeParagraph();
      }
      else if (textRef)
      {
        parseText(get(textRef));
      }

      IWORKOutputElements elements;
      m_currentText->draw(elements);
      m_currentText.reset();

      // 3. Insert the cell
      m_currentTable->m_table->insertCell(column, row, text, elements, 1, 1, none, cellStyle, cellType);
    }
  }
}

void IWAParser::parseHeaders(const unsigned id, TableHeader &header)
{
  const ObjectMessage msg(*this, id, IWAObjectType::Headers);
  if (!msg)
    return;

  for (IWAMessageField::const_iterator it = get(msg).message(2).begin(); it != get(msg).message(2).end(); ++it)
  {
    if (it->uint32(1))
    {
      const unsigned index = get(it->uint32(1));
      if (index >= header.m_sizes.max_key())
      {
        ETONYEK_DEBUG_MSG(("IWAParser::parseHeaders: invalid row/column index %u\n", index));
        continue;
      }
      if (it->float_(2))
        header.m_sizes.insert_back(index, index + 1, get(it->float_(2)));
      if (it->bool_(3))
        header.m_hidden.insert_back(index, index + 1, get(it->bool_(3)));
    }
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
