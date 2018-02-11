/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY2PARSERSTATE_H_INCLUDED
#define KEY2PARSERSTATE_H_INCLUDED

#include "IWORKXMLParserState.h"

#include <IWORKText_fwd.h>

namespace libetonyek
{

class KEY2Parser;
class KEYCollector;
struct KEY2Dictionary;

class KEY2ParserState : public IWORKXMLParserState
{
  // not copyable
  KEY2ParserState(const KEY2ParserState &);
  KEY2ParserState &operator=(const KEY2ParserState &);

public:
  KEY2ParserState(KEY2Parser &parser, KEYCollector &collector, KEY2Dictionary &dict);

  void setVersion(unsigned version);
  unsigned getVersion() const;

  KEY2Dictionary &getDictionary();
  KEYCollector &getCollector();

  // Keynote v2: the title and body content is stored in bullets element
  void openBullets();
  void openHeadline(int depth);
  void closeHeadline();
  void closeBullets();

  IWORKTextPtr_t getBodyText();
  IWORKTextPtr_t getTitleText();

private:
  unsigned m_version;
  KEY2Dictionary &m_dict;
  KEYCollector &m_collector;

  bool m_isHeadlineOpened;
  bool m_isBulletsOpened;

  IWORKTextPtr_t m_bodyText;
  IWORKTextPtr_t m_titleText;
};

}

#endif // KEY2PARSERSTATE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
