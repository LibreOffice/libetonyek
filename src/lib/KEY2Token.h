/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY2TOKEN_H_INCLUDED
#define KEY2TOKEN_H_INCLUDED

#include <boost/static_assert.hpp>

#include "IWORKToken.h"
#include "IWORKTokenInfo.h"

namespace libetonyek
{

namespace KEY2Token
{

enum
{
  INVALID_TOKEN = 0,
  // redefine for convenience
  sticky_note = IWORKToken::sticky_note,
  size = IWORKToken::size,
  text = IWORKToken::text,
  FIRST_TOKEN = IWORKTokenInfo<KEY2Parser>::first,

  // namespace prefixes
  key,

  // elements
  body_placeholder,
  c,
  headlineParagraphStyle,
  master_slide,
  master_slides,
  metadata,
  notes,
  page,
  presentation,
  slide,
  slide_list,
  sticky_notes,
  stylesheet,
  theme,
  theme_list,
  title_placeholder,

  // attributes
  right,
  version,

  // attribute values
  COMPATIBLE_VERSION_STR_3,
  VERSION_STR_2,
  VERSION_STR_3,
  VERSION_STR_4,
  VERSION_STR_5,
  double_,
  i,
  star,

  LAST_TOKEN
};

BOOST_STATIC_ASSERT(IWORKTokenInfo<KEY2Parser>::last >= LAST_TOKEN);

enum Namespace
{
  NS_URI_KEY = key << 16
};

}

struct KEY2Tokenizer
{
  int operator()(const char *str) const;
};

}

#endif // KEY2TOKEN_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
