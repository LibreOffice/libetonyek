/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cmath>
#include <deque>

#include <boost/bind.hpp>

#include "KNPath.h"
#include "KNShape.h"
#include "KNTransformation.h"
#include "KNTypes.h"

using std::deque;

namespace libkeynote
{

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

bool operator==(const Point &left, const Point &right)
{
  return (left.x == right.x) && (left.y == right.y);
}

bool operator!=(const Point &left, const Point &right)
{
  return !(left == right);
}

}

namespace
{

using namespace transformations;

deque<Point> rotatePoint(const Point &point, const unsigned n)
{
  deque<Point> points;


  const double angle = 2 * M_PI / n;

  points.push_back(point);
  for (unsigned i = 1; i < n; ++i)
  {
    Point pt(point);
    const KNTransformation rot(rotate(i * angle));
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

KNPathPtr_t makePolyLine(const deque<Point> inputPoints, bool close = true)
{
  KNPathPtr_t path;

// need at least 2 points to make a polyline
  if (inputPoints.size() < 2)
    return path;

  // remove multiple points
  deque<Point> points;
  {
    deque<Point>::const_iterator first = inputPoints.begin();
    deque<Point>::const_iterator last = first;
    do
    {
      last = adjacent_find(first, inputPoints.end());
      copy(first, last, back_inserter(points));
      // skip the range of adjacent, equal points
      first = adjacent_find(last, inputPoints.end(), boost::bind(operator!=, _1, _2));
      if (inputPoints.end() != first)
        ++first;
    }
    while (first != inputPoints.end());
  }

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
  TransformPoint(const KNTransformation &tr)
    : m_tr(tr)
  {
  }

  void operator()(Point &point) const
  {
    m_tr(point.x, point.y);
  }

private:
  const KNTransformation &m_tr;
};

void transform(deque<Point> &points, const KNTransformation &tr)
{
  for_each(points.begin(), points.end(), TransformPoint(tr));
}

}

KNObjectPtr_t makeObject(const KNShapePtr_t &shape)
{
  // TODO: implement me
  (void) shape;
  return KNObjectPtr_t();
}

KNPathPtr_t makePolygonPath(const KNSize &size, const unsigned edges)
{
  // user space canvas: [-1:1] x [-1:1]

  deque<Point> points = rotatePoint(Point(0, -1), edges);

  transform(points, translate(0, 1) * scale(0.5, 0.5) * scale(size.width, size.height));
  const KNPathPtr_t path = makePolyLine(points);

  return path;
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
  deque<Point> points = drawArrowHalf(headWidth, stemThickness);

  // mirror around the x axis
  deque<Point> mirroredPoints = points;
  transform(mirroredPoints, flip(false, true));

  // join the two point sets
  copy(mirroredPoints.rbegin(), mirroredPoints.rend(), back_inserter(points));

  // transform and create path
  transform(points, translate(0, 1) * scale(0, 0.5) * scale(size.width, size.height));
  const KNPathPtr_t path = makePolyLine(points);
  return path;
}

KNPathPtr_t makeDoubleArrowPath(const KNSize &size, const double headWidth, const double stemThickness)
{
  deque<Point> points = drawArrowHalf(2 * headWidth, stemThickness);

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
  const KNPathPtr_t path = makePolyLine(points);
  return path;
}

KNPathPtr_t makeStarPath(const KNSize &size, const unsigned points, const double innerRadius)
{
  // user space canvas: [-1:1] x [-1:1]

  // create outer points
  const deque<Point> outerPoints = rotatePoint(Point(0, -1), points);

  // create inner points
  const double angle = 2 * M_PI / points;
  deque<Point> innerPoints(points);
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
  transform(pathPoints, translate(0, 1) * scale(0.5, 0.5) * scale(size.width, size.height));
  const KNPathPtr_t path = makePolyLine(pathPoints);

  return path;
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
