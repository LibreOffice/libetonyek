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

namespace libetonyek
{

typedef std::string ID_t;

struct IWORKSize;
struct IWORKPosition;

struct IWORKGeometry;

typedef boost::shared_ptr<IWORKGeometry> IWORKGeometryPtr_t;

}

#endif //  IWORKTYPES_FWD_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
