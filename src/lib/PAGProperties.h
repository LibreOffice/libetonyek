/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PAGPROPERTIES_H_INCLUDED
#define PAGPROPERTIES_H_INCLUDED

#include "IWORKPropertyInfo.h"
#include "PAGTypes.h"

namespace libetonyek
{

IWORK_DECLARE_PROPERTY(EvenPageMaster, PAGPageMaster);
IWORK_DECLARE_PROPERTY(FirstPageMaster, PAGPageMaster);
IWORK_DECLARE_PROPERTY(OddPageMaster, PAGPageMaster);

}

#endif // PAGPROPERTIES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
