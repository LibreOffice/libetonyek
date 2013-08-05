/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KNParser.h"
#include "KNCollector.h"

namespace libkeynote
{

KNParser::KNParser(WPXInputStream *const input, KNCollector *const collector)
  : m_input(input)
  , m_collector(collector)
{
}

KNParser::~KNParser()
{
}

bool KNParser::parse()
{
  // TODO: implement me
  return false;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
