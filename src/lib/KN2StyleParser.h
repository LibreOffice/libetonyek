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

#include "KNStyle.h"

namespace libkeynote
{

class KNCollector;
class KNXMLReader;

class KN2StyleParser
{
public:
  KN2StyleParser(int nameId, int nsId, KNCollector *collector);

  void parse(const KNXMLReader &reader);

private:
  void parsePropertyMap(const KNXMLReader &reader);

private:
  const int m_nameId;
  const int m_nsId;
  KNCollector *const m_collector;
  KNStylePtr_t m_style;
};

}

#endif // KN2STYLEPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
