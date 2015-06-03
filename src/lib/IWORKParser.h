/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKPARSER_H_INCLUDED
#define IWORKPARSER_H_INCLUDED

#include "libetonyek_utils.h"
#include "IWORKXMLContext.h"

namespace libetonyek
{

struct IWORKDictionary;
class IWORKTokenizer;
class IWORKXMLParserState;

class IWORKParser
{
  // -Weffc++
  IWORKParser(const IWORKParser &);
  IWORKParser &operator=(const IWORKParser &);

public:
  IWORKParser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package);
  virtual ~IWORKParser() = 0;
  bool parse();

  RVNGInputStreamPtr_t &getInput();
  RVNGInputStreamPtr_t getInput() const;
  RVNGInputStreamPtr_t &getPackage();
  RVNGInputStreamPtr_t getPackage() const;

  virtual const IWORKTokenizer &getTokenizer() const = 0;

private:
  virtual IWORKXMLContextPtr_t createDocumentContext() = 0;
  virtual IWORKXMLContextPtr_t createDiscardContext() = 0;

private:
  RVNGInputStreamPtr_t m_input;
  RVNGInputStreamPtr_t m_package;
};

} // namespace libetonyek

#endif // IWORKPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
