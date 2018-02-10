/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKStylesheet.h"

#include <set>

#include "libetonyek_utils.h"

namespace libetonyek
{

IWORKStylesheet::IWORKStylesheet()
  : parent()
  , m_styles()
{
}

IWORKStylePtr_t IWORKStylesheet::find(const std::string &name) const
{
  IWORKStylesheet const *currentStylesheet = this;
  std::set<IWORKStylesheet const *> seen;
  do
  {
    if (seen.find(currentStylesheet)!=seen.end())
    {
      ETONYEK_DEBUG_MSG(("IWORKStylesheet::find: oops, find a looop in parent zone\n"));
      break;
    }
    seen.insert(currentStylesheet);
    const IWORKStyleMap_t::const_iterator it = currentStylesheet->m_styles.find(name);
    if (currentStylesheet->m_styles.end() != it)
      return it->second;
    if (currentStylesheet == currentStylesheet->parent.get())
      currentStylesheet = 0;
    else
      currentStylesheet = currentStylesheet->parent.get();
    if (!currentStylesheet)
    {
      ETONYEK_DEBUG_MSG(("IWORKStylesheet::find: can not find parent %s\n", name.c_str()));
    }
  }
  while (currentStylesheet);
  return IWORKStylePtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
