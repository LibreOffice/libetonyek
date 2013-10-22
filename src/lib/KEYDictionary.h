/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYDICTIONARY_H_INCLUDED
#define KEYDICTIONARY_H_INCLUDED

#include "KEYPath.h"
#include "KEYStyles_fwd.h"
#include "KEYStylesheet.h"
#include "KEYTypes.h"

namespace libetonyek
{

/** A dictionary of all referenceable entities in the presentation.
  *
  * In other words, all entities that might be included through a -ref
  * element in the format. This is purely a file format concept; all
  * that matters when an entity is inserted by a reference is that there
  * has been an entity of the same type and with the same content
  * inserted previously (or at least that is how I understand it. I
  * might be wrong about it). That means we need to know all entities in
  * the whole presentation to resolve references.
  *
  * @note The dictionary contains (among other things) all master
  * styles, therefore we do not handle them separately. (We might do
  * that in the future, to ensure correctness of the format.)
  */
struct KEYDictionary
{
  KEYCharacterStyleMap_t characterStyles;
  KEYGraphicStyleMap_t graphicStyles;
  KEYLayoutStyleMap_t layoutStyles;
  KEYParagraphStyleMap_t paragraphStyles;
  KEYPlaceholderStyleMap_t placeholderStyles;

  KEYStylesheetMap_t stylesheets;

  KEYPathMap_t beziers;

  KEYDataMap_t data;
  KEYMediaContentMap_t unfiltereds;
  KEYMediaContentMap_t filteredImages;
  KEYImageMap_t images;

  KEYPlaceholderMap_t bodyPlaceholders;
  KEYPlaceholderMap_t titlePlaceholders;

  KEYDictionary();
};

}

#endif //  KEYDICTIONARY_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
