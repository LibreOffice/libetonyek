/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>

#include <boost/bind.hpp>

#include "IWORKObject.h"

namespace libetonyek
{

IWORKObject::~IWORKObject()
{
}

void drawAll(const IWORKObjectList_t &list, librevenge::RVNGPresentationInterface *const painter)
{
  for_each(list.begin(), list.end(), boost::bind(&IWORKObject::draw, _1, painter));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
