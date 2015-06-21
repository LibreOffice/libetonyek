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
  page_group,
  prototype,
  publication_info,
  section,
  section_prototypes,
  stylesheet,

  // attributes
  page,
  rpage,
  version,

  // attribute values
  body,
  cell,
  footer,
  footnote,
  note,
  textbox,
  VERSION_STR_4,

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
