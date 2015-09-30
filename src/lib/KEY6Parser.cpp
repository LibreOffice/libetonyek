/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */ /*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY6Parser.h"

#include "KEYCollector.h"

namespace libetonyek
{

KEY6Parser::KEY6Parser(const RVNGInputStreamPtr_t &fragments, const RVNGInputStreamPtr_t &package, KEYCollector &collector)
  : IWAParser(fragments, package, collector)
  , m_collector(collector)
{
}

bool KEY6Parser::parseDocument()
{
  return false;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
