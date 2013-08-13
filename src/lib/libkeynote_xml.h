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

#include <libxml/xmlreader.h>

#include "libkeynote_utils.h"

namespace libkeynote
{

bool moveToNextNode(xmlTextReaderPtr reader);
void skipElement(xmlTextReaderPtr reader);

bool isEmptyElement(xmlTextReaderPtr reader);
bool isStartElement(xmlTextReaderPtr reader);
bool isEndElement(xmlTextReaderPtr reader);

const char *getName(xmlTextReaderPtr reader);
const char *getNamespace(xmlTextReaderPtr reader);
const char *getText(xmlTextReaderPtr reader);

}

#endif // LIBKEYNOTE_XML_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
