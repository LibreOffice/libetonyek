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
#include <memory>

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
  , m_order()
  , m_resizeFlags()
  , m_path()
  , m_text()
  , m_locked(false)
{
}

namespace
{

struct Point
{
  double x;
  double y;

  Point(double x_, double y_);
};

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

  path = std::make_shared<IWORKPath>();

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
  explicit TransformPoint(const glm::dmat3 &tr)
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
  if (radius<=0)
  {
    // user space canvas: [-1:1] x [-1:1]
    deque<Point> points= rotatePoint(Point(1, 1), 4);

    transform(points, scale(size.m_width, size.m_height) * scale(0.5, 0.5) * translate(1, 1));
    const IWORKPathPtr_t path = makePolyLine(points);

    return path;
  }
  double wRadius=2*radius<size.m_width ? radius : size.m_width/2;
  double hRadius=2*radius<size.m_height ? radius : size.m_height/2;
  IWORKPathPtr_t path(new IWORKPath);
  path->appendMoveTo(size.m_width-wRadius,0);
  path->appendQCurveTo(size.m_width,0, size.m_width,hRadius);
  path->appendLineTo(size.m_width,size.m_height-hRadius);
  path->appendQCurveTo(size.m_width,size.m_height, size.m_width-wRadius,size.m_height);
  path->appendLineTo(wRadius,size.m_height);
  path->appendQCurveTo(0,size.m_height, 0,size.m_height-hRadius);
  path->appendLineTo(0,hRadius);
  path->appendQCurveTo(0,0, wRadius,0);
  path->appendClose();
  return path;
}

