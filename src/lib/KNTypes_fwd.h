/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNTYPES_FWD_H_INCLUDED
#define KNTYPES_FWD_H_INCLUDED

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

namespace libkeynote
{

typedef std::string ID_t;

struct KNSize;
struct KNPosition;

struct KNGeometry;

typedef boost::shared_ptr<KNGeometry> KNGeometryPtr_t;

struct KNLine;

typedef boost::shared_ptr<KNLine> KNLinePtr_t;

struct KNData;

typedef boost::shared_ptr<KNData> KNDataPtr_t;
typedef boost::unordered_map<ID_t, KNDataPtr_t> KNDataMap_t;

struct KNMediaContent;

typedef boost::shared_ptr<KNMediaContent> KNMediaContentPtr_t;
typedef boost::unordered_map<ID_t, KNMediaContentPtr_t> KNMediaContentMap_t;

struct KNImage;

typedef boost::shared_ptr<KNImage> KNImagePtr_t;
typedef boost::unordered_map<ID_t, KNImagePtr_t> KNImageMap_t;

struct KNMedia;

typedef boost::shared_ptr<KNMedia> KNMediaPtr_t;

struct KNWrap;

typedef boost::shared_ptr<KNWrap> KNWrapPtr_t;

struct KNGroup;

typedef boost::shared_ptr<KNGroup> KNGroupPtr_t;

struct KNLayer;

typedef boost::shared_ptr<KNLayer> KNLayerPtr_t;
typedef boost::unordered_map<ID_t, KNLayerPtr_t> KNLayerMap_t;

struct KNPlaceholder;

typedef boost::shared_ptr<KNPlaceholder> KNPlaceholderPtr_t;
typedef boost::unordered_map<ID_t, KNPlaceholderPtr_t> KNPlaceholderMap_t;

}

#endif //  KNTYPES_FWD_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
