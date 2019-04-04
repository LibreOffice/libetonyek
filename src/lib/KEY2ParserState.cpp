/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY2ParserState.h"

#include "IWORKText.h"

#include "KEY2Dictionary.h"
#include "KEY2Parser.h"
#include "KEYCollector.h"

namespace libetonyek
{

KEY2ParserState::KEY2ParserState(KEY2Parser &parser, KEYCollector &collector, KEY2Dictionary &dict)
  : IWORKXMLParserState(parser, collector, dict)
  , m_version(0)
  , m_dict(dict)
  , m_collector(collector)
  , m_isHeadlineOpened(false)
  , m_isBulletsOpened(false)
  , m_bodyText()
  , m_titleText()
{
}

void KEY2ParserState::setVersion(unsigned version)
{
  m_version=version;
}

unsigned KEY2ParserState::getVersion() const
{
  return m_version;
}

KEY2Dictionary &KEY2ParserState::getDictionary()
{
  return m_dict;
}

KEYCollector &KEY2ParserState::getCollector()
{
  return m_collector;
}

IWORKTextPtr_t KEY2ParserState::getBodyText()
{
  return m_bodyText;
}

IWORKTextPtr_t KEY2ParserState::getTitleText()
{
  return m_titleText;
}

void KEY2ParserState::openBullets()
{
  if (m_isBulletsOpened)
  {
    ETONYEK_DEBUG_MSG(("KEY2ParserState::openBullets: oops the bullets are already opened\n"));
    return;
  }
  m_isBulletsOpened=true;
  m_bodyText.reset();
  m_titleText.reset();
}

void KEY2ParserState::openHeadline(int depth)
{
  if (m_isHeadlineOpened)
  {
    ETONYEK_DEBUG_MSG(("KEY2ParserState::openHeadline: oops the headline are already opened\n"));
    return;
  }
  m_isHeadlineOpened=true;
  assert(!m_currentText);
  if (depth==0)
  {
    if (!m_titleText)
      m_titleText=m_collector.createText(m_langManager, true);
    m_currentText=m_titleText;
  }
  else
  {
    if (!m_bodyText)
      m_bodyText=m_collector.createText(m_langManager, true);
    m_currentText=m_bodyText;
  }
}

void KEY2ParserState::closeHeadline()
{
  if (!m_isHeadlineOpened)
  {
    ETONYEK_DEBUG_MSG(("KEY2ParserState::openHeadline: oops the headline are already close\n"));
    return;
  }
  m_isHeadlineOpened=false;
  if (m_currentText)
  {
    m_currentText->flushParagraph();
    m_currentText.reset();
  }
}

void KEY2ParserState::closeBullets()
{
  if (!m_isBulletsOpened)
  {
    ETONYEK_DEBUG_MSG(("KEY2ParserState::openBullets: oops the bullets are already close\n"));
    return;
  }
  m_isBulletsOpened=false;
  m_bodyText.reset();
  m_titleText.reset();
}

}
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
