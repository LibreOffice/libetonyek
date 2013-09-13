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

class KNTransformation
{
public:
  /** Construct a unit transformation.
    */
  KNTransformation();

  explicit KNTransformation(const KNGeometry &geometry);

  /** Construct a transformation from matrix components.
    */
  KNTransformation(double xx, double xy, double yx, double yy, double x1, double y1);

  /** Apply another transformation on top of this one.
    */
  KNTransformation &operator*=(const KNTransformation &tr);

private:
  // transformation matrix
  double m_xx;
  double m_xy;
  double m_x1;
  double m_yx;
  double m_yy;
  double m_y1;

  bool m_horizontalFlip;
  bool m_verticalFlip;
};

KNTransformation operator*(const KNTransformation &left, const KNTransformation right);

}

#endif // KNTRANSFORMATION_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
