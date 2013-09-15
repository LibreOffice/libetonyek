/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNPARSER_H_INCLUDED
#define KNPARSER_H_INCLUDED

#include "libkeynote_utils.h"
#include "KNXMLReader.h"

namespace libkeynote
{

class KNCollector;

class KNParser
{
  // -Weffc++
  KNParser(const KNParser &);
  KNParser &operator=(const KNParser &);

public:
  KNParser(const WPXInputStreamPtr_t &input, KNCollector *collector);
  virtual ~KNParser() = 0;
  bool parse();

  KNCollector *getCollector() const;

private:
  virtual void processXmlNode(const KNXMLReader &reader) = 0;
  virtual KNXMLReader::TokenizerFunction_t getTokenizer() const = 0;

  bool processXmlDocument(const KNXMLReader &reader);

private:
  WPXInputStreamPtr_t m_input;
  KNCollector *m_collector;
};

} // namespace libkeynote

#endif //  KNPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
