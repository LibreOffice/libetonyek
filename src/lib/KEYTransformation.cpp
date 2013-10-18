/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cmath>

#include "KEYTransformation.h"
#include "KEYTypes.h"

namespace libkeynote
{

KEYTransformation::KEYTransformation()
  : m_xx(1)
  , m_yx(0)
  , m_xy(0)
  , m_yy(1)
  , m_x0(0)
  , m_y0(0)
{
}

KEYTransformation::KEYTransformation(const double xx, const double yx, const double xy, const double yy, const double x0, const double y0)
  : m_xx(xx)
  , m_yx(yx)
  , m_xy(xy)
  , m_yy(yy)
  , m_x0(x0)
  , m_y0(y0)
{
}

KEYTransformation &KEYTransformation::operator*=(const KEYTransformation &tr)
{
  const double xx = m_xx * tr.m_xx + m_yx * tr.m_xy;
  const double yx = m_xx * tr.m_yx + m_yx * tr.m_yy;
  const double xy = m_xy * tr.m_xx + m_yy * tr.m_xy;
  const double yy = m_xy * tr.m_yx + m_yy * tr.m_yy;
  const double x0 = m_x0 * tr.m_xx + m_y0 * tr.m_xy + tr.m_x0;
  const double y0 = m_x0 * tr.m_yx + m_y0 * tr.m_yy + tr.m_y0;

  m_xx = xx;
  m_yx = yx;
  m_xy = xy;
  m_yy = yy;
  m_x0 = x0;
  m_y0 = y0;

  return *this;
}

bool KEYTransformation::approxEqual(const KEYTransformation &other, const double eps) const
{
  using libkeynote::approxEqual;
  return approxEqual(m_xx, other.m_xx, eps)
         && approxEqual(m_yx, other.m_yx, eps)
         && approxEqual(m_xy, other.m_xy, eps)
         && approxEqual(m_yy, other.m_yy, eps)
         && approxEqual(m_x0, other.m_x0, eps)
         && approxEqual(m_y0, other.m_y0, eps)
         ;
}

KEYTransformation operator*(const KEYTransformation &left, const KEYTransformation &right)
{
  KEYTransformation result(left);
  return result *= right;
}

void KEYTransformation::operator()(double &x, double &y, const bool distance) const
{
  double x_new = m_xx * x + m_xy * y;
  double y_new = m_yx * x + m_yy * y;
  if (!distance)
  {
    x_new += m_x0;
    y_new += m_y0;
  }

  x = x_new;
  y = y_new;
}

bool operator==(const KEYTransformation &left, const KEYTransformation &right)
{
  return left.approxEqual(right);
}

bool operator!=(const KEYTransformation &left, const KEYTransformation &right)
{
  return !(left == right);
}

KEYTransformation makeTransformation(const KEYGeometry &geometry)
{
  using namespace transformations;

  KEYTransformation tr;

  const double w = geometry.naturalSize.width;
  const double h = geometry.naturalSize.height;

  tr *= origin(w, h);

  // TODO: make sure the order of transformations is right
  if (geometry.shearXAngle || geometry.shearYAngle)
    tr *= shear(get_optional_value_or(geometry.shearXAngle, 0), get_optional_value_or(geometry.shearYAngle, 0));

  if (geometry.horizontalFlip)
    tr *= flip(get(geometry.horizontalFlip), false);
  if (geometry.verticalFlip)
    tr *= flip(false, get(geometry.verticalFlip));

  if (geometry.angle)
    tr *= rotate(get(geometry.angle));

  tr *= center(w, h);
  tr *= translate(geometry.position.x, geometry.position.y);

  return tr;
}

namespace transformations
{

KEYTransformation center(const double width, const double height)
{
  return translate(width / 2, height / 2);
}

KEYTransformation origin(const double width, const double height)
{
  return translate(-width / 2, -height / 2);
}

KEYTransformation flip(const bool horizontal, const bool vertical)
{
  return scale(horizontal ? -1 : 1, vertical ? -1 : 1);
}

KEYTransformation rotate(const double angle)
{
  const double c = std::cos(angle);
  const double s = std::sin(angle);
  return KEYTransformation(c, s, -s, c, 0, 0);
}

KEYTransformation scale(const double ratioX, const double ratioY)
{
  return KEYTransformation(ratioX, 0, 0, ratioY, 0, 0);
}

KEYTransformation shear(const double angleX, const double angleY)
{
  // TODO: check this
  return KEYTransformation(1, std::tan(angleY), std::tan(angleX), 1, 0, 0);
}

KEYTransformation translate(const double offsetX, const double offsetY)
{
  return KEYTransformation(1, 0, 0, 1, offsetX, offsetY);
}

}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
