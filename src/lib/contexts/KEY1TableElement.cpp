/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <set>

#include <boost/fusion/include/std_pair.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include "KEY1TableElement.h"

#include "IWORKProperties.h"
#include "IWORKTable.h"
#include "IWORKText.h"
#include "IWORKTokenizer.h"

#include "KEYCollector.h"
#include "KEY1ContentElement.h"
#include "KEY1ParserState.h"
#include "KEY1StringConverter.h"
#include "KEY1Token.h"
#include "KEY1XMLContextBase.h"

#include "libetonyek_xml.h"

namespace libetonyek
{

namespace
{
template<typename T>
struct StringConverter
{
  static boost::optional<T> convert(const char *value);
};

#define DECLARE_STRING_CONVERTER(type) \
template<> \
struct StringConverter<type> \
{ \
static boost::optional<type> convert (const char *value); \
}

DECLARE_STRING_CONVERTER(IWORKPosition);
DECLARE_STRING_CONVERTER(IWORKSize);

namespace ascii = boost::spirit::ascii;
namespace qi =  boost::spirit::qi;
using  boost::phoenix::ref;
using boost::phoenix::construct;

boost::optional<IWORKPosition> StringConverter<IWORKPosition>::convert(const char *value)
{
  std::string val(value);
  std::string::const_iterator it = val.begin(), end = val.end();
  std::pair<int, int> d;
  boost::optional<IWORKPosition> res;
  if (qi::phrase_parse(it, end, qi::lit('{') >> qi::double_ >> qi::lit(',') >> qi::double_ >> qi::lit('}'), ascii::space, d) && it==end)
  {
    res=IWORKPosition();
    res->m_x=d.first;
    res->m_y=d.second;
  }
  else
  {
    ETONYEK_DEBUG_MSG(("StringConverter<IWORKPosition>: can not parse %s\n", value));
  }
  return res;
}

boost::optional<IWORKSize> StringConverter<IWORKSize>::convert(const char *value)
{
  std::string val(value);
  std::string::const_iterator it = val.begin(), end = val.end();
  std::pair<int, int> d;
  boost::optional<IWORKSize> res;
  if (qi::phrase_parse(it, end, qi::lit('{') >> qi::double_ >> qi::lit(',') >> qi::double_ >> qi::lit('}'), ascii::space, d) && it==end)
  {
    res=IWORKSize();
    res->m_width=d.first;
    res->m_height=d.second;
  }
  else
  {
    ETONYEK_DEBUG_MSG(("StringConverter<IWORKSize>: can not parse %s\n", value));
  }
  return res;
}

}

namespace
{
struct Element
{
  Element()
    : m_tl(0)
    , m_tr(0)
    , m_bl(0)
    , m_br(0)
    , m_content()
  {
  }
  unsigned m_tl, m_tr, m_bl, m_br;
  IWORKTextPtr_t m_content;
};

struct TableSegment
{
  TableSegment()
    : m_startPoint(0)
    , m_endPoint(0)
    , m_horizontal(true)
  {
  }
  unsigned m_startPoint;
  unsigned m_endPoint;
  bool m_horizontal;
};
}

struct KEY1TableElement::TableData
{
  TableData();

  std::deque<Element> m_elements;
  std::deque<TableSegment> m_horizontalSegments;
  std::deque<TableSegment> m_verticalSegments;

