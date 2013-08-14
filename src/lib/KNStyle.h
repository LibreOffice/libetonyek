/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNSTYLE_H_INCLUDED
#define KNSTYLE_H_INCLUDED

#include <string>

#include <boost/optional.hpp>

#include "KNTypes.h"

namespace libkeynote
{

// TODO: it might make more sense to have specific structs for various
// style types. But unfortunately there is no schema available, so I do
// not know which properties belong to which style type. So I am just
// going to go with a grab bag of everything...
struct KNStyle
{
  boost::optional<std::string> ident;
  boost::optional<std::string> name;
  boost::optional<std::string> parentIdent;
  boost::optional<bool> locked;

  boost::optional<std::string> listStyle;
  boost::optional<KNGeometry> geometry;
  boost::optional<KNPadding> padding;

  boost::optional<std::string> layoutParagraphStyle;
  boost::optional<std::string> layoutStyle;

  // TODO: and many more... add as necessary

  KNStyle();
};

}

#endif // KNSTYLE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
