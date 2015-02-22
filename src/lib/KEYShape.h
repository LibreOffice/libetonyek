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

#include "IWORKPath_fwd.h"
#include "KEYObject.h"
#include "KEYStyle.h"
#include "KEYText_fwd.h"

namespace libetonyek
{

struct IWORKSize;
class IWORKTransformation;

/** Representation of a shape.
 */
struct KEYShape
{
  IWORKGeometryPtr_t geometry;
  KEYStylePtr_t style;
  IWORKPathPtr_t path;
  KEYTextPtr_t text;

  KEYShape();
};

typedef boost::shared_ptr<KEYShape> KEYShapePtr_t;

/** Object creator function for shape.
 */
KEYObjectPtr_t makeObject(const KEYShapePtr_t &shape, const IWORKTransformation &trafo);

/** Path creator functions for stock shapes.
 */
IWORKPathPtr_t makePolygonPath(const IWORKSize &size, unsigned edges);
IWORKPathPtr_t makeRoundedRectanglePath(const IWORKSize &size, double radius);

IWORKPathPtr_t makeArrowPath(const IWORKSize &size, double headWidth, double stemThickness);
IWORKPathPtr_t makeDoubleArrowPath(const IWORKSize &size, double headWidth, double stemThickness);
IWORKPathPtr_t makeStarPath(const IWORKSize &size, unsigned points, double innerRadius);
IWORKPathPtr_t makeConnectionPath(const IWORKSize &size, double middleX, double middleY);

IWORKPathPtr_t makeCalloutPath(const IWORKSize &size, double radius, double tailSize, double tailX, double tailY);
IWORKPathPtr_t makeQuoteBubblePath(const IWORKSize &size, double radius, double tailSize, double tailX, double tailY);

}

#endif // KEYSHAPE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
