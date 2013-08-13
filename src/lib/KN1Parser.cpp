/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KN1Parser.h"

namespace libkeynote
{

KN1Parser::KN1Parser(WPXInputStream *const input, KNCollector *const collector)
  : KNParser(input, collector)
{
}

KN1Parser::~KN1Parser()
{
}

void KN1Parser::processXmlNode(xmlTextReaderPtr reader)
{
  // TODO: implement me
  (void) reader;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
