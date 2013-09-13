/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cmath>

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

void KNTransformation::operator()(double &x, double &y, const bool distance) const
{
  double x_new = m_xx * x + m_xy * y;
  double y_new = m_yx * x + m_yy * y;
  if (distance)
  {
    x_new += m_x0;
    y_new += m_y0;
  }

  x = x_new;
  y = y_new;
}

KNTransformation makeTransformation(const KNGeometry &geometry)
{
  using namespace transformations;

  KNTransformation tr;

  double w = 0;
  double h = 0;

  if (geometry.naturalSize)
  {
    w = get(geometry.naturalSize).width;
    h = get(geometry.naturalSize).height;
  }
  else if (geometry.size)
  {
    w = get(geometry.size).width;
    h = get(geometry.size).height;
  }

  if ((0 != w) || (0 != h))
    tr *= center(w, h);

  // TODO: make sure the order of transformations is right
  if (geometry.shearXAngle || geometry.shearYAngle)
    tr *= shear(geometry.shearXAngle, geometry.shearYAngle);

  if (geometry.horizontalFlip || geometry.verticalFlip)
    tr *= flip(geometry.horizontalFlip, geometry.verticalFlip);

  if (geometry.angle != 0)
    tr *= rotate(geometry.angle);

  if ((0 != w) || (0 != h))
    tr *= decenter(w, h);

  if (geometry.position)
    tr *= translate(get(geometry.position).x, get(geometry.position).y);

  return tr;
}

namespace transformations
{

KNTransformation center(const double width, const double height)
{
  return translate(width / 2, height / 2);
}

KNTransformation decenter(const double width, const double height)
{
  return translate(-width / 2, -height / 2);
}

KNTransformation flip(const bool horizontal, const bool vertical)
{
  return scale(horizontal ? -1 : 1, vertical ? -1 : 1);
}

KNTransformation rotate(const double angle)
{
  const double c = std::cos(angle);
  const double s = std::sin(angle);
  return KNTransformation(c, s, -s, c, 0, 0);
}

KNTransformation scale(const double ratioX, const double ratioY)
{
  return KNTransformation(ratioX, 0, 0, ratioY, 0, 0);
}

KNTransformation shear(const double angleX, const double angleY)
{
  // TODO: check this
  return KNTransformation(1, std::tan(angleY), std::tan(angleX), 1, 0, 0);
}

KNTransformation translate(const double offsetX, const double offsetY)
{
  return KNTransformation(1, 0, 0, 1, offsetX, offsetY);
}

}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
