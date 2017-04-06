/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYTYPES_FWD_H_INCLUDED
#define KEYTYPES_FWD_H_INCLUDED

#include <memory>
#include <string>

#include <boost/unordered_map.hpp>

#include "IWORKTypes_fwd.h"

namespace libetonyek
{

struct KEYLayer;

typedef std::shared_ptr<KEYLayer> KEYLayerPtr_t;
typedef boost::unordered_map<ID_t, KEYLayerPtr_t> KEYLayerMap_t;

struct KEYPlaceholder;

typedef std::shared_ptr<KEYPlaceholder> KEYPlaceholderPtr_t;
typedef boost::unordered_map<ID_t, KEYPlaceholderPtr_t> KEYPlaceholderMap_t;

}

#endif //  KEYTYPES_FWD_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
