/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKPATH_FWD_H_INCLUDED
#define IWORKPATH_FWD_H_INCLUDED

#include <memory>
#include <unordered_map>

#include "IWORKTypes_fwd.h"

namespace libetonyek
{

class IWORKPath;

typedef std::shared_ptr<IWORKPath> IWORKPathPtr_t;
typedef std::unordered_map<ID_t, IWORKPathPtr_t> IWORKPathMap_t;

}

#endif //  IWORKPATH_FWD_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */

