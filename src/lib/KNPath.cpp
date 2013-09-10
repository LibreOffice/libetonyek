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
#include "KNTypes.h"

namespace libkeynote
{

namespace
{

class MoveTo : public KNPath::Element
{
public:
  MoveTo(double x, double y);

  virtual MoveTo *clone() const;

private:
  const double m_x;
  const double m_y;
};

MoveTo::MoveTo(double x, double y)
  : m_x(x)
  , m_y(y)
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
  LineTo(double x, double y);

  virtual LineTo *clone() const;

private:
  const double m_x;
  const double m_y;
};

LineTo::LineTo(const double x, const double y)
  : m_x(x)
  , m_y(y)
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
  CurveTo(double x1, double y1, double x2, double y2, double x, double y);

  virtual CurveTo *clone() const;

private:
  const double m_x1;
  const double m_y1;
  const double m_x2;
  const double m_y2;
  const double m_x;
  const double m_y;
};

CurveTo::CurveTo(const double x1, const double y1, const double x2, const double y2, const double x, const double y)
  : m_x1(x1)
  , m_y1(y1)
  , m_x2(x2)
  , m_y2(y2)
  , m_x(x)
  , m_y(y)
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
{
}

KNPath::KNPath(const KNPath &other)
  : m_elements()
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
}

void KNPath::clear()
{
  for (std::deque<Element *>::const_iterator it = m_elements.begin(); it != m_elements.end(); ++it)
    delete *it;
  m_elements.clear();
}

void KNPath::appendMoveTo(const double x, const double y)
{
  m_elements.push_back(new MoveTo(x, y));
}

void KNPath::appendLineTo(const double x, const double y)
{
  m_elements.push_back(new LineTo(x, y));
}

void KNPath::appendCurveTo(const double x1, const double y1, const double x2, const double y2, const double x, const double y)
{
  m_elements.push_back(new CurveTo(x1, y1, x2, y2, x, y));
}

void KNPath::appendClose()
{
  m_elements.push_back(new Close());
}

namespace
{

class PathObject : public KNObject
{
public:
  explicit PathObject(const KNPathPtr_t &path);

private:
  virtual void draw(libwpg::WPGPaintInterface *painter, const KNTransformation &tr);

private:
  const KNPathPtr_t m_path;
};

PathObject::PathObject(const KNPathPtr_t &path)
  : m_path(path)
{
}

void PathObject::draw(libwpg::WPGPaintInterface *const painter, const KNTransformation &tr)
{
  // TODO: implement me
  (void) painter;
  (void) tr;
}

}

KNObjectPtr_t makeObject(const KNPathPtr_t &path)
{
  const KNObjectPtr_t object(new PathObject(path));
  return object;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
