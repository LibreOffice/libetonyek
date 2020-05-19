/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>
#include <functional>

#include "NUM3Parser.h"

#include "IWAMessage.h"
#include "IWAObjectType.h"
#include "IWORKTable.h"
#include "NUM3ObjectType.h"
#include "NUMCollector.h"

namespace libetonyek
{

NUM3Parser::NUM3Parser(const RVNGInputStreamPtr_t &fragments, const RVNGInputStreamPtr_t &package, NUMCollector &collector)
  : IWAParser(fragments, package, collector)
  , m_collector(collector)
{
}

bool NUM3Parser::parseSheet(unsigned id)
{
  const ObjectMessage msg(*this, id, NUM3ObjectType::Sheet);
  if (!msg) return false;
  // 1: is the worksheet name
  // 2: is the list of table/other drawing in this page
  boost::optional<std::string> name = get(msg).string(1).optional();
  m_collector.startWorkSpace(name);
  const std::deque<unsigned> &tableListRefs = readRefs(get(msg), 2);
  for (auto cId : tableListRefs)
    dispatchShape(cId);
  m_collector.endWorkSpace(m_tableNameMap);

  return true;
}

bool NUM3Parser::parseShapePlacement(const IWAMessage &msg, IWORKGeometryPtr_t &geometry, boost::optional<unsigned> &)
{
  geometry = std::make_shared<IWORKGeometry>();
  const boost::optional<IWAMessage> &g = msg.message(1).optional();
  if (g)
  {
    const boost::optional<IWORKPosition> &pos = readPosition(get(g), 1);
    if (pos)
      geometry->m_position = get(pos);
    const boost::optional<IWORKSize> &size = readSize(get(g), 2);
    if (size)
      geometry->m_naturalSize = geometry->m_size = get(size);
    // CHECKME what means get(g).uint32(3) : 0,3 seems normal ?
    if (get(g).float_(4))
      geometry->m_angle = -deg2rad(get(get(g).float_(4)));
  }
  geometry->m_aspectRatioLocked = msg.bool_(7).optional();

  return true;
}

bool NUM3Parser::parseStickyNote(const IWAMessage &msg)
{
  assert(!m_currentText);

  m_collector.startLevel();
  // 1: a text box, 2: a comment ref

  // FIXME: actually, this creates a textbox but the background and
  //        padding are bad and we do not retrieve the author, ...
  auto const &shape=msg.message(1);
  if (shape)
    dispatchShapeWithMessage(get(shape),IWAObjectType::DrawableShape);
  m_collector.endLevel();

  return true;
}

bool NUM3Parser::parseDocument()
{
  const ObjectMessage msg(*this, 1, NUM3ObjectType::Document);
  if (!msg) return false;

  m_collector.startDocument();
  auto info=get(msg).message(8);
  if (info)
  {
    auto customRef=readRef(get(info),12);
    if (customRef) parseCustomFormat(get(customRef));
  }
  // const optional<IWAMessage> size = get(msg).message(12).optional();
  // if (size) define the page size
  const std::deque<unsigned> &sheetListRefs = readRefs(get(msg), 1);
  std::for_each(sheetListRefs.begin(), sheetListRefs.end(), std::bind(&NUM3Parser::parseSheet, this, std::placeholders::_1));

  m_collector.endDocument();
  return true;
}
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
