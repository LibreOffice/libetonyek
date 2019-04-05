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
#include <iomanip>
#include <map>
#include <memory>
#include <sstream>
#include <utility>

#include <boost/optional.hpp>

#include "IWAObjectType.h"
#include "IWAText.h"
#include "IWORKCollector.h"
#include "IWORKFormula.h"
#include "IWORKNumberConverter.h"
#include "IWORKPath.h"
#include "IWORKProperties.h"
#include "IWORKTable.h"
#include "IWORKText.h"
#include "IWORKTransformation.h"
#include "IWORKTypes.h"

#include "PAGCollector.h"

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

void parseColumnOffsets(const RVNGInputStreamPtr_t &input, const unsigned length, map<unsigned,unsigned> &offsets)
{
  try
  {
    unsigned col=0;
    while (!input->isEnd())
    {
      const unsigned offset = readU16(input);
      if (offset<length && offset+4 < length)
        offsets[col]=offset;
      ++col;
    }
  }
  catch (...)
  {
    // ignore failure to read the last word: it can only happen in a broken file
  }
  // TODO: check that the valid offsets are sorted in ascending order
}

deque<IWORKColumnRowSize> makeSizes(const mdds::flat_segment_tree<unsigned, float> &sizes)
{
  IWORKColumnRowSize defVal;
  if (sizes.default_value()>0) defVal=IWORKColumnRowSize(sizes.default_value(),false);
  deque<IWORKColumnRowSize> out(sizes.max_key(), IWORKColumnRowSize());
  for (mdds::flat_segment_tree<unsigned, float>::const_iterator it = sizes.begin(); it != sizes.end();)
  {
    const deque<IWORKColumnRowSize>::iterator start(out.begin() + deque<double>::iterator::difference_type(it->first));
    const double size = it->second;
    ++it;
    const deque<IWORKColumnRowSize>::iterator end(it == sizes.end() ? out.end() : out.begin() +  deque<double>::iterator::difference_type(it->first));
    std::fill(start, end, size>0 ? IWORKColumnRowSize(size) : defVal);
  }
  return out;
}

}

IWAParser::Format::Format()
  : m_type()
  , m_format()
{
}

IWAParser::PageMaster::PageMaster()
  : m_style()
  , m_headerFootersSameAsPrevious(true)
{
}

IWAParser::TableHeader::TableHeader(const unsigned count, float defValue)
  : m_sizes(0, count, defValue)
  , m_hidden(0, count, false)
{
}

IWAParser::TableInfo::TableInfo(const shared_ptr<IWORKTable> &table, const unsigned columns, const unsigned rows)
  : m_table(table)
  , m_columns(columns)
  , m_rows(rows)
  , m_style()
  , m_columnHeader(columns)
  , m_rowHeader(rows,20)
  , m_simpleTextList()
  , m_cellStyleList()
  , m_formattedTextList()
  , m_formulaList()
  , m_formatList()
  , m_commentList()
{
}

IWAParser::IWAParser(const RVNGInputStreamPtr_t &fragments, const RVNGInputStreamPtr_t &package, IWORKCollector &collector)
  : m_langManager()
  , m_tableNameMap(std::make_shared<IWORKTableNameMap_t>())
  , m_currentText()
  , m_collector(collector)
  , m_index(fragments, package)
  , m_visited()
  , m_charStyles()
  , m_paraStyles()
  , m_sectionStyles()
  , m_graphicStyles()
  , m_mediaStyles()
  , m_cellStyles()
  , m_tableStyles()
  , m_listStyles()
  , m_currentTable()
  , m_uidFormatMap()
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
  else
  {
    ETONYEK_DEBUG_MSG(("IWAParser::ObjectMessage::ObjectMessage: object %u is actually visited\n", id));
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
  m_index.queryObject(id, type, msg);
}

boost::optional<unsigned> IWAParser::getObjectType(const unsigned id) const
{
  return m_index.getObjectType(id);
}

const RVNGInputStreamPtr_t IWAParser::queryFile(const unsigned id) const
{
  return m_index.queryFile(id);
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

boost::optional<uint64_t> IWAParser::readUID(const IWAMessage &msg, unsigned field)
{
  const IWAMessageField &id = msg.message(field);
  if (!id) return boost::none;
  if (id && get(id).uint32(1) && get(id).uint32(2))
    return (uint64_t(get(get(id).uint32(1)))<<32) | get(get(id).uint32(2));
  ETONYEK_DEBUG_MSG(("IWAParser::readUID: can not find the id zone\n"));
  return boost::none;
}

std::deque<uint64_t> IWAParser::readUIDs(const IWAMessage &msg, unsigned field)
{
  const std::deque<IWAMessage> &objs = msg.message(field).repeated();
  std::deque<uint64_t> res;
  for (const auto &obj : objs)
  {
    if (obj.uint32(1) && obj.uint32(2))
      res.push_back((uint64_t(get(obj.uint32(1)))<<32) | get(obj.uint32(2)));
  }
  return res;
}

boost::optional<std::string> IWAParser::readUUID(const IWAMessage &msg, const unsigned field)
{
  const IWAMessageField &mId = msg.message(field);
  if (!mId) return boost::none;
  auto const &id = get(mId).message(1);
  if (id && get(id).uint32(2) && get(id).uint32(3) && get(id).uint32(4) && get(id).uint32(5))
  {
    std::string res;
    bool hasValues=false;
    for (unsigned w=2; w<=5; ++w)
    {
      std::stringstream s;
      auto val=get(get(id).uint32(w));
      if (val) hasValues=true;
      std::uppercase(s);
      s << std::hex << std::setfill('0') << std::setw(8) << val;
      if (s.str().size()!=8)
      {
        ETONYEK_DEBUG_MSG(("IWAParser::readUUID: bad size\n"));
        return boost::none;
      }
      for (size_t c=0; c<4; ++c)
      {
        if ((w==3 || w==4) && (c%2)==0) res+='-';
        res+=s.str()[6-2*c];
        res+=s.str()[7-2*c];
      }
    }
    if (!hasValues) return none;
    return res;
  }
  ETONYEK_DEBUG_MSG(("IWAParser::readUUID: can not find the id zone\n"));
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
    stroke.m_pattern.m_type = IWORK_STROKE_TYPE_SOLID;
    if (msg.message(6).uint32(1))
    {
      switch (get(msg.message(6).uint32(1)))
      {
      default :
        ETONYEK_DEBUG_MSG(("IWAParser::readStroke: unknown stroke value: %u", get(msg.message(6).uint32(1))));
        ETONYEK_FALLTHROUGH;
      case 1:
        stroke.m_pattern.m_type = IWORK_STROKE_TYPE_SOLID;
        break;
      case 0:
        stroke.m_pattern.m_type = IWORK_STROKE_TYPE_DASHED;
        break;
      case 2:
        stroke.m_pattern.m_type = IWORK_STROKE_TYPE_NONE;
        break;
      }
    }
    unsigned remaining = 0;
    if (msg.message(6).uint32(3))
      remaining = get(msg.message(6).uint32(3));
    const deque<float> &elements = msg.message(6).float_(4).repeated();
    for (auto it = elements.begin(); it != elements.end() && remaining != 0; ++it)
      stroke.m_pattern.m_values.push_back(*it);
  }
  // todo: check also if there is a picture frame msg.message(8), if yes, use it as border
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
    IWORKMediaContent bitmap;
    if (msg.message(3).uint32(2))
    {
      switch (get(msg.message(3).uint32(2)))
      {
      default :
        ETONYEK_DEBUG_MSG(("IWAParser::readFill: unknown bitmap fill type: %u", get(msg.message(3).uint32(2))));
        ETONYEK_FALLTHROUGH;
      case 0 :
        bitmap.m_type = IWORK_IMAGE_TYPE_ORIGINAL_SIZE;
        break;
      case 1 :
        bitmap.m_type = IWORK_IMAGE_TYPE_STRETCH;
        break;
      case 2 :
        bitmap.m_type = IWORK_IMAGE_TYPE_TILE;
        break;
      case 3 :
        bitmap.m_type = IWORK_IMAGE_TYPE_SCALE_TO_FILL;
        break;
      case 4 :
        bitmap.m_type = IWORK_IMAGE_TYPE_SCALE_TO_FIT;
        break;
      }
    }
    bitmap.m_fillColor = readColor(get(msg.message(3)), 3);
    bitmap.m_size = readSize(get(msg.message(3)), 4);
    if (!bitmap.m_size) bitmap.m_size=IWORKSize(); // to do not change result from previous code
    const optional<unsigned> &fileRef = readRef(get(msg.message(3)), 6);
    if (fileRef)
    {
      // find also 16 with no file...
      bitmap.m_data = std::make_shared<IWORKData>();
      bitmap.m_data->m_stream = queryFile(get(fileRef));
      if (!bitmap.m_data->m_stream && !bitmap.m_fillColor) bitmap.m_fillColor = m_index.queryFileColor(get(fileRef));
    }
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
  // 4. blur
  if (msg.float_(5))
    shadow.m_opacity = get(msg.float_(5));
  // 6: bool true
  if (msg.bool_(6))
    shadow.m_visible = get(msg.bool_(6));
  // 7: type enum 0: drop,
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
  return dispatchShapeWithMessage(get(msg), msg.getType());
}

bool IWAParser::dispatchShapeWithMessage(const IWAMessage &msg, unsigned type)
{
  switch (type)
  {
  case IWAObjectType::ConnectionLine :
  case IWAObjectType::DrawableShape :
    return parseDrawableShape(msg, type==IWAObjectType::ConnectionLine);
  case IWAObjectType::Group :
    return parseGroup(msg);
  case IWAObjectType::Image :
    return parseImage(msg);
  case IWAObjectType::StickyNote:
    return parseStickyNote(msg);
  case IWAObjectType::TabularInfo :
    return parseTabularInfo(msg);
  default:
  {
    static bool first=true;
    if (first)
    {
      first=false;
      ETONYEK_DEBUG_MSG(("IWAParser::dispatchShape: find some unknown shapes, type=%d\n", int(type)));
    }
  }
  }

  return false;
}

