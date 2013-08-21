/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/optional.hpp>

#include "KNStyle.h"
#include "KNTypes.h"

namespace libkeynote
{

namespace
{

void mergeParentStyle(KNStyle &style, const KNStyle &parent)
{
  // TODO: implement me
  (void) style;
  (void) parent;
}

}

KNStyle::KNStyle()
  : ident()
  , name()
  , parentIdent()
  , locked()

  , listStyle()
  // , geometry()
  // , padding()

  , layoutParagraphStyle()
  , layoutStyle()
{
}

void resolveStyle(KNStyle &style, const KNStyleMap_t &styles)
{
  boost::optional<std::string> parent;

  while (parent)
  {
    const KNStyleMap_t::const_iterator it = styles.find(get(parent));
    if (styles.end() != it)
    {
      const KNStylePtr_t parentStyle = it->second;
      mergeParentStyle(style, *parentStyle);
      // parent = parentStyle.parent;
    }
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
