/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTransformation.h"

#include <cmath>

#include "IWORKTypes.h"

namespace libetonyek
{

IWORKTransformation::IWORKTransformation()
  : m_xx(1)
  , m_yx(0)
  , m_xy(0)
  , m_yy(1)
  , m_x0(0)
  , m_y0(0)
{
}

IWORKTransformation::IWORKTransformation(const double xx, const double yx, const double xy, const double yy, const double x0, const double y0)
  : m_xx(xx)
  , m_yx(yx)
  , m_xy(xy)
  , m_yy(yy)
  , m_x0(x0)
  , m_y0(y0)
{
}

IWORKTransformation &IWORKTransformation::operator*=(const IWORKTransformation &tr)
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

bool IWORKTransformation::approxEqual(const IWORKTransformation &other, const double eps) const
{
  using libetonyek::approxEqual;
  return approxEqual(m_xx, other.m_xx, eps)
         && approxEqual(m_yx, other.m_yx, eps)
         && approxEqual(m_xy, other.m_xy, eps)
         && approxEqual(m_yy, other.m_yy, eps)
         && approxEqual(m_x0, other.m_x0, eps)
         && approxEqual(m_y0, other.m_y0, eps)
         ;
}

IWORKTransformation operator*(const IWORKTransformation &left, const IWORKTransformation &right)
{
  IWORKTransformation result(left);
  return result *= right;
}

void IWORKTransformation::operator()(double &x, double &y, const bool distance) const
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

bool operator==(const IWORKTransformation &left, const IWORKTransformation &right)
{
  return left.approxEqual(right);
}

bool operator!=(const IWORKTransformation &left, const IWORKTransformation &right)
{
  return !(left == right);
}

IWORKTransformation makeTransformation(const IWORKGeometry &geometry)
{
  using namespace transformations;

  IWORKTransformation tr;

  const double w = geometry.m_naturalSize.m_width;
  const double h = geometry.m_naturalSize.m_height;

  tr *= origin(w, h);

  // TODO: make sure the order of transformations is right
  if (geometry.m_shearXAngle || geometry.m_shearYAngle)
    tr *= shear(get_optional_value_or(geometry.m_shearXAngle, 0), get_optional_value_or(geometry.m_shearYAngle, 0));

  if (geometry.m_horizontalFlip)
    tr *= flip(get(geometry.m_horizontalFlip), false);
  if (geometry.m_verticalFlip)
    tr *= flip(false, get(geometry.m_verticalFlip));

  if (geometry.m_angle)
    tr *= rotate(get(geometry.m_angle));

  tr *= center(w, h);
  tr *= translate(geometry.m_position.m_x, geometry.m_position.m_y);

  return tr;
}

namespace transformations
{

IWORKTransformation center(const double width, const double height)
{
  return translate(width / 2, height / 2);
}

IWORKTransformation origin(const double width, const double height)
{
  return translate(-width / 2, -height / 2);
}

IWORKTransformation flip(const bool horizontal, const bool vertical)
{
  return scale(horizontal ? -1 : 1, vertical ? -1 : 1);
}

IWORKTransformation rotate(const double angle)
{
  const double c = std::cos(angle);
  const double s = std::sin(angle);
  return IWORKTransformation(c, s, -s, c, 0, 0);
}

IWORKTransformation scale(const double ratioX, const double ratioY)
{
  return IWORKTransformation(ratioX, 0, 0, ratioY, 0, 0);
}

IWORKTransformation shear(const double angleX, const double angleY)
{
  // TODO: check this
  return IWORKTransformation(1, std::tan(angleY), std::tan(angleX), 1, 0, 0);
}

IWORKTransformation translate(const double offsetX, const double offsetY)
{
  return IWORKTransformation(1, 0, 0, 1, offsetX, offsetY);
}

}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
