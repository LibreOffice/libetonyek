/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYPARSER_H_INCLUDED
#define KEYPARSER_H_INCLUDED

#include "libetonyek_utils.h"
#include "IWORKXMLReader.h"

namespace libetonyek
{

class KEYCollector;
class KEYDefaults;

class KEYParser
{
  // -Weffc++
  KEYParser(const KEYParser &);
  KEYParser &operator=(const KEYParser &);

public:
  KEYParser(const RVNGInputStreamPtr_t &input, KEYCollector *collector, const KEYDefaults &defaults);
  virtual ~KEYParser() = 0;
  bool parse();

  KEYCollector *getCollector() const;
  const KEYDefaults &getDefaults() const;

private:
  virtual void processXmlNode(const IWORKXMLReader &reader) = 0;
  virtual IWORKXMLReader::TokenizerFunction_t getTokenizer() const = 0;

  bool processXmlDocument(const IWORKXMLReader &reader);

private:
  RVNGInputStreamPtr_t m_input;
  KEYCollector *m_collector;
  const KEYDefaults &m_defaults;
};

} // namespace libetonyek

#endif //  KEYPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
