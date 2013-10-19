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

#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/spirit/include/classic_core.hpp>

#include "libkeynote_utils.h"
#include "KEYPath.h"
#include "KEYTransformation.h"
#include "KEYTypes.h"

using boost::bind;
using boost::cref;

using std::string;

namespace libkeynote
{

/** An element of path.
  */
class KEYPath::Element
{
public:
  virtual ~Element() = 0;

  virtual Element *clone() const = 0;

  /** Test whether this element is equal to other.
    *
    * @note Because all implementations of this interface are directly
    * derived from it (i.e., thay have no subclasses), it is enough to
    * check if @c other has a compatible type, not that @c this and @c
    * other have the @em same type (which would require double
    * dispatch).
    *
    * @return true if @c this and @c other are equal.
    */
  virtual bool approxEqualsTo(const Element *other, double eps) const = 0;

  /** Transform this path element.
    *
    * @arg[in] tr the transformation
    */
  virtual void transform(const KEYTransformation &tr) = 0;

  /** Create WPG representation of this path element.
   */
  virtual WPXPropertyList toWPG() const = 0;
};

namespace
{

class MoveTo : public KEYPath::Element
{
public:
  MoveTo(double x, double y);

  virtual MoveTo *clone() const;

  virtual bool approxEqualsTo(const Element *other, double eps) const;

  virtual void transform(const KEYTransformation &tr);

  virtual WPXPropertyList toWPG() const;

private:
  double m_x;
  double m_y;
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

bool MoveTo::approxEqualsTo(const Element *other, const double eps) const
{
  const MoveTo *const that = dynamic_cast<const MoveTo *>(other);

  if (that)
    return approxEqual(m_x, that->m_x, eps) && approxEqual(m_y, that->m_y, eps);

  return false;
}

void MoveTo::transform(const KEYTransformation &tr)
{
  tr(m_x, m_y);
}

WPXPropertyList MoveTo::toWPG() const
{
  WPXPropertyList element;

  element.insert("libwpg:path-action", "M");
  element.insert("svg:x", pt2in(m_x));
  element.insert("svg:y", pt2in(m_y));

  return element;
}

}

namespace
{

class LineTo : public KEYPath::Element
{
public:
  LineTo(double x, double y);

  virtual LineTo *clone() const;

  virtual bool approxEqualsTo(const Element *other, double eps) const;

  virtual void transform(const KEYTransformation &tr);

  virtual WPXPropertyList toWPG() const;

private:
  double m_x;
  double m_y;
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

bool LineTo::approxEqualsTo(const Element *other, const double eps) const
{
  const LineTo *const that = dynamic_cast<const LineTo *>(other);

  if (that)
    return approxEqual(m_x, that->m_x, eps) && approxEqual(m_y, that->m_y, eps);

  return false;
}

void LineTo::transform(const KEYTransformation &tr)
{
  tr(m_x, m_y);
}

WPXPropertyList LineTo::toWPG() const
{
  WPXPropertyList element;

  element.insert("libwpg:path-action", "L");
  element.insert("svg:x", pt2in(m_x));
  element.insert("svg:y", pt2in(m_y));

  return element;
}

}

namespace
{

class CurveTo : public KEYPath::Element
{
public:
  CurveTo(double x1, double y1, double x2, double y2, double x, double y);

  virtual CurveTo *clone() const;

  virtual bool approxEqualsTo(const Element *other, double eps) const;

  virtual void transform(const KEYTransformation &tr);

  virtual WPXPropertyList toWPG() const;

private:
  double m_x1;
  double m_y1;
  double m_x2;
  double m_y2;
  double m_x;
  double m_y;
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

bool CurveTo::approxEqualsTo(const Element *other, const double eps) const
{
  const CurveTo *const that = dynamic_cast<const CurveTo *>(other);

  if (that)
    return approxEqual(m_x1, that->m_x1, eps) && approxEqual(m_y1, that->m_y1, eps)
           && approxEqual(m_x2, that->m_x2, eps) && approxEqual(m_y2, that->m_y2, eps)
           && approxEqual(m_x, that->m_x, eps) && approxEqual(m_y, that->m_y, eps)
           ;

  return false;
}

void CurveTo::transform(const KEYTransformation &tr)
{
  tr(m_x, m_y);
  tr(m_x1, m_y1);
  tr(m_x2, m_y2);
}

WPXPropertyList CurveTo::toWPG() const
{
  WPXPropertyList element;

  element.insert("libwpg:path-action", "C");
  element.insert("svg:x", pt2in(m_x));
  element.insert("svg:y", pt2in(m_y));
  element.insert("svg:x1", pt2in(m_x1));
  element.insert("svg:y1", pt2in(m_y1));
  element.insert("svg:x2", pt2in(m_x2));
  element.insert("svg:y2", pt2in(m_y2));

  return element;
}

}

namespace
{

class Close : public KEYPath::Element
{
public:
  Close();

