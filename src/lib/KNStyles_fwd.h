/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNSTYLES_FWD_H_INCLUDED
#define KNSTYLES_FWD_H_INCLUDED

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include "KNTypes_fwd.h"

namespace libkeynote
{

class KNCellStyle;

typedef boost::shared_ptr<KNCellStyle> KNCellStylePtr_t;
typedef boost::unordered_map<ID_t, KNCellStylePtr_t> KNCellStyleMap_t;

class KNCharacterStyle;

typedef boost::shared_ptr<KNCharacterStyle> KNCharacterStylePtr_t;
typedef boost::unordered_map<ID_t, KNCharacterStylePtr_t> KNCharacterStyleMap_t;

class KNConnectionStyle;

typedef boost::shared_ptr<KNConnectionStyle> KNConnectionStylePtr_t;
typedef boost::unordered_map<ID_t, KNConnectionStylePtr_t> KNConnectionStyleMap_t;

class KNGraphicStyle;

typedef boost::shared_ptr<KNGraphicStyle> KNGraphicStylePtr_t;
typedef boost::unordered_map<ID_t, KNGraphicStylePtr_t> KNGraphicStyleMap_t;

class KNLayoutStyle;

typedef boost::shared_ptr<KNLayoutStyle> KNLayoutStylePtr_t;
typedef boost::unordered_map<ID_t, KNLayoutStylePtr_t> KNLayoutStyleMap_t;

class KNListStyle;

typedef boost::shared_ptr<KNListStyle> KNListStylePtr_t;
typedef boost::unordered_map<ID_t, KNListStylePtr_t> KNListStyleMap_t;

class KNParagraphStyle;

typedef boost::shared_ptr<KNParagraphStyle> KNParagraphStylePtr_t;
typedef boost::unordered_map<ID_t, KNParagraphStylePtr_t> KNParagraphStyleMap_t;

class KNPlaceholderStyle;

typedef boost::shared_ptr<KNPlaceholderStyle> KNPlaceholderStylePtr_t;
typedef boost::unordered_map<ID_t, KNPlaceholderStylePtr_t> KNPlaceholderStyleMap_t;

class KNSlideStyle;

typedef boost::shared_ptr<KNSlideStyle> KNSlideStylePtr_t;
typedef boost::unordered_map<ID_t, KNSlideStylePtr_t> KNSlideStyleMap_t;

class KNTabularStyle;

typedef boost::shared_ptr<KNTabularStyle> KNTabularStylePtr_t;
typedef boost::unordered_map<ID_t, KNTabularStylePtr_t> KNTabularStyleMap_t;

class KNVectorStyle;

typedef boost::shared_ptr<KNVectorStyle> KNVectorStylePtr_t;
typedef boost::unordered_map<ID_t, KNVectorStylePtr_t> KNVectorStyleMap_t;

}

#endif // KNSTYLES_FWD_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
