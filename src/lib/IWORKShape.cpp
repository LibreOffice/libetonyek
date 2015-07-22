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

#include <glm/glm.hpp>

#include "IWORKDocumentInterface.h"
#include "IWORKPath.h"
#include "IWORKText.h"
#include "IWORKTypes.h"
#include "IWORKTransformation.h"

using std::deque;

namespace libetonyek
{

IWORKShape::IWORKShape()
  : m_geometry()
  , m_style()
  , m_path()
  , m_text()
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
  virtual void draw(IWORKDocumentInterface *document);

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

void ShapeObject::draw(IWORKDocumentInterface *const document)
{
  if (bool(m_shape) && bool(m_shape->m_path))
  {
    // TODO: make style

    const IWORKPath path = *m_shape->m_path * m_trafo;

    librevenge::RVNGPropertyList props;
    props.insert("svg:d", path.toWPG());

    document->setStyle(librevenge::RVNGPropertyList());
    document->drawPath(props);

    if (bool(m_shape->m_text))
      makeObject(m_shape->m_text, m_trafo)->draw(document);
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
    const glm::dmat3 rot(rotate(i * angle));
    glm::dvec3 vec = rot * glm::dvec3(pt.x, pt.y, 1);
    pt.x = vec[0];
    pt.y = vec[1];
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
  TransformPoint(const glm::dmat3 &tr)
    : m_tr(tr)
  {
  }

  void operator()(Point &point) const
  {
    glm::dvec3 vec = m_tr * glm::dvec3(point.x, point.y, 1);
    point.x = vec[0];
    point.y = vec[1];
  }

private:
  const glm::dmat3 &m_tr;
};

void transform(deque<Point> &points, const glm::dmat3 &tr)
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
  transform(points, scale(size.m_width, size.m_height) * scale(0.5, 0.5) * translate(1, 1));
  const IWORKPathPtr_t path = makePolyLine(points);

  return path;
}

IWORKPathPtr_t makeRoundedRectanglePath(const IWORKSize &size, const double radius)
{
  // user space canvas: [-1:1] x [-1:1]

  // TODO: draw rounded corners
  const double angle = etonyek_two_pi / 8;
  Point p;
  points.push_back(Point(-1,-1));
  for (unsigned i = 1; i <4; ++i)
  {
    Point pt(point);
    const IWORKTransformation rot(rotate(i * angle));
    
    
    switch(i){
      case 1:
      points.push(Point(pt.x +1,pt.y));
      break;
      case 2:
      points.push(Point(pt.x+1,pt.y+1));
      break;
      case 3:
      points.push(Point(pt.x,pt.y+1));
      break;
      default:
      }
      rot(pt.x, pt.y);
    points.push_back(pt);
  }
  //(void) radius;

  //deque<Point> points = rotatePoint(Point(1, 1), 4);

  transform(points, scale(size.m_width, size.m_height) * scale(0.5, 0.5) * translate(1, 1));
  const IWORKPathPtr_t path = makePolyLine(points);

  return path;
}

IWORKPathPtr_t makeArrowPath(const IWORKSize &size, const double headWidth, const double stemThickness)
{
  deque<Point> points = drawArrowHalf(headWidth / size.m_width, 1 - 2 * stemThickness);

  // mirror around the x axis
  deque<Point> mirroredPoints = points;
  transform(mirroredPoints, flip(false, true));

  // join the two point sets
  copy(mirroredPoints.rbegin(), mirroredPoints.rend(), back_inserter(points));

  // transform and create path
  transform(points, scale(size.m_width, size.m_height) * scale(1, 0.5) * translate(0, 1));
  const IWORKPathPtr_t path = makePolyLine(points);
  return path;
}

IWORKPathPtr_t makeDoubleArrowPath(const IWORKSize &size, const double headWidth, const double stemThickness)
{
  deque<Point> points = drawArrowHalf(2 * headWidth / size.m_width, 1 - 2 * stemThickness);

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

  transform(points, scale(size.m_width, size.m_height) * scale(0.5, 0.5) * translate(1, 1));
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
  transform(innerPoints, scale(innerRadius, innerRadius) * rotate(angle / 2));

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
  transform(pathPoints, scale(size.m_width, size.m_height) * scale(0.5, 0.5) * translate(1, 1));
  const IWORKPathPtr_t path = makePolyLine(pathPoints);

  return path;
}

//Added Support for some new shapes in Keynote 6.2

//Draw Circle
IWORKPathPtr_t makeCircle(const IWORKSize &size,const double centreX,const double centreY, const double radius)
{
  
  const MAX_EDGES = 5000; 
  const deque<Point> points = rotatePoint(Point(0 + radius,0),MAX_EDGES);
  transform(points,translate(centreX,centreY)*translate(1,1)*scale(0.5,0.5)*scale(size.m_width,size.m_height));
  const IWORKPathPtr_t path = makePolyLine(points);

  return path;
}

IWORKPathPtr_t makeConnectionPath(const IWORKSize &size, const double middleX, const double middleY)
{
  
  deque<Point> points;
  points.push_back(Point(-1,-1));
  points.push_back(Point(middleX,middleY));
  points.push_back(Point(1,1));
  transform(points,translate(centreX,centreY)*translate(1,1)*scale(0.5,0.5)*scale(size.m_width,size.m_height));
  const IWORKPathPtr_t path = makePolyLine(points);
  return path;
}

IWORKPathPtr_t makeCalloutPath(const IWORKSize &size, const double radius, const double tailSize, const double tailX, const double tailY)
{
  // user space canvas: [-1:1] x [-1:1]

  deque<Point> points;
  const double angle = etonyek_two_pi / 8;
  Point p;
  points.push_back(Point(-1,-1));
  for (unsigned i = 1; i <4; ++i)
  {
    Point pt(point);
    const IWORKTransformation rot(rotate(i * angle));
    switch(i){
      case 1:
      points.push(Point(pt.x +1,pt.y));
      break;
      case 2:
      points.push(Point(pt.x+1,pt.y+1));
      break;
      case 3:
      points.push(Point(pt.x,pt.y+1));
      break;
      default:
      break;
      }
      rot(pt.x, pt.y);
    points.push_back(pt);
  }
  points.push_back(Point(-1, 0.5));
  points.push_back(Point(-2, 0));
  points.push_back(Point(-1, -0.5));


  // create the path
  transform(points, scale(size.m_width, size.m_height) * scale(0.5, 0.5) * translate(1, 1));
  const IWORKPathPtr_t path = makePolyLine(points);

  return path;
}

IWORKPathPtr_t makeQuoteBubblePath(const IWORKSize &size, const double radius, const double tailSize, const double tailX, const double tailY)
{
      const MAX_EDGES = 5000; 
      deque<Point> points = rotatePoint(Point(0 + radius,0),MAX_EDGES);
      transform(points,scale(0.7,1));
      deque<Point> inner_points(points);
      transform(inner_points,scale(0.5,0.5));
      inner_points.push_back(Point(tailX,tailY));
      transform(inner_points, translate(1, 1) * scale(0.5, 0.5) * scale(size.m_width, size.m_height));
      const IWORKPathPtr_t path = makePolyLine(inner_points);

}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
