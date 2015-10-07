/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */ /*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY6Parser.h"

#include <algorithm>

#include <boost/bind.hpp>

#include "IWAMessage.h"
#include "KEY6ObjectType.h"
#include "KEYCollector.h"

namespace libetonyek
{

using boost::bind;
using boost::optional;

using std::deque;
using std::for_each;

KEY6Parser::KEY6Parser(const RVNGInputStreamPtr_t &fragments, const RVNGInputStreamPtr_t &package, KEYCollector &collector)
  : IWAParser(fragments, package, collector)
  , m_collector(collector)
{
}

bool KEY6Parser::parseDocument()
{
  const ObjectMessage msg(*this, 1, KEY6ObjectType::Document);
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
  const ObjectMessage msg(*this, id, KEY6ObjectType::Presentation);
  if (!msg)
    return false;

  m_collector.startDocument();
  const optional<IWAMessage> size = get(msg).message(4);
  if (size && get(size).float_(1) && get(size).float_(2))
    m_collector.collectPresentationSize(IWORKSize(get(size).float_(1).get(), get(size).float_(2).get()));
  optional<unsigned> slideListRef;
  if (get(msg).message(3))
    slideListRef = readRef(get(msg).message(3).get(), 1);
  bool success = true;
  if (slideListRef)
  {
    m_collector.startSlides();
    success = parseSlideList(get(slideListRef));
    m_collector.endSlides();
  }
  m_collector.endDocument();
  return success;
}

bool KEY6Parser::parseSlideList(const unsigned id)
{
  const ObjectMessage msg(*this, id, KEY6ObjectType::SlideList);
  if (!msg)
    return false;

  const deque<unsigned> &slideListRefs = readRefs(get(msg), 1);
  for_each(slideListRefs.begin(), slideListRefs.end(), bind(&KEY6Parser::parseSlideList, this, _1));
  const deque<unsigned> &slideRefs = readRefs(get(msg), 2);
  for_each(slideRefs.begin(), slideRefs.end(), bind(&KEY6Parser::parseSlide, this, _1, false));
  return true;
}

bool KEY6Parser::parseSlide(const unsigned id, const bool master)
{
  const ObjectMessage msg(*this, id, KEY6ObjectType::Slide);
  if (!msg)
    return false;

  if (!master)
    m_collector.startPage();

  const optional<unsigned> &masterRef = readRef(get(msg), 17);
  if (masterRef)
    parseSlide(get(masterRef), true);

  m_collector.startLayer();

  // const optional<unsigned> &styleRef = readRef(get(msg), 1);
  // const optional<unsigned> &titlePlaceholderRef = readRef(get(msg), 5);
  // const optional<unsigned> &bodyPlaceholderRef = readRef(get(msg), 6);

  const deque<unsigned> &shapeRefs = readRefs(get(msg), 7);
  for_each(shapeRefs.begin(), shapeRefs.end(), bind(&KEY6Parser::parseDrawableShape, this, _1));

  // const optional<unsigned> &noteRef = readRef(get(msg), 27);

  const KEYLayerPtr_t layer = m_collector.collectLayer();
  m_collector.endLayer();
  m_collector.insertLayer(layer);

  if (!master)
  {
    m_collector.collectPage();
    m_collector.endPage();
  }

  return true;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
