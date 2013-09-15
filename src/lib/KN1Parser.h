/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KN1PARSER_H_INCLUDED
#define KN1PARSER_H_INCLUDED

#include "KNParser.h"

namespace libkeynote
{

class KN1Parser : public KNParser
{
public:
  KN1Parser(const WPXInputStreamPtr_t &input, KNCollector *collector);
  virtual ~KN1Parser();

private:
  virtual void processXmlNode(const KNXMLReader &reader);
  virtual KNXMLReader::TokenizerFunction_t getTokenizer() const;
};

}

#endif //  KN1PARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
