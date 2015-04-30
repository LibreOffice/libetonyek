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

glm::mat3 makeTransformation(const IWORKGeometry &geometry)
{
  using namespace transformations;

  glm::mat3 tr;

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

glm::mat3 center(const double width, const double height)
{
  return translate(width / 2, height / 2);
}

glm::mat3 origin(const double width, const double height)
{
  return translate(-width / 2, -height / 2);
}

glm::mat3 flip(const bool horizontal, const bool vertical)
{
  return scale(horizontal ? -1 : 1, vertical ? -1 : 1);
}

glm::mat3 rotate(const double angle)
{
  const double c = std::cos(angle);
  const double s = std::sin(angle);
  return glm::mat3(c, s, 0, -s, c, 0, 0, 0, 1);
}

glm::mat3 scale(const double ratioX, const double ratioY)
{
  return glm::mat3(ratioX, 0, 0, 0, ratioY, 0, 0, 0, 1);
}

glm::mat3 shear(const double angleX, const double angleY)
{
  // TODO: check this
  return glm::mat3(1, std::tan(angleY), 0, std::tan(angleX), 1, 0, 0, 0, 1);
}

glm::mat3 translate(const double offsetX, const double offsetY)
{
  return glm::mat3(1, 0, 0, 0, 1, 0, offsetX, offsetY, 1);
}

}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
