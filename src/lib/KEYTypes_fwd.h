/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYTYPES_FWD_H_INCLUDED
#define KEYTYPES_FWD_H_INCLUDED

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

namespace libkeynote
{

typedef std::string ID_t;

struct KEYSize;
struct KEYPosition;

struct KEYGeometry;

typedef boost::shared_ptr<KEYGeometry> KEYGeometryPtr_t;

struct KEYLine;

typedef boost::shared_ptr<KEYLine> KEYLinePtr_t;

struct KEYData;

typedef boost::shared_ptr<KEYData> KEYDataPtr_t;
typedef boost::unordered_map<ID_t, KEYDataPtr_t> KEYDataMap_t;

struct KEYMediaContent;

typedef boost::shared_ptr<KEYMediaContent> KEYMediaContentPtr_t;
typedef boost::unordered_map<ID_t, KEYMediaContentPtr_t> KEYMediaContentMap_t;

struct KEYImage;

typedef boost::shared_ptr<KEYImage> KEYImagePtr_t;
typedef boost::unordered_map<ID_t, KEYImagePtr_t> KEYImageMap_t;

struct KEYMedia;

typedef boost::shared_ptr<KEYMedia> KEYMediaPtr_t;

struct KEYWrap;

typedef boost::shared_ptr<KEYWrap> KEYWrapPtr_t;

struct KEYGroup;

typedef boost::shared_ptr<KEYGroup> KEYGroupPtr_t;

struct KEYLayer;

typedef boost::shared_ptr<KEYLayer> KEYLayerPtr_t;
typedef boost::unordered_map<ID_t, KEYLayerPtr_t> KEYLayerMap_t;

struct KEYPlaceholder;

typedef boost::shared_ptr<KEYPlaceholder> KEYPlaceholderPtr_t;
typedef boost::unordered_map<ID_t, KEYPlaceholderPtr_t> KEYPlaceholderMap_t;

}

#endif //  KEYTYPES_FWD_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
