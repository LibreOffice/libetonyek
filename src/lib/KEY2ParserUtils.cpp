/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/lexical_cast.hpp>

#include "libetonyek_xml.h"
#include "IWORKToken.h"
#include "IWORKTypes.h"
#include "IWORKXMLReader.h"
#include "KEY2ParserUtils.h"
#include "KEY2Token.h"
#include "KEYTypes.h"

using boost::lexical_cast;
using boost::optional;

using std::pair;

namespace libetonyek
{

namespace
{

template <typename T1, typename T2>
pair<T1, T2>
readAttributePair(const IWORKXMLReader &reader, const int name1, const int ns1, const int name2, const int ns2, const bool empty = true)
{
  optional<T1> a1;
  optional<T2> a2;

  IWORKXMLReader::AttributeIterator attr(reader);
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

KEY2ParserUtils::~KEY2ParserUtils()
{
}

optional<ID_t> KEY2ParserUtils::readID(const IWORKXMLReader &reader)
{
  optional<ID_t> id;

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((IWORKToken::NS_URI_SFA == getNamespaceId(attr)) && (IWORKToken::ID == getNameId(attr)))
      id = attr.getValue();
  }

  return id;
}

ID_t KEY2ParserUtils::readRef(const IWORKXMLReader &reader)
{
  optional<ID_t> id = readOnlyElementAttribute(reader, IWORKToken::IDREF, IWORKToken::NS_URI_SFA);
  if (!id)
    throw GenericException();
  return get(id);
}

pair<optional<double>, optional<double> > KEY2ParserUtils::readPoint(const IWORKXMLReader &reader)
{
  pair<optional<double>, optional<double> > point;

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (IWORKToken::NS_URI_SFA == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case IWORKToken::x :
        point.first = lexical_cast<double>(attr.getValue());
        break;
      case IWORKToken::y :
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

IWORKPosition KEY2ParserUtils::readPosition(const IWORKXMLReader &reader)
{
  pair<double, double> point = readAttributePair<double, double>(reader, IWORKToken::x, IWORKToken::NS_URI_SFA, IWORKToken::y, IWORKToken::NS_URI_SFA);

  return IWORKPosition(point.first, point.second);
}

IWORKSize KEY2ParserUtils::readSize(const IWORKXMLReader &reader)
{
  const pair<double, double> size = readAttributePair<double, double>(reader, IWORKToken::h, IWORKToken::NS_URI_SFA, IWORKToken::w, IWORKToken::NS_URI_SFA);
  return IWORKSize(size.second, size.first);
}

bool KEY2ParserUtils::bool_cast(const char *value)
{
  IWORKTokenizer tok;
  switch (tok(value))
  {
  case IWORKToken::_1 :
  case IWORKToken::true_ :
    return true;
  case IWORKToken::_0 :
  case IWORKToken::false_ :
  default :
    return false;
  }

  return false;
}

double KEY2ParserUtils::double_cast(const char *value)
{
  return lexical_cast<double, const char *>(value);
}

int KEY2ParserUtils::int_cast(const char *value)
{
  return lexical_cast<int, const char *>(value);
}

double KEY2ParserUtils::deg2rad(double value)
{
  // normalize range
  while (360 <= value)
    value -= 360;
  while (0 > value)
    value += 360;

  // convert
  return etonyek_pi / 180 * value;
}

KEY2ParserUtils::KEY2ParserUtils()
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
