/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PAG1TOKEN_H_INCLUDED
#define PAG1TOKEN_H_INCLUDED

#include <boost/static_assert.hpp>

#include "IWORKTokenInfo.h"

namespace libetonyek
{

class IWORKTokenizer;

namespace PAG1Token
{

enum
{
  INVALID_TOKEN = 0,
  FIRST_TOKEN = IWORKTokenInfo<PAG1Parser>::first,

  // namespace prefixes
  sl,

  // elements
  SLCreationDateProperty,
  date,
  document,
  drawables,
  header,
  kSFWPFootnoteGapProperty,
  kSFWPFootnoteKindProperty,
  layout,
  number,
  order,
  page_group,
  print_info,
  prototype,
  publication_info,
  section,
  section_prototypes,
  slprint_info,
  stylesheet,

  // attributes
  page,
  page_height,
  page_scale,
  page_width,
  rpage,
  version,

  // attribute values
  VERSION_STR_4,
  body,
  cell,
  footer,
  footnote,
  note,
  textbox,

  LAST_TOKEN
};

BOOST_STATIC_ASSERT(IWORKTokenInfo<PAG1Parser>::last >= LAST_TOKEN);

enum Namespace
{
  NS_URI_SL = sl << 16
};

const IWORKTokenizer &getTokenizer();

}

}

#endif // PAG1TOKEN_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
