/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKShape.h"

#include <algorithm>
#include <cmath>
#include <deque>

#include "IWORKPath.h"
#include "IWORKText.h"
#include "IWORKTransformation.h"
#include "IWORKTypes.h"

using std::deque;

namespace libetonyek
{

IWORKShape::IWORKShape()
  : geometry()
  , style()
  , path()
  , text()
{
}

namespace
{

class ShapeObject : public IWORKObject
{
public:
  ShapeObject(const IWORKShapePtr_t &shape, const IWORKTransformation &trafo);
  virtual ~ShapeObject();

private:
  virtual void draw(librevenge::RVNGPresentationInterface *painter);

private:
  const IWORKShapePtr_t m_shape;
  const IWORKTransformation m_trafo;
};

ShapeObject::ShapeObject(const IWORKShapePtr_t &shape, const IWORKTransformation &trafo)
  : m_shape(shape)
  , m_trafo(trafo)
{
}

ShapeObject::~ShapeObject()
{
}

void ShapeObject::draw(librevenge::RVNGPresentationInterface *const painter)
{
  if (bool(m_shape) && bool(m_shape->path))
  {
    // TODO: make style

    const IWORKPath path = *m_shape->path * m_trafo;

    librevenge::RVNGPropertyList props;
    props.insert("svg:d", path.toWPG());

    painter->setStyle(librevenge::RVNGPropertyList());
    painter->drawPath(props);

    if (bool(m_shape->text))
      makeObject(m_shape->text, m_trafo)->draw(painter);
  }
}

}

IWORKObjectPtr_t makeObject(const IWORKShapePtr_t &shape, const IWORKTransformation &trafo)
{
  const IWORKObjectPtr_t object(new ShapeObject(shape, trafo));
  return object;
}

namespace
{

struct Point
{
  double x;
  double y;

  Point();
  Point(double x_, double y_);
};

Point::Point()
  : x(0)
  , y(0)
{
}

Point::Point(const double x_, const double y_)
  : x(x_)
  , y(y_)
{
}

bool approxEqual(const Point &left, const Point &right, const double eps = ETONYEK_EPSILON)
{
  using libetonyek::approxEqual;
  return approxEqual(left.x, right.x, eps) && approxEqual(left.y, right.y, eps);
}

bool operator==(const Point &left, const Point &right)
{
  return approxEqual(left, right);
}

}

namespace
{

using namespace transformations;

deque<Point> rotatePoint(const Point &point, const unsigned n)
{
  deque<Point> points;


  const double angle = etonyek_two_pi / n;

  points.push_back(point);
  for (unsigned i = 1; i < n; ++i)
  {
    Point pt(point);
    const IWORKTransformation rot(rotate(i * angle));
    rot(pt.x, pt.y);
    points.push_back(pt);
  }

  return points;
}

deque<Point> drawArrowHalf(const double headWidth, const double stemThickness)
{
  // user space canvas: [0:1] x [0:1]

  deque<Point> points;
  points.push_back(Point(0, stemThickness));
  points.push_back(Point(1 - headWidth, stemThickness));
  points.push_back(Point(1 - headWidth, 1));
  points.push_back(Point(1, 0));

  return points;
}

IWORKPathPtr_t makePolyLine(const deque<Point> inputPoints, bool close = true)
{
  IWORKPathPtr_t path;

  // need at least 2 points to make a polyline
  if (inputPoints.size() < 2)
    return path;

  // remove multiple points
  deque<Point> points;
  std::unique_copy(inputPoints.begin(), inputPoints.end(), back_inserter(points));

  // close path if the first and last points are equal
  if (points.front() == points.back())
  {
    points.pop_back();
    close = true;
  }

  // ... but there must be at least 3 points to make a closed path
  if (points.size() < 3)
    close = false;

  // need at least 2 points to make a polyline
  if (points.size() < 2)
    return path;

  path.reset(new IWORKPath());

  deque<Point>::const_iterator it = points.begin();
  path->appendMoveTo(it->x, it->y);
  ++it;
  for (; it != points.end(); ++it)
    path->appendLineTo(it->x, it->y);

  if (close)
    path->appendClose();

  return path;
}

struct TransformPoint
{
  TransformPoint(const IWORKTransformation &tr)
    : m_tr(tr)
  {
  }

