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
#include "IWORKTransformation.h"
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
    , path()
    , curve()
    , move()
    , line()
    , cCurve()
    , qCurve()
    , close()
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
  explicit Writer(RVNGPropertyListVector &path, double deltaX, double deltaY)
    : m_path(path)
    , m_deltaX(deltaX)
    , m_deltaY(deltaY)
  {
  }

  void operator()(const MoveTo &element) const
  {
    RVNGPropertyList command;
    command.insert("librevenge:path-action", "M");
    command.insert("svg:x", pt2in(element.m_x+m_deltaX));
    command.insert("svg:y", pt2in(element.m_y+m_deltaY));
    m_path.append(command);
  }

  void operator()(const LineTo &element) const
  {
    RVNGPropertyList command;
    command.insert("librevenge:path-action", "L");
    command.insert("svg:x", pt2in(element.m_x+m_deltaX));
    command.insert("svg:y", pt2in(element.m_y+m_deltaY));
    m_path.append(command);
  }

  void operator()(const CCurveTo &element) const
  {
    RVNGPropertyList command;
    command.insert("librevenge:path-action", "C");
    command.insert("svg:x", pt2in(element.m_x+m_deltaX));
    command.insert("svg:y", pt2in(element.m_y+m_deltaY));
    command.insert("svg:x1", pt2in(element.m_x1+m_deltaX));
    command.insert("svg:y1", pt2in(element.m_y1+m_deltaY));
    command.insert("svg:x2", pt2in(element.m_x2+m_deltaX));
    command.insert("svg:y2", pt2in(element.m_y2+m_deltaY));
    m_path.append(command);
  }

  void operator()(const QCurveTo &element) const
  {
    RVNGPropertyList command;
    command.insert("librevenge:path-action", "Q");
    command.insert("svg:x", pt2in(element.m_x+m_deltaX));
    command.insert("svg:y", pt2in(element.m_y+m_deltaY));
    command.insert("svg:x1", pt2in(element.m_x1+m_deltaX));
    command.insert("svg:y1", pt2in(element.m_y1+m_deltaY));
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
  double m_deltaX;
  double m_deltaY;
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
        << 'Z'
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

void IWORKPath::computeBoundingBox(double &minX, double &minY, double &maxX, double &maxY, double factor) const
{
  ComputeBoundingBox bdCompute;
  for (auto &it : m_impl->m_path)
  {
    for (auto &cIt : it)
      apply_visitor(bdCompute, cIt);
  }
  minX=factor*bdCompute.m_boundX[0];
  maxX=factor*bdCompute.m_boundX[1];
  minY=factor*bdCompute.m_boundY[0];
  maxY=factor*bdCompute.m_boundY[1];
}

bool IWORKPath::isRectangle() const
{
  if (m_impl->m_path.size()!=1) return false;
  Curve_t const &curve=m_impl->m_path[0];
  if (curve.size()!=4 && (curve.size()!=5 || !boost::get<ClosePolygon>(&curve.back())))
    return false;
  double x[5] = {0};
  double y[5] = {0};
  int pt=0;
  for (const auto &it : curve)
  {
    if (pt==0 && boost::get<MoveTo>(&it))
    {
      x[0]=x[4]=boost::get<MoveTo>(&it)->m_x;
      y[0]=y[4]=boost::get<MoveTo>(&it)->m_y;
    }
    else if (pt && boost::get<LineTo>(&it))
    {
      x[pt]=boost::get<LineTo>(&it)->m_x;
      y[pt]=boost::get<LineTo>(&it)->m_y;
    }
    else
      return false;
    if (++pt==4)
      break;
  }
  int id=(x[0]<=x[1] && x[0]>=x[1]) ? 0 : 1;
  if (x[id]<x[id+1] || x[id]>x[id+1] || // check axis
      y[id+1]<y[id+2] || y[id+1]>y[id+2] ||
      x[id+2]<x[id+3] || x[id+2]>x[id+3] ||
      y[id+3]<y[(id+4)%4] || y[id+3]>y[(id+4)%4])
    return false;
  if ((x[id]<=x[id+2] && x[id]>=x[id+2]) ||
      (x[id+1]<=x[id+3] && x[id+1]>=x[id+3]))   // empty diagonal
  {
    // check if all points are the same
    for (int i=0; i<4; ++i)
    {
      if (x[i]<x[i+1] || x[i]>x[i+1] || y[i]<y[i+1] || y[i]>y[i+1])
        return false;
    }
    // a point
    return true;
  }
  return true;
}

void IWORKPath::closePath(bool closeOnlyIsSamePoint)
{
  bool lastClosed=false;
  for (auto &curve : m_impl->m_path)
  {
    lastClosed=false;
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
    for (const auto &cIt : *it)
    {
      if (!first)
        sink << ' ';
      else
        first=false;
      apply_visitor(SVGPrinter(sink), cIt);
    }
  }

  return sink.str();
}

void IWORKPath::write(librevenge::RVNGPropertyListVector &vec, double deltaX, double deltaY) const
{
  for (Path_t::const_iterator it = m_impl->m_path.begin(); it != m_impl->m_path.end(); ++it)
  {
    for (const auto &cIt : *it)
      apply_visitor(Writer(vec, deltaX, deltaY), cIt);
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
    for (auto cLit = lit->begin(), cRit = rit->begin(); cLit != lit->end(); ++cLit, ++cRit)
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

IWORKConnectionPath::IWORKConnectionPath()
  : m_size()
  , m_isSpline(true)
{
}

IWORKPathPtr_t IWORKConnectionPath::getPath() const
{
  for (auto const &pos : m_positions)
  {
    if (pos) continue;
    ETONYEK_DEBUG_MSG(("IWORKConnectionPath::getPath: can not find some positions\n"));
    return IWORKPathPtr_t();
  }
  IWORKPathPtr_t path(new IWORKPath());
  path->appendMoveTo(get(m_positions[0]).m_x, get(m_positions[0]).m_y);
  if (m_isSpline)   // bezier curve which goes through each points
  {
    IWORKPosition dir(0.2*(get(m_positions[2]).m_x-get(m_positions[0]).m_x),
                      0.2*(get(m_positions[2]).m_y-get(m_positions[0]).m_y));
    path->appendQCurveTo(get(m_positions[1]).m_x-dir.m_x, get(m_positions[1]).m_y-dir.m_y,
                         get(m_positions[1]).m_x, get(m_positions[1]).m_y);
    path->appendQCurveTo(get(m_positions[1]).m_x+dir.m_x, get(m_positions[1]).m_y+dir.m_y,
                         get(m_positions[2]).m_x, get(m_positions[2]).m_y);
  }
  else
  {
    path->appendLineTo(get(m_positions[0]).m_x, get(m_positions[1]).m_y);
    path->appendLineTo(get(m_positions[2]).m_x, get(m_positions[1]).m_y);
    path->appendLineTo(get(m_positions[2]).m_x, get(m_positions[2]).m_y);
  }
  if (m_size)
    *path *= transformations::scale(get(m_size).m_width / 100, get(m_size).m_height / 100);
  // TODO: implement me
  return path;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
