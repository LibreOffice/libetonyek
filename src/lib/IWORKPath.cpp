/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKPath.h"

#include <cassert>
#include <deque>
#include <sstream>
#include <utility>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>

#include "libetonyek_utils.h"
#include "IWORKTypes.h"

namespace phoenix = boost::phoenix;

using boost::apply_visitor;
using boost::static_visitor;

using librevenge::RVNGPropertyList;
using librevenge::RVNGPropertyListVector;

using std::string;

namespace libetonyek
{

struct MoveTo
{
  double m_x;
  double m_y;
};

struct LineTo
{
  double m_x;
  double m_y;
};

struct CCurveTo
{
  double m_x1;
  double m_y1;
  double m_x2;
  double m_y2;
  double m_x;
  double m_y;
};

struct QCurveTo
{
  double m_x1;
  double m_y1;
  double m_x;
  double m_y;
};

struct ClosePolygon
{
  char m_char;
  //operator bool() const { return true; }
};

typedef boost::variant<MoveTo, LineTo, CCurveTo, QCurveTo, ClosePolygon> CurveElement_t;
typedef std::deque<CurveElement_t> Curve_t;
typedef std::deque<Curve_t> Path_t;

struct IWORKPath::Impl
{
  Impl();

  Path_t m_path;
  bool m_closed;
  bool m_segmented;
};

IWORKPath::Impl::Impl()
  : m_path()
  , m_closed(false)
{
}

}

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::MoveTo,
  (double, m_x)
  (double, m_y)
)

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::LineTo,
  (double, m_x)
  (double, m_y)
)

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::CCurveTo,
  (double, m_x1)
  (double, m_y1)
  (double, m_x2)
  (double, m_y2)
  (double, m_x)
  (double, m_y)
)

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::QCurveTo,
  (double, m_x1)
  (double, m_y1)
  (double, m_x)
  (double, m_y)
)

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::ClosePolygon,
  (char, m_char)
)

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::IWORKPath::Impl,
  (libetonyek::Path_t, m_path)
)

namespace libetonyek
{

namespace
{

namespace ascii = boost::spirit::ascii;
namespace qi = boost::spirit::qi;

#if defined _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100) // unreferenced formal parameter in boost
#endif

template<typename Iterator>
struct PathGrammar : public qi::grammar<Iterator, IWORKPath::Impl(), ascii::space_type>
{
  PathGrammar()
    : PathGrammar::base_type(path, "path")
  {
    using qi::attr;
    using qi::as;
    using qi::double_;
    using qi::lit;
    using qi::omit;

    move %= 'M' >> double_ >> double_;

    line %= 'L' >> double_ >> double_;

    qCurve %= 'Q' >> double_ >> double_ >> double_ >> double_;

    cCurve %= 'C' >> double_ >> double_ >> double_ >> double_ >> double_ >> double_;

    close = 'Z';

    curve %= as<Curve_t>()[move >> +(line | cCurve | qCurve) >> -close];
    path %= as<Path_t>()[+curve >> *(omit[move]) ];

    close.name("close");
    move.name("move");
    line.name("line");
    cCurve.name("cCurve");
    qCurve.name("qCurve");
    curve.name("curve");
    path.name("path");
  }

  qi::rule<Iterator, IWORKPath::Impl(), ascii::space_type> path;
  qi::rule<Iterator, Curve_t(), ascii::space_type> curve;
  qi::rule<Iterator, MoveTo(), ascii::space_type> move;
  qi::rule<Iterator, LineTo(), ascii::space_type> line;
  qi::rule<Iterator, CCurveTo(), ascii::space_type> cCurve;
  qi::rule<Iterator, QCurveTo(), ascii::space_type> qCurve;
  qi::rule<Iterator, ClosePolygon(), ascii::space_type> close;
};

#if defined _MSC_VER
#pragma warning(pop)
#endif

}

namespace
{

struct Transformer : public static_visitor<void>
{
  explicit Transformer(const glm::dmat3 &tr)
    : m_tr(tr)
  {
  }

  template<typename T>
  void operator()(T &element) const
  {
    transform(element.m_x, element.m_y);
  }

