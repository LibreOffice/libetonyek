/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYSHAPE_H_INCLUDED
#define KEYSHAPE_H_INCLUDED

#include <boost/shared_ptr.hpp>

#include "KEYPath_fwd.h"
#include "KEYStyle.h"
#include "KEYText_fwd.h"

namespace libetonyek
{

struct KEYSize;

/** Representation of a shape.
 */
struct KEYShape
{
  KEYGeometryPtr_t geometry;
  KEYStylePtr_t style;
  KEYPathPtr_t path;
  KEYTextPtr_t text;

  KEYShape();
};

typedef boost::shared_ptr<KEYShape> KEYShapePtr_t;

/** Object creator function for shape.
 */
KEYObjectPtr_t makeObject(const KEYShapePtr_t &shape);

/** Path creator functions for stock shapes.
 */
KEYPathPtr_t makePolygonPath(const KEYSize &size, unsigned edges);
KEYPathPtr_t makeRoundedRectanglePath(const KEYSize &size, double radius);

KEYPathPtr_t makeArrowPath(const KEYSize &size, double headWidth, double stemThickness);
KEYPathPtr_t makeDoubleArrowPath(const KEYSize &size, double headWidth, double stemThickness);
KEYPathPtr_t makeStarPath(const KEYSize &size, unsigned points, double innerRadius);
KEYPathPtr_t makeConnectionPath(const KEYSize &size, double middleX, double middleY);

KEYPathPtr_t makeCalloutPath(const KEYSize &size, double radius, double tailSize, double tailX, double tailY);
KEYPathPtr_t makeQuoteBubblePath(const KEYSize &size, double radius, double tailSize, double tailX, double tailY);

}

#endif // KEYSHAPE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