bool IWAParser::parseText(const unsigned id, bool createNoteAsFootnote, const std::function<void(unsigned, IWORKStylePtr_t)> &openPageFunction)
{
  assert(bool(m_currentText));
  const ObjectMessage msg(*this, id);
  if (!msg)
    return false;
  if (msg.getType()==IWAObjectType::TextRef)
  {
    auto textRef=readRef(get(msg),1);
    if (!textRef)
    {
      ETONYEK_DEBUG_MSG(("IWAParser::parseText: can not find the text reference\n"));
      return false;
    }
    return parseText(get(textRef),createNoteAsFootnote,openPageFunction);
  }
  if (msg.getType()!=IWAObjectType::Text)
  {
    ETONYEK_DEBUG_MSG(("IWAParser::parseText: unexpected object type, type=%d\n", int(msg.getType())));
    return false;
  }
  std::multimap<unsigned, std::function<void(unsigned, bool &)> > attachments;
  const IWAStringField &text = get(msg).string(3);
  if (text || (openPageFunction && get(msg).message(17)))
  {
    // special case, when the document is empty and openPageFunction is
    //          defined, we still need to retrieve the headers/footers
    IWAText textParser(get_optional_value_or(text," "), m_langManager);
    const size_t length = text ? get(text).size() : 1;

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
    if (get(msg).message(9))
    {
      map<unsigned, IWORKFieldType> fields;
      for (const auto &it : get(msg).message(9).message(1))
      {
        if (!it.uint32(1))
        {
          ETONYEK_DEBUG_MSG(("IWAParser::parseText[9]: can not find the position\n"));
          continue;
        }
        const optional<unsigned> &ref = readRef(it, 2);
        if (!ref) continue;
        const ObjectMessage attachment(*this, get(ref));
        if (!attachment) continue;
        switch (attachment.getType())
        {
        case IWAObjectType::NoteStart: // the first character of a note seems special, so ...
          attachments.insert(make_pair(get(it.uint32(1)), [](unsigned, bool &ignore)
          {
            ignore=true;
          }));
          continue;
        case IWAObjectType::PageField:
        {
          if (!get(attachment).message(1)) break;
          const auto &field=get(get(attachment).message(1));
          if (field.uint32(2))
          {
            switch (get(field.uint32(2)))
            {
            case 0:
              attachments.insert(make_pair(get(it.uint32(1)),
                                           [this](unsigned, bool &ignore)
              {
                ignore=true;
                m_currentText->insertField(IWORKFieldType::IWORK_FIELD_PAGENUMBER);
              }));
              break;
            case 1:
              attachments.insert(make_pair(get(it.uint32(1)),
                                           [this](unsigned, bool &ignore)
              {
                ignore=true;
                m_currentText->insertField(IWORKFieldType::IWORK_FIELD_PAGECOUNT);
              }));
              break;
            default:
              ETONYEK_DEBUG_MSG(("IWAParser::parseText[9]: unknown field enum=%d\n", int(get(field.uint32(2)))));
            }
            continue;
          }
          break;
        }
        case IWAObjectType::ShapeField:
          if (!openPageFunction)
          {
            ETONYEK_DEBUG_MSG(("IWAParser::parseText[9]: find unexpected shape's attachment at pos=%d\n", int(get(it.uint32(1)))));
          }
          else
            attachments.insert(make_pair(get(it.uint32(1)),
                                         [this,ref](unsigned, bool &ignore)
          {
            ignore=true;
            parseAttachment(get(ref));
          }));
          continue;
        default:
          ETONYEK_DEBUG_MSG(("IWAParser::parseText[9]: find unknown object %d at position=%d\n", int(attachment.getType()), int(get(it.uint32(1)))));
          continue;
        }
        ETONYEK_DEBUG_MSG(("IWAParser::parseText[9]: can not read the object at position=%d\n", int(get(it.uint32(1)))));
      }
    }
    if (get(msg).message(11))
    {
      // placeholder:2031 or link:2032 or time field:2034
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
    if (openPageFunction && get(msg).message(12))
    {
      map<unsigned, IWORKStylePtr_t> sections;
      for (const auto &it : get(msg).message(12).message(1))
      {
        if (!it.uint32(1)) continue;
        const optional<unsigned> &sectionRef = readRef(it, 2);
        if (!sectionRef) continue;
        const IWORKStylePtr_t &sectionStyle = querySectionStyle(get(sectionRef));
        if (sectionStyle)
          sections.insert(sections.end(), make_pair(get(it.uint32(1)), sectionStyle));
      }
      textParser.setSections(sections);
    }
    if (get(msg).message(16))
    {
      for (const auto &it : get(msg).message(16).message(1))
      {
        if (!it.uint32(1)) continue;
        const optional<unsigned> &noteRef = readRef(it, 2);
        if (!noteRef) continue;
        const ObjectMessage noteMsg(*this, get(noteRef), IWAObjectType::Note);
        if (!noteMsg) continue;
        auto textRef=readRef(get(noteMsg), 2);
        if (textRef)
        {
          attachments.insert(make_pair(get(it.uint32(1)),
                                       [this,createNoteAsFootnote,textRef](unsigned, bool &ignore)
          {
            ignore=true;
            auto currentText=m_currentText;
            m_currentText = m_collector.createText(m_langManager);
            parseText(get(textRef));
            IWORKOutputElements elements;
            if (createNoteAsFootnote)
              elements.addOpenFootnote(librevenge::RVNGPropertyList());
            else
              elements.addOpenEndnote(librevenge::RVNGPropertyList());
            m_currentText->draw(elements);
            if (createNoteAsFootnote)
              elements.addCloseFootnote();
            else
              elements.addCloseEndnote();
            m_currentText=currentText;
            m_currentText->insertInlineContent(elements);
          }));
        }
        else
        {
          ETONYEK_DEBUG_MSG(("IWAParser::parseText[16]: can not find a note\n"));
        }
      }
    }
    if (openPageFunction && get(msg).message(17))
    {
      map<unsigned, IWORKStylePtr_t> pageMasters;
      for (const auto &it : get(msg).message(17).message(1))
      {
        if (!it.uint32(1)) continue;
        const optional<unsigned> &pageMasterRef = readRef(it, 2);
        if (!pageMasterRef) continue;
        PageMaster pageMaster;
        parsePageMaster(get(pageMasterRef), pageMaster);
        pageMasters.insert(pageMasters.end(), make_pair(get(it.uint32(1)), pageMaster.m_style));
      }
      textParser.setPageMasters(pageMasters);
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
    if (get(msg).message(23))
    {
      for (const auto &it : get(msg).message(23).message(1))
      {
        // no position
        if (!it.uint32(1)) continue;
        if (!it.message(2)) continue; // no text ref means end of comment, ...
        auto const &commentRef = readRef(it, 2);
        if (!commentRef) continue;
        const ObjectMessage commentMsg(*this, get(commentRef), IWAObjectType::CommentField);
        if (!commentMsg) continue;
        auto textRef=readRef(get(commentMsg), 1);
        // field 2: some small integer
        if (textRef)
        {
          attachments.insert(make_pair(get(it.uint32(1)),
                                       [this,textRef](unsigned, bool &)
          {
            auto currentText=m_currentText;
            m_currentText = m_collector.createText(m_langManager);
            parseComment(get(textRef));
            IWORKOutputElements elements;
            elements.addOpenComment(librevenge::RVNGPropertyList());
            m_currentText->draw(elements);
            elements.addCloseComment();
            m_currentText=currentText;
            m_currentText->insertInlineContent(elements);
          }));
        }
        else
        {
          ETONYEK_DEBUG_MSG(("IWAParser::parseText[23]: can not find a comment\n"));
        }
      }
    }
    textParser.setAttachments(attachments);
    textParser.parse(*m_currentText, openPageFunction);
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

const IWORKStylePtr_t IWAParser::querySectionStyle(const unsigned id) const
{
  return queryStyle(id, m_sectionStyles, bind(&IWAParser::parseSectionStyle, const_cast<IWAParser *>(this), _1, _2));
}

const IWORKStylePtr_t IWAParser::queryGraphicStyle(const unsigned id) const
{
  return queryStyle(id, m_graphicStyles, bind(&IWAParser::parseGraphicStyle, const_cast<IWAParser *>(this), _1, _2));
}

const IWORKStylePtr_t IWAParser::queryMediaStyle(const unsigned id) const
{
  return queryStyle(id, m_mediaStyles, bind(&IWAParser::parseMediaStyle, const_cast<IWAParser *>(this), _1, _2));
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

bool IWAParser::parseAttachment(const unsigned id)
{
  auto collector=dynamic_cast<PAGCollector *>(&m_collector);
  if (!collector)
  {
    ETONYEK_DEBUG_MSG(("IWAParser::parseAttachment: can not find the page collector\n"));
    return false;
  }
  const ObjectMessage msg(*this, id, IWAObjectType::ShapeField);
  if (!msg)
  {
    ETONYEK_DEBUG_MSG(("IWAParser::parseAttachment: can not find the attachment\n"));
    return false;
  }
  auto objectRef=readRef(get(msg),1);
  if (!objectRef)
  {
    ETONYEK_DEBUG_MSG(("IWAParser::parseAttachment: can not find the attached object\n"));
    return false;
  }

  IWORKPosition position;
  // 2: false
  auto x=get(msg).float_(3);
  // 4: false
  auto y=get(msg).float_(5);
  if (x && !std::isnan(get(x))) position.m_x=get(x);
  else
  {
    ETONYEK_DEBUG_MSG(("IWAParser::parseAttachment: can not find the x's position\n"));
  }
  if (y && !std::isnan(get(y))) position.m_y=get(y);
  else
  {
    ETONYEK_DEBUG_MSG(("IWAParser::parseAttachment: can not find the y's position\n"));
  }

  const ObjectMessage object(*this, get(objectRef));
  if (!object)
  {
    ETONYEK_DEBUG_MSG(("IWAParser::parseAttachment: can not find the attached object[II]\n"));
    return false;
  }
  auto currentText=m_currentText;
  m_currentText.reset();
  collector->startLevel();
  collector->startAttachments();
  collector->startAttachment();
  collector->collectAttachmentPosition(position);
  collector->getOutputManager().push();

  bool ok=false, sendInBlock=false;
  switch (object.getType())
  {
  case IWAObjectType::ConnectionLine :
  case IWAObjectType::DrawableShape :
    ok=parseDrawableShape(get(object), object.getType()==IWAObjectType::ConnectionLine);
    break;
  case IWAObjectType::Group :
    ok=parseGroup(get(object));
    break;
  case IWAObjectType::Image :
    ok=parseImage(get(object));
    break;
  case IWAObjectType::TabularInfo :
    sendInBlock=true;
    collector->collectAttachmentPosition(IWORKPosition());
    ok=parseTabularInfo(get(object));
    break;
  default:
  {
    static bool first=true;
    if (first)
    {
      first=false;
      ETONYEK_DEBUG_MSG(("IWAParser::parseAttachment: unknown object type\n"));
    }
  }
  }
  auto cId=collector->getOutputManager().save();
  auto content = collector->getOutputManager().get(cId);
  collector->getOutputManager().pop();
  collector->endAttachment();
  collector->endAttachments();
  collector->endLevel();

  if (ok)
  {
    if (sendInBlock)
      currentText->insertBlockContent(content);
    else
      currentText->insertInlineContent(content);
  }
  m_currentText=currentText;
  return ok;
}

bool IWAParser::parseArrowProperties(const IWAMessage &arrow, IWORKPropertyMap &props, bool headArrow)
{
  IWORKMarker marker;
  bool hasPath=false;
  if (arrow.message(1))
  {
    const auto &arrowProp=get(arrow.message(1));
    IWORKPathPtr_t path;
    if (parsePath(arrowProp, path) && path && !path->str().empty())
    {
      marker.m_path=path->str();
      hasPath=true;
    }
  }
  marker.m_endPoint=readPosition(arrow,3);
  // 2: a bool, 4: a bool, 5: name
  if (headArrow)
  {
    if (hasPath)
      props.put<property::HeadLineEnd>(marker);
    else
      props.clear<property::HeadLineEnd>();
  }
  else
  {
    if (hasPath)
      props.put<property::TailLineEnd>(marker);
    else
      props.clear<property::TailLineEnd>();
  }
  return true;
}

bool IWAParser::parsePath(const IWAMessage &msg, IWORKPathPtr_t &path)
{
  const deque<IWAMessage> &elements = msg.message(1).repeated();
  bool closed = false;
  bool closingMove = false;
  path.reset(new IWORKPath());
  for (auto it : elements)
  {
    const auto &type = it.uint32(1).optional();
    if (!type)
    {
      ETONYEK_DEBUG_MSG(("IWAParser::parsePath: can not read the type\n"));
      continue;
    }
    if (closed && closingMove)
    {
      ETONYEK_DEBUG_MSG(("IWAParser::parsePath: unexpected element %c after the closing move\n", get(type)));
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
      const optional<IWORKPosition> &coords = readPosition(it, 2);
      if (!coords)
      {
        ETONYEK_DEBUG_MSG(("IWAParser::parsePath: missing coordinates for %c element\n", get(type) == 1 ? 'M' : 'L'));
        return false;
      }
      if (get(type) == 1)
        path->appendMoveTo(get(coords).m_x, get(coords).m_y);
      else
      {
        if (path->empty())
        {
          ETONYEK_DEBUG_MSG(("IWAParser::parsePath: missing prior MoveTo subsequent LineTo\n"));
          return false;
        }
        path->appendLineTo(get(coords).m_x, get(coords).m_y);
      }
      break;
    }
    case 4 :
    {
      if (it.message(2))
      {
        const std::deque<IWAMessage> &positions = it.message(2).repeated();
        if (positions.size() >= 3)
        {
          if (positions.size() > 3)
          {
            ETONYEK_DEBUG_MSG(("IWAParser::parsePath: a curve has got %u control coords\n", unsigned(positions.size())));
          }
          const optional<float> &x = positions[0].float_(1).optional();
          const optional<float> &y = positions[0].float_(2).optional();
          const optional<float> &x1 = positions[1].float_(1).optional();
          const optional<float> &y1 = positions[1].float_(2).optional();
          const optional<float> &x2 = positions[2].float_(1).optional();
          const optional<float> &y2 = positions[2].float_(2).optional();
          path->appendCCurveTo(get_optional_value_or(x, 0), get_optional_value_or(y, 0),
                               get_optional_value_or(x1, 0), get_optional_value_or(y1, 0),
                               get_optional_value_or(x2, 0), get_optional_value_or(y2, 0));
        }
        else
        {
          ETONYEK_DEBUG_MSG(("IWAParser::parsePath: %u is not enough coords for a curve\n", unsigned(positions.size())));
          return false;
        }
      }
      break;
    }
    case 5 :
      path->appendClose();
      closed = true;
      break;
    default :
      ETONYEK_DEBUG_MSG(("IWAParser::parsePath: unknown bezier path element type %u\n", get(type)));
      return false;
    }
  }
  return true;
}

bool IWAParser::parseStickyNote(const IWAMessage &/*msg*/)
{
  ETONYEK_DEBUG_MSG(("IWAParser::parseStickyNote: not implemented\n"));
  return false;
}

bool IWAParser::parseDrawableShape(const IWAMessage &msg, bool isConnectionLine)
{
  m_collector.startLevel();

  const optional<IWAMessage> &shape = msg.message(1).optional();
  if (shape)
  {
    const optional<IWAMessage> &placement = get(shape).message(1).optional();
    if (placement)
      parseShapePlacement(get(placement));

    IWORKStylePtr_t style;
    const optional<unsigned> styleRef = readRef(get(shape), 2);
    if (styleRef)
      style=queryGraphicStyle(get(styleRef));
    // look for arrow Keynote 6
    if (get(shape).message(4) || get(shape).message(5))
    {
      if (!style)
        style=std::make_shared<IWORKStyle>(IWORKPropertyMap(),boost::none, boost::none);
      for (size_t st=0; st<2; ++st)
      {
        if (!get(shape).message(st+4)) continue;
        parseArrowProperties(get(get(shape).message(st+4)),style->getPropertyMap(),st==0);
      }
    }
    if (style)
      m_collector.setGraphicStyle(style);

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
            m_collector.collectStarPath(get(size), unsigned(get(point).m_x+0.4), get(point).m_y);
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
            m_collector.collectPolygonPath(get(size), unsigned(get(value)+0.4));
            break;
          default :
            ETONYEK_DEBUG_MSG(("IWAParser::parseDrawableShape: unknown scalar path type %u\n", get(type)));
            break;
          }
        }
      }
      else if (get(path).message(5))
      {
        auto const &bezier = get(path).message(5).get().message(3).optional();
        if (bezier)
        {
          IWORKPathPtr_t bezierPath;
          if (parsePath(get(bezier),bezierPath))
          {
            const optional<IWORKSize> &size = readSize(get(get(path).message(5)), 2);
            if (size)
            {
              double x[2]= {0,0}, y[2]= {0,0};
              if (bezierPath)
                bezierPath->computeBoundingBox(x[0], y[0], x[1], y[1]);
              // if we can not use the bounding box, assume tha the path is in unit area
              *bezierPath *= transformations::scale(get(size).m_width / (x[1]>x[0] ? x[1]-x[0] : 100), get(size).m_height / (y[1]>y[0] ? y[1]-y[0] : 100));
            }
            m_collector.collectBezier(bezierPath);
            m_collector.collectBezierPath();
          }
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
      else if (get(path).message(7))
      {
        auto rootMsg=get(get(path).message(7)).message(1).optional();
        if (rootMsg)
        {
          IWORKConnectionPath cPath;
          cPath.m_size=readSize(get(rootMsg), 2);
          cPath.m_isSpline=!get_optional_value_or(get(get(path).message(7)).bool_(2),false);
          auto const &bezier = rootMsg.get().message(3).optional();
          if (bezier)
          {
            const deque<IWAMessage> &elements = get(bezier).message(1).repeated();
            int pos=0;
            for (auto it : elements)
            {
              // normally first point (type 1) followed by 2 points (type 2)
              // const auto &type = it.uint32(1).optional();
              if (pos>=3)
              {
                ETONYEK_DEBUG_MSG(("IWAParser::parseDrawableShape[connection]: oops find unexpected number of points\n"));
                break;
              }
              cPath.m_positions[pos++]=readPosition(it, 2);
            }
            if (pos==3)
            {
              m_collector.collectConnectionPath(cPath);
            }
            else
            {
              ETONYEK_DEBUG_MSG(("IWAParser::parseDrawableShape[connection]: oops find unexpected number of points\n"));
            }
          }
          else
          {
            ETONYEK_DEBUG_MSG(("IWAParser::parseDrawableShape[connection]: can not find the points zone\n"));
          }
        }
        else
        {
          ETONYEK_DEBUG_MSG(("IWAParser::parseDrawableShape[connection]: can not find the root bezier zone\n"));
        }
      }
      else if (get(path).message(8)) // editable path
      {
        const IWAMessageField &pathPoints = get(get(path).message(8)).message(1);
        if (pathPoints && !pathPoints.message(1).empty())
        {
          const IWORKPathPtr_t editablePath(new IWORKPath());
          const IWAMessageField &points = pathPoints.message(1);
          std::vector<IWORKPosition> positions;
          // cubic bezier patch, [prev pt dir], pt, [next pt dir]
          for (auto it : points)
          {
            const optional<IWORKPosition> &point1 = readPosition(it, 1);
            const optional<IWORKPosition> &point2 = readPosition(it, 2);
            const optional<IWORKPosition> &point3 = readPosition(it, 3);
            // [4 type: {1: line, 3:curve}
            if (!point2)
            {
              ETONYEK_DEBUG_MSG(("IWAParser::parseDrawableShape: no control points for point2\n"));
              continue;
            }
            positions.push_back(get_optional_value_or(point1, get(point2)));
            positions.push_back(get(point2));
            positions.push_back(get_optional_value_or(point3, get(point2)));
          }
          size_t nbPt=positions.size()/3;
          if (nbPt<=1)
          {
            ETONYEK_DEBUG_MSG(("IWAParser::parseDrawableShape: find only %d points\n", int(nbPt)));
          }
          else
          {
            editablePath->appendMoveTo(positions[1].m_x, positions[1].m_y);
            bool isClosed= get_optional_value_or(pathPoints.bool_(2),false);
            for (size_t i=0; i<nbPt; ++i)
            {
              if (i+1==nbPt && !isClosed) break;
              auto const &prevPoint=positions[3*i+1];
              auto const &pt1=positions[3*i+2];
              auto const &pt2=positions[3*((i+1)%nbPt)];
              auto const &pt3=positions[3*((i+1)%nbPt)+1];
              if (samePoint(prevPoint,pt1) && samePoint(pt2,pt3))
                editablePath->appendLineTo(pt3.m_x, pt3.m_y);
              else
                editablePath->appendCCurveTo(pt1.m_x,pt1.m_y, pt2.m_x,pt2.m_y, pt3.m_x,pt3.m_y);
            }
            if (isClosed)
              editablePath->appendClose();
            m_collector.collectBezier(editablePath);
            m_collector.collectBezierPath();
          }
        }
      }
    }
  }
  bool hasText=false;
  if (!isConnectionLine)
  {
    const optional<unsigned> &textRef = readRef(msg, 2);
    if (textRef)
    {
      m_currentText = m_collector.createText(m_langManager, true);
      parseText(get(textRef));
      if (!m_currentText->empty())
      {
        hasText=true;
        m_collector.collectText(m_currentText);
      }
    }
  }

  if (shape || hasText)
    m_collector.collectShape();
  m_currentText.reset();

  m_collector.endLevel();

  return true;
}

bool IWAParser::parseGroup(const IWAMessage &msg)
{
  m_collector.startLevel();
  if (msg.message(1))
    parseShapePlacement(get(msg.message(1)));
  if (!msg.message(2).empty())
  {
    m_collector.startGroup();
    m_collector.openGroup();
    const deque<unsigned> &shapeRefs = readRefs(msg, 2);
    std::for_each(shapeRefs.begin(), shapeRefs.end(), bind(&IWAParser::dispatchShape, this, _1));
    m_collector.closeGroup();
    m_collector.endGroup();
  }
  m_collector.endLevel();

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
      geometry->m_naturalSize = geometry->m_size = get(size);
    if (get(g).uint32(3))
    {
      switch (get(get(g).uint32(3)))
      {
      case 0: // centered x, centered y
      {
        // the position is the position of the center of the shape
        // (often auto grow textboxes)
        if (pos && size && get(size).m_width>0 && get(size).m_height>0)
        {
          geometry->m_position.m_x -= get(size).m_width/2.;
          geometry->m_position.m_y -= get(size).m_height/2.;
          break;
        }
        static bool first=true;
        if (first)
        {
          ETONYEK_DEBUG_MSG(("IWAParser::parseShapePlacement: Ooops, find some centered shape\n"));
          first=false;
        }
        break;
      }
      case 1: // fixed x, centered y
      {
        if (pos && size && get(size).m_height>0)
        {
          geometry->m_position.m_y -= get(size).m_height/2.;
          break;
        }
        static bool first=true;
        if (first)
        {
          ETONYEK_DEBUG_MSG(("IWAParser::parseShapePlacement: Ooops, find some y centered shape\n"));
          first=false;
        }
        break;
      }
      // case 2: centered x, fixed y?
      case 3 : // normal
        break;
      case 7 : // horizontal flip
        geometry->m_horizontalFlip = true;
        break;
      default :
        ETONYEK_DEBUG_MSG(("IWAParser::parseShapePlacement: unknown transformation %u\n", get(get(g).uint32(3))));
        break;
      }
    }
    if (get(g).float_(4))
      geometry->m_angle = -deg2rad(get(get(g).float_(4)));
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

void IWAParser::parseMask(unsigned id, IWORKGeometryPtr_t &geometry, IWORKPathPtr_t &/*path*/)
{
  const ObjectMessage msg(*this, id, IWAObjectType::Mask);
  if (!msg)
    return;
  if (get(msg).message(1))
    parseShapePlacement(get(get(msg).message(1)), geometry);
  // if (get(msg).message(2)) same code as parseDrawableShape
}

void IWAParser::parseObjectIndex()
{
  m_index.parse();
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

  style = std::make_shared<IWORKStyle>(props, name, parent);
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
      auto const &lineSpace=paraProps.message(13).float_(2);
      if (lineSpace)
      {
        auto const &type=paraProps.message(13).uint32(1).optional();
        if (!type) // in line
          props.put<LineSpacing>(IWORKLineSpacing(get(lineSpace), true));
        else if (get(type)==1)   // at least in point
        {
          IWORKLineSpacing spacing(get(lineSpace), false);
          spacing.m_atLeast=true;
          props.put<LineSpacing>(spacing);
        }
        else if (get(type)==2) // in point
          props.put<LineSpacing>(IWORKLineSpacing(get(lineSpace), false));
        else if (get(type)==4) // between in point, transform in percent (and assume 12pt)
          props.put<LineSpacing>(IWORKLineSpacing(1.+get(lineSpace)/12., true));
        else   // unknown, use heuristic
        {
          props.put<LineSpacing>(IWORKLineSpacing(get(lineSpace), get(lineSpace)<3));
          ETONYEK_DEBUG_MSG(("IWAParser::parseParagraphStyle: unknown type %u\n", get(type)));
        }
      }
      // TODO what is paraProps.message(13).float_(3);
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
          tabs.push_back(IWORKTabStop(IWORK_TABULATION_LEFT, get(tabStop.float_(1))));
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

  style = std::make_shared<IWORKStyle>(props, name, parent);
}

void IWAParser::parseSectionStyle(const unsigned id, IWORKStylePtr_t &style)
{
  const ObjectMessage msg(*this, id, IWAObjectType::SectionStyle);
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
      parent = querySectionStyle(get(parentRef));
  }
  // 10: int 2 or 9
  //
  IWORKPropertyMap props;
  if (get(msg).message(11))
    parseColumnsProperties(get(get(msg).message(11)), props);
  style = std::make_shared<IWORKStyle>(props, name, parent);
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
        readShadow(get(styleProps.message(4)),shadow);
        props.put<Shadow>(shadow);
      }
      for (size_t st=0; st<2; ++st)
      {
        if (!get(styleProps).message(st+6)) continue;
        parseArrowProperties(get(get(styleProps).message(st+6)),props, st==0);
      }
    }
  }

  if (get(msg).message(11))
  {
    const IWAMessageField &layout = get(msg).message(11);
    auto vAlign=layout.uint32(2);
    if (vAlign)
    {
      IWORKVerticalAlignment const aligns[]=
      {IWORK_VERTICAL_ALIGNMENT_TOP, IWORK_VERTICAL_ALIGNMENT_MIDDLE,IWORK_VERTICAL_ALIGNMENT_BOTTOM};
      if (get(vAlign) < ETONYEK_NUM_ELEMENTS(aligns))
      {
        props.put<VerticalAlignment>(aligns[get(vAlign)]);
      }
      else
      {
        ETONYEK_DEBUG_MSG(("IWAParser::parseGraphicStyle: unknown vAlign %u\n", get(vAlign)));
      }
    }
    if (get(layout).message(6))
    {
      IWORKPadding padding;
      readPadding(get(get(layout).message(6)), padding);
      props.put<LayoutMargins>(padding);
    }
    const optional<unsigned> &paraRef = readRef(get(layout), 10);
    if (paraRef)
    {
      const IWORKStylePtr_t &paraStyle = queryParagraphStyle(get(paraRef));
      if (paraStyle)
        props.put<LayoutParagraphStyle>(paraStyle);
    }

    // TODO: other layout props: 1: shrink text, 4: columns
  }

  style = std::make_shared<IWORKStyle>(props, name, parent);
}

