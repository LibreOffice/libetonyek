/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KNPath.h"
#include "KNShape.h"
#include "KNTypes.h"

namespace libkeynote
{

KNPathPtr_t makePolygonPath(const KNSize &size, const unsigned edges)
{
  // TODO: implement me
  (void) size;
  (void) edges;
  return KNPathPtr_t();
}

KNObjectPtr_t makeObject(const KNShapePtr_t &shape)
{
  // TODO: implement me
  (void) shape;
  return KNObjectPtr_t();
}

KNPathPtr_t makeRoundedRectanglePath(const KNSize &size, const double radius)
{
  // TODO: implement me
  (void) size;
  (void) radius;
  return KNPathPtr_t();
}

KNPathPtr_t makeArrowPath(const KNSize &size, const double headWidth, const double stemThickness)
{
  // TODO: implement me
  (void) size;
  (void) headWidth;
  (void) stemThickness;
  return KNPathPtr_t();
}

KNPathPtr_t makeDoubleArrowPath(const KNSize &size, const double headWidth, const double stemThickness)
{
  // TODO: implement me
  (void) size;
  (void) headWidth;
  (void) stemThickness;
  return KNPathPtr_t();
}

KNPathPtr_t makeStarPath(const KNSize &size, const unsigned points, const double innerRadius)
{
  // TODO: implement me
  (void) size;
  (void) points;
  (void) innerRadius;
  return KNPathPtr_t();
}

KNPathPtr_t makeConnectionPath(const KNSize &size, const double middleX, const double middleY)
{
  // TODO: implement me
  (void) size;
  (void) middleX;
  (void) middleY;
  return KNPathPtr_t();
}

KNPathPtr_t makeCalloutPath(const KNSize &size, const double radius, const double tailSize, const double tailX, const double tailY)
{
  // TODO: implement me
  (void) size;
  (void) radius;
  (void) tailSize;
  (void) tailX;
  (void) tailY;
  return KNPathPtr_t();
}

KNPathPtr_t makeQuoteBubblePath(const KNSize &size, const double radius, const double tailSize, const double tailX, const double tailY)
{
  // TODO: implement me
  (void) size;
  (void) radius;
  (void) tailSize;
  (void) tailX;
  (void) tailY;
  return KNPathPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
