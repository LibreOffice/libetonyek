/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNSTYLE_FWD_H_INCLUDED
#define KNSTYLE_FWD_H_INCLUDED

#include "KNTypes_fwd.h"

namespace libkeynote
{

struct KNStyle;

typedef boost::shared_ptr<KNStyle> KNStylePtr_t;
typedef boost::unordered_map<ID_t, KNStylePtr_t> KNStyleMap_t;

}

#endif //  KNSTYLE_FWD_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
