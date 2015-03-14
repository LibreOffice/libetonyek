/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBETONYEK_XML_H_INCLUDED
#define LIBETONYEK_XML_H_INCLUDED

#include <string>

#include "libetonyek_utils.h"

#define ETONYEK_DEBUG_XML_TODO(type, name, ns) ETONYEK_DEBUG_MSG(("TODO: unprocessed %s %s%s%s%s\n", type, ns ? "{" : "", ns, ns ? "}" : "", name))

#define ETONYEK_DEBUG_XML_TODO_ELEMENT(element) ETONYEK_DEBUG_XML_TODO("element", (element).getName(), (element).getNamespace())
#define ETONYEK_DEBUG_XML_TODO_ATTRIBUTE(attr) ETONYEK_DEBUG_XML_TODO("attribute", (attr).getName(), (attr).getNamespace())

namespace libetonyek
{

namespace
{

extern "C" int readFromStream(void *context, char *buffer, int len);

extern "C" int closeStream(void * /* context */);
}

namespace
{

struct XMLException {};

}

class IWORKXMLReader;

void skipElement(const IWORKXMLReader &reader);

bool checkElement(const IWORKXMLReader &reader, int name, int ns);
bool checkEmptyElement(const IWORKXMLReader &reader);
bool checkNoAttributes(const IWORKXMLReader &reader);

std::string readOnlyAttribute(const IWORKXMLReader &reader, int name, int ns);
std::string readOnlyElementAttribute(const IWORKXMLReader &reader, int name, int ns);

/** Convert string value to bool.
  *
  * @arg value the string
  * @returns the boolean value of the string
  */
bool bool_cast(const char *value);
double double_cast(const char *value);
int int_cast(const char *value);

}

#endif // LIBETONYEK_XML_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
