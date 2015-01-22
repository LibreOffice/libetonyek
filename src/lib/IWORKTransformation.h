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

#include "libetonyek_utils.h"

namespace libetonyek
{

struct IWORKGeometry;

/** Represents an affine transformation.
  *
  * The transformation matrix looks like:
  * <pre>
  * xx yx 0
  * xy yy 0
  * x0 y0 1
  * </pre>
  */
class IWORKTransformation
{
public:
  /** Construct a unit transformation.
    */
  IWORKTransformation();

  /** Construct a transformation from matrix components.
    */
  IWORKTransformation(double xx, double yx, double xy, double yy, double x0, double y0);

  /** Apply a transformation on top of this one.
    *
    * The effect is as if transformation @tr was applied after @c this.
    *
    * @arg[in] tr the transformation to apply after this one
    * @result this transformation
    */
  IWORKTransformation &operator*=(const IWORKTransformation &tr);

  /** Apply transformation to a point or distance.
    *
    * @arg[inout] x x coordinate of the point or distance.
    * @arg[inout] y y coordinate of the point or distance.
    * @arg[in] distance if @c true, the transformed entity is a
    *   distance (in that case translation is ignored).
    */
  void operator()(double &x, double &y, bool distance = false) const;

  /** Tests for approximate equality of transformations.
    *
    * @arg[in] other the transformation for equality comparison
    * @arg[in] eps precision
    * @returns @c true if this transformation and @c other are equal, @c
    * false otherwise
    */
  bool approxEqual(const IWORKTransformation &other, double eps = ETONYEK_EPSILON) const;

private:
  // transformation matrix
  double m_xx;
  double m_yx;
  double m_xy;
  double m_yy;
  double m_x0;
  double m_y0;
};

/** Create a new transformation as a composition of two transformations.
  *
  * Application of the resulting transformation has the same effect as
  * if transformation @c left was applied before @c right.
  *
  * @return the new transformation
  */
IWORKTransformation operator*(const IWORKTransformation &left, const IWORKTransformation &right);

/** Tests for equality of transformations.
  *
  * @arg[in] left the first transformation
  * @arg[in] right the second transformation
  * @returns @c true if the transformations are equal, @c false if they are not
  */
bool operator==(const IWORKTransformation &left, const IWORKTransformation &right);

/** Tests for inequality of transformations.
  *
  * @arg[in] left the first transformation
  * @arg[in] right the second transformation
  * @returns @c true if the transformations are not equal, @c false if they are
  */
bool operator!=(const IWORKTransformation &left, const IWORKTransformation &right);

/** Create a transformation from a geometry.
  *
  * @arg[in] a geometry
  * @return the created transformation
  */
IWORKTransformation makeTransformation(const IWORKGeometry &geometry);

/// Special transformation constructors.
namespace transformations
{

/** Translate to the center of viewport.
  *
  * @arg[in] width width of the viewport.
  * @arg[in] height height of the viewport.
  * @return the transformation
  */
IWORKTransformation center(double width, double height);

/** Translate from the center of viewport to the origin.
  *
  * This is inverse operation to center().
  *
  * @arg[in] width width of the viewport.
  * @arg[in] height height of the viewport.
  * @return the transformation
  */
IWORKTransformation origin(double width, double height);

/** Flip horizontally and/or vertically.
  *
  * @arg[in] horizontal flip horizontally.
  * @arg[in] vertical flip vertically.
  * @return the transformation
  */
IWORKTransformation flip(bool horizontal, bool vertical);

/** Rotate counterclockwise around origin.
  *
  * @arg[in] angle rotation angle in radians. 0 is at the @c x axis on
  *   the right side.
  * @return the transformation
  */
IWORKTransformation rotate(double angle);

/** Scale along @c x and/or @c y axis.
  *
  * @arg[in] ratioX scale ratio along @c x axis.
  * @arg[in] ratioY scale ratio along @c y axis.
  * @return the transformation
  */
IWORKTransformation scale(double ratioX, double ratioY);

/** Shear along @c x and/or @c y axis.
  *
  * @arg[in] angleX shear angle along @c x axis.
  * @arg[in] angleY shear angle along @c y axis.
  * @return the transformation
  */
IWORKTransformation shear(double angleX, double angleY);

/** Translate along @c x and/or @c y axis.
  *
  * @arg[in] offsetX translation offset along @c x axis.
  * @arg[in] offsetY translation offset along @c y axis.
  * @return the transformation
  */
IWORKTransformation translate(double offsetX, double offsetY);

}

}

#endif // IWORKTRANSFORMATION_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
