/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>

#include <boost/bind.hpp>

#include "KEYObject.h"
#include "KEYOutput.h"

namespace libkeynote
{

KEYObject::~KEYObject()
{
}

void drawAll(const KEYObjectList_t &list, const KEYOutput &output)
{
  for_each(list.begin(), list.end(), boost::bind(&KEYObject::draw, _1, output));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
