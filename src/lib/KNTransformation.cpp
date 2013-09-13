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
  , m_yx(0)
  , m_xy(0)
  , m_yy(1)
  , m_x0(0)
  , m_y0(0)
{
}

KNTransformation::KNTransformation(const double xx, const double yx, const double xy, const double yy, const double x0, const double y0)
  : m_xx(xx)
  , m_yx(yx)
  , m_xy(xy)
  , m_yy(yy)
  , m_x0(x0)
  , m_y0(y0)
{
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

KNTransformation makeTransformation(const KNGeometry &geometry)
{
  // TODO: implement me
  (void) geometry;
  return KNTransformation();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
