/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include "libetonyek_xml.h"
#include "KEYXMLReader.h"

using boost::optional;

using std::string;

namespace libetonyek
{

namespace
{

struct XMLException {};

}

void skipElement(const KEYXMLReader &reader)
{
  KEYXMLReader::ElementIterator elements(reader);
  while (elements.next())
    skipElement(elements);
}

bool checkElement(const KEYXMLReader &reader, const int name, const int ns)
{
  return (getNamespaceId(reader) == ns) && (getNameId(reader) == name);
}

bool checkEmptyElement(const KEYXMLReader &reader)
{
  bool empty = true;

  KEYXMLReader::ElementIterator elements(reader);
  while (elements.next())
  {
    empty = false;
    skipElement(elements);
  }

  return empty;
}

bool checkNoAttributes(const KEYXMLReader &reader)
{
  unsigned count = 0;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    ++count;
  }

  return 0 == count;
}

string readOnlyAttribute(const KEYXMLReader &reader, const int name, const int ns)
{
  optional<string> value;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((getNamespaceId(attr) == ns) && (getNameId(attr) == name))
      value = attr.getValue();
  }

  if (!value)
    throw GenericException();

  return get(value);
}

string readOnlyElementAttribute(const KEYXMLReader &reader, const int name, const int ns)
{
  const string value = readOnlyAttribute(reader, name, ns);

  checkEmptyElement(reader);

  return value;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
