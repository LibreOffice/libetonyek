/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTYPES_FWD_H_INCLUDED
#define IWORKTYPES_FWD_H_INCLUDED

#include <deque>
#include <memory>
#include <string>

#include <boost/unordered_map.hpp>

namespace libetonyek
{

typedef std::string ID_t;

typedef std::deque<double> IWORKColumnSizes_t;

typedef std::deque<double> IWORKRowSizes_t;

typedef unsigned IWORKOutputID_t;

typedef boost::unordered_map<ID_t, IWORKOutputID_t> IWORKOutputMap_t;

typedef boost::unordered_map<std::string, std::string> IWORKTableNameMap_t;

typedef std::shared_ptr<IWORKTableNameMap_t> IWORKTableNameMapPtr_t;

typedef boost::unordered_map<std::string, std::string> IWORKContentMap_t;

struct IWORKSize;
struct IWORKPosition;

struct IWORKGeometry;

typedef std::shared_ptr<IWORKGeometry> IWORKGeometryPtr_t;

struct IWORKLine;

typedef std::shared_ptr<IWORKLine> IWORKLinePtr_t;

struct IWORKData;

typedef std::shared_ptr<IWORKData> IWORKDataPtr_t;
typedef boost::unordered_map<ID_t, IWORKDataPtr_t> IWORKDataMap_t;

struct IWORKMediaContent;

typedef std::shared_ptr<IWORKMediaContent> IWORKMediaContentPtr_t;
typedef boost::unordered_map<ID_t, IWORKMediaContentPtr_t> IWORKMediaContentMap_t;

struct IWORKImage;

typedef std::shared_ptr<IWORKImage> IWORKImagePtr_t;
typedef boost::unordered_map<ID_t, IWORKImagePtr_t> IWORKImageMap_t;

struct IWORKMedia;

typedef std::shared_ptr<IWORKMedia> IWORKMediaPtr_t;

struct IWORKWrap;

typedef std::shared_ptr<IWORKWrap> IWORKWrapPtr_t;

struct IWORKGroup;

typedef std::shared_ptr<IWORKGroup> IWORKGroupPtr_t;

struct IWORKTableData;

typedef std::shared_ptr<IWORKTableData> IWORKTableDataPtr_t;

}

#endif //  IWORKTYPES_FWD_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
