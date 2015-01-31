/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PAGTOKEN_H_INCLUDED
#define PAGTOKEN_H_INCLUDED

#include <boost/static_assert.hpp>

#include "IWORKTokenInfo.h"

namespace libetonyek
{

namespace PAGToken
{

enum
{
  INVALID_TOKEN = 0,
  FIRST_TOKEN = IWORKTokenInfo<PAGParser>::first,

  // namespace prefixes
  sl,

  // elements
  document,
  header,
  layout,
  section,
  section_prototypes,
  stylesheet,

  // attributes

  // attribute values
  body,
  cell,
  footnote,
  note,
  textbox,

  LAST_TOKEN
};

BOOST_STATIC_ASSERT(IWORKTokenInfo<PAGParser>::last >= LAST_TOKEN);

enum Namespace
{
  NS_URI_SL = sl << 16
};

}

struct PAGTokenizer
{
  int operator()(const char *str) const;
};

}

#endif // PAGTOKEN_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
