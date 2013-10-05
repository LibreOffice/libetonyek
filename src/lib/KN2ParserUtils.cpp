/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/lexical_cast.hpp>

#include "libkeynote_xml.h"
#include "KN2ParserUtils.h"
#include "KN2Token.h"
#include "KNTypes.h"
#include "KNXMLReader.h"

using boost::lexical_cast;
using boost::optional;

using std::pair;

namespace libkeynote
{

namespace
{

template <typename T1, typename T2>
pair<T1, T2>
readAttributePair(const KNXMLReader &reader, const int name1, const int ns1, const int name2, const int ns2, const bool empty = true)
{
  optional<T1> a1 = T1();
  a1.reset();
  optional<T2> a2 = T2();
  a2.reset();

  KNXMLReader::AttributeIterator attr(reader);
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
    else
    {
      KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
    }
  }

  if (empty)
    checkEmptyElement(reader);

  if (!a1 || !a2)
    throw GenericException();

  return pair<T1, T2>(get(a1), get(a2));
}

}

KN2ParserUtils::~KN2ParserUtils()
{
}

optional<ID_t> KN2ParserUtils::readID(const KNXMLReader &reader)
{
  optional<ID_t> id;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SFA == getNamespaceId(attr)) && (KN2Token::ID == getNameId(attr)))
    {
      id = attr.getValue();
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
    }
  }

  return id;
}

ID_t KN2ParserUtils::readRef(const KNXMLReader &reader)
{
  optional<ID_t> id = readOnlyElementAttribute(reader, KN2Token::IDREF, KN2Token::NS_URI_SFA);
  if (!id)
    throw GenericException();
  return get(id);
}

pair<optional<double>, optional<double> > KN2ParserUtils::readPoint(const KNXMLReader &reader)
{
  pair<optional<double>, optional<double> > point;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (KN2Token::NS_URI_SFA == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KN2Token::x :
        point.first = lexical_cast<double>(attr.getValue());
        break;
      case KN2Token::y :
        point.second = lexical_cast<double>(attr.getValue());
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
    }
  }

  checkEmptyElement(reader);

  return point;
}

KNPosition KN2ParserUtils::readPosition(const KNXMLReader &reader)
{
  pair<double, double> point = readAttributePair<double, double>(reader, KN2Token::x, KN2Token::NS_URI_SFA, KN2Token::y, KN2Token::NS_URI_SFA);

  return KNPosition(point.first, point.second);
}

KNSize KN2ParserUtils::readSize(const KNXMLReader &reader)
{
  const pair<double, double> size = readAttributePair<double, double>(reader, KN2Token::h, KN2Token::NS_URI_SFA, KN2Token::w, KN2Token::NS_URI_SFA);
  return KNSize(size.first, size.second);
}

bool KN2ParserUtils::bool_cast(const char *const value)
{
  KN2Tokenizer tok;
  switch (tok(value))
  {
  case KN2Token::_1 :
  case KN2Token::true_ :
    return true;
  case KN2Token::_0 :
  case KN2Token::false_ :
  default :
    return false;
  }

  return false;
}

KN2ParserUtils::KN2ParserUtils()
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
