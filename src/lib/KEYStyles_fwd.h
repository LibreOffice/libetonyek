/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYSTYLES_FWD_H_INCLUDED
#define KEYSTYLES_FWD_H_INCLUDED

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include "KEYTypes_fwd.h"

namespace libetonyek
{

class KEYCellStyle;

typedef boost::shared_ptr<KEYCellStyle> KEYCellStylePtr_t;
typedef boost::unordered_map<ID_t, KEYCellStylePtr_t> KEYCellStyleMap_t;

class KEYCharacterStyle;

typedef boost::shared_ptr<KEYCharacterStyle> KEYCharacterStylePtr_t;
typedef boost::unordered_map<ID_t, KEYCharacterStylePtr_t> KEYCharacterStyleMap_t;

class KEYConnectionStyle;

typedef boost::shared_ptr<KEYConnectionStyle> KEYConnectionStylePtr_t;
typedef boost::unordered_map<ID_t, KEYConnectionStylePtr_t> KEYConnectionStyleMap_t;

class KEYGraphicStyle;

typedef boost::shared_ptr<KEYGraphicStyle> KEYGraphicStylePtr_t;
typedef boost::unordered_map<ID_t, KEYGraphicStylePtr_t> KEYGraphicStyleMap_t;

class KEYLayoutStyle;

typedef boost::shared_ptr<KEYLayoutStyle> KEYLayoutStylePtr_t;
typedef boost::unordered_map<ID_t, KEYLayoutStylePtr_t> KEYLayoutStyleMap_t;

class KEYListStyle;

typedef boost::shared_ptr<KEYListStyle> KEYListStylePtr_t;
typedef boost::unordered_map<ID_t, KEYListStylePtr_t> KEYListStyleMap_t;

class KEYParagraphStyle;

typedef boost::shared_ptr<KEYParagraphStyle> KEYParagraphStylePtr_t;
typedef boost::unordered_map<ID_t, KEYParagraphStylePtr_t> KEYParagraphStyleMap_t;

class KEYPlaceholderStyle;

typedef boost::shared_ptr<KEYPlaceholderStyle> KEYPlaceholderStylePtr_t;
typedef boost::unordered_map<ID_t, KEYPlaceholderStylePtr_t> KEYPlaceholderStyleMap_t;

class KEYSlideStyle;

typedef boost::shared_ptr<KEYSlideStyle> KEYSlideStylePtr_t;
typedef boost::unordered_map<ID_t, KEYSlideStylePtr_t> KEYSlideStyleMap_t;

class KEYTabularStyle;

typedef boost::shared_ptr<KEYTabularStyle> KEYTabularStylePtr_t;
typedef boost::unordered_map<ID_t, KEYTabularStylePtr_t> KEYTabularStyleMap_t;

class KEYVectorStyle;

typedef boost::shared_ptr<KEYVectorStyle> KEYVectorStylePtr_t;
typedef boost::unordered_map<ID_t, KEYVectorStylePtr_t> KEYVectorStyleMap_t;

}

#endif // KEYSTYLES_FWD_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
