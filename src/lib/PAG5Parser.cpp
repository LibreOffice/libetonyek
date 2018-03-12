/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAG5Parser.h"

#include <algorithm>
#include <deque>
#include <functional>
#include <memory>

#include "PAG5ObjectType.h"
#include "PAGCollector.h"

namespace libetonyek
{

using boost::optional;
using namespace std::placeholders;

PAG5Parser::PAG5Parser(const RVNGInputStreamPtr_t &fragments, const RVNGInputStreamPtr_t &package, PAGCollector &collector)
  : IWAParser(fragments, package, collector)
  , m_collector(collector)
{
}

bool PAG5Parser::dispatchShape(const unsigned id)
{
  m_collector.startLevel();
  bool ok=IWAParser::dispatchShape(id);
  m_collector.endLevel();
  return ok;
}

bool PAG5Parser::parseGroupRef(unsigned id)
{
  const ObjectMessage msg(*this, id, PAG5ObjectType::PageGroup);
  if (!msg)
  {
    ETONYEK_DEBUG_MSG(("PAG5Parser::parseGroupRef: can not find %d object\n", int(id)));
    return false;
  }
  for (const auto &pIt : get(msg).message(1))
  {
    if (!pIt.uint32(1))
    {
      ETONYEK_DEBUG_MSG(("PAG5Parser::parseGroupRef: can not find a page\n"));
      continue;
    }
    m_collector.openPageGroup(int(get(pIt.uint32(1)))+1);
    std::deque<unsigned> shapeRefs;
    const std::deque<IWAMessage> &objs = pIt.message(4).repeated();
    for (const auto &obj : objs)
    {
      auto ref=readRef(obj, 1);
      if (ref)
        shapeRefs.push_back(get(ref));
      else
      {
        ETONYEK_DEBUG_MSG(("PAG5Parser::parseGroupRef: can not find an object\n"));
      }
    }
    std::for_each(shapeRefs.begin(), shapeRefs.end(), std::bind(&PAG5Parser::dispatchShape, this, _1));
    m_collector.closePageGroup();
  }
  return true;
}

bool PAG5Parser::parseDocument()
{
  const ObjectMessage msg(*this, 1, PAG5ObjectType::Document);
  if (msg)
  {
    m_collector.startDocument();
    auto const &message=get(msg);
    IWORKPrintInfo printInfo;
    if (message.float_(30))
      printInfo.m_width=get(message.float_(30));
    if (message.float_(31))
      printInfo.m_height=get(message.float_(31));
    if (message.float_(32))
      printInfo.m_marginLeft=get(message.float_(32));
    if (message.float_(33))
      printInfo.m_marginRight=get(message.float_(33));
    if (message.float_(34))
      printInfo.m_marginTop=get(message.float_(34));
    if (message.float_(35))
      printInfo.m_marginBottom=get(message.float_(35));
    if (message.float_(36))
      printInfo.m_headerHeight=get(message.float_(36));
    if (message.float_(37))
      printInfo.m_footerHeight=get(message.float_(37));
    if (message.uint32(42))
      printInfo.m_footerHeight=get(message.uint32(42));
    m_collector.setPageDimensions(printInfo);

    const optional<unsigned> groupRef(readRef(message, 3));
    if (groupRef)
      parseGroupRef(get(groupRef));
    const optional<unsigned> textRef(readRef(message, 4));
    if (textRef)
    {
      m_currentText = m_collector.createText(m_langManager);
      bool opened=false;
      parseText(get(textRef), [this,&opened](unsigned pos, IWORKStylePtr_t style)
      {
        if (pos && opened)
        {
          m_collector.collectText(m_currentText);
          m_collector.closeSection();
        }
        m_collector.openSection(style);
        opened=true;
      });
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