void IWAParser::parseMediaStyle(const unsigned id, IWORKStylePtr_t &style)
{
  const ObjectMessage msg(*this, id, IWAObjectType::MediaStyle);
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
      parent = queryMediaStyle(get(parentRef));
  }
  const IWAMessageField &styleProps = get(msg).message(11);
  if (styleProps)
  {
    if (styleProps.message(1))
    {
      IWORKStroke stroke;
      readStroke(get(styleProps.message(1)), stroke);
      props.put<Stroke>(stroke);
    }
    if (styleProps.float_(2))
      props.put<Opacity>(get(styleProps.float_(2)));
    if (styleProps.message(3))
    {
      IWORKShadow shadow;
      readShadow(get(styleProps.message(3)),shadow);
      props.put<Shadow>(shadow);
    }
    // 4: reflection
  }
  style = std::make_shared<IWORKStyle>(props, name, parent);
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
    if (properties.uint32(8))
    {
      auto align=get(properties.uint32(8));
      if (align<=2)
      {
        const IWORKVerticalAlignment aligns[] =
        {
          IWORK_VERTICAL_ALIGNMENT_TOP, IWORK_VERTICAL_ALIGNMENT_MIDDLE, IWORK_VERTICAL_ALIGNMENT_BOTTOM
        };
        props.put<VerticalAlignment>(aligns[align]);
      }
      else
      {
        ETONYEK_DEBUG_MSG(("IWAParser::parseCellStyle: unknown align=%u\n", align));
      }
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

  style = std::make_shared<IWORKStyle>(props, name, parent);
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

  style = std::make_shared<IWORKStyle>(props, name, parent);
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
  const IWAMessageField &images = get(msg).message(17);
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
    {
      // try to find the image, and revert to a default bullet if we find nothing
      char const defBullet[]= {char(0xe2), char(0x80), char(0xa2),0};
      if (level >= images.size())
      {
        // FIXME, in fact, the image is in the parent style...
        levelProps[level].put<ListLabelTypeInfo>(std::string(defBullet));
        break;
      }
      auto ref=readRef(images[level],3);
      if (!ref)
      {
        ETONYEK_DEBUG_MSG(("parseListStyle: can not find image ref for level %u\n", level));
        levelProps[level].put<ListLabelTypeInfo>(std::string(defBullet));
        break;
      }
      const IWORKMediaContentPtr_t content = make_shared<IWORKMediaContent>();
      auto stream = queryFile(get(ref));
      if (!stream)
      {
        // the image is probably in the theme model
        levelProps[level].put<ListLabelTypeInfo>(std::string(defBullet));
        break;
      }
      const IWORKDataPtr_t data = make_shared<IWORKData>();
      data->m_stream = stream;
      content->m_data = data;
      levelProps[level].put<ListLabelTypeInfo>(content);
      break;
    }
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
        default:
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
    const IWORKListLevels_t &parentStyle = parent->get<ListLevelStyles>();
    for (const auto &it : parentStyle)
      levelProps[it.first].setParent(&it.second->getPropertyMap());
  }

  IWORKListLevels_t listStyle;
  for (map<unsigned, IWORKPropertyMap>::const_iterator it = levelProps.begin(); it != levelProps.end(); ++it)
    listStyle[it->first] = std::make_shared<IWORKStyle>(it->second, none, none);

  IWORKPropertyMap props;
  props.put<ListLevelStyles>(listStyle);
  style = std::make_shared<IWORKStyle>(props, name, none);
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
  if (msg.float_(19)) // CHECKME
    props.put<Outline>(get(msg.float_(19))>0);
  if (msg.message(21))
  {
    IWORKShadow shadow;
    readShadow(get(msg.message(21)),shadow);
    props.put<TextShadow>(shadow);
  }
  const auto bgColor = readColor(msg, 26);
  if (bgColor)
    props.put<TextBackground>(get(bgColor));
  if (msg.float_(27))
    props.put<Tracking>(get(msg.float_(27)));
}

