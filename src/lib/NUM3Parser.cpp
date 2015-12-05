/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "NUM3Parser.h"

#include "NUM3ObjectType.h"
#include "NUMCollector.h"

namespace libetonyek
{

NUM3Parser::NUM3Parser(const RVNGInputStreamPtr_t &fragments, const RVNGInputStreamPtr_t &package, NUMCollector &collector)
  : IWAParser(fragments, package, collector)
  , m_collector(collector)
{
}

bool NUM3Parser::parseDocument()
{
  const ObjectMessage msg(*this, 1, NUM3ObjectType::Document);
  if (msg)
  {
    m_collector.startDocument();
    m_collector.endDocument();
    return true;
  }
  return false;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
