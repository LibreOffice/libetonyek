/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstring>

#include "KN1Token.h"

using std::strlen;

namespace libkeynote
{
namespace
{

#include "KN1Token.inc"

}
}

namespace libkeynote
{

const KNToken *getKN1Token(const char *const str, const unsigned length)
{
  return Perfect_Hash::in_word_set(str, length);
}

const KNToken *getKN1Token(const char *const str)
{
  return getKN1Token(str, strlen(str));
}

const KNToken *getKN1Token(const unsigned char *const str)
{
  return getKN1Token(reinterpret_cast<const char *>(str));
}

int getKN1TokenID(const KNToken &token)
{
  return getKN1TokenID(&token);
}

int getKN1TokenID(const KNToken *const token)
{
  return token ? token->id : INVALID_TOKEN;
}

int getKN1TokenID(const char *str, const unsigned length)
{
  return getKN1TokenID(getKN1Token(str, length));
}

int getKN1TokenID(const char *str)
{
  return getKN1TokenID(str, strlen(str));
}

int getKN1TokenID(const unsigned char *str)
{
  return getKN1TokenID(reinterpret_cast<const char *>(str));
}

const char *getKN1TokenName(const KNToken &token)
{
  return getKN1TokenName(&token);
}

const char *getKN1TokenName(const KNToken *const token)
{
  return token ? token->name : 0;
}

const char *getKN1TokenName(const char *str, const unsigned length)
{
  return getKN1TokenName(getKN1Token(str, length));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