void IWAParser::parseColumnsProperties(const IWAMessage &msg, IWORKPropertyMap &props)
{
  using namespace property;
  // 1,2,3,5 bool
  // 4 float 0
  if (msg.message(7))
  {
    auto columnsMsg=get(msg.message(7));
    IWORKColumns columns;
    columns.m_columns.clear();
    if (columnsMsg.message(1))   // same columns size
    {
      auto columnDef=get(columnsMsg.message(1));
      columns.m_equal=true;
      auto n=get_optional_value_or(columnDef.uint32(1).optional(),0);
      auto s=get_optional_value_or(columnDef.float_(2).optional(),0.f);
      if (n>=1 && n<20)
      {
        IWORKColumns::Column column;
        column.m_width=(1.-(n-1)*double(s))/double(n);
        column.m_spacing=s;
        columns.m_columns.resize(size_t(n), column);
      }
    }
    else if (columnsMsg.message(2))
    {
      auto columnsDef=get(columnsMsg.message(2));
      columns.m_equal=false;
      IWORKColumns::Column column;
      column.m_width=get_optional_value_or(columnsDef.float_(1).optional(),0.f);
      columns.m_columns.push_back(column);
      for (const auto &it : columnsDef.message(2))
      {
        column.m_spacing=get_optional_value_or(it.float_(1).optional(),0.f);
        column.m_width=get_optional_value_or(it.float_(2).optional(),0.f);
        columns.m_columns.push_back(column);
      }
    }
    if (!columns.m_columns.empty())
      props.put<property::Columns>(columns);
  }
}

