/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAG5Parser.h"

#include "PAG5ObjectType.h"
#include "PAGCollector.h"

namespace libetonyek
{

using boost::optional;

PAG5Parser::PAG5Parser(const RVNGInputStreamPtr_t &fragments, const RVNGInputStreamPtr_t &package, PAGCollector &collector)
  : IWAParser(fragments, package, collector)
  , m_collector(collector)
{
}

bool PAG5Parser::parseDocument()
{
  const ObjectMessage msg(*this, 1, PAG5ObjectType::Document);
  if (msg)
  {
    m_collector.startDocument();
    const optional<unsigned> textRef(readRef(get(msg), 4));
    if (textRef)
    {
      m_currentText = m_collector.createText();
      parseText(get(textRef));
      m_collector.collectText(m_currentText);
      m_currentText.reset();
      m_collector.collectTextBody();
    }
    m_collector.endDocument();
    return true;
  }
  return false;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
