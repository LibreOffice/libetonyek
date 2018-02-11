/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY1PARSERSTATE_H_INCLUDED
#define KEY1PARSERSTATE_H_INCLUDED

#include "IWORKXMLParserState.h"

namespace libetonyek
{

class KEY1Parser;
class KEYCollector;
struct KEY1Dictionary;

enum KEY1BulletType
{
  KEY1_BULLETTYPE_CHARACTER,
  KEY1_BULLETTYPE_IMAGE,
  KEY1_BULLETTYPE_INHERITED,
  KEY1_BULLETTYPE_SEQUENCE,
  KEY1_BULLETTYPE_NONE
};

struct KEY1Bullet
{
  KEY1Bullet()
    : m_type(KEY1_BULLETTYPE_NONE)
    , m_level()
    , m_spacing()
  {
  }
  KEY1BulletType m_type;
  boost::optional<int> m_level;
  boost::optional<double> m_spacing;
};

class KEY1ParserState : public IWORKXMLParserState
{
  // not copyable
  KEY1ParserState(const KEY1ParserState &);
  KEY1ParserState &operator=(const KEY1ParserState &);

public:
  KEY1ParserState(KEY1Parser &parser, KEYCollector &collector, KEY1Dictionary &dict);

  KEY1Dictionary &getDictionary();
  KEYCollector &getCollector();

  void pushIsPrototype(bool isPrototype);
  void popIsPrototype();
  void pushIsMasterSlide(bool isMasterSlide);
  void popIsMasterSlide();

  void openBullets();
  void openBullet(KEY1Bullet const &bullet);
  void closeBullet();
  void closeBullets();
  bool m_isPrototype;
  bool m_isMasterSlide;

private:
  void storeCurrentPlaceholder();

  KEY1Dictionary &m_dict;
  KEYCollector &m_collector;

  std::deque<bool> m_prototypes;
  std::deque<bool> m_masterSlides;

  bool m_isBulletOpened;
  bool m_isBulletsOpened;

  bool m_isBodyContentOpened;
  bool m_isTitleContentOpened;
  bool m_bodyContentFound;
  bool m_titleContentFound;
};

}

#endif // KEY1PARSERSTATE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
