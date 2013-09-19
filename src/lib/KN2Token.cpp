/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstring>

#include "KN2Token.h"

using std::strlen;

namespace libkeynote
{
namespace
{

#include "KN2Token.inc"

}
}

namespace libkeynote
{

const KNToken *getKN2Token(const char *const str, const unsigned length)
{
  static const KNToken empty = {"", NS_NONE};
  return (length != 0) ? Perfect_Hash::in_word_set(str, length) : &empty;
}

const KNToken *getKN2Token(const char *const str)
{
  return getKN2Token(str, strlen(str));
}

const KNToken *getKN2Token(const unsigned char *const str)
{
  return getKN2Token(reinterpret_cast<const char *>(str));
}

int getKN2TokenID(const KNToken &token)
{
  return getKN2TokenID(&token);
}

int getKN2TokenID(const KNToken *const token)
{
  return token ? token->id : INVALID_TOKEN;
}

int getKN2TokenID(const char *str, const unsigned length)
{
  return getKN2TokenID(getKN2Token(str, length));
}

int getKN2TokenID(const char *str)
{
  return getKN2TokenID(str, strlen(str));
}

int getKN2TokenID(const unsigned char *str)
{
  return getKN2TokenID(reinterpret_cast<const char *>(str));
}

const char *getKN2TokenName(const KNToken &token)
{
  return getKN2TokenName(&token);
}

const char *getKN2TokenName(const KNToken *const token)
{
  return token ? token->name : 0;
}

const char *getKN2TokenName(const char *str, const unsigned length)
{
  return getKN2TokenName(getKN2Token(str, length));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
