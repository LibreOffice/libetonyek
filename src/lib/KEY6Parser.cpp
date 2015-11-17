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
#include <boost/make_shared.hpp>

#include "IWAMessage.h"
#include "IWAObjectType.h"
#include "IWORKProperties.h"
#include "IWORKText.h"
#include "KEY6ObjectType.h"
#include "KEYCollector.h"

namespace libetonyek
{

using boost::bind;
using boost::make_shared;
using boost::none;
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
  const optional<IWAMessage> size = get(msg).message(4).optional();
  if (size && get(size).float_(1) && get(size).float_(2))
    m_collector.collectPresentationSize(IWORKSize(get(size).float_(1).get(), get(size).float_(2).get()));
  optional<unsigned> slideListRef;
  if (get(msg).message(3))
  {
    slideListRef = readRef(get(msg).message(3).get(), 1);
    if (!slideListRef)
      slideListRef = readRef(get(msg).message(3).get(), 2);
  }
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
  if (!master)
  {
    const optional<unsigned> &titlePlaceholderRef = readRef(get(msg), 5);
    if (titlePlaceholderRef)
      parsePlaceholder(get(titlePlaceholderRef));
    const optional<unsigned> &bodyPlaceholderRef = readRef(get(msg), 6);
    if (bodyPlaceholderRef)
      parsePlaceholder(get(bodyPlaceholderRef));
  }

  const deque<unsigned> &shapeRefs = readRefs(get(msg), 7);
  for_each(shapeRefs.begin(), shapeRefs.end(), bind(&KEY6Parser::dispatchShape, this, _1));

  const optional<unsigned> &notesRef = readRef(get(msg), 27);
  if (notesRef)
    parseNotes(get(notesRef));

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

bool KEY6Parser::parsePlaceholder(const unsigned id)
{
  const ObjectMessage msg(*this, id, KEY6ObjectType::Placeholder);
  if (!msg)
    return false;

  if (get(msg).uint32(2))
  {
    const unsigned type = get(get(msg).uint32(2));
    switch (type)
    {
    case 2 : // title
    case 3 : // body
    {
      const IWAMessageField &drawableShape = get(msg).message(1);
      if (drawableShape)
      {
        const IWAMessageField &shape = get(drawableShape).message(1);
        const optional<unsigned> &textRef = readRef(get(drawableShape), 2);
        if (shape && textRef)
        {
          m_collector.startLevel();
          IWORKGeometryPtr_t geometry;
          const IWAMessageField &placement = get(shape).message(1);
          if (placement)
            parseShapePlacement(get(placement), geometry);
          assert(!m_currentText);
          m_currentText = m_collector.createText();
          parseText(get(textRef));
          if (!m_currentText->empty())
            m_collector.collectText(m_currentText);
          m_currentText.reset();
          IWORKPropertyMap props;
          if (bool(geometry))
            props.put<property::Geometry>(geometry);
          const IWORKStylePtr_t style = make_shared<IWORKStyle>(props, none, none);
          const KEYPlaceholderPtr_t &placeholder = m_collector.collectTextPlaceholder(style, type == 2);
          m_collector.insertTextPlaceholder(placeholder);
          m_collector.endLevel();
        }
      }
      break;
    }
    default :
      ETONYEK_DEBUG_MSG(("KEY6Parser::parsePlaceholder: unhandled placeholder type %u\n", type));
      break;
    }
  }

  return true;
}

void KEY6Parser::parseNotes(const unsigned id)
{
  const ObjectMessage msg(*this, id, KEY6ObjectType::Notes);
  if (!msg)
    return;

  const optional<unsigned> &textRef = readRef(get(msg), 1);
  if (textRef)
  {
    assert(!m_currentText);
    m_currentText = m_collector.createText();
    parseText(get(textRef));
    m_collector.collectText(m_currentText);
    m_currentText.reset();
    m_collector.collectNote();
  }
}

bool KEY6Parser::dispatchShape(const unsigned id)
{
  {
    const ObjectMessage msg(*this, id);
    if (!msg)
      return false;

    if (msg.getType() == IWAObjectType::StickyNote)
      return parseStickyNote(get(msg));
  }

  return IWAParser::dispatchShape(id);
}

bool KEY6Parser::parseStickyNote(const IWAMessage &msg)
{
  m_collector.startLevel();
  assert(!m_currentText);

  if (msg.message(1))
  {
    const optional<unsigned> &textRef = readRef(get(msg.message(1)), 2);
    if (textRef)
    {
      m_currentText = m_collector.createText();
      parseText(get(textRef));
    }
  }
  else
  {
    const optional<unsigned> &commentRef = readRef(msg, 2);
    if (commentRef)
    {
      m_currentText = m_collector.createText();
      parseComment(get(commentRef));
    }
  }

  if (bool(m_currentText))
  {
    m_collector.collectText(m_currentText);
    m_currentText.reset();
  }
  m_collector.collectStickyNote();
  m_collector.endLevel();

  return true;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
