/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY2Token.h"

#include <cassert>
#include <cstring>

#include <boost/make_shared.hpp>

#include "IWORKTokenizerBase.h"

using std::strlen;

namespace libetonyek
{
namespace
{

#include "KEY2Token.inc"

}
}

namespace libetonyek
{

namespace
{

class Tokenizer : public IWORKTokenizerBase
{
  virtual int queryId(const char *name) const;
};

int Tokenizer::queryId(const char *const name) const
{
  assert(name);

  const size_t length = strlen(name);
  assert(0 < length);

  const Token *const token = Perfect_Hash::in_word_set(name, length);
  return token ? token->id : 0;
}

}

namespace KEY2Token
{

const IWORKTokenizer &getTokenizer()
{
  static Tokenizer tokenizer;
  return tokenizer;
}

}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
