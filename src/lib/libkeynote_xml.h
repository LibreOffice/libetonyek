/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBKEYNOTE_XML_H_INCLUDED
#define LIBKEYNOTE_XML_H_INCLUDED

#include <string>

#include "libkeynote_utils.h"

#define KEY_DEBUG_XML_NOT_EMPTY(name, ns) KEY_DEBUG_MSG(("element %s%s%s%s is not empty, skipping content...\n", ns ? "{" : "", ns, ns ? "}" : "", name))
#define KEY_DEBUG_XML_UNKNOWN(type, name, ns) KEY_DEBUG_MSG(("unknown %s %s%s%s%s\n", type, ns ? "{" : "", ns, ns ? "}" : "", name))
#define KEY_DEBUG_XML_TODO(type, name, ns) KEY_DEBUG_MSG(("TODO: unprocessed %s %s%s%s%s\n", type, ns ? "{" : "", ns, ns ? "}" : "", name))

#define KEY_DEBUG_XML_TODO_ELEMENT(element) KEY_DEBUG_XML_TODO("element", (element).getName(), (element).getNamespace())
#define KEY_DEBUG_XML_TODO_ATTRIBUTE(attr) KEY_DEBUG_XML_TODO("attribute", (attr).getName(), (attr).getNamespace())
#define KEY_DEBUG_XML_UNKNOWN_ELEMENT(element) KEY_DEBUG_XML_UNKNOWN("element", (element).getName(), (element).getNamespace())
#define KEY_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr) KEY_DEBUG_XML_UNKNOWN("attribute", (attr).getName(), (attr).getNamespace())

namespace libkeynote
{

class KEYXMLReader;

void skipElement(const KEYXMLReader &reader);

bool checkElement(const KEYXMLReader &reader, int name, int ns);
bool checkEmptyElement(const KEYXMLReader &reader);
bool checkNoAttributes(const KEYXMLReader &reader);

std::string readOnlyAttribute(const KEYXMLReader &reader, int name, int ns);
std::string readOnlyElementAttribute(const KEYXMLReader &reader, int name, int ns);

}

#endif // LIBKEYNOTE_XML_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
