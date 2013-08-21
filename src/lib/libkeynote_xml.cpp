/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include "libkeynote_xml.h"
#include "KNXMLReader.h"

using boost::optional;

using std::string;

namespace libkeynote
{

namespace
{

struct XMLException {};

}

double asDouble(const char *const value)
{
  return boost::lexical_cast<double>(value);
}

void skipElement(const KNXMLReader &reader)
{
  KNXMLReader::ElementIterator elements(reader);
  while (elements.next())
    skipElement(elements);
}

bool checkElement(const KNXMLReader &reader, const int name, const int ns)
{
  return (getNamespaceId(reader) == ns) && (getNameId(reader) == name);
}

bool checkEmptyElement(const KNXMLReader &reader)
{
  bool empty = true;

  KNXMLReader::ElementIterator elements(reader);
  while (elements.next())
  {
    empty = false;
    skipElement(elements);
  }

  return empty;
}

bool checkNoAttributes(const KNXMLReader &reader)
{
  unsigned count = 0;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    ++count;
    KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
  }

  return 0 == count;
}

string readOnlyAttribute(const KNXMLReader &reader, const int name, const int ns)
{
  optional<string> value;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((getNamespaceId(attr) == ns) && (getNameId(attr) == name))
      value = attr.getValue();
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
    }
  }

  if (!value)
    throw GenericException();

  return get(value);
}

string readOnlyElementAttribute(const KNXMLReader &reader, const int name, const int ns)
{
  const char *const elementName = reader.getName();
  const char *const elementNs = reader.getNamespace();

  const string value = readOnlyAttribute(reader, name, ns);

  KNXMLReader::ElementIterator element(reader);
  KN_DEBUG_XML_NOT_EMPTY(elementName, elementNs);
  skipElement(element);

  return value;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
