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

#include <boost/optional.hpp>
#include <boost/unordered_map.hpp>

namespace libkeynote
{

typedef std::string ID_t;

struct KNGeometry;

typedef boost::shared_ptr<KNGeometry> KNGeometryPtr_t;
typedef boost::unordered_map<ID_t, KNGeometryPtr_t> KNGeometryMap_t;

struct KNLine;

typedef boost::shared_ptr<KNLine> KNLinePtr_t;
typedef boost::unordered_map<ID_t, KNLinePtr_t> KNLineMap_t;

struct KNImage;

typedef boost::shared_ptr<KNImage> KNImagePtr_t;
typedef boost::unordered_map<ID_t, KNImagePtr_t> KNImageMap_t;

struct KNMedia;

typedef boost::shared_ptr<KNMedia> KNMediaPtr_t;
typedef boost::unordered_map<ID_t, KNMediaPtr_t> KNMediaMap_t;

struct KNWrap;

typedef boost::shared_ptr<KNWrap> KNWrapPtr_t;

struct KNGroup;

typedef boost::shared_ptr<KNGroup> KNGroupPtr_t;
typedef boost::unordered_map<ID_t, KNGroupPtr_t> KNGroupMap_t;

struct KNLayer;

typedef boost::shared_ptr<KNLayer> KNLayerPtr_t;
typedef boost::unordered_map<ID_t, KNLayerPtr_t> KNLayerMap_t;

}

#endif //  KNTYPES_FWD_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
