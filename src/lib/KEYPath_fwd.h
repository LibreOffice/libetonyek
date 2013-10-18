/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYPATH_FWD_H_INCLUDED
#define KEYPATH_FWD_H_INCLUDED

#include "KEYTypes_fwd.h"

namespace libkeynote
{

class KEYPath;

typedef boost::shared_ptr<KEYPath> KEYPathPtr_t;
typedef boost::unordered_map<ID_t, KEYPathPtr_t> KEYPathMap_t;

}

#endif //  KEYPATH_FWD_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */

