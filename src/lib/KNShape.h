/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNSHAPE_H_INCLUDED
#define KNSHAPE_H_INCLUDED

#include <boost/shared_ptr.hpp>

#include "KNPath_fwd.h"
#include "KNStyle.h"
#include "KNText_fwd.h"

namespace libkeynote
{

struct KNSize;

/** Representation of a shape.
 */
struct KNShape
{
  KNGeometryPtr_t geometry;
  KNStylePtr_t style;
  KNPathPtr_t path;
  KNTextPtr_t text;

  KNShape();
};

typedef boost::shared_ptr<KNShape> KNShapePtr_t;

/** Object creator function for shape.
 */
KNObjectPtr_t makeObject(const KNShapePtr_t &shape);

/** Path creator functions for stock shapes.
 */
KNPathPtr_t makePolygonPath(const KNSize &size, unsigned edges);
KNPathPtr_t makeRoundedRectanglePath(const KNSize &size, double radius);

KNPathPtr_t makeArrowPath(const KNSize &size, double headWidth, double stemThickness);
KNPathPtr_t makeDoubleArrowPath(const KNSize &size, double headWidth, double stemThickness);
KNPathPtr_t makeStarPath(const KNSize &size, unsigned points, double innerRadius);
KNPathPtr_t makeConnectionPath(const KNSize &size, double middleX, double middleY);

KNPathPtr_t makeCalloutPath(const KNSize &size, double radius, double tailSize, double tailX, double tailY);
KNPathPtr_t makeQuoteBubblePath(const KNSize &size, double radius, double tailSize, double tailX, double tailY);

}

#endif // KNSHAPE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