void IWAParser::parsePageMaster(unsigned id, PageMaster &pageMaster)
{
  const ObjectMessage msg(*this, id, IWAObjectType::PageMaster);
  if (!msg)
    return;
  if (get(msg).bool_(17))
    pageMaster.m_headerFootersSameAsPrevious=get(get(msg).bool_(17));
  bool hideHeaderOnFirstPage=false;
  if (get(msg).bool_(28))
    hideHeaderOnFirstPage=get(get(msg).bool_(28));
  // 18-22: some bool ?
  IWORKPropertyMap props;
  for (unsigned i=0; i<3; ++i)
  {
    auto hfRef=readRef(get(msg),23+i);
    if (!hfRef) continue;
    IWORKPageMaster pMaster;
    parseHeaderAndFooter(get(hfRef), pMaster);
    if (pMaster.m_header.empty() && pMaster.m_footer.empty())
      continue;
    // only the last pagemaster seem used...
    if (i!=2) continue;
    props.put<property::OddPageMaster>(pMaster);
    props.put<property::EvenPageMaster>(pMaster);
    if (!hideHeaderOnFirstPage)
      props.put<property::FirstPageMaster>(pMaster);
  }
  pageMaster.m_style = std::make_shared<IWORKStyle>(props, none, none);
  auto dataRef=readRef(get(msg),29);
  if (dataRef)
  {
    // useme: type 10016, field 1 a bool, [field 2] a ref to type 3047
  }
}

void IWAParser::parseHeaderAndFooter(unsigned id, IWORKPageMaster &hf)
{
  const ObjectMessage msg(*this, id, IWAObjectType::HeadersAndFooters);
  if (!msg)
    return;
  for (size_t wh=0; wh<2; ++wh)
  {
    bool empty=true;
    std::stringstream name;
    name << (wh==0 ? "PMHeader" : "PMFooter") << id;
    for (auto it = get(msg).message(wh+1).begin(); it != get(msg).message(wh+1).end(); ++it)
    {
      auto ref=it->uint32(1).optional();
      if (!ref) continue;
      auto currentText=m_currentText;
      m_currentText = m_collector.createText(m_langManager, true);
      parseText(get(ref));
      if (!m_currentText->empty())
      {
        m_collector.collectText(m_currentText);
        if (wh==0)
          m_collector.collectHeader(name.str());
        else
          m_collector.collectFooter(name.str());
        empty=false;
      }
      m_currentText=currentText;
    }
    if (empty) continue;
    if (wh==0)
      hf.m_header=name.str();
    else
      hf.m_footer=name.str();
  }

}

bool IWAParser::parseImage(const IWAMessage &msg)
{
  m_collector.startLevel();
  IWORKGeometryPtr_t geometry;
  if (msg.message(1))
  {
    parseShapePlacement(get(msg.message(1)), geometry);
    m_collector.collectGeometry(geometry);
  }

  const optional<unsigned> styleRef = readRef(msg, 3);
  if (styleRef)
    m_collector.setGraphicStyle(queryMediaStyle(get(styleRef)));

  IWORKGeometryPtr_t cropGeometry;
  const optional<unsigned> cropRef = readRef(msg, 5);
  if (cropRef)
  {
    IWORKPathPtr_t path;
    parseMask(get(cropRef), cropGeometry, path);
  }
  if (cropGeometry && geometry)
  {
    // CHANGEME: collector suppose that cropGeometry position is the
    //   final position but mask is relative to the original picture
    cropGeometry->m_position.m_x += geometry->m_position.m_x;
    cropGeometry->m_position.m_y += geometry->m_position.m_y;
  }

  const IWORKMediaContentPtr_t content = make_shared<IWORKMediaContent>();
  // 15: filtered, 11: basic image?, 12: small image, 13: ?
  unsigned const ids[]= {15, 13, 11, 12};
  for (auto id : ids)
  {
    auto const &ref=readRef(msg, id);
    if (!ref) continue;
    auto stream = queryFile(get(ref));
    if (!stream) continue;
    const IWORKDataPtr_t data = make_shared<IWORKData>();
    data->m_stream = stream;
    content->m_data = data;
    break;
  }
  content->m_size = readSize(msg, 9);
  if (!content->m_size)
    content->m_size = readSize(msg, 4);

  m_collector.collectMedia(content, cropGeometry);
  m_collector.endLevel();

  return true;
}

void IWAParser::parseAuthorInComment(unsigned id)
{
  assert(bool(m_currentText));
  const ObjectMessage msg(*this, id, IWAObjectType::AuthorStorage);
  if (!msg)
    return;
  if (get(msg).string(1))
  {
    auto str=get(get(msg).string(1));
    auto len=str.size();
    if (len==0) return;
    IWAText text(str+"\t", m_langManager);
    std::map<unsigned, IWORKStylePtr_t> spans;
    IWORKPropertyMap props;
    // normally yellow, but blue may be better in LO
    props.put<property::FontColor>(IWORKColor(0,0,1,1));
    spans[0]=std::make_shared<IWORKStyle>(props, boost::none, nullptr);
    // reset color to default, if not, comment will be blue colored
    props.put<property::FontColor>(IWORKColor(0,0,0,1));
    spans[unsigned(len)]=std::make_shared<IWORKStyle>(props, boost::none, nullptr);
    text.setSpans(spans);
    text.parse(*m_currentText);
  }
}

