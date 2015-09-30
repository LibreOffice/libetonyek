/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */ /*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY6Parser.h"

#include "IWAMessage.h"
#include "KEY6ObjectType.h"
#include "KEYCollector.h"

namespace libetonyek
{

using boost::optional;

KEY6Parser::KEY6Parser(const RVNGInputStreamPtr_t &fragments, const RVNGInputStreamPtr_t &package, KEYCollector &collector)
  : IWAParser(fragments, package, collector)
  , m_collector(collector)
{
}

bool KEY6Parser::parseDocument()
{
  const optional<IWAMessage> msg(queryObject(1, KEY6ObjectType::Document));
  if (msg)
  {
    const optional<unsigned> presRef(readRef(get(msg), 2));
    if (presRef)
      return parsePresentation(get(presRef));
  }
  return false;
}

bool KEY6Parser::parsePresentation(const unsigned id)
{
  const optional<IWAMessage> msg(queryObject(id, KEY6ObjectType::Presentation));
  if (!msg)
    return false;
  m_collector.startDocument();
  m_collector.endDocument();
  return true;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
