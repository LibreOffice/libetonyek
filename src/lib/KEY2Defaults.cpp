/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY2Defaults.h"

namespace libetonyek
{

namespace
{

static const KEYSize PRESENTATION_SIZE = KEYSize(1024, 768);
static const KEYSize SHAPE_SIZE = KEYSize(100, 100);

}

KEY2Defaults::~KEY2Defaults()
{
}

void KEY2Defaults::applyPresentationSize(boost::optional<KEYSize> &size) const
{
  if (!size)
    size = PRESENTATION_SIZE;
}

void KEY2Defaults::applyGeometry(boost::optional<KEYSize> &naturalSize, boost::optional<KEYPosition> &position) const
{
  if (!naturalSize)
    naturalSize = SHAPE_SIZE;
  if (!position)
    position = KEYPosition(PRESENTATION_SIZE.width - SHAPE_SIZE.width, PRESENTATION_SIZE.height - SHAPE_SIZE.width);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
