/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef NUM1TOKEN_H_INCLUDED
#define NUM1TOKEN_H_INCLUDED

#include <boost/static_assert.hpp>

#include "IWORKTokenInfo.h"

namespace libetonyek
{

namespace NUM1Token
{

enum
{
  INVALID_TOKEN = 0,
  FIRST_TOKEN = IWORKTokenInfo<NUM1Parser>::first,

  // namespace prefixes
  ls,

  // elements
  document,
  page_info,
  stylesheet,
  workspace,
  workspace_array,

  // attributes
  version,

  // attribute values
  VERSION_STR_2,

  LAST_TOKEN
};

BOOST_STATIC_ASSERT(IWORKTokenInfo<NUM1Parser>::last >= LAST_TOKEN);

enum Namespace
{
  NS_URI_LS = ls << 16
};

}

struct NUM1Tokenizer
{
  int operator()(const char *str) const;
};

}

#endif // NUM1TOKEN_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
