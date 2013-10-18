/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYSTYLESHEET_H_INCLUDED
#define KEYSTYLESHEET_H_INCLUDED

#include "KEYStyles_fwd.h"
#include "KEYTypes_fwd.h"

namespace libkeynote
{

struct KEYStylesheet;
typedef boost::shared_ptr<KEYStylesheet> KEYStylesheetPtr_t;
typedef boost::unordered_map<ID_t, KEYStylesheetPtr_t> KEYStylesheetMap_t;

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
struct KEYStylesheet
{
  KEYStylesheetPtr_t parent;

  KEYCellStyleMap_t cellStyles;
  KEYCharacterStyleMap_t characterStyles;
  KEYConnectionStyleMap_t connectionStyles;
  KEYGraphicStyleMap_t graphicStyles;
  KEYLayoutStyleMap_t layoutStyles;
  KEYListStyleMap_t listStyles;
  KEYParagraphStyleMap_t paragraphStyles;
  KEYPlaceholderStyleMap_t placeholderStyles;
  KEYSlideStyleMap_t slideStyles;
  KEYTabularStyleMap_t tabularStyles;
  KEYVectorStyleMap_t vectorStyles;

  KEYStylesheet();
};

}

#endif // KEYSTYLESHEET_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
