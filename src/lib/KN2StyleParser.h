/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KN2STYLEPARSER_H_INCLUDED
#define KN2STYLEPARSER_H_INCLUDED

#include "KN2ParserUtils.h"
#include "KNPropertyMap.h"

namespace libkeynote
{

class KNCollector;
class KNXMLReader;

class KN2StyleParser : private KN2ParserUtils
{
  // disable copying
  KN2StyleParser(const KN2StyleParser &other);
  KN2StyleParser &operator=(const KN2StyleParser &other);

public:
  KN2StyleParser(int nameId, int nsId, KNCollector *collector, bool nested = false);

  void parse(const KNXMLReader &reader);

private:
  void parseProperty(const KNXMLReader &reader, const char *key = 0);
  bool parsePropertyImpl(const KNXMLReader &reader, const char *key);
  void parsePropertyMap(const KNXMLReader &reader);

private:
  const int m_nameId;
  const int m_nsId;
  const bool m_nested;
  KNCollector *const m_collector;
  KNPropertyMap m_props;
};

}

#endif // KN2STYLEPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
