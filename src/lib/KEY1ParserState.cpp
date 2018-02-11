/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY1ParserState.h"

#include "IWORKText.h"

#include "KEY1Parser.h"
#include "KEY1Dictionary.h"
#include "KEY1Parser.h"
#include "KEYCollector.h"

namespace libetonyek
{

KEY1ParserState::KEY1ParserState(KEY1Parser &parser, KEYCollector &collector, KEY1Dictionary &dict)
  : IWORKXMLParserState(parser, collector, dict)
  , m_isPrototype(false)
  , m_isMasterSlide(false)
  , m_dict(dict)
  , m_collector(collector)
  , m_prototypes()
  , m_masterSlides()
  , m_isBulletOpened(false)
  , m_isBulletsOpened(false)
  , m_isBodyContentOpened(false)
  , m_isTitleContentOpened(false)
  , m_bodyContentFound(false)
  , m_titleContentFound(false)
{
}

KEY1Dictionary &KEY1ParserState::getDictionary()
{
  return m_dict;
}

KEYCollector &KEY1ParserState::getCollector()
{
  return m_collector;
}

void KEY1ParserState::pushIsPrototype(bool isPrototype)
{
  m_prototypes.push_back(m_isPrototype);
  m_isPrototype=isPrototype;
}

void KEY1ParserState::popIsPrototype()
{
  if (m_prototypes.empty())
  {
    ETONYEK_DEBUG_MSG(("KEY1ParserState::popIsPrototype: the stack is empty\n"));
    return;
  }
  m_isPrototype=m_prototypes.back();
  m_prototypes.pop_back();
}

void KEY1ParserState::pushIsMasterSlide(bool isMasterSlide)
{
  m_masterSlides.push_back(m_isMasterSlide);
  m_isMasterSlide=isMasterSlide;
}

void KEY1ParserState::popIsMasterSlide()
{
  if (m_masterSlides.empty())
  {
    ETONYEK_DEBUG_MSG(("KEY1ParserState::popIsMasterSlide: the stack is empty\n"));
    return;
  }
  m_isMasterSlide=m_masterSlides.back();
  m_masterSlides.pop_back();
}

void KEY1ParserState::openBullets()
{
  if (m_isBulletsOpened)
  {
    ETONYEK_DEBUG_MSG(("KEY1ParserState::openBullets: oops the bullets are already opened\n"));
    return;
  }
  m_isBulletsOpened=true;
}

void KEY1ParserState::openBullet(KEY1Bullet const &bullet)
{
  if (m_isBulletOpened)
  {
    ETONYEK_DEBUG_MSG(("KEY1ParserState::openBullet: oops the bullet are already opened\n"));
    return;
  }
  m_isBulletOpened=true;
  if (m_isPrototype || m_isMasterSlide || !m_enableCollector)
    return;

  if (!bullet.m_level)
  {
    ETONYEK_DEBUG_MSG(("KEY1ParserState::openBullet: oops can not find the bullet level\n"));
    return;
  }
  bool isTitle=get(bullet.m_level)==0;
  if (isTitle && !m_isTitleContentOpened && m_titleContentFound)
  {
    ETONYEK_DEBUG_MSG(("KEY1ParserState::openBullet: oops find unexpected bullet with level 0\n"));
    isTitle=false;
  }
  if (!isTitle && !m_isBodyContentOpened && m_bodyContentFound)
  {
    ETONYEK_DEBUG_MSG(("KEY1ParserState::openBullet: oops find unexpected bullet\n"));
    return;
  }
  if ((isTitle && m_isBodyContentOpened) || (!isTitle && m_isTitleContentOpened))
    storeCurrentPlaceholder();
  if (m_isBodyContentOpened || m_isTitleContentOpened)
    return;
  if (isTitle)
    m_isTitleContentOpened=true;
  else
    m_isBodyContentOpened=true;
  if (!m_enableCollector)
    return;
  m_currentText = m_collector.createText(m_langManager, false);
}

void KEY1ParserState::closeBullet()
{
  if (!m_isBulletOpened)
  {
    ETONYEK_DEBUG_MSG(("KEY1ParserState::openBullet: oops the bullet are already close\n"));
    return;
  }
  m_isBulletOpened=false;

  if (m_isPrototype || m_isMasterSlide || !m_enableCollector)
    return;
  if (m_currentText)
    m_currentText->flushParagraph();
}

void KEY1ParserState::closeBullets()
{
  if (!m_isBulletsOpened)
  {
    ETONYEK_DEBUG_MSG(("KEY1ParserState::openBullets: oops the bullets are already close\n"));
    return;
  }
  m_isBulletsOpened=false;
  if (m_isBodyContentOpened || m_isTitleContentOpened)
    storeCurrentPlaceholder();
}

void KEY1ParserState::storeCurrentPlaceholder()
{
  if (!m_isBodyContentOpened && !m_isTitleContentOpened)
    return;
  bool isTitle=m_isTitleContentOpened;
  m_isBodyContentOpened = m_isTitleContentOpened = false;
  if (!m_enableCollector)
    return;
  if (m_currentText && !m_currentText->empty())
  {
    if (isTitle && getDictionary().getTitlePlaceholder())
      getDictionary().getTitlePlaceholder()->m_text=m_currentText;
    else if (!isTitle && getDictionary().getBodyPlaceholder())
      getDictionary().getBodyPlaceholder()->m_text=m_currentText;
    else
    {
      ETONYEK_DEBUG_MSG(("KEY1ParserState::storeCurrentPlaceholder: oops can not store the text zone\n"));
    }
  }
  m_currentText.reset();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
