/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY6PARSER_H_INCLUDED
#define KEY6PARSER_H_INCLUDED

#include "IWAParser.h"

#include "KEYTypes_fwd.h"

namespace libetonyek
{

class KEYCollector;

class KEY6Parser : public IWAParser
{
public:
  KEY6Parser(const RVNGInputStreamPtr_t &fragments, const RVNGInputStreamPtr_t &package, KEYCollector &collector);

private:
  bool parseDocument() override;

  bool parsePresentation(unsigned id);
  bool parseSlideList(unsigned id);
  KEYSlidePtr_t parseSlide(unsigned id, bool master);
  bool parsePlaceholder(unsigned id);
  void parseNotes(unsigned id);

  bool parseStickyNote(const IWAMessage &msg) override;

  const IWORKStylePtr_t querySlideStyle(unsigned id) const;

  void parseSlideStyle(unsigned id, IWORKStylePtr_t &style);

private:
  KEYCollector &m_collector;

  mutable std::unordered_map<unsigned, KEYSlidePtr_t> m_masterSlides;
  mutable std::deque<KEYSlidePtr_t> m_slides;
  mutable StyleMap_t m_slideStyles;
};

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