void IWAParser::parseComment(const unsigned id)
{
  assert(bool(m_currentText));

  unsigned actId=id;
  std::set<unsigned> seens;
  while (1)
  {
    if (seens.find(actId)!=seens.end())
    {
      ETONYEK_DEBUG_MSG(("IWAParser::parseComment: find a loop\n"));
      break;
    }
    seens.insert(actId);
    const ObjectMessage msg(*this, actId, IWAObjectType::Comment);
    if (!msg)
      return;
    auto authorRef=readRef(get(msg), 3);
    if (authorRef)
      parseAuthorInComment(*authorRef);
    // TODO add date which is in position 2
    if (get(msg).string(1))
    {
      IWAText text(get(get(msg).string(1)), m_langManager);
      text.parse(*m_currentText);
    }

    auto nextRef=readRef(get(msg), 4);
    if (!nextRef) break;
    actId=*nextRef;
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

  m_currentTable = std::make_shared<TableInfo>(m_collector.createTable(m_tableNameMap, m_langManager), get(columns), get(rows));
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
        parseTableHeaders(get(rowHeadersRef), m_currentTable->m_rowHeader);
    }
    const optional<unsigned> &columnHeadersRef = readRef(grid, 2);
    if (columnHeadersRef)
      parseTableHeaders(get(columnHeadersRef), m_currentTable->m_columnHeader);

    const optional<unsigned> &simpleTextListRef = readRef(grid, 4);
    if (simpleTextListRef)
      parseDataList(get(simpleTextListRef), m_currentTable->m_simpleTextList);
    const optional<unsigned> &cellStyleListRef = readRef(grid, 5);
    if (cellStyleListRef)
      parseDataList(get(cellStyleListRef), m_currentTable->m_cellStyleList);
    const optional<unsigned> &formulaListRef = readRef(grid, 6);
    if (formulaListRef)
      parseDataList(get(formulaListRef), m_currentTable->m_formulaList);
    const optional<unsigned> &formatListRef = readRef(grid, 11);
    if (formatListRef)
      parseDataList(get(formatListRef), m_currentTable->m_formatList);
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
  if (get(msg).string(8))
  {
    auto finalName=get(get(msg).string(8));
    // also update the table name map?
    if (m_tableNameMap->find(finalName)!=m_tableNameMap->end())
    {
      ETONYEK_DEBUG_MSG(("IWAParser::parseTabularModel: a table with name %s already exists\n", finalName.c_str()));
      // let create an unique name
      int nId=0;
      while (true)
      {
        std::stringstream s;
        s << finalName << "_" << ++nId;
        if (m_tableNameMap->find(s.str())!=m_tableNameMap->end()) continue;
        finalName=s.str();
        break;
      }
    }
    (*m_tableNameMap)[finalName]=finalName;
    if (get(msg).string(1))
      (*m_tableNameMap)[std::string("SFTGlobalID_")+get(get(msg).string(1))] = finalName;
    m_currentTable->m_table->setName(finalName);
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

  styleRef = readRef(get(msg), 49);
  if (styleRef)
  {
    IWORKGridLineMap_t gridLines[4];
    parseTableGridLines(get(styleRef), gridLines);
    m_currentTable->m_table->setBorders(gridLines[0],gridLines[1],gridLines[2],gridLines[3]);
  }

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
    case 2 :
      // it.uint32(2): some type
      if (it.message(6))
      {
        Format format;
        if (parseFormat(get(it.message(6)), format))
          dataList[index]=format;
      }
      else
      {
        ETONYEK_DEBUG_MSG(("IWAParser::parseDataList: can not find the format\n"));
      }
      break;
    case 3 :
    case 5 : // invalid formula
      if (it.message(5))
      {
        IWORKFormulaPtr_t formula;
        if (parseFormula(get(it.message(5)), formula) && formula)
          dataList[index]=formula;
      }
      else
      {
        ETONYEK_DEBUG_MSG(("IWAParser::parseDataList: can not find the formula\n"));
      }
      break;
    case 4 :
    {
      auto styleRef=readRef(it,4);
      if (styleRef)
        dataList[index]=get(styleRef);
      else if (it.uint32(4))
        dataList[index] = get(it.uint32(4));
      break;
    }
    case 8 :   // paragraph ref
    {
      auto textRef=readRef(it,9);
      if (textRef)
        dataList[index]=get(textRef);
      else
      {
        ETONYEK_DEBUG_MSG(("IWAParser::parseDataList: can not find the para ref\n"));
      }
      break;
    }
    case 9 :
      if (it.uint32(9))
        dataList[index] = get(it.uint32(9));
      break;
    case 10 :
    {
      auto commentRef=readRef(it,10);
      if (commentRef)
        dataList[index]=get(commentRef);
      else
      {
        ETONYEK_DEBUG_MSG(("IWAParser::parseDataList: can not find the cpmment ref\n"));
      }
      break;
    }
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
  for (auto it : rows)
  {
    const RVNGInputStreamPtr_t &input = get(it.second->bytes(3));
    auto length = unsigned(getLength(input));
    if (length >= 0xffff)
    {
      ETONYEK_DEBUG_MSG(("IWAParser::parseTile: invalid column data length: %u\n", length));
      length = 0xffff;
    }

    map<unsigned,unsigned> offsets;
    parseColumnOffsets(get(it.second->bytes(4)), length, offsets);

    for (auto offIt : offsets)
    {
      if (offIt.second+10>length)
      {
        ETONYEK_DEBUG_MSG(("IWAParser::parseTile: unexpected offset position\n"));
        continue;
      }
      const unsigned column = offIt.first;
      const unsigned row = it.first;

      IWORKCellType cellType = IWORK_CELL_TYPE_TEXT;
      IWORKStylePtr_t cellStyle;
      optional<unsigned> comment;
      IWORKFormulaPtr_t formula;
      optional<Format> format;
      optional<string> text;
      optional<IWORKDateTimeData> dateTime;
      optional<unsigned> textRef;

      // 1. Read the cell record
      // NOTE: The structure of the record is still not completely understood,
      // so we catch possible over-reading exceptions and continue.
      try
      {
        // 0: 4?
        input->seek((long) offIt.second+1, librevenge::RVNG_SEEK_SET);
        auto type=readU8(input);
        switch (type)
        {
        case 2:
        case 7: // duration (changeme)
          cellType=IWORK_CELL_TYPE_NUMBER;
          break;
        case 0: // empty (ok)
        case 3: // text (ok)
        case 9: // text zone
          break;
        case 5:
          cellType=IWORK_CELL_TYPE_DATE_TIME;
          break;
        case 6:
          cellType=IWORK_CELL_TYPE_BOOL;
          break;
        default:
          ETONYEK_DEBUG_MSG(("IWAParser::parseTile: unknown type %d\n", int(type)));
          break;
        }
        // 2,3: ?
        input->seek((long) offIt.second + 4, librevenge::RVNG_SEEK_SET);
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
        if (flags & 0x80) // unknown
          readU32(input);
        // flags & 0xc00, read 2 int
        if (flags & 0x4)   // format
        {
          const unsigned formatId=readU32(input);
          auto const formatIt=m_currentTable->m_formatList.find(formatId);
          if (formatIt !=m_currentTable->m_formatList.end())
          {
            if (auto ref = boost::get<Format>(&formatIt->second))
            {
              format=*ref;
              if (format->m_type && get(format->m_type)==IWORK_CELL_TYPE_NUMBER && cellType!=IWORK_CELL_TYPE_TEXT)
                format->m_type=cellType;
            }
          }
          else
          {
            ETONYEK_DEBUG_MSG(("IWAParser::parseTile: can not find format %d\n", int(formatId)));
          }
        }
        if (flags & 0x8) // formula
        {
          const unsigned formulaId = readU32(input);
          auto const formulaIt = m_currentTable->m_formulaList.find(formulaId);
          if (formulaIt !=m_currentTable->m_formulaList.end())
          {
            if (auto ref = boost::get<IWORKFormulaPtr_t>(&formulaIt->second))
              formula=*ref;
          }
          else
          {
            ETONYEK_DEBUG_MSG(("IWAParser::parseTile: can not find formula %d\n", int(formulaId)));
          }
        }
        if (flags & 0x1000) // comment
        {
          const unsigned commentId=readU32(input);
          auto const commentIt = m_currentTable->m_commentList.find(commentId);
          if (commentIt !=m_currentTable->m_commentList.end())
            comment=boost::get<unsigned>(commentIt->second);
          else
          {
            ETONYEK_DEBUG_MSG(("IWAParser::parseTile: can not find comment %d\n", int(commentId)));
          }
        }
        if (flags & 0x10) // simple text
        {
          const unsigned textId = readU32(input);
          const DataList_t::const_iterator listIt = m_currentTable->m_simpleTextList.find(textId);
          if (listIt != m_currentTable->m_simpleTextList.end())
          {
            if (const string *const s = boost::get<string>(&listIt->second))
              text = *s;
          }
          else
          {
            ETONYEK_DEBUG_MSG(("IWAParser::parseTile: can not find text %d\n", int(textId)));
          }
        }
        if (flags & 0x20) // number or duration(in second)
        {
          std::stringstream s;
          s << readDouble(input);
          text=s.str();
          if (!format)
          {
            format=Format();
            get(format).m_type = cellType==IWORK_CELL_TYPE_TEXT ? IWORK_CELL_TYPE_NUMBER : cellType;
            get(format).m_format=IWORKNumberFormat();
          }
        }
        if (flags & 0x40) // date
        {
          std::stringstream s;
          s << readDouble(input);
          text=s.str();
          if (!format)
          {
            format=Format();
            get(format).m_type=IWORK_CELL_TYPE_DATE_TIME;
            get(format).m_format=IWORKDateTimeFormat();
          }
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
      }
      catch (...)
      {
        // ignore failure to read the last record
      }

      if (format)
      {
        if (get(format).m_type) cellType=get(get(format).m_type);
        IWORKPropertyMap props;
        if (boost::get<IWORKNumberFormat>(&get(format).m_format))
          props.put<property::SFTCellStylePropertyNumberFormat>(*boost::get<IWORKNumberFormat>(&get(format).m_format));
        else if (boost::get<IWORKDateTimeFormat>(&get(format).m_format))
          props.put<property::SFTCellStylePropertyDateTimeFormat>(*boost::get<IWORKDateTimeFormat>(&get(format).m_format));
        else if (boost::get<IWORKDurationFormat>(&get(format).m_format))
          props.put<property::SFTCellStylePropertyDurationFormat>(*boost::get<IWORKDurationFormat>(&get(format).m_format));
        cellStyle.reset(new IWORKStyle(props, none, cellStyle));
      }

      bool needText=textRef || (bool(text) && !formula && cellType == IWORK_CELL_TYPE_TEXT);
      if (needText)
      {
        assert(!m_currentText);
        m_currentText = m_collector.createText(m_langManager);
        if (textRef)
          parseText(get(textRef));
        else
        {
          m_currentText = m_collector.createText(m_langManager);
          // update the style
          m_currentText->pushBaseLayoutStyle(m_currentTable->m_table->getDefaultLayoutStyle(column, row));
          // do we need to set m_currentTable->m_style->has<property::FontName>() ?
          m_currentText->pushBaseParagraphStyle(m_currentTable->m_table->getDefaultParagraphStyle(column, row));
          m_currentText->insertText(get(text));
          m_currentText->flushSpan();
          m_currentText->flushParagraph();
        }
      }
      m_currentTable->m_table->insertCell(column, row, text, m_currentText, dateTime, 1, 1, formula, unsigned(row*256+column), cellStyle, cellType);
      if (comment)
      {
        auto currentText=m_currentText;
        m_currentText = m_collector.createText(m_langManager);
        parseComment(get(comment));
        IWORKOutputElements elements;
        m_currentText->draw(elements);
        m_currentText=currentText;
        m_currentTable->m_table->setComment(column, row, elements);
      }
      m_currentText.reset();
    }
  }
}

void IWAParser::parseTableHeaders(const unsigned id, TableHeader &header)
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
        ETONYEK_DEBUG_MSG(("IWAParser::parseTableHeaders: invalid row/column index %u\n", index));
        continue;
      }
      if (it.float_(2))
        header.m_sizes.insert_back(index, index + 1, get(it.float_(2)));
      if (it.bool_(3))
        header.m_hidden.insert_back(index, index + 1, get(it.bool_(3)));
    }
  }
}

void IWAParser::parseTableGridLines(unsigned id, IWORKGridLineMap_t (&gridLines)[4])
{
  const ObjectMessage msg(*this, id, IWAObjectType::GridLines);
  if (!msg)
  {
    ETONYEK_DEBUG_MSG(("IWAParser::parseTableGridLInes: can not find grid lines for index %u\n", id));
    return;
  }
  for (unsigned wh=0; wh<4; ++wh)
  {
    if (get(msg).message(wh+4).empty()) continue;
    auto const &lineRefs = readRefs(get(msg), wh+4);
    auto &gridLine=gridLines[wh];
    std::for_each(lineRefs.begin(), lineRefs.end(),
                  [this,&gridLine](unsigned lineId)
    {
      parseTableGridLine(lineId, gridLine);
    }
                 );
  }
}

void IWAParser::parseTableGridLine(unsigned id, IWORKGridLineMap_t &gridLine)
{
  const ObjectMessage msg(*this, id, IWAObjectType::GridLine);
  if (!msg)
  {
    ETONYEK_DEBUG_MSG(("IWAParser::parseTableGridLine: can not find grid line for index %u\n", id));
    return;
  }
  if (!get(msg).uint32(1))
  {
    ETONYEK_DEBUG_MSG(("IWAParser::parseTableGridLine: can not find the main position index %u\n", id));
    return;
  }
  auto pos1=get(get(msg).uint32(1));
  const deque<IWAMessage> &lines = get(msg).message(2).repeated();
  if (gridLine.find(pos1)==gridLine.end())
    gridLine.insert(IWORKGridLineMap_t::value_type(pos1,IWORKGridLine_t(0,4096,nullptr)));
  auto &flatSegments=gridLine.find(pos1)->second;
  for (auto it : lines)
  {
    if (!it.uint32(1) || !it.uint32(2))
    {
      ETONYEK_DEBUG_MSG(("IWAParser::parseTableGridLine: can not find the second position index %u\n", id));
      continue;
    }
    IWORKPropertyMap props;
    if (it.message(3))
    {
      IWORKStroke stroke;
      readStroke(get(it.message(3)), stroke);
      props.put<property::SFTStrokeProperty>(stroke);
    }
    flatSegments.insert_back(get(it.uint32(1)),get(it.uint32(1))+get(it.uint32(2)), std::make_shared<IWORKStyle>(props,none,none));
  }
}

