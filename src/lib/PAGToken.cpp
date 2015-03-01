/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAGToken.h"

#include <cstring>

using std::strlen;

namespace libetonyek
{
namespace
{

#include "PAGToken.inc"

}
}

namespace libetonyek
{

int PAGTokenizer::operator()(const char *const str) const
{
  if (!str)
    return INVALID_TOKEN;

  const size_t length = strlen(str);

  if (0 == length)
    return 0;

  const Token *const token = Perfect_Hash::in_word_set(str, length);
  return token ? token->id : INVALID_TOKEN;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
