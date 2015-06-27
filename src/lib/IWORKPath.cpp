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

struct CurveTo
{
  double m_x1;
  double m_y1;
  double m_x2;
  double m_y2;
  double m_x;
  double m_y;
};

typedef boost::variant<MoveTo, LineTo, CurveTo> PathElement_t;
typedef std::deque<PathElement_t> Path_t;

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
  , m_segmented(false)
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
  libetonyek::CurveTo,
  (double, m_x1)
  (double, m_y1)
  (double, m_x2)
  (double, m_y2)
  (double, m_x)
  (double, m_y)
)

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::IWORKPath::Impl,
  (libetonyek::Path_t, m_path)
  (bool, m_closed)
)

namespace libetonyek
{

namespace
{

namespace ascii = boost::spirit::ascii;
namespace qi = boost::spirit::qi;

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

  close = lit('Z');

  move %= 'M' >> double_ >> double_;

  line %= 'L' >> double_ >> double_;

  curve %= 'C' >> double_ >> double_ >> double_ >> double_ >> double_ >> double_;

  path %= as<Path_t>()[move >> +(line | curve)] >> (close >> attr(true) >> omit[move] | attr(false));

  close.name("close");
  move.name("move");
  line.name("line");
  curve.name("curve");
  path.name("path");
}

qi::rule<Iterator, IWORKPath::Impl(), ascii::space_type> path;
qi::rule<Iterator, MoveTo(), ascii::space_type> move;
qi::rule<Iterator, LineTo(), ascii::space_type> line;
qi::rule<Iterator, CurveTo(), ascii::space_type> curve;
qi::rule<Iterator, void(), ascii::space_type> close;
};

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

  void operator()(CurveTo &element) const
  {
    transform(element.m_x1, element.m_y1);
    transform(element.m_x2, element.m_y2);
    transform(element.m_x, element.m_y);
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

  void operator()(const CurveTo &element) const
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

  bool approxEqual(const CurveTo &left, const CurveTo &right) const
  {
    using libetonyek::approxEqual;
    return approxEqual(left.m_x1, right.m_x1, m_eps) && approxEqual(left.m_y1, right.m_y1, m_eps)
           && approxEqual(left.m_x2, right.m_x2, m_eps) && approxEqual(left.m_y2, right.m_y2, m_eps)
           && approxEqual(left.m_x, right.m_x, m_eps) && approxEqual(left.m_y, right.m_y, m_eps)
           ;
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

  void operator()(const CurveTo &element) const
  {
    m_sink
        << 'C'
        << ' ' << element.m_x1 << ' ' << element.m_y1
        << ' ' << element.m_x2 << ' ' << element.m_y2
        << ' ' << element.m_x << ' ' << element.m_y
        ;
  }

private:
  ostringstream &m_sink;
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
  m_impl->m_segmented = false;
}

void IWORKPath::appendMoveTo(const double x, const double y)
{
  assert(!m_impl->m_closed);

  if (!m_impl->m_path.empty())
    m_impl->m_segmented = true;
  MoveTo element;
  element.m_x = x;
  element.m_y = y;
  m_impl->m_path.push_back(element);
}

void IWORKPath::appendLineTo(const double x, const double y)
{
  assert(!m_impl->m_closed);

  LineTo element;
  element.m_x = x;
  element.m_y = y;
  m_impl->m_path.push_back(element);
}

void IWORKPath::appendCurveTo(const double x1, const double y1, const double x2, const double y2, const double x, const double y)
{
  assert(!m_impl->m_closed);

  CurveTo element;
  element.m_x1 = x1;
  element.m_y1 = y1;
  element.m_x2 = x2;
  element.m_y2 = y2;
  element.m_x = x;
  element.m_y = y;
  m_impl->m_path.push_back(element);
}

void IWORKPath::appendClose()
{
  assert(!m_impl->m_closed);
  assert(!m_impl->m_segmented);

  m_impl->m_closed = true;
}

void IWORKPath::operator*=(const glm::dmat3 &tr)
{
  for (Path_t::iterator it = m_impl->m_path.begin(); it != m_impl->m_path.end(); ++it)
    apply_visitor(Transformer(tr), *it);
}

const std::string IWORKPath::str() const
{
  ostringstream sink;

  Path_t::const_iterator it = m_impl->m_path.begin();
  const Path_t::const_iterator end = m_impl->m_path.end();
  if (it != end)
  {
    apply_visitor(SVGPrinter(sink), *it);
    for (++it; it != end; ++it)
    {
      sink << ' ';
      apply_visitor(SVGPrinter(sink), *it);
    }
    if (m_impl->m_closed)
      sink << " Z";
  }

  return sink.str();
}

void IWORKPath::write(librevenge::RVNGPropertyListVector &vec) const
{
  for (Path_t::const_iterator it = m_impl->m_path.begin(); it != m_impl->m_path.end(); ++it)
    apply_visitor(Writer(vec), *it);
  if (m_impl->m_closed)
  {
    librevenge::RVNGPropertyList element;
    element.insert("librevenge:path-action", "Z");
    vec.append(element);
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
    if (!apply_visitor(Comparator(eps), *lit, *rit))
      return false;
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
