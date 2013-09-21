/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNSTYLESHEET_H_INCLUDED
#define KNSTYLESHEET_H_INCLUDED

#include "KNStyles_fwd.h"
#include "KNTypes_fwd.h"

namespace libkeynote
{

struct KNStylesheet;
typedef boost::shared_ptr<KNStylesheet> KNStylesheetPtr_t;
typedef boost::unordered_map<ID_t, KNStylesheetPtr_t> KNStylesheetMap_t;

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
struct KNStylesheet
{
  KNStylesheetPtr_t parent;

  KNCellStyleMap_t cellStyles;
  KNCharacterStyleMap_t characterStyles;
  KNConnectionStyleMap_t connectionStyles;
  KNGraphicStyleMap_t graphicStyles;
  KNLayoutStyleMap_t layoutStyles;
  KNListStyleMap_t listStyles;
  KNParagraphStyleMap_t paragraphStyles;
  KNPlaceholderStyleMap_t placeholderStyles;
  KNSlideStyleMap_t slideStyles;
  KNTabularStyleMap_t tabularStyles;
  KNVectorStyleMap_t vectorStyles;

  KNStylesheet();
};

}

#endif // KNSTYLESHEET_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
