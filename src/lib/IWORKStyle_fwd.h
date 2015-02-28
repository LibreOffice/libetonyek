/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKSTYLE_FWD_H_INCLUDED
#define IWORKSTYLE_FWD_H_INCLUDED

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include "IWORKTypes_fwd.h"

namespace libetonyek
{

class IWORKStyle;

typedef boost::shared_ptr<IWORKStyle> IWORKStylePtr_t;
typedef boost::unordered_map<ID_t, IWORKStylePtr_t> IWORKStyleMap_t;

}

#endif // IWORKSTYLE_FWD_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