bool IWAParser::parseFormat(const IWAMessage &msg, IWAParser::Format &format)
{
  if (!msg.uint32(1))
  {
    ETONYEK_DEBUG_MSG(("IWAParser::parseFormat: can not find the main type\n"));
    return false;
  }
  auto uid=readUID(msg,41);
  if (uid)
  {
    auto it= m_uidFormatMap.find(get(uid));
    if (it==m_uidFormatMap.end())
    {
      ETONYEK_DEBUG_MSG(("IWAParser::parseFormat: can not find the format %llx\n", get(uid)));
      return false;
    }
    format=it->second;
    return true;
  }

  auto type=get(msg.uint32(1));
  format.m_type=IWORK_CELL_TYPE_NUMBER;
  IWORKCellNumberType nType=IWORK_CELL_NUMBER_TYPE_DOUBLE;
  switch (type)
  {
  case 1: // automatic
    return true;
  case 263: // checkbox
  case 264: // stepper
  case 265: // slider
  case 266: // pop-up menu
  case 267: // star rating
    return false;
  case 257: // currency
    nType=IWORK_CELL_NUMBER_TYPE_CURRENCY;
    break;
  case 258: // percentage
    nType=IWORK_CELL_NUMBER_TYPE_PERCENTAGE;
    break;
  case 259: // scientific
    nType=IWORK_CELL_NUMBER_TYPE_SCIENTIFIC;
    break;
  case 262: // fraction
    nType=IWORK_CELL_NUMBER_TYPE_FRACTION;
    break;
  case 256: // number
  case 269: // numeral system
    break;
  case 270: // custom number
    // TODO
    break;
  case 260:
    format.m_type=IWORK_CELL_TYPE_TEXT;
    return true;
  case 271: // custom text
    // normally, we use msg.string(18) as text for a not empty cell
    // ie. store it when we see it in a custom format cell
    //     then use it to define the cell content
    format.m_type=IWORK_CELL_TYPE_TEXT;
    return true;
  case 261:
    format.m_type=IWORK_CELL_TYPE_DATE_TIME;
    if (msg.string(14))
    {
      IWORKDateTimeFormat dtFormat;
      dtFormat.m_format=get(msg.string(14));
      format.m_format=dtFormat;
      return true;
    }
    else
    {
      ETONYEK_DEBUG_MSG(("IWAParser::parseFormat: can not find the format string\n"));
      return false;
    }
    break;
  case 272:
    format.m_type=IWORK_CELL_TYPE_DATE_TIME;
    if (msg.string(18))
    {
      IWORKDateTimeFormat dtFormat;
      dtFormat.m_format=get(msg.string(18));
      format.m_format=dtFormat;
      return true;
    }
    ETONYEK_DEBUG_MSG(("IWAParser::parseFormat: can not find the date/time format\n"));
    return false;
  case 268:
  {
    format.m_type=IWORK_CELL_TYPE_DURATION;
    // read 7: style and then 15 and 16
    IWORKDurationFormat dFormat;
    dFormat.m_format="%H:%M:%S";
    format.m_format=dFormat;
    return true;
  }
  default:
    ETONYEK_DEBUG_MSG(("IWAParser::parseFormat: find unknown type\n"));
    return false;
  }
  IWORKNumberFormat nFormat;
  nFormat.m_type=nType;
  if (msg.uint32(2)) nFormat.m_decimalPlaces=int(get(msg.uint32(2)));
  else if (msg.uint32(9)) nFormat.m_decimalPlaces=int(get(msg.uint32(9)));
  if (nFormat.m_decimalPlaces>128) nFormat.m_decimalPlaces=-1; // 253 means automatic?
  if (msg.string(3)) nFormat.m_currencyCode=get(msg.string(3));
  if (msg.bool_(4)) nFormat.m_thousandsSeparator=get(msg.bool_(4));
  if (msg.bool_(5)) nFormat.m_accountingStyle=get(msg.bool_(5));
  if (msg.uint32(8)) nFormat.m_base=int(get(msg.uint32(8)));
  if (msg.uint32(11)) nFormat.m_fractionAccuracy=int(get(msg.uint32(11)));
  format.m_format=nFormat;
  return true;
}

void IWAParser::parseCustomFormat(unsigned id)
{
  const ObjectMessage msg(*this, id, IWAObjectType::CustomDateTimeFormat);
  if (!msg) return;
  auto const &uidLists = readUIDs(get(msg),1);
  auto const &formatList = get(msg).message(2).repeated();
  if (uidLists.size()!=formatList.size())
  {
    ETONYEK_DEBUG_MSG(("IWAParser::parseFormat: find unexpected data size\n"));
    return;
  }
  std::map<uint64_t, std::string> idToFormatMap;
  for (size_t i=0; i<uidLists.size(); ++i)
  {
    auto const &formatMsg=formatList[i];
    // 1: name, 2: type
    auto const &formatDef=formatMsg.message(3);
    Format format;
    if (!formatDef || !parseFormat(get(formatDef), format))
    {
      ETONYEK_DEBUG_MSG(("IWAParser::parseFormat: can not find string zone\n"));
      continue;
    }
    m_uidFormatMap[uidLists[i]]=format;
  }
}

