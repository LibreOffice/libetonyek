/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKSHAPE_H_INCLUDED
#define IWORKSHAPE_H_INCLUDED

#include <boost/shared_ptr.hpp>

#include "IWORKObject.h"
#include "IWORKPath_fwd.h"
#include "IWORKStyle.h"
#include "IWORKText_fwd.h"

namespace libetonyek
{

struct IWORKSize;
class IWORKTransformation;

/** Representation of a shape.
 */
struct IWORKShape
{
  IWORKGeometryPtr_t m_geometry;
  IWORKStylePtr_t m_style;
  IWORKPathPtr_t m_path;
  IWORKTextPtr_t m_text;

  IWORKShape();
};

typedef boost::shared_ptr<IWORKShape> IWORKShapePtr_t;

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

#endif // IWORKSHAPE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