IWORKPathPtr_t makeArrowPath(const IWORKSize &size, const double headWidth, const double stemThickness)
{
  deque<Point> points = drawArrowHalf(size.m_width > 0 ? headWidth / size.m_width : 1., 1 - 2 * stemThickness);

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
  deque<Point> points = drawArrowHalf(size.m_width > 0 ? 2 * headWidth / size.m_width : 1., 1 - 2 * stemThickness);

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

IWORKPathPtr_t makeCalloutPath(const IWORKSize &size, const double radius, const double tailSize, const double tailX, const double tailY)
{
  double wRadius=2*radius<size.m_width ? radius : size.m_width/2;
  double hRadius=2*radius<size.m_height ? radius : size.m_height/2;
  double tail[2]= {tailX-size.m_width/2, tailY-size.m_height/2};
  double xPos[4]= {-size.m_width/2,-size.m_width/2+wRadius,size.m_width/2-wRadius,size.m_width/2};
  double yPos[4]= {-size.m_height/2,-size.m_height/2+hRadius,size.m_height/2-hRadius,size.m_height/2};
  const double tSize=tailSize<0 ? -tailSize : tailSize;
  glm::dmat3 trans=translate(size.m_width/2, size.m_height/2);
  // reorient figure so that the tail is in RB
  trans=trans*scale(tail[0]<0 ? -1 : 1,tail[1]<0 ? -1 : 1);
  if (tail[0]<0) tail[0]*=-1;
  if (tail[1]<0) tail[1]*=-1;
  // first check if tail is in the round rect
  if (tailX>=0 && tailX<=size.m_width && tailY>=0 && tailY<=size.m_height &&
      (tail[0]-xPos[2])*(tail[0]-xPos[2])*hRadius*hRadius+
      (tail[1]-yPos[2])*(tail[1]-yPos[2])*wRadius*wRadius
      <= wRadius*wRadius*hRadius*hRadius)
    return makeRoundedRectanglePath(size,radius);
  if (tail[0]*yPos[3]<tail[1]*xPos[3])
  {
    using std::swap;
    swap(tail[0],tail[1]);
    swap(wRadius,hRadius);
    for (int i=0; i<4; ++i) swap(xPos[i], yPos[i]);
    trans=trans*glm::dmat3(0, 1, 0, 1, 0, 0, 0, 0, 1);
  }
  deque<Point> points;
  std::vector<char> orders;
  orders.push_back('M');
  points.push_back(Point(xPos[1],yPos[3]));
  orders.push_back('Q');
  points.push_back(Point(xPos[0],yPos[3]));
  points.push_back(Point(xPos[0],yPos[2]));
  orders.push_back('L');
  points.push_back(Point(xPos[0],yPos[1]));
  orders.push_back('Q');
  points.push_back(Point(xPos[0],yPos[0]));
  points.push_back(Point(xPos[1],yPos[0]));
  orders.push_back('L');
  points.push_back(Point(xPos[2],yPos[0]));
  orders.push_back('Q');
  points.push_back(Point(xPos[3],yPos[0]));
  points.push_back(Point(xPos[3],yPos[1]));

  // ok first compute the intersection of OT with the rectangle side x=xPos[3]
  double y1=tail[0]>0 ? tail[1]*xPos[3]/tail[0] : 0;
  // go to the y1-tSize
  if (y1-tSize <= yPos[1]) // ok
    ;
  else if (y1-tSize <= yPos[2])
  {
    orders.push_back('L');
    points.push_back(Point(xPos[3],y1-tSize));
  }
  else
  {
    orders.push_back('L');
    points.push_back(Point(xPos[3],yPos[2]));

    double delta[2]= {wRadius,y1-tSize-yPos[2]};
    double alpha=std::atan2(wRadius*delta[1],hRadius*delta[0]);
    orders.push_back('Q');
    points.push_back(Point(xPos[3],yPos[2]+hRadius*std::tan(alpha/2)));
    points.push_back(Point(xPos[2]+wRadius*std::cos(alpha),yPos[2]+hRadius*std::sin(alpha)));
  }
  // the tail
  orders.push_back('L');
  points.push_back(Point(tail[0],tail[1]));
  // after the tail
  if (y1+tSize <= yPos[2])
  {
    orders.push_back('L');
    points.push_back(Point(xPos[3],y1+tSize));
    orders.push_back('L');
    points.push_back(Point(xPos[3],yPos[2]));
    orders.push_back('Q');
    points.push_back(Point(xPos[3],yPos[3]));
    points.push_back(Point(xPos[2],yPos[3]));
  }
  else if (y1+tSize < yPos[3])
  {
    double delta[2]= {wRadius,y1+tSize-yPos[2]};
    double alpha=std::atan2(wRadius*delta[1],hRadius*delta[0]);
    // go back to circle
    orders.push_back('L');
    points.push_back(Point(xPos[2]+wRadius*std::cos(alpha),yPos[2]+hRadius*std::sin(alpha)));
    // end circle
    orders.push_back('Q');
    points.push_back(Point(xPos[2]+wRadius*std::tan((1.5707963268-alpha)/2),yPos[3]));
    points.push_back(Point(xPos[2],yPos[3]));
  }
  else if (xPos[2]-(y1+tSize-yPos[3])>xPos[1])
  {
    // clearly to small but...
    points.push_back(Point(xPos[2]-(y1+tSize-yPos[3]),yPos[3]));
    orders.push_back('L');
  }
  transform(points, trans);
  IWORKPathPtr_t path(new IWORKPath);
  size_t numPoints=points.size();
  for (size_t i=0, pPos=0; i<orders.size(); ++i)
  {
    switch (orders[i])
    {
    case 'M':
      if (pPos>=numPoints)
      {
        ETONYEK_DEBUG_MSG(("makeCalloutPath[IWORKShape]: can not find a point\n"));
        return IWORKPathPtr_t();
      }
      path->appendMoveTo(points[pPos].x, points[pPos].y);
      ++pPos;
      break;
    case 'L':
      if (pPos>=numPoints)
      {
        ETONYEK_DEBUG_MSG(("makeCalloutPath[IWORKShape]: can not find a point\n"));
        return IWORKPathPtr_t();
      }
      path->appendLineTo(points[pPos].x, points[pPos].y);
      ++pPos;
      break;
    case 'Q':
      if (pPos+1>=numPoints)
      {
        ETONYEK_DEBUG_MSG(("makeCalloutPath[IWORKShape]: can not find a point\n"));
        return IWORKPathPtr_t();
      }
      path->appendQCurveTo(points[pPos].x, points[pPos].y, points[pPos+1].x, points[pPos+1].y);
      pPos+=2;
      break;
    default:
      ETONYEK_DEBUG_MSG(("makeCalloutPath[IWORKShape]: unknown order %c\n", orders[i]));
      return IWORKPathPtr_t();
    }
  }
  path->appendClose();

  return path;
}

IWORKPathPtr_t makeQuoteBubblePath(const IWORKSize &size, const double radius, const double tailSize, const double tailX, const double tailY)
{
  // TODO: really draw this instead of just approximating
  return makeCalloutPath(size, radius, tailSize, tailX, tailY);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