  void operator()(CCurveTo &element) const
  {
    transform(element.m_x1, element.m_y1);
    transform(element.m_x2, element.m_y2);
    transform(element.m_x, element.m_y);
  }

  void operator()(QCurveTo &element) const
  {
    transform(element.m_x1, element.m_y1);
    transform(element.m_x, element.m_y);
  }

  void operator()(ClosePolygon &) const
  {
  }
private:
  void transform(double &x, double &y) const
  {
    glm::dvec3 vec = m_tr * glm::dvec3(x, y, 1);

    x = vec[0];
    y = vec[1];
  }

private:
  const glm::dmat3 &m_tr;
};

}

namespace
{

struct Writer : public static_visitor<void>
{
  explicit Writer(RVNGPropertyListVector &path)
    : m_path(path)
  {
  }

  void operator()(const MoveTo &element) const
  {
    RVNGPropertyList command;
    command.insert("librevenge:path-action", "M");
    command.insert("svg:x", pt2in(element.m_x));
    command.insert("svg:y", pt2in(element.m_y));
    m_path.append(command);
  }

  void operator()(const LineTo &element) const
  {
    RVNGPropertyList command;
    command.insert("librevenge:path-action", "L");
    command.insert("svg:x", pt2in(element.m_x));
    command.insert("svg:y", pt2in(element.m_y));
    m_path.append(command);
  }

  void operator()(const CCurveTo &element) const
  {
    RVNGPropertyList command;
    command.insert("librevenge:path-action", "C");
    command.insert("svg:x", pt2in(element.m_x));
    command.insert("svg:y", pt2in(element.m_y));
    command.insert("svg:x1", pt2in(element.m_x1));
    command.insert("svg:y1", pt2in(element.m_y1));
    command.insert("svg:x2", pt2in(element.m_x2));
    command.insert("svg:y2", pt2in(element.m_y2));
    m_path.append(command);
  }

  void operator()(const QCurveTo &element) const
  {
    RVNGPropertyList command;
    command.insert("librevenge:path-action", "Q");
    command.insert("svg:x", pt2in(element.m_x));
    command.insert("svg:y", pt2in(element.m_y));
    command.insert("svg:x1", pt2in(element.m_x1));
    command.insert("svg:y1", pt2in(element.m_y1));
    m_path.append(command);
  }

  void operator()(const ClosePolygon &) const
  {
    RVNGPropertyList command;
    command.insert("librevenge:path-action", "Z");
    m_path.append(command);
  }
private:
  RVNGPropertyListVector &m_path;
};

}

namespace
{

struct Comparator : public static_visitor<bool>
{
  explicit Comparator(double eps)
    : m_eps(eps)
  {
  }

  template<typename T, typename U>
  bool operator()(const T &, const U &) const
  {
    return false;
  }

  template<typename T>
  bool operator()(const T &left, const T &right) const
  {
    return approxEqual(left, right);
  }

private:
  template<typename T>
  bool approxEqual(const T &left, const T &right) const
  {
    using libetonyek::approxEqual;
    return approxEqual(left.m_x, right.m_x, m_eps) && approxEqual(left.m_y, right.m_y, m_eps);
  }

  bool approxEqual(const QCurveTo &left, const QCurveTo &right) const
  {
    using libetonyek::approxEqual;
    return approxEqual(left.m_x1, right.m_x1, m_eps) && approxEqual(left.m_y1, right.m_y1, m_eps)
           && approxEqual(left.m_x, right.m_x, m_eps) && approxEqual(left.m_y, right.m_y, m_eps)
           ;
  }

  bool approxEqual(const CCurveTo &left, const CCurveTo &right) const
  {
    using libetonyek::approxEqual;
    return approxEqual(left.m_x1, right.m_x1, m_eps) && approxEqual(left.m_y1, right.m_y1, m_eps)
           && approxEqual(left.m_x2, right.m_x2, m_eps) && approxEqual(left.m_y2, right.m_y2, m_eps)
           && approxEqual(left.m_x, right.m_x, m_eps) && approxEqual(left.m_y, right.m_y, m_eps)
           ;
  }

