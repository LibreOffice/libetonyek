/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY1Defaults.h"

namespace libkeynote
{

KEY1Defaults::~KEY1Defaults()
{
}

void KEY1Defaults::applyPresentationSize(boost::optional<KEYSize> &size) const
{
  // TODO: implement me
  (void) size;
}

void KEY1Defaults::applyGeometry(boost::optional<KEYSize> &naturalSize, boost::optional<KEYPosition> &position) const
{
  // TODO: implement me
  (void) naturalSize;
  (void) position;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
