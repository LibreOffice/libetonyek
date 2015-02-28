/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKSTYLESHEET_H_INCLUDED
#define IWORKSTYLESHEET_H_INCLUDED

#include "IWORKStyle_fwd.h"
#include "IWORKTypes_fwd.h"

namespace libetonyek
{

struct IWORKStylesheet;
typedef boost::shared_ptr<IWORKStylesheet> IWORKStylesheetPtr_t;
typedef boost::unordered_map<ID_t, IWORKStylesheetPtr_t> IWORKStylesheetMap_t;

/** Representation of a stylesheet.
  *
  * Stylesheets are nested; the hierarchy is:
  * @li theme stylesheet
  * @li master slide stylesheet
  * @li slide stylesheet
  *
  * @note The retrieval key for a style is its @c ident attribute, not
  * its ID.
  */
struct IWORKStylesheet
{
  IWORKStylesheetPtr_t parent;

  IWORKStyleMap_t m_styles;

  IWORKStylesheet();
};

}

#endif // IWORKSTYLESHEET_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
