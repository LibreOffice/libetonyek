/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY2STYLEPARSER_H_INCLUDED
#define KEY2STYLEPARSER_H_INCLUDED

#include "KEY2ParserUtils.h"
#include "KEYPropertyMap.h"
#include "KEYTypes_fwd.h"

namespace libetonyek
{

class KEYCollector;
class KEYDefaults;
class KEYXMLReader;

class KEY2StyleParser : private KEY2ParserUtils
{
  // disable copying
  KEY2StyleParser(const KEY2StyleParser &other);
  KEY2StyleParser &operator=(const KEY2StyleParser &other);

public:
  KEY2StyleParser(int nameId, int nsId, KEYCollector *collector, const KEYDefaults &defaults, bool nested = false);

  void parse(const KEYXMLReader &reader);

private:
  void parseProperty(const KEYXMLReader &reader, const char *key = 0);
  bool parsePropertyImpl(const KEYXMLReader &reader, const char *key);
  void parsePropertyMap(const KEYXMLReader &reader);

  KEYGeometryPtr_t readGeometry(const KEYXMLReader &reader);

private:
  const int m_nameId;
  const int m_nsId;
  const bool m_nested;
  KEYCollector *const m_collector;
  const KEYDefaults &m_defaults;
  KEYPropertyMap m_props;
};

}

#endif // KEY2STYLEPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
