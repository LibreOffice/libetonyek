/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNTRANSFORMATION_H_INCLUDED
#define KNTRANSFORMATION_H_INCLUDED

namespace libkeynote
{

struct KNGeometry;

/** Represents an affine transformation.
  *
  * The transformation matrix looks like:
  * <pre>
  * xx yx 0
  * xy yy 0
  * x0 y0 1
  * </pre>
  */
class KNTransformation
{
public:
  /** Construct a unit transformation.
    */
  KNTransformation();

  /** Construct a transformation from matrix components.
    */
  KNTransformation(double xx, double yx, double xy, double yy, double x0, double y0);

  /** Apply a transformation on top of this one.
    *
    * The effect is as if transformation @tr was applied after @c this.
    *
    * @arg[in] tr the transformation to apply after this one
    * @result this transformation
    */
  KNTransformation &operator*=(const KNTransformation &tr);

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
KNTransformation operator*(const KNTransformation &left, const KNTransformation right);

/** Create a transformation from a geometry.
  *
  * @arg[in] a geometry
  * @return the created transformation
  */
KNTransformation makeTransformation(const KNGeometry &geometry);

}

#endif // KNTRANSFORMATION_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
