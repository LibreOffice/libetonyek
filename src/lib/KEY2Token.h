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

#include "IWORKTokenInfo.h"

namespace libetonyek
{

class IWORKTokenizer;

namespace KEY2Token
{

enum
{
  INVALID_TOKEN = 0,
  FIRST_TOKEN = IWORKTokenInfo<KEY2Parser>::first,

  // namespace prefixes
  key,

  // elements
  BGBuildDurationProperty,
  animationAuto,
  animationAutoPlay,
  animationDelay,
  animationDuration,
  authors,
  body_placeholder,
  build,
  build_chunk,
  build_chunks,
  bullets,
  c,
  comment,
  direction,
  events,
  headline,
  headlineParagraphStyle,
  info_ref,
  keywords,
  master_slide,
  master_slides,
  master_ref,
  metadata,
  notes,
  number,
  object_placeholder,
  page,
  parent_build_ref,
  presentation,
  size,
  slide,
  slide_list,
  slide_style,
  slide_number_placeholder,
  sticky_note,
  sticky_notes,
  string,
  style_ref,
  stylesheet,
  text,
  theme,
  theme_list,
  title,
  title_placeholder,

  // attributes
  depth,
  name,
  type,
  version,

  // attribute values
  COMPATIBLE_VERSION_STR_3,
  VERSION_STR_2,
  VERSION_STR_3,
  VERSION_STR_4,
  VERSION_STR_5,
  i,

  LAST_TOKEN
};

BOOST_STATIC_ASSERT(IWORKTokenInfo<KEY2Parser>::last >= LAST_TOKEN);

enum Namespace
{
  NS_URI_KEY = key << 16
};

const IWORKTokenizer &getTokenizer();

}

}

#endif // KEY2TOKEN_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
