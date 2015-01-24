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

namespace libetonyek
{

namespace PAGToken
{

namespace detail
{

template<unsigned N, unsigned P>
struct log_impl
{
  static const unsigned value = log_impl<(N >> 1), P + 1>::value;
};

template<unsigned P>
struct log_impl<1, P>
{
  static const unsigned value = P;
};

template<unsigned P>
struct log_impl<0, P>
{
};

template<unsigned N>
struct log
{
  static const unsigned value = log_impl<N, 0>::value;
};

}

enum
{
  INVALID_TOKEN = 0,

  // elements
  br,
  document,
  footers,
  header,
  headers,
  layout,
  lnbr,
  metadata,
  p,
  section,
  section_prototypes,
  span,
  stylesheet,
  tab,
  text_body,
  text_storage,

  // attributes
  kind,

  // attribute values
  body,
  cell,
  footnote,
  note,
  textbox,

  LAST_TOKEN
};

// namespaces
enum
{
  TOKEN_RANGE = 2 << (detail::log<LAST_TOKEN + 1>::value + 1),
  NS_NONE = TOKEN_RANGE,
  NS_URI_SF = NS_NONE + TOKEN_RANGE,
  NS_URI_SFA = NS_URI_SF + TOKEN_RANGE,
  NS_URI_SL = NS_URI_SFA + TOKEN_RANGE,
  NS_URI_XSI = NS_URI_SL + TOKEN_RANGE
};

}

struct PAGTokenizer
{
  int operator()(const char *str) const;
};

}

#endif // PAGTOKEN_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
