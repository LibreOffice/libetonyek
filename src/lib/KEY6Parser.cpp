/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */ /*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY6Parser.h"

#include <algorithm>
#include <functional>
#include <memory>

#include "IWAMessage.h"
#include "IWAObjectType.h"
#include "IWORKProperties.h"
#include "IWORKText.h"
#include "KEY6ObjectType.h"
#include "KEYCollector.h"

namespace libetonyek
{

using boost::none;
using boost::optional;

using namespace std::placeholders;

using std::bind;
using std::deque;
using std::for_each;
using std::make_shared;
using std::string;

KEY6Parser::KEY6Parser(const RVNGInputStreamPtr_t &fragments, const RVNGInputStreamPtr_t &package, KEYCollector &collector)
  : IWAParser(fragments, package, collector)
  , m_collector(collector)
  , m_masterSlides()
  , m_slides()
  , m_slideStyles()
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
  m_collector.startSlides();
  bool success = true;
  if (get(msg).message(3))
  {
    optional<unsigned> slideListRef;
    slideListRef = readRef(get(msg).message(3).get(), 1);
    if (slideListRef)
    {
      success = parseSlideList(get(slideListRef));
    }
    else
    {
      const deque<unsigned> &slideListRefs = readRefs(get(get(msg).message(3)), 2);
      for_each(slideListRefs.begin(), slideListRefs.end(), bind(&KEY6Parser::parseSlideList, this, _1));
    }
  }
  m_collector.endSlides();

  m_collector.sendSlides(m_slides);
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

KEYSlidePtr_t KEY6Parser::parseSlide(const unsigned id, const bool master)
{
  const ObjectMessage msg(*this, id, KEY6ObjectType::Slide);
  if (!msg)
    return KEYSlidePtr_t();

  const optional<unsigned> &masterRef = readRef(get(msg), 17);
  KEYSlidePtr_t masterSlide;
  if (!master && masterRef)
  {
    if (m_masterSlides.find(get(masterRef))!=m_masterSlides.end())
      masterSlide=m_masterSlides.find(get(masterRef))->second;
    else
      masterSlide=parseSlide(get(masterRef), true);
  }

  m_collector.startPage();
  m_collector.startLayer();

  IWORKStylePtr_t style;
  const optional<unsigned> &styleRef = readRef(get(msg), 1);
  if (styleRef)
    style = querySlideStyle(get(styleRef));
  m_collector.setSlideStyle(style);
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

  KEYSlidePtr_t slide=m_collector.collectSlide();
  m_collector.endPage();

  if (slide)
  {
    slide->m_masterSlide=masterSlide;
    if (!master)
      m_slides.push_back(slide);
    else
      m_masterSlides[id]=slide;
  }
  return slide;
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
          m_currentText = m_collector.createText(m_langManager);
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
    m_currentText = m_collector.createText(m_langManager);
    parseText(get(textRef));
    m_collector.collectText(m_currentText);
    m_currentText.reset();
    m_collector.collectNote();
  }
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
      m_currentText = m_collector.createText(m_langManager);
      parseText(get(textRef));
    }
  }
  else
  {
    const optional<unsigned> &commentRef = readRef(msg, 2);
    if (commentRef)
    {
      m_currentText = m_collector.createText(m_langManager);
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

const IWORKStylePtr_t KEY6Parser::querySlideStyle(const unsigned id) const
{
  return queryStyle(id, m_slideStyles, bind(&KEY6Parser::parseSlideStyle, const_cast<KEY6Parser *>(this), _1, _2));
}

void KEY6Parser::parseSlideStyle(const unsigned id, IWORKStylePtr_t &style)
{
  const ObjectMessage msg(*this, id, KEY6ObjectType::SlideStyle);
  if (!msg)
    return;

  optional<string> name;
  IWORKStylePtr_t parent;
  IWORKPropertyMap props;

  using namespace property;

  const IWAMessageField &styleInfo = get(msg).message(1);
  if (styleInfo)
  {
    name = styleInfo.string(2).optional();
    const optional<unsigned> &parentRef = readRef(get(styleInfo), 3);
    if (parentRef)
      parent = querySlideStyle(get(parentRef));
  }

  if (get(msg).message(11))
  {
    const IWAMessage &properties = get(get(msg).message(11));

    if (properties.message(1))
    {
      IWORKFill fill;
      if (readFill(get(properties.message(1)), fill))
        props.put<Fill>(fill);
      else
        props.clear<Fill>();
    }
  }

  style = std::make_shared<IWORKStyle>(props, name, parent);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
