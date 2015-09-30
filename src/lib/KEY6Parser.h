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

namespace libetonyek
{

class KEYCollector;

class KEY6Parser : public IWAParser
{
public:
  KEY6Parser(const RVNGInputStreamPtr_t &fragments, const RVNGInputStreamPtr_t &package, KEYCollector &collector);

private:
  virtual bool parseDocument();

  bool parsePresentation(unsigned id);

private:
  KEYCollector &m_collector;
};

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
