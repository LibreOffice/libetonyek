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
  : m_mat(glm::mat3(1))
{
}

IWORKTransformation::IWORKTransformation(const double xx, const double yx, const double xy, const double yy, const double x0, const double y0)
  : m_mat(glm::mat3(xx, yx, 0, xy, yy, 0, x0, y0, 1))
{
}

IWORKTransformation &IWORKTransformation::operator*=(const IWORKTransformation &tr)
{
  m_mat *= tr.m_mat;
  return *this;
}

bool IWORKTransformation::approxEqual(const IWORKTransformation &other, const double eps) const
{
  using libetonyek::approxEqual;
  return m_mat == other.m_mat;
}

IWORKTransformation operator*(const IWORKTransformation &left, const IWORKTransformation &right)
{
  IWORKTransformation result(left);
  return result *= right;
}

void IWORKTransformation::operator()(double &x, double &y, const bool distance) const
{
  glm::vec3 vec_new = m_mat * glm::vec3(x, y, !distance);

  x = vec_new[0];
  y = vec_new[1];
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
