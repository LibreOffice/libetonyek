/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTYPES_FWD_H_INCLUDED
#define IWORKTYPES_FWD_H_INCLUDED

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

namespace libetonyek
{

typedef std::string ID_t;

typedef std::deque<double> ColumnSizes_t;

typedef std::deque<double> RowSizes_t;

typedef unsigned IWORKOutputID_t;

typedef boost::unordered_map<ID_t, IWORKOutputID_t> IWORKOutputMap_t;

struct IWORKSize;
struct IWORKPosition;

struct IWORKGeometry;

typedef boost::shared_ptr<IWORKGeometry> IWORKGeometryPtr_t;

struct IWORKLine;

typedef boost::shared_ptr<IWORKLine> IWORKLinePtr_t;

struct IWORKData;

typedef boost::shared_ptr<IWORKData> IWORKDataPtr_t;
typedef boost::unordered_map<ID_t, IWORKDataPtr_t> IWORKDataMap_t;

struct IWORKMediaContent;

typedef boost::shared_ptr<IWORKMediaContent> IWORKMediaContentPtr_t;
typedef boost::unordered_map<ID_t, IWORKMediaContentPtr_t> IWORKMediaContentMap_t;

struct IWORKImage;

typedef boost::shared_ptr<IWORKImage> IWORKImagePtr_t;
typedef boost::unordered_map<ID_t, IWORKImagePtr_t> IWORKImageMap_t;

struct IWORKMedia;

typedef boost::shared_ptr<IWORKMedia> IWORKMediaPtr_t;

struct IWORKWrap;

typedef boost::shared_ptr<IWORKWrap> IWORKWrapPtr_t;

struct IWORKGroup;

typedef boost::shared_ptr<IWORKGroup> IWORKGroupPtr_t;

}

#endif //  IWORKTYPES_FWD_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
