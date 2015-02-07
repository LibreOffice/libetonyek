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
#include "IWORKPropertyMap.h"
#include "KEYTypes_fwd.h"

namespace libetonyek
{

class KEYCollector;
class KEYDefaults;
class IWORKXMLReader;

class KEY2StyleParser : private KEY2ParserUtils
{
  // disable copying
  KEY2StyleParser(const KEY2StyleParser &other);
  KEY2StyleParser &operator=(const KEY2StyleParser &other);

public:
  KEY2StyleParser(int id, KEYCollector *collector, const KEYDefaults &defaults, bool nested = false);

  void parse(const IWORKXMLReader &reader);

private:
  void parseProperty(const IWORKXMLReader &reader, const char *key = 0);

  /** Parse the value of property \c propertyId.
    *
    * IOW, this parses the element nested in the property element.
    *
    * @param[in] reader XML reader
    * @param[in] propertyId token ID of the property that is
    *   currently being parsed
    * @param[in] key key for insertion into the map
    */
  bool parsePropertyImpl(const IWORKXMLReader &reader, const int propertyId, const char *key);

  void parsePropertyMap(const IWORKXMLReader &reader);

  IWORKGeometryPtr_t readGeometry(const IWORKXMLReader &reader);

private:
  const int m_id;
  const bool m_nested;
  KEYCollector *const m_collector;
  const KEYDefaults &m_defaults;
  IWORKPropertyMap m_props;
};

}

#endif // KEY2STYLEPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