  void operator()(Point &point) const
  {
    m_tr(point.x, point.y);
  }

private:
  const IWORKTransformation &m_tr;
};

void transform(deque<Point> &points, const IWORKTransformation &tr)
{
  for_each(points.begin(), points.end(), TransformPoint(tr));
}

}

IWORKPathPtr_t makePolygonPath(const IWORKSize &size, const unsigned edges)
{
  // user space canvas: [-1:1] x [-1:1]

  deque<Point> points = rotatePoint(Point(0, -1), edges);

  // FIXME: the shape should probably be scaled to whole width/height.
  // Check.
  transform(points, translate(1, 1) * scale(0.5, 0.5) * scale(size.width, size.height));
  const IWORKPathPtr_t path = makePolyLine(points);

  return path;
}

IWORKPathPtr_t makeRoundedRectanglePath(const IWORKSize &size, const double radius)
{
  // user space canvas: [-1:1] x [-1:1]

  // TODO: draw rounded corners
  (void) radius;

  deque<Point> points = rotatePoint(Point(1, 1), 4);

  transform(points, translate(1, 1) * scale(0.5, 0.5) * scale(size.width, size.height));
  const IWORKPathPtr_t path = makePolyLine(points);

  return path;
}

IWORKPathPtr_t makeArrowPath(const IWORKSize &size, const double headWidth, const double stemThickness)
{
  deque<Point> points = drawArrowHalf(headWidth / size.width, 1 - 2 * stemThickness);

  // mirror around the x axis
  deque<Point> mirroredPoints = points;
  transform(mirroredPoints, flip(false, true));

  // join the two point sets
  copy(mirroredPoints.rbegin(), mirroredPoints.rend(), back_inserter(points));

  // transform and create path
  transform(points, translate(0, 1) * scale(1, 0.5) * scale(size.width, size.height));
  const IWORKPathPtr_t path = makePolyLine(points);
  return path;
}

IWORKPathPtr_t makeDoubleArrowPath(const IWORKSize &size, const double headWidth, const double stemThickness)
{
  deque<Point> points = drawArrowHalf(2 * headWidth / size.width, 1 - 2 * stemThickness);

  {
    // mirror around the y axis
    deque<Point> mirroredPoints = points;
    transform(mirroredPoints, flip(true, false));

    copy(mirroredPoints.begin(), mirroredPoints.end(), front_inserter(points));
  }

  {
    // mirror around the x axis
    deque<Point> mirroredPoints = points;
    transform(mirroredPoints, flip(false, true));

    copy(mirroredPoints.rbegin(), mirroredPoints.rend(), back_inserter(points));
  }

  transform(points, translate(1, 1) * scale(0.5, 0.5) * scale(size.width, size.height));
  const IWORKPathPtr_t path = makePolyLine(points);
  return path;
}

IWORKPathPtr_t makeStarPath(const IWORKSize &size, const unsigned points, const double innerRadius)
{
  // user space canvas: [-1:1] x [-1:1]

  // create outer points
  const deque<Point> outerPoints = rotatePoint(Point(0, -1), points);

  // create inner points
  const double angle = etonyek_two_pi / points;
  deque<Point> innerPoints(outerPoints);
  transform(innerPoints, rotate(angle / 2) * scale(innerRadius, innerRadius));

  // merge them together
  deque<Point> pathPoints;
  assert(outerPoints.size() == innerPoints.size());
  for (deque<Point>::const_iterator itO = outerPoints.begin(), itI = innerPoints.begin();
       (itO != outerPoints.end()) && (itI != innerPoints.end());
       ++itO, ++itI)
  {
    pathPoints.push_back(*itO);
    pathPoints.push_back(*itI);
  }

  // create the path
  transform(pathPoints, translate(1, 1) * scale(0.5, 0.5) * scale(size.width, size.height));
  const IWORKPathPtr_t path = makePolyLine(pathPoints);

  return path;
}

IWORKPathPtr_t makeConnectionPath(const IWORKSize &size, const double middleX, const double middleY)
{
  // TODO: implement me
  (void) size;
  (void) middleX;
  (void) middleY;
  return IWORKPathPtr_t();
}

IWORKPathPtr_t makeCalloutPath(const IWORKSize &size, const double radius, const double tailSize, const double tailX, const double tailY)
{
  // user space canvas: [-1:1] x [-1:1]

  // TODO: draw correctly instead of just approximating
  (void) radius;
  (void) tailSize;
  (void) tailX;
  (void) tailY;

  deque<Point> points = rotatePoint(Point(-1, -1), 4);
  points.push_back(Point(-1, 0.5));
  points.push_back(Point(-2, 0));
  points.push_back(Point(-1, -0.5));

  // create the path
  transform(points, translate(1, 1) * scale(0.5, 0.5) * scale(size.width, size.height));
  const IWORKPathPtr_t path = makePolyLine(points);

  return path;
}

IWORKPathPtr_t makeQuoteBubblePath(const IWORKSize &size, const double radius, const double tailSize, const double tailX, const double tailY)
{
  // TODO: really draw this instead of just approximating
  return makeCalloutPath(size, radius, tailSize, tailX, tailY);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
