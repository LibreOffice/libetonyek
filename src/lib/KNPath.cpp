/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>
#include <utility>

#include "KNPath.h"

namespace libkeynote
{

namespace
{

class MoveTo : public KNPath::Element
{
public:
  explicit MoveTo(const KNPoint &point);

  virtual MoveTo *clone() const;

private:
  const KNPoint m_point;
};

MoveTo::MoveTo(const KNPoint &point)
  : m_point(point)
{
}

MoveTo *MoveTo::clone() const
{
  return new MoveTo(*this);
}

}

namespace
{

class LineTo : public KNPath::Element
{
public:
  explicit LineTo(const KNPoint &point);

  virtual LineTo *clone() const;

private:
  const KNPoint m_point;
};

LineTo::LineTo(const KNPoint &point)
  : m_point(point)
{
}

LineTo *LineTo::clone() const
{
  return new LineTo(*this);
}

}

namespace
{

class CurveTo : public KNPath::Element
{
public:
  CurveTo(const KNPoint &controlPoint1, const KNPoint &controlPoint2, const KNPoint &point);

  virtual CurveTo *clone() const;

private:
  const KNPoint m_controlPoint1;
  const KNPoint m_controlPoint2;
  const KNPoint m_point;
};

CurveTo::CurveTo(const KNPoint &controlPoint1, const KNPoint &controlPoint2, const KNPoint &point)
  : m_controlPoint1(controlPoint1)
  , m_controlPoint2(controlPoint2)
  , m_point(point)
{
}

CurveTo *CurveTo::clone() const
{
  return new CurveTo(*this);
}

}

namespace
{

class Close : public KNPath::Element
{
public:
  Close();

  virtual Close *clone() const;
};

Close::Close()
{
}

Close *Close::clone() const
{
  return new Close();
}

}

KNPath::Element::~Element()
{
}

KNPath::KNPath()
  : m_elements()
  , m_geometry()
{
}

KNPath::KNPath(const KNGeometry &geometry)
  : m_elements()
  , m_geometry(geometry)
{
}

KNPath::KNPath(const KNPath &other)
  : m_elements()
  , m_geometry(other.m_geometry)
{
  try
  {
    for (std::deque<Element *>::const_iterator it = other.m_elements.begin(); it != other.m_elements.end(); ++it)
      m_elements.push_back((*it)->clone());
  }
  catch (...)
  {
    clear();
    throw;
  }
}

KNPath::~KNPath()
{
  clear();
}

KNPath &KNPath::operator=(const KNPath &other)
{
  KNPath copy(other);
  swap(copy);
  return *this;
}

void KNPath::swap(KNPath &other)
{
  using std::swap;
  swap(m_elements, other.m_elements);
  swap(m_geometry, other.m_geometry);
}

void KNPath::clear()
{
  for (std::deque<Element *>::const_iterator it = m_elements.begin(); it != m_elements.end(); ++it)
    delete *it;
  m_elements.clear();
}

void KNPath::setGeometry(const KNGeometry &geometry)
{
  m_geometry = geometry;
}

void KNPath::appendMoveTo(const KNPoint &point)
{
  m_elements.push_back(new MoveTo(point));
}

void KNPath::appendLineTo(const KNPoint &point)
{
  m_elements.push_back(new LineTo(point));
}

void KNPath::appendCurveTo(const KNPoint &controlPoint1, const KNPoint &controlPoint2, const KNPoint &point)
{
  m_elements.push_back(new CurveTo(controlPoint1, controlPoint2, point));
}

void KNPath::appendClose()
{
  m_elements.push_back(new Close());
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
