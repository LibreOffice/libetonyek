/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/lexical_cast.hpp>
#include <boost/math/constants/constants.hpp>

#include "libetonyek_xml.h"
#include "KEY2ParserUtils.h"
#include "KEY2Token.h"
#include "KEYTypes.h"
#include "KEYXMLReader.h"

using boost::lexical_cast;
using boost::optional;

using std::pair;

namespace libetonyek
{

namespace
{

template <typename T1, typename T2>
pair<T1, T2>
readAttributePair(const KEYXMLReader &reader, const int name1, const int ns1, const int name2, const int ns2, const bool empty = true)
{
  optional<T1> a1;
  optional<T2> a2;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((ns1 == getNamespaceId(attr)) && (name1 == getNameId(attr)))
    {
      a1 = lexical_cast<T1>(attr.getValue());
    }
    else if ((ns2 == getNamespaceId(attr)) && (name2 == getNameId(attr)))
    {
      a2 = lexical_cast<T2>(attr.getValue());
    }
  }

  if (empty)
    checkEmptyElement(reader);

  if (!a1 || !a2)
    throw GenericException();

  return pair<T1, T2>(get(a1), get(a2));
}

}

optional<ID_t> KEY2ParserUtils::readID(const KEYXMLReader &reader)
{
  optional<ID_t> id;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KEY2Token::NS_URI_SFA == getNamespaceId(attr)) && (KEY2Token::ID == getNameId(attr)))
      id = attr.getValue();
  }

  return id;
}

ID_t KEY2ParserUtils::readRef(const KEYXMLReader &reader)
{
  optional<ID_t> id = readOnlyElementAttribute(reader, KEY2Token::IDREF, KEY2Token::NS_URI_SFA);
  if (!id)
    throw GenericException();
  return get(id);
}

pair<optional<double>, optional<double> > KEY2ParserUtils::readPoint(const KEYXMLReader &reader)
{
  pair<optional<double>, optional<double> > point;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (KEY2Token::NS_URI_SFA == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KEY2Token::x :
        point.first = lexical_cast<double>(attr.getValue());
        break;
      case KEY2Token::y :
        point.second = lexical_cast<double>(attr.getValue());
        break;
      default :
        break;
      }
    }
  }

  checkEmptyElement(reader);

  return point;
}

KEYPosition KEY2ParserUtils::readPosition(const KEYXMLReader &reader)
{
  pair<double, double> point = readAttributePair<double, double>(reader, KEY2Token::x, KEY2Token::NS_URI_SFA, KEY2Token::y, KEY2Token::NS_URI_SFA);

  return KEYPosition(point.first, point.second);
}

KEYSize KEY2ParserUtils::readSize(const KEYXMLReader &reader)
{
  const pair<double, double> size = readAttributePair<double, double>(reader, KEY2Token::h, KEY2Token::NS_URI_SFA, KEY2Token::w, KEY2Token::NS_URI_SFA);
  return KEYSize(size.second, size.first);
}

bool KEY2ParserUtils::bool_cast(const char *const value)
{
  KEY2Tokenizer tok;
  switch (tok(value))
  {
  case KEY2Token::_1 :
  case KEY2Token::true_ :
    return true;
  case KEY2Token::_0 :
  case KEY2Token::false_ :
  default :
    return false;
  }

  return false;
}

double KEY2ParserUtils::deg2rad(double value)
{
  // normalize range
  while (360 <= value)
    value -= 360;
  while (0 > value)
    value += 360;

  // convert
  return boost::math::double_constants::pi / 180 * value;
}

KEY2ParserUtils::KEY2ParserUtils()
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