  bool approxEqual(const ClosePolygon &, const ClosePolygon &) const
  {
    return true;
  }

private:
  const double m_eps;
};

}

namespace
{

using std::ostringstream;

struct SVGPrinter : public static_visitor<void>
{
  explicit SVGPrinter(ostringstream &sink)
    : m_sink(sink)
  {
  }

  void operator()(const MoveTo &element) const
  {
    m_sink << "M " << element.m_x << ' ' << element.m_y;
  }

  void operator()(const LineTo &element) const
  {
    m_sink << "L " << element.m_x << ' ' << element.m_y;
  }

  void operator()(const CCurveTo &element) const
  {
    m_sink
        << 'C'
        << ' ' << element.m_x1 << ' ' << element.m_y1
        << ' ' << element.m_x2 << ' ' << element.m_y2
        << ' ' << element.m_x << ' ' << element.m_y
        ;
  }

  void operator()(const QCurveTo &element) const
  {
    m_sink
        << 'Q'
        << ' ' << element.m_x1 << ' ' << element.m_y1
        << ' ' << element.m_x << ' ' << element.m_y
        ;
  }

  void operator()(const ClosePolygon &) const
  {
    m_sink
        << 'Q'
        ;
  }
private:
  ostringstream &m_sink;
};

}


namespace
{

struct ComputeBoundingBox : public static_visitor<void>
{
  explicit ComputeBoundingBox()
    : m_first(true)
    , m_x(0)
    , m_y(0)
  {
    for (int i=0; i<2; ++i) m_boundX[i]=m_boundY[i]=0;
  }

  void operator()(const MoveTo &element)
  {
    m_x=element.m_x;
    m_y=element.m_y;
    addPoint(m_x, m_y);
  }

  void operator()(const LineTo &element)
  {
    m_x=element.m_x;
    m_y=element.m_y;
    addPoint(m_x, m_y);
  }

  void operator()(const CCurveTo &element)
  {
    getCubicBezierBBox(m_x, m_y, element.m_x1, element.m_y1, element.m_x2, element.m_y2, element.m_x, element.m_y);
    m_x=element.m_x;
    m_y=element.m_y;
  }

  void operator()(const QCurveTo &element)
  {
    getQuadraticBezierBBox(m_x, m_y, element.m_x1, element.m_y1, element.m_x, element.m_y);
    m_x=element.m_x;
    m_y=element.m_y;
  }

  void operator()(const ClosePolygon &)
  {
  }
public:
  double m_boundX[2], m_boundY[2];
protected:
  void addPoint(double x, double y)
  {
    if (m_first)
    {
      m_boundX[0]=m_boundX[1]=x;
      m_boundY[0]=m_boundY[1]=y;
      m_first=false;
      return;
    }
    if (x<m_boundX[0])
      m_boundX[0]=x;
    else if (x>m_boundX[1])
      m_boundX[1]=x;
    if (y<m_boundY[0])
      m_boundY[0]=y;
    else if (y>m_boundY[1])
      m_boundY[1]=y;
  }

  static double quadraticExtreme(double t, double a, double b, double c)
  {
    return (1.0-t)*(1.0-t)*a + 2.0*(1.0-t)*t*b + t*t*c;
  }

  static double quadraticDerivative(double a, double b, double c)
  {
    double denominator = a - 2.0*b + c;
    if (fabs(denominator)>1e-10*(a-b))
      return (a - b)/denominator;
    return -1.0;
  }

  void getQuadraticBezierBBox(double x0, double y0, double x1, double y1, double x, double y)
  {
    addPoint(x0,y0);

    double t = quadraticDerivative(x0, x1, x);
    if (t>=0 && t<=1)
    {
      double tmpx = quadraticExtreme(t, x0, x1, x);
      if (tmpx < m_boundX[0])
        m_boundX[0]=tmpx;
      else if (tmpx > m_boundX[1])
        m_boundX[1]=tmpx;
    }

    t = quadraticDerivative(y0, y1, y);
    if (t>=0 && t<=1)
    {
      double tmpy = quadraticExtreme(t, y0, y1, y);
      if (tmpy < m_boundY[0])
        m_boundY[0]=tmpy;
      else if (tmpy > m_boundY[1])
        m_boundY[1]=tmpy;
    }
  }

