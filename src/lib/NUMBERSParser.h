/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef NUMBERSPARSER_H_INCLUDED
#define NUMBERSPARSER_H_INCLUDED

#include "libetonyek_utils.h"

namespace libetonyek
{

class NUMBERSCollector;
class KEYXMLReader;

class NUMBERSParser
{
  // disable copying
  NUMBERSParser(const NUMBERSParser &);
  NUMBERSParser &operator=(const NUMBERSParser &);

public:
  NUMBERSParser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, NUMBERSCollector *collector);

  bool parse();

private:
  void parseDocument(const KEYXMLReader &reader);

private:
  RVNGInputStreamPtr_t m_input;
  RVNGInputStreamPtr_t m_package;
  NUMBERSCollector *m_collector;
};

} // namespace libetonyek

#endif //  NUMBERSPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