  virtual Close *clone() const;

  virtual bool approxEqualsTo(const Element *other, double eps) const;

  virtual void transform(const KEYTransformation &tr);

  virtual WPXPropertyList toWPG() const;
};

Close::Close()
{
}

Close *Close::clone() const
{
  return new Close();
}

bool Close::approxEqualsTo(const Element *other, double) const
{
  return dynamic_cast<const Close *>(other);
}

void Close::transform(const KEYTransformation &)
{
}

WPXPropertyList Close::toWPG() const
{
  WPXPropertyList element;

  element.insert("libwpg:path-action", "Z");

  return element;
}

}

KEYPath::Element::~Element()
{
}

KEYPath::KEYPath()
  : m_elements()
{
}

KEYPath::KEYPath(const std::string &path)
  : m_elements()
{
  using namespace boost::spirit::classic;

  double x = 0;
  double y = 0;
  double x1 = 0;
  double y1 = 0;
  double x2 = 0;
  double y2 = 0;

  const rule<> r =
    +(
      (
        ('C' && space_p && real_p[assign_a(x)] && space_p && real_p[assign_a(y)] && space_p && real_p[assign_a(x1)] && space_p && real_p[assign_a(y1)] && space_p && real_p[assign_a(x2)] && space_p && real_p[assign_a(y2)])[bind(&KEYPath::appendCurveTo, this, cref(x), cref(y), cref(x1), cref(y1), cref(x2), cref(y2))]
        | ('L' && space_p && real_p[assign_a(x)] && space_p && real_p[assign_a(y)])[bind(&KEYPath::appendLineTo, this, cref(x), cref(y))]
        | ('M' && space_p && real_p[assign_a(x)] && space_p && real_p[assign_a(y)])[bind(&KEYPath::appendMoveTo, this, cref(x), cref(y))]
        | ch_p('Z')[bind(&KEYPath::appendClose, this)]
      )
      && *space_p
    )
    ;

  if (!parse(path.c_str(), r).full)
  {
    KEY_DEBUG_MSG(("parsing of path '%s' failed\n", path.c_str()));
    throw GenericException();
  }
}

KEYPath::KEYPath(const KEYPath &other)
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

KEYPath::~KEYPath()
{
  clear();
}

KEYPath &KEYPath::operator=(const KEYPath &other)
{
  KEYPath copy(other);
  swap(copy);
  return *this;
}

void KEYPath::swap(KEYPath &other)
{
  using std::swap;
  swap(m_elements, other.m_elements);
}

void KEYPath::clear()
{
  for (std::deque<Element *>::const_iterator it = m_elements.begin(); it != m_elements.end(); ++it)
    delete *it;
  m_elements.clear();
}

void KEYPath::appendMoveTo(const double x, const double y)
{
  m_elements.push_back(new MoveTo(x, y));
}

void KEYPath::appendLineTo(const double x, const double y)
{
  m_elements.push_back(new LineTo(x, y));
}

void KEYPath::appendCurveTo(const double x1, const double y1, const double x2, const double y2, const double x, const double y)
{
  m_elements.push_back(new CurveTo(x1, y1, x2, y2, x, y));
}

void KEYPath::appendClose()
{
  m_elements.push_back(new Close());
}

void KEYPath::operator*=(const KEYTransformation &tr)
{
  for_each(m_elements.begin(), m_elements.end(), bind(&Element::transform, _1, cref(tr)));
}

WPXPropertyListVector KEYPath::toWPG() const
{
  WPXPropertyListVector vec;

  for(std::deque<Element *>::const_iterator it = m_elements.begin(); m_elements.end() != it; ++it)
    vec.append((*it)->toWPG());

  return vec;
}

bool approxEqual(const KEYPath &left, const KEYPath &right, const double eps)
{
  return left.m_elements.size() == right.m_elements.size()
         && std::equal(left.m_elements.begin(), left.m_elements.end(), right.m_elements.begin(),
                       boost::bind(&KEYPath::Element::approxEqualsTo, _1, _2, eps))
         ;
}

bool operator==(const KEYPath &left, const KEYPath &right)
{
  return approxEqual(left, right);
}

bool operator!=(const KEYPath &left, const KEYPath &right)
{
  return !(left == right);
}

KEYPath operator*(const KEYPath &path, const KEYTransformation &tr)
{
  KEYPath newPath(path);
  newPath *= tr;
  return newPath;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