  std::unordered_map<unsigned, IWORKPosition> m_idToPositionMap;
};

KEY1TableElement::TableData::TableData()
  : m_elements()
  , m_horizontalSegments()
  , m_verticalSegments()
  , m_idToPositionMap()
{
}

namespace
{
class NodeElement : public KEY1XMLElementContextBase
{
public:
  explicit NodeElement(KEY1ParserState &state, KEY1TableElement::TableData &table);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  KEY1TableElement::TableData &m_table;
  boost::optional<unsigned> m_index;
  boost::optional<IWORKPosition> m_position;
};

NodeElement::NodeElement(KEY1ParserState &state, KEY1TableElement::TableData &table)
  : KEY1XMLElementContextBase(state)
  , m_table(table)
  , m_index()
  , m_position()
{
}

void NodeElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::ident :
    m_index = try_int_cast(value);
    break;
  case KEY1Token::pos :
    m_position = StringConverter<IWORKPosition>::convert(value);
    break;
  default :
    ETONYEK_DEBUG_MSG(("NodeElement::attribute[KEY1TableElement.cpp]: unexpected attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t NodeElement::element(const int name)
{
  switch (name)
  {
  default:
    ETONYEK_DEBUG_MSG(("NodeElement::element[KEY1TableElement.cpp]: unknown element\n"));
  }
  return IWORKXMLContextPtr_t();
}

void NodeElement::endOfElement()
{
  if (m_index && m_position)
    m_table.m_idToPositionMap[get(m_index)] = get(m_position);
}

}

namespace
{
class ElementElement : public KEY1XMLElementContextBase
{
public:
  explicit ElementElement(KEY1ParserState &state, KEY1TableElement::TableData &table);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  KEY1TableElement::TableData &m_table;
  unsigned m_tl, m_tr, m_bl, m_br;
  bool m_isText;
};

ElementElement::ElementElement(KEY1ParserState &state, KEY1TableElement::TableData &table)
  : KEY1XMLElementContextBase(state)
  , m_table(table)
  , m_tl(0)
  , m_tr(0)
  , m_bl(0)
  , m_br(0)
  , m_isText(false)
{
}

void ElementElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::type :
  {
    switch (getState().getTokenizer().getId(value))
    {
    case KEY1Token::text :
      m_isText=true;
      break;
    default :
      ETONYEK_DEBUG_MSG(("ElementElement::attribute[KEY1TableElement.cpp]: unexpected type %s\n", value));
      break;
    }
    break;
  }
  case KEY1Token::tl:
    m_tl=(unsigned) int_cast(value);
    break;
  case KEY1Token::tr:
    m_tr=(unsigned) int_cast(value);
    break;
  case KEY1Token::bl:
    m_bl=(unsigned) int_cast(value);
    break;
  case KEY1Token::br:
    m_br=(unsigned) int_cast(value);
    break;
  default :
    ETONYEK_DEBUG_MSG(("ElementElement::attribute[KEY1TableElement.cpp]: unexpected attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t ElementElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::content | KEY1Token::NS_URI_KEY :
    if (isCollector())
    {
      assert(!getState().m_currentText);
      getState().m_currentText = getCollector().createText(getState().m_langManager, false);
    }
    return makeContext<KEY1ContentElement>(getState());
  case KEY1Token::dict | KEY1Token::NS_URI_KEY :
    break;
  case KEY1Token::node | KEY1Token::NS_URI_KEY :
    return makeContext<NodeElement>(getState(), m_table);
  default:
    ETONYEK_DEBUG_MSG(("ElementElement::element[KEY1TableElement.cpp]: unknown element\n"));
  }
  return IWORKXMLContextPtr_t();
}

void ElementElement::endOfElement()
{
  m_table.m_elements.push_back(Element());
  Element &elt=m_table.m_elements.back();
  elt.m_tl=m_tl;
  elt.m_tr=m_tr;
  elt.m_bl=m_bl;
  elt.m_br=m_br;
  elt.m_content=getState().m_currentText;
  getState().m_currentText.reset();
}

}

namespace
{
class SegmentElement : public KEY1XMLElementContextBase
{
public:
  explicit SegmentElement(KEY1ParserState &state, KEY1TableElement::TableData &table);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  KEY1TableElement::TableData &m_table;
  TableSegment m_segment;
};

SegmentElement::SegmentElement(KEY1ParserState &state, KEY1TableElement::TableData &table)
  : KEY1XMLElementContextBase(state)
  , m_table(table)
  , m_segment()
{
}

void SegmentElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::direction :
  {
    switch (getState().getTokenizer().getId(value))
    {
    case KEY1Token::horizontal :
      m_segment.m_horizontal=true;
      break;
    case KEY1Token::vertical :
      m_segment.m_horizontal=false;
      break;
    default :
      ETONYEK_DEBUG_MSG(("SegmentElement::attribute[KEY1TableElement.cpp]: unexpected direction %s\n", value));
      break;
    }
    break;
  }
  case KEY1Token::start :
    m_segment.m_startPoint = (unsigned) int_cast(value);
    break;
  case KEY1Token::end :
    m_segment.m_endPoint = (unsigned) int_cast(value);
    break;
  default :
    ETONYEK_DEBUG_MSG(("SegmentElement::attribute[KEY1TableElement.cpp]: unexpected attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t SegmentElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::dict | KEY1Token::NS_URI_KEY :
    break;
  default:
    ETONYEK_DEBUG_MSG(("SegmentElement::element[KEY1TableElement.cpp]: unknown element\n"));
  }
  return IWORKXMLContextPtr_t();
}

void SegmentElement::endOfElement()
{
  if (m_segment.m_horizontal)
    m_table.m_horizontalSegments.push_back(m_segment);
  else
    m_table.m_verticalSegments.push_back(m_segment);
}

}

KEY1TableElement::KEY1TableElement(KEY1ParserState &state, boost::optional<IWORKSize> &size)
  : KEY1XMLElementContextBase(state)
  , m_key()
  , m_size(size)
  , m_tableData(new TableData)
{
}

void KEY1TableElement::attribute(const int name, const char *value)
{
  switch (name)
  {
  case KEY1Token::key :
    m_key = value;
    break;
  case KEY1Token::size :
    m_size = StringConverter<IWORKSize>::convert(value);
    break;
  case KEY1Token::version : // a double 1.2
    break;
  default:
    ETONYEK_DEBUG_MSG(("KEY1TableElement::attribute: unknown attribute with value=%s\n", value));
  }
}

void KEY1TableElement::startOfElement()
{
  if (isCollector())
  {
    getCollector().startLevel();

    assert(!getState().m_currentTable);
    getState().m_currentTable = getCollector().createTable(getState().m_tableNameMap, getState().m_langManager);
  }
}

IWORKXMLContextPtr_t KEY1TableElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::dict | KEY1Token::NS_URI_KEY :
    break;
  case KEY1Token::element | KEY1Token::NS_URI_KEY :
    return makeContext<ElementElement>(getState(), *m_tableData);
  case KEY1Token::segment | KEY1Token::NS_URI_KEY :
    return makeContext<SegmentElement>(getState(), *m_tableData);
  default:
    ETONYEK_DEBUG_MSG(("KEY1TableElement::element: unknown element\n"));
  }
  return IWORKXMLContextPtr_t();
}

void KEY1TableElement::endOfElement()
{
  if (!isCollector())
    return;

  auto const &table=getState().m_currentTable;
  if (table)
  {
    table->setRepeated(false, false);
    table->setHeaders(0, 0, 0);

    // compute the correspondance between the coordinate and the line
    std::set<double> xPosSet, yPosSet;
    std::unordered_map<unsigned, IWORKPosition>::const_iterator pIt;
    for (pIt=m_tableData->m_idToPositionMap.begin(); pIt!=m_tableData->m_idToPositionMap.end() ; ++pIt)
    {
      xPosSet.insert(pIt->second.m_x);
      yPosSet.insert(pIt->second.m_y);
    }
    std::deque<double> columnSizes, rowSizes;
    std::unordered_map<double, unsigned> xPosToLineMap, yPosToLineMap;
    unsigned line=0;
    double lastPos=0;
    bool first=true;
    for (std::set<double>::const_iterator lIt=xPosSet.begin() ; lIt!=xPosSet.end(); ++lIt)
    {
      xPosToLineMap[*lIt]=line++;
      if (!first)
        columnSizes.push_back(*lIt-lastPos);
      else
        first=false;
      lastPos=*lIt;
    }
    // y are inverted
    line=(unsigned) yPosSet.size();
    lastPos=0;
    first=true;
    for (std::set<double>::const_iterator lIt=yPosSet.begin() ; lIt!=yPosSet.end(); ++lIt)
    {
      yPosToLineMap[*lIt]=--line;
      if (!first)
        rowSizes.push_front(*lIt-lastPos);
      else
        first=false;
      lastPos=*lIt;
    }
    table->setSizes(columnSizes, rowSizes);

    // associate the point
    std::unordered_map<unsigned, unsigned> idToXPosMap, idToYPosMap;
    for (pIt=m_tableData->m_idToPositionMap.begin(); pIt!=m_tableData->m_idToPositionMap.end() ; ++pIt)
    {
      if (xPosToLineMap.find(pIt->second.m_x) != xPosToLineMap.end())
        idToXPosMap[pIt->first]=xPosToLineMap.find(pIt->second.m_x)->second;
      else
      {
        ETONYEK_DEBUG_MSG(("KEY1TableElement::endOfElement: can not find a column for id=%d\n", int(pIt->first)));
      }
      if (yPosToLineMap.find(pIt->second.m_y) != yPosToLineMap.end())
        idToYPosMap[pIt->first]=yPosToLineMap.find(pIt->second.m_y)->second;
      else
      {
        ETONYEK_DEBUG_MSG(("KEY1TableElement::endOfElement: can not find a row for id=%d\n", int(pIt->first)));
      }
    }
    // TODO recreate the border styles

    // recreate the cell
    for (std::deque<Element>::const_iterator eIt=m_tableData->m_elements.begin(); eIt!=m_tableData->m_elements.end(); ++eIt)
    {
      Element const &elt=*eIt;
      if (idToXPosMap.find(elt.m_tl)==idToXPosMap.end() ||
          idToYPosMap.find(elt.m_tl)==idToYPosMap.end())
      {
        ETONYEK_DEBUG_MSG(("KEY1TatleElement::endOfElement: can not find a cell position\n"));
        continue;
      }
      unsigned column=idToXPosMap.find(elt.m_tl)->second;
      unsigned row=idToYPosMap.find(elt.m_tl)->second;
      unsigned columnSpan=idToXPosMap.find(elt.m_br)!=idToXPosMap.end() && idToXPosMap.find(elt.m_br)->second>column ?
                          idToXPosMap.find(elt.m_br)->second-column : 1;
      unsigned rowSpan=idToYPosMap.find(elt.m_br)!=idToYPosMap.end() && idToYPosMap.find(elt.m_br)->second>row ?
                       idToYPosMap.find(elt.m_br)->second-row : 1;
      table->insertCell
      (column, row, boost::none, elt.m_content, boost::none, columnSpan, rowSpan);
    }

    // CHANGEME: the table is collected in KEY1Parser
  }
  getCollector().endLevel();
}
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
