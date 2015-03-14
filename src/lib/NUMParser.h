/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef NUMPARSER_H_INCLUDED
#define NUMPARSER_H_INCLUDED

#include "libetonyek_utils.h"

namespace libetonyek
{

class NUMCollector;

class NUMParser
{
  // disable copying
  NUMParser(const NUMParser &);
  NUMParser &operator=(const NUMParser &);

public:
  NUMParser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, NUMCollector *collector);

  bool parse();

private:
  // void parseDocument(const IWORKXMLReader &reader);

private:
  RVNGInputStreamPtr_t m_input;
  RVNGInputStreamPtr_t m_package;
  NUMCollector *m_collector;
};

} // namespace libetonyek

#endif //  NUMPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
