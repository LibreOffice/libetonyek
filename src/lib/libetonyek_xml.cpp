/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "libetonyek_xml.h"

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include "libetonyek_utils.h"
#include "IWORKToken.h"
#include "IWORKXMLReader.h"

using boost::lexical_cast;
using boost::optional;

using std::string;

namespace libetonyek
{

namespace
{

struct XMLException {};

}

void skipElement(const IWORKXMLReader &reader)
{
  IWORKXMLReader::ElementIterator elements(reader);
  while (elements.next())
    skipElement(elements);
}

bool checkElement(const IWORKXMLReader &reader, const int name, const int ns)
{
  return (getNamespaceId(reader) == ns) && (getNameId(reader) == name);
}

bool checkEmptyElement(const IWORKXMLReader &reader)
{
  bool empty = true;

  IWORKXMLReader::ElementIterator elements(reader);
  while (elements.next())
  {
    empty = false;
    skipElement(elements);
  }

  return empty;
}

bool checkNoAttributes(const IWORKXMLReader &reader)
{
  unsigned count = 0;

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    ++count;
  }

  return 0 == count;
}

string readOnlyAttribute(const IWORKXMLReader &reader, const int name, const int ns)
{
  optional<string> value;

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((getNamespaceId(attr) == ns) && (getNameId(attr) == name))
      value = attr.getValue();
  }

  if (!value)
    throw GenericException();

  return get(value);
}

string readOnlyElementAttribute(const IWORKXMLReader &reader, const int name, const int ns)
{
  const string value = readOnlyAttribute(reader, name, ns);

  checkEmptyElement(reader);

  return value;
}

bool bool_cast(const char *value)
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

double double_cast(const char *value)
{
  return lexical_cast<double, const char *>(value);
}

int int_cast(const char *value)
{
  return lexical_cast<int, const char *>(value);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