  static double cubicBase(double t, double a, double b, double c, double d)
  {
    return (1.0-t)*(1.0-t)*(1.0-t)*a + 3.0*(1.0-t)*(1.0-t)*t*b + 3.0*(1.0-t)*t*t*c + t*t*t*d;
  }

  void getCubicBezierBBox(double x0, double y0, double x1, double y1, double x2, double y2, double x, double y)
  {
    addPoint(x0,y0);
    for (int i=0; i<=100; ++i)
    {
      double t=double(i)/100.;
      addPoint(cubicBase(t, x0, x1, x2, x), cubicBase(t, y0, y1, y2, y));
    }
  }
  bool m_first;
  double m_x, m_y;
};

}

IWORKPath::IWORKPath()
  : m_impl(new Impl())
{
}

IWORKPath::IWORKPath(const std::string &path)
  : m_impl(new Impl())
{
  PathGrammar<string::const_iterator> grammar;
  string::const_iterator it = path.begin();
  const bool r = qi::phrase_parse(it, path.end(), grammar, ascii::space, *m_impl);

  if (!r || (path.end() != it))
  {
    ETONYEK_DEBUG_MSG(("parsing of path '%s' failed\n", path.c_str()));
    throw InvalidException();
  }
}

IWORKPath::IWORKPath(const IWORKPath &other)
  : m_impl(new Impl(*other.m_impl))
{
}

IWORKPath &IWORKPath::operator=(const IWORKPath &other)
{
  IWORKPath copy(other);
  swap(copy);
  return *this;
}

void IWORKPath::swap(IWORKPath &other)
{
  m_impl.swap(other.m_impl);
}

void IWORKPath::clear()
{
  m_impl->m_path.clear();
  m_impl->m_closed = false;
}

void IWORKPath::appendMoveTo(const double x, const double y)
{
  if (!m_impl->m_path.empty() && m_impl->m_path.back().size()<=1)
  {
    ETONYEK_DEBUG_MSG(("IWORKPath::appendMoveTo: find a single point path\n"));
    m_impl->m_path.pop_back();
  }
  MoveTo element;
  element.m_x = x;
  element.m_y = y;
  m_impl->m_path.push_back(Curve_t());
  m_impl->m_path.back().push_back(element);
  m_impl->m_closed=false;
}

void IWORKPath::appendLineTo(const double x, const double y)
{
  assert(!m_impl->m_closed && !m_impl->m_path.empty());

  LineTo element;
  element.m_x = x;
  element.m_y = y;
  m_impl->m_path.back().push_back(element);
}

void IWORKPath::appendCCurveTo(const double x1, const double y1, const double x2, const double y2, const double x, const double y)
{
  assert(!m_impl->m_closed && !m_impl->m_path.empty());

  CCurveTo element;
  element.m_x1 = x1;
  element.m_y1 = y1;
  element.m_x2 = x2;
  element.m_y2 = y2;
  element.m_x = x;
  element.m_y = y;
  m_impl->m_path.back().push_back(element);
}

void IWORKPath::appendQCurveTo(const double x1, const double y1, const double x, const double y)
{
  assert(!m_impl->m_closed && !m_impl->m_path.empty());

  QCurveTo element;
  element.m_x1 = x1;
  element.m_y1 = y1;
  element.m_x = x;
  element.m_y = y;
  m_impl->m_path.back().push_back(element);
}

void IWORKPath::appendClose()
{
  assert(!m_impl->m_closed && !m_impl->m_path.empty());
  if (m_impl->m_path.back().size()==1)
  {
    ETONYEK_DEBUG_MSG(("IWORKPath::appendClose: impossible to close an path with one point\n"));
    m_impl->m_path.pop_back();
    m_impl->m_closed = true;
    return;
  }
  m_impl->m_path.back().push_back(ClosePolygon());

  m_impl->m_closed = true;
}

void IWORKPath::operator*=(const glm::dmat3 &tr)
{
  for (auto &it : m_impl->m_path)
  {
    for (auto &cIt : it)
      apply_visitor(Transformer(tr), cIt);
  }
}

void IWORKPath::computeBoundingBox(double &minX, double &minY, double &maxX, double &maxY) const
{
  ComputeBoundingBox bdCompute;
  for (auto &it : m_impl->m_path)
  {
    for (auto &cIt : it)
      apply_visitor(bdCompute, cIt);
  }
  minX=bdCompute.m_boundX[0];
  maxX=bdCompute.m_boundX[1];
  minY=bdCompute.m_boundY[0];
  maxY=bdCompute.m_boundY[1];
}

void IWORKPath::closePath(bool closeOnlyIsSamePoint)
{
  bool lastClosed=false;
  for (Path_t::iterator it = m_impl->m_path.begin(); it != m_impl->m_path.end(); ++it)
  {
    lastClosed=false;
    Curve_t &curve=*it;
    if (curve.size()<=1) continue;
    const CurveElement_t front=curve.front();
    const CurveElement_t back=curve.back();
    if (!closeOnlyIsSamePoint)
    {
      if (!boost::get<ClosePolygon>(&back) && (curve.size()>1 || !boost::get<LineTo>(&back)))
      {
        curve.push_back(ClosePolygon());
        lastClosed=true;
      }
      continue;
    }
    if (!boost::get<MoveTo>(&front)) return;
    double origin[2]= {boost::get<MoveTo>(&front)->m_x,boost::get<MoveTo>(&front)->m_y};
    double dest[2];
    if (boost::get<LineTo>(&back))
    {
      dest[0]=boost::get<LineTo>(&back)->m_x;
      dest[1]=boost::get<LineTo>(&back)->m_y;
    }
    else if (boost::get<CCurveTo>(&back))
    {
      dest[0]=boost::get<CCurveTo>(&back)->m_x;
      dest[1]=boost::get<CCurveTo>(&back)->m_y;
    }
    else if (boost::get<QCurveTo>(&back))
    {
      dest[0]=boost::get<QCurveTo>(&back)->m_x;
      dest[1]=boost::get<QCurveTo>(&back)->m_y;
    }
    else
      return;
    if (origin[0]<=dest[0] && origin[0]>=dest[0] &&
        origin[1]<=dest[1] && origin[1]>=dest[1])
    {
      curve.push_back(ClosePolygon());
      lastClosed=true;
    }
  }
  if (lastClosed)
    m_impl->m_closed=true;
}

const std::string IWORKPath::str() const
{
  ostringstream sink;

  bool first=true;
  for (Path_t::const_iterator it = m_impl->m_path.begin(); it != m_impl->m_path.end(); ++it)
  {
    for (Curve_t::const_iterator cIt = it->begin(); cIt != it->end(); ++cIt)
    {
      if (!first)
        sink << ' ';
      else
        first=false;
      apply_visitor(SVGPrinter(sink), *cIt);
    }
  }

  return sink.str();
}

void IWORKPath::write(librevenge::RVNGPropertyListVector &vec) const
{
  for (Path_t::const_iterator it = m_impl->m_path.begin(); it != m_impl->m_path.end(); ++it)
  {
    for (Curve_t::const_iterator cIt = it->begin(); cIt != it->end(); ++cIt)
      apply_visitor(Writer(vec), *cIt);
  }
}

bool approxEqual(const IWORKPath &left, const IWORKPath &right, const double eps)
{
  if ((left.m_impl->m_closed != right.m_impl->m_closed)
      || (left.m_impl->m_path.size() != right.m_impl->m_path.size()))
    return false;
  for (Path_t::const_iterator lit = left.m_impl->m_path.begin(), rit = right.m_impl->m_path.begin();
       lit != left.m_impl->m_path.end();
       ++lit, ++rit)
  {
    if (lit->size()!=rit->size()) return false;
    for (Curve_t::const_iterator cLit = lit->begin(), cRit = rit->begin(); cLit != lit->end(); ++cLit, ++cRit)
    {
      if (!apply_visitor(Comparator(eps), *cLit, *cRit))
        return false;
    }
  }
  return true;
}

bool operator==(const IWORKPath &left, const IWORKPath &right)
{
  return approxEqual(left, right);
}

bool operator!=(const IWORKPath &left, const IWORKPath &right)
{
  return !(left == right);
}

IWORKPath operator*(const IWORKPath &path, const glm::dmat3 &tr)
{
  IWORKPath newPath(path);
  newPath *= tr;
  return newPath;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
