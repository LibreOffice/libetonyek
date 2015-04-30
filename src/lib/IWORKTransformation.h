/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTRANSFORMATION_H_INCLUDED
#define IWORKTRANSFORMATION_H_INCLUDED

#include <cassert>

#include <glm/glm.hpp>

#include "libetonyek_utils.h"

namespace libetonyek
{

struct IWORKGeometry;

/** Create a transformation from a geometry.
  *
  * @arg[in] a geometry
  * @return the created transformation
  */
glm::dmat3 makeTransformation(const IWORKGeometry &geometry);

template<class T>
bool approxEqual(const T &left, const T &right, const double eps = ETONYEK_EPSILON)
{
  assert(left.length() == right.length());

  for (int i = 0; i != left.length(); ++i)
  {
    if (!approxEqual(left[i], right[i], eps))
      return false;
  }
  return true;
}

/// Special transformation constructors.
namespace transformations
{

/** Translate to the center of viewport.
  *
  * @arg[in] width width of the viewport.
  * @arg[in] height height of the viewport.
  * @return the transformation
  */
glm::dmat3 center(double width, double height);

/** Translate from the center of viewport to the origin.
  *
  * This is inverse operation to center().
  *
  * @arg[in] width width of the viewport.
  * @arg[in] height height of the viewport.
  * @return the transformation
  */
glm::dmat3 origin(double width, double height);

/** Flip horizontally and/or vertically.
  *
  * @arg[in] horizontal flip horizontally.
  * @arg[in] vertical flip vertically.
  * @return the transformation
  */
glm::dmat3 flip(bool horizontal, bool vertical);

/** Rotate counterclockwise around origin.
  *
  * @arg[in] angle rotation angle in radians. 0 is at the @c x axis on
  *   the right side.
  * @return the transformation
  */
glm::dmat3 rotate(double angle);

/** Scale along @c x and/or @c y axis.
  *
  * @arg[in] ratioX scale ratio along @c x axis.
  * @arg[in] ratioY scale ratio along @c y axis.
  * @return the transformation
  */
glm::dmat3 scale(double ratioX, double ratioY);

/** Shear along @c x and/or @c y axis.
  *
  * @arg[in] angleX shear angle along @c x axis.
  * @arg[in] angleY shear angle along @c y axis.
  * @return the transformation
  */
glm::dmat3 shear(double angleX, double angleY);

/** Translate along @c x and/or @c y axis.
  *
  * @arg[in] offsetX translation offset along @c x axis.
  * @arg[in] offsetY translation offset along @c y axis.
  * @return the transformation
  */
glm::dmat3 translate(double offsetX, double offsetY);

}

}

#endif // IWORKTRANSFORMATION_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
