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

glm::dmat3 makeTransformation(const IWORKGeometry &geometry)
{
  using namespace transformations;

  glm::dmat3 tr;

  const double w = geometry.m_naturalSize.m_width;
  const double h = geometry.m_naturalSize.m_height;

  tr *= translate(geometry.m_position.m_x, geometry.m_position.m_y);
  tr *= center(w, h);

  if (geometry.m_angle)
    tr *= rotate(get(geometry.m_angle));

  if (geometry.m_verticalFlip)
    tr *= flip(false, get(geometry.m_verticalFlip));
  if (geometry.m_horizontalFlip)
    tr *= flip(get(geometry.m_horizontalFlip), false);

  // TODO: make sure the order of transformations is right
  if (geometry.m_shearXAngle || geometry.m_shearYAngle)
    tr *= shear(get_optional_value_or(geometry.m_shearXAngle, 0), get_optional_value_or(geometry.m_shearYAngle, 0));

  tr *= origin(w, h);

  return tr;
}

namespace transformations
{

glm::dmat3 center(const double width, const double height)
{
  return translate(width / 2, height / 2);
}

glm::dmat3 origin(const double width, const double height)
{
  return translate(-width / 2, -height / 2);
}

glm::dmat3 flip(const bool horizontal, const bool vertical)
{
  return scale(horizontal ? -1 : 1, vertical ? -1 : 1);
}

glm::dmat3 rotate(const double angle)
{
  const double c = std::cos(angle);
  const double s = std::sin(angle);
  return glm::dmat3(c, s, 0, -s, c, 0, 0, 0, 1);
}

glm::dmat3 scale(const double ratioX, const double ratioY)
{
  return glm::dmat3(ratioX, 0, 0, 0, ratioY, 0, 0, 0, 1);
}

glm::dmat3 shear(const double angleX, const double angleY)
{
  // TODO: check this
  return glm::dmat3(1, std::tan(angleY), 0, std::tan(angleX), 1, 0, 0, 0, 1);
}

glm::dmat3 translate(const double offsetX, const double offsetY)
{
  return glm::dmat3(1, 0, 0, 0, 1, 0, offsetX, offsetY, 1);
}

}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