bool IWAParser::parseFormula(const IWAMessage &msg, IWORKFormulaPtr_t &formula)
{
  if (!msg.message(1))
  {
    ETONYEK_DEBUG_MSG(("IWAParser::parseFormula: can not find the token table\n"));
    return false;
  }
  const deque<IWAMessage> &tokens = get(msg.message(1)).message(1).repeated();

  typedef std::vector<IWORKFormula::Token> Formula;
  std::vector<Formula> stack;
  bool ok=true;
  for (auto it : tokens)
  {
    auto type=it.uint32(1).optional();
    if (!type)
    {
      ETONYEK_DEBUG_MSG(("IWAParser::parseFormula: can not find the token type\n"));
      ok=false;
      break;
    }
    switch (get(type))
    {
    case 16:
      if (it.uint32(2) && it.uint32(3))
      {
        static std::map<unsigned,std::string> functionsMap=
        {
          {1, "Abs"}, {2, "Accrint"}, {3, "AccrintM"}, {4, "Acos"}, {5, "Acosh"},
          {6, "IWORKFormula::Address"}, {7, "And"}, {8, "Areas"}, {9, "Asin"}, {10, "AsinH"},
          {11, "Atan"}, {12, "Atan2"}, {13, "AtanH"}, {14, "AverageDev"}, {15, "Average"},
          {16, "AverageA"}, {17, "Ceiling"}, {18, "Char"}, {19,"Choose"}, {20, "Clean"},
          {21, "Code"}, {22, "Column"}, {23, "Columns"}, {24, "ComBin"}, {25, "Concatenate"},
          {26, "Confidence"}, {27, "Correl"}, {28, "Cos"}, {29, "CosH"}, {30, "Count"},
          {31, "CountA"}, {32, "CountBlank"}, {33, "CountIf"}, {34, "CoupDayBs"}, {35, "CoupDays"},
          {36, "CoupDaySNC"}, {37, "CoupNum"}, {38, "CoVar"}, {39, "Date"}, {40, "DateDif"},
          {41, "Day"}, {42, "DB"}, {43, "DDB"}, {44, "Degrees"}, {45, "Disc"},
          {46, "Dollar"}, {47, "EDate"}, {48, "Even"}, {49, "Exact"}, {50, "Exp"},
          {51, "Fact"}, {52, "False"}, {53, "Find"}, {54, "Fixed"}, {55, "Floor"},
          {56, "Forecast"}, {57, "Frequency"}, {58, "GCD"}, {59, "HLookUp"}, {60, "Hour"},
          {61, "HyperLink"}, {62, "If"}, {63, "Index"}, {64, "Indirect"}, {65, "Int"},
          {66, "Intercept"}, {67, "IPMT"}, {68, "Irr"}, {69, "IsBlank"}, {70, "IsError"},
          {71, "IsEven"}, {72, "IsOdd"}, {73, "IsPMT"}, {74, "Large"}, {75, "LCM"},
          {76, "Left"}, {77, "Len"}, {78, "LN"}, {79, "Log"}, {80, "Log10"},
          {81, "LookUp"}, {82, "Lower"}, {83, "Match"}, {84, "Max"}, {85, "MaxA"},
          {86, "Median"}, {87, "Mid"}, {88, "Min"}, {89, "MinA"}, {90, "Minute"},
          {91, "Mirr"}, {92, "Mod"}, {93, "Mode"}, {94, "Month"}, {95, "MRound"},
          {96, "Not"}, {97, "Now"}, {98, "NPer"}, {99, "NPV"}, {100, "Odd"},
          {101, "Offset"}, {102, "Or"}, {103, "Percentile"}, {104, "Pi"}, {105, "PMT"},
          {106, "Poisson"}, {107, "Power"}, {108, "PPMT"}, {109, "Price"}, {110, "PriceDist"},
          {111, "PriceMat"}, {112, "Prob"}, {113, "Product"}, {114, "Proper"}, {115, "PV"},
          {116, "Quotient"}, {117, "Radians"}, {118, "Rand"}, {119, "RandBetween"}, {120, "Rank"},
          {121, "Rate"}, {122, "Replace"}, {123, "Repeat"}, {124, "Right"}, {125, "Roman"},
          {126, "Round"}, {127, "RoundDown"}, {128, "RoundUp"}, {129, "Row"}, {130, "Rows"},
          {131, "Search"}, {132, "Second"}, {133, "Sign"}, {134, "Sin"}, {135, "SinH"},
          {136, "SLN"}, {137, "Slope"}, {138, "Small"}, {139, "Sqrt"}, {140, "STDEV"},
          {141, "STDEVA"}, {142, "STDEVP"}, {143, "STDEVPA"}, {144, "Substitute"}, {145, "SumIf"},
          {146, "SumProduct"}, {147, "SumSqrt"}, {148, "Syd"}, {149, "T"}, {150, "Tan"},
          {151, "TanH"}, {152, "Time"}, {153, "TimeValue"}, {154, "Today"}, {155, "Trim"},
          {156, "True"}, {157, "Trunc"}, {158, "Upper"}, {159, "Value"}, {160, "Var"},
          {161, "VarA"}, {162, "VarP"}, {163, "VarPA"}, {164, "VDB"}, {165, "VLookup"},
          {166, "WeekDay"}, {167, "Year"}, {168, "Sum"},
          {185, "Effect"},
          {186, "Nominal"}, {187, "NormDist"}, {188, "NormsDist"}, {189, "NormInv"},  {190, "NormsInv"},
          {191, "Yield"}, {192, "YieldDist"}, {193, "YieldMat"}, {194, "BondDuration"}, {195, "BondMDuration"},
          {196, "Erf"}, {197, "ErfC"}, {198, "Standardize"}, {199, "IntRate"}, {200, "Received"},
          {201, "CUMIPMT"}, {202, "CUMPRINC"}, {203, "EOMonth"}, {204, "WorkDay"}, {205, "MonthName"},
          {206, "WeekNum"}, {207, "Dur2Hours"}, {208, "Dur2Minutes"}, {209, "Dur2Seconds"}, {210, "Dur2Days"},
          {211, "Dur2Weeks"}, {212, "Duration"}, {213, "ExpOnDist"}, {214, "YearFrac"}, {215, "ZTest"},
          {216, "SumX2MY2"}, {217, "SumX2PY2"}, {218, "SumXMY2"}, {219, "SqrtPi"}, {220, "Transpose"},
          {221, "DevSQ"}, {222, "FV"}, {223, "Delta"}, {224, "FactDouble"}, {225, "GEStep"},
          {226, "PercentRank"},{227, "GammaLN"},{228, "DateValue"},{229, "GammaDist"},{230, "GammaInv"},
          {231, "SumIfs"}, {232, "AverageIfs"}, {233, "CountIfs"}, {234, "AverageIf"}, {235, "IfError"},
          {236, "DayName"}, {237, "BesselJ"}, {238,"BesselY"}, {239,"LogNormDist"}, {240,"LogInv"},
          {241, "TDist"}, {242, "BinomDist"}, {243, "NegBinomDist"}, {244, "FDist"}, {245, "Permut"},
          {246, "ChiDist"}, {247, "ChiTest"}, {248, "TTest"}, {249, "Quartile"}, {250, "Multinomial"},
          {251, "CritBinom"}, {252, "BaseToNum"}, {253, "NumToBase"}, {254, "TInv"}, {255, "Convert"},
          {256, "ChiInv"}, {257, "FInv"}, {258, "BetaDist"}, {259, "BetaInv"}, {260, "NetWorkDays"},
          {261, "Days360"}, {262, "HarMean"}, {263, "GeoMin"}, {264, "Dec2Hex"}, {265, "Dec2Bin"},
          {266, "Dec2Oct"}, {267, "Bin2Hex"}, {268, "Bin2Dec"}, {269, "Bin2Oct"}, {270, "Oct2Bin"},
          {271, "Oct2Dec"}, {272, "Oct2Hex"}, {273, "Hex2Bin"}, {274, "Hex2Dec"}, {275, "Hex2Oct"},
          {276, "Linest"}, {277, "Dur2Milliseconds"}, {278, "StripDuration"}, {280, "Intercept.Ranges"},
          {285, "Union.Ranges"},
          {286, "SeriesSum"}, {287, "Polynomial"}, {288, "WeiBull"},
          {297, "PlainText"}, {298, "Stock"}, {299, "StockH"}, {300, "Currency"},
          {301, "CurrencyH"}, {302, "CurrencyConvert"}, {303, "CurrencyCode"}
        };
        Formula child;
        std::ostringstream s;
        if (functionsMap.find(get(it.uint32(2)))!=functionsMap.end())
          s << functionsMap.find(get(it.uint32(2)))->second;
        else
          s << "Funct" << get(it.uint32(2));
        child.push_back(IWORKFormula::Token(s.str(), IWORKFormula::Token::Function));

        size_t numArgs=get(it.uint32(3));
        size_t numData=stack.size();
        if (numData<numArgs)
        {
          ETONYEK_DEBUG_MSG(("IWAParser::parseFormula: bad stack for function\n"));
          ok=false;
          break;
        }
        child.push_back(IWORKFormula::Token("(", IWORKFormula::Token::Operator));
        for (size_t i=numData-numArgs; i<numData; ++i)
        {
          if (i!=numData-numArgs) child.push_back(IWORKFormula::Token(";", IWORKFormula::Token::Operator));
          child.insert(child.end(), stack[i].begin(),stack[i].end());
        }
        child.push_back(IWORKFormula::Token(")", IWORKFormula::Token::Operator));
        stack.resize(numData-numArgs);
        stack.push_back(child);
      }
      else
      {
        ETONYEK_DEBUG_MSG(("IWAParser::parseFormula: can not read a function\n"));
        ok=false;
      }
      break;
    case 17:
      if (it.double_(4))
        stack.push_back({IWORKFormula::Token(get(it.double_(4)))});
      else
      {
        ETONYEK_DEBUG_MSG(("IWAParser::parseFormula: can not read a double\n"));
        ok=false;
      }
      break;
    case 18:
      if (it.bool_(5))
      {
        stack.push_back({IWORKFormula::Token(get(it.bool_(5)) ? "True" : "False", IWORKFormula::Token::Function),
                         IWORKFormula::Token("(", IWORKFormula::Token::Operator), IWORKFormula::Token(")", IWORKFormula::Token::Operator)
                        });
      }
      else
      {
        ETONYEK_DEBUG_MSG(("IWAParser::parseFormula: can not read a bool\n"));
        ok=false;
      }
      break;
    case 19:
      if (it.string(6))
        stack.push_back({IWORKFormula::Token(get(it.string(6)),IWORKFormula::Token::String)});
      else
      {
        ETONYEK_DEBUG_MSG(("IWAParser::parseFormula: can not read a string\n"));
        ok=false;
      }
      break;
    case 22: // empty?
      stack.push_back({});
      break;
    case 23:
      if (it.bool_(10))
        stack.push_back({});
      else
      {
        ETONYEK_DEBUG_MSG(("IWAParser::parseFormula: can not read a optional argument\n"));
        ok=false;
      }
      break;
    case 25:
      if (it.uint32(13))
      {
        size_t numArgs=get(it.uint32(13));
        size_t numData=stack.size();
        if (numData<numArgs)
        {
          ETONYEK_DEBUG_MSG(("IWAParser::parseFormula: bad stack for ()\n"));
          ok=false;
          break;
        }
        Formula child;
        child.push_back(IWORKFormula::Token("(", IWORKFormula::Token::Operator));
        for (size_t i=numData-numArgs; i<numData; ++i)
        {
          if (i!=numData-numArgs) child.push_back(IWORKFormula::Token(";", IWORKFormula::Token::Operator));
          child.insert(child.end(), stack[i].begin(),stack[i].end());
        }
        child.push_back(IWORKFormula::Token(")", IWORKFormula::Token::Operator));
        stack.resize(numData-numArgs);
        stack.push_back(child);
      }
      else
      {
        ETONYEK_DEBUG_MSG(("IWAParser::parseFormula: can not read a ()\n"));
        ok=false;
      }
      break;
    case 32: // separator,
    case 33:
      break;
    case 36:
    {
      IWORKFormula::Address address;
      for (unsigned i=0; i<2; ++i)
      {
        auto pos=it.message(26+i);
        if (!pos) continue;
        if (!get(pos).sint32(1))
        {
          ETONYEK_DEBUG_MSG(("IWAParser::parseFormula: can not read a position\n"));
          ok=false;
          break;
        }
        IWORKFormula::Coord coord;
        coord.m_absolute = get_optional_value_or(get(pos).bool_(2).optional(), false);
        coord.m_coord=get(get(pos).sint32(1))+1;
        if (i==0)
          address.m_column=coord;
        else
          address.m_row=coord;
      }
      address.m_table=readUUID(it,28);
      // readUUID(it,38); filename ?
      if (!ok)
        break;
      stack.push_back({IWORKFormula::Token(address)});
      break;
    }
    case 34: // arg begin
    case 35: // arg end
      break;
    default:
      if ((get(type)>=1 && get(type)<=15) || get(type)==29)
      {
        char const *wh[] =
        {
          nullptr, "+", "-", "*", "/",
          "^", "&", ">", ">=",
          "<", "<=", "=", "<>",
          "-", "+", "%"
        };
        if (get(type)!=29 && !wh[get(type)])
        {
          ETONYEK_DEBUG_MSG(("IWAParser::parseFormula: find unexpected type=%u\n", get(type)));
          ok=false;
          break;
        }
        size_t numArgs=(get(type)<13 || get(type)==29) ? 2 : 1;
        size_t numData=stack.size();
        if (numData<numArgs)
        {
          ETONYEK_DEBUG_MSG(("IWAParser::parseFormula: bad stack for type=%u\n", get(type)));
          ok=false;
          break;
        }
        Formula child;
        if (numArgs==2)
        {
          child.insert(child.end(), stack[numData-2].begin(),stack[numData-2].end());
          child.push_back(IWORKFormula::Token(get(type)==29 ? ":" : wh[get(type)], IWORKFormula::Token::Operator));
          child.insert(child.end(), stack[numData-1].begin(),stack[numData-1].end());
        }
        else if (get(type)<15)
        {
          child.push_back(IWORKFormula::Token(wh[get(type)], IWORKFormula::Token::Operator));
          child.insert(child.end(), stack[numData-1].begin(),stack[numData-1].end());
        }
        else
        {
          child.insert(child.end(), stack[numData-1].begin(),stack[numData-1].end());
          child.push_back(IWORKFormula::Token(wh[get(type)], IWORKFormula::Token::Operator));
        }
        stack.resize(numData-numArgs);
        stack.push_back(child);
        break;
      }
      ETONYEK_DEBUG_MSG(("IWAParser::parseFormula: find unexpected type=%u\n", get(type)));
      ok=false;
      break;
    }
    if (!ok)
      break;
  }
  if (stack.size()!=1) ok=false;
  if (!ok)
  {
    std::ostringstream readData;
    for (auto const &form : stack)
      for (auto const &dt : form)
        readData << dt << ",";
    ETONYEK_DEBUG_MSG(("IWAParser::parseFormula: can not find parse a formula=%s\n", readData.str().c_str()));
  }
  else
  {
    formula.reset(new IWORKFormula(0));
    formula->parse(stack[0]);
  }
  return ok;
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
