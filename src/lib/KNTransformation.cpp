/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KNTransformation.h"
#include "KNTypes.h"

namespace libkeynote
{

KNTransformation::KNTransformation()
  : m_xx(1)
  , m_xy(0)
  , m_x1(0)
  , m_yx(0)
  , m_yy(1)
  , m_y1(0)
  , m_horizontalFlip(false)
  , m_verticalFlip(false)
{
}

KNTransformation::KNTransformation(const KNGeometry &geometry)
  : m_xx(1)
  , m_xy(0)
  , m_x1(0)
  , m_yx(1)
  , m_yy(0)
  , m_y1(0)
  , m_horizontalFlip(false)
  , m_verticalFlip(false)
{
  // TODO: implement me
  (void) geometry;
}

KNTransformation &KNTransformation::operator*=(const KNTransformation &tr)
{
  // TODO: implement me
  (void) tr;
  return *this;
}

KNTransformation operator*(const KNTransformation &left, const KNTransformation right)
{
  KNTransformation result(left);
  return result *= right;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
