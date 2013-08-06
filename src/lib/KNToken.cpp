/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstring>

#include "KNToken.h"

using std::strlen;

namespace libkeynote
{

struct KNToken
{
  const char *name;
  int id;
};

}

namespace libkeynote
{

#include "KNToken.inc"

}

namespace libkeynote
{

const KNToken *getKNToken(const char *const str, const unsigned length)
{
  return Perfect_Hash::in_word_set(str, length);
}

const KNToken *getKNToken(const char *const str)
{
  return getKNToken(str, strlen(str));
}

const KNToken *getKNToken(const unsigned char *const str)
{
  return getKNToken(reinterpret_cast<const char *>(str));
}

int getKNTokenID(const KNToken &token)
{
  return getKNTokenID(&token);
}

int getKNTokenID(const KNToken *const token)
{
  return token ? token->id : INVALID_TOKEN;
}

int getKNTokenID(const char *str, const unsigned length)
{
  return getKNTokenID(getKNToken(str, length));
}

int getKNTokenID(const char *str)
{
  return getKNTokenID(str, strlen(str));
}

int getKNTokenID(const unsigned char *str)
{
  return getKNTokenID(reinterpret_cast<const char *>(str));
}

const char *getKNTokenName(const KNToken &token)
{
  return getKNTokenName(&token);
}

const char *getKNTokenName(const KNToken *const token)
{
  return token ? token->name : 0;
}

const char *getKNTokenName(const char *str, const unsigned length)
{
  return getKNTokenName(getKNToken(str, length));
}

const char *getKNTokenName(const char *str)
{
  return getKNTokenName(str, strlen(str));
}

const char *getKNTokenName(const unsigned char *str)
{
  return getKNTokenName(reinterpret_cast<const char *>(str));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
