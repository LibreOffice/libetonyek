/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYPROPERTIES_H_INCLUDED
#define KEYPROPERTIES_H_INCLUDED

#include "IWORKPropertyInfo.h"
#include "KEYTypes.h"

namespace libetonyek
{
IWORK_DECLARE_PROPERTY(AnimationAutoPlay, bool);
IWORK_DECLARE_PROPERTY(AnimationDelay, double);
IWORK_DECLARE_PROPERTY(AnimationDuration, double);
IWORK_DECLARE_PROPERTY(Transition, KEYTransition);
}

#endif // KEYPROPERTIES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
