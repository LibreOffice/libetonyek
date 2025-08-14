/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string>

#include <boost/fusion/include/std_pair.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>

#include "KEY1StringConverter.h"

#include "libetonyek_xml.h"

namespace libetonyek
{

namespace ascii = boost::spirit::ascii;
namespace qi =  boost::spirit::qi;
using  boost::phoenix::ref;
using boost::phoenix::construct;

boost::optional<glm::dmat3> KEY1StringConverter<glm::dmat3>::convert(const char *value)
{
  std::string val(value);
  std::string::const_iterator it = val.begin(), end = val.end();
  glm::dmat3 res;
  if (qi::phrase_parse(it, end, (qi::double_ >> qi::double_ >> qi::double_ >> qi::double_ >> qi::double_ >> qi::double_)
                       [ref(res)=construct<glm::dmat3>(qi::_1,qi::_2,0,qi::_3,qi::_4,0,qi::_5,qi::_6,1)], ascii::space) && it==end)
  {
    return boost::optional<glm::dmat3>(res);
  }
  else
  {
    ETONYEK_DEBUG_MSG(("KEY1StringConverter<glm::dmat3>: can not parse %s\n", value));
  }
  return boost::optional<glm::dmat3>();
}

boost::optional<IWORKColor> KEY1StringConverter<IWORKColor>::convert(const char *value)
{
  std::string val(value);
  std::string::const_iterator it = val.begin(), end = val.end();
  IWORKColor col;
  if (qi::phrase_parse(it, end,
                       (qi::lit('g') >> qi::double_)[ref(col)=construct<IWORKColor>(qi::_1,qi::_1,qi::_1,1)]
                       | (qi::double_ >> qi::double_ >> qi::double_ >> qi::double_)[ref(col)=construct<IWORKColor>(qi::_1,qi::_2,qi::_3,qi::_4)]
                       | (qi::double_ >> qi::double_ >> qi::double_)[ref(col)=construct<IWORKColor>(qi::_1,qi::_2,qi::_3,1)]
                       , ascii::space) && it==end)
    return boost::optional<IWORKColor>(col);
  else
  {
    ETONYEK_DEBUG_MSG(("KEY1StringConverter<IWORKColor>: can not parse %s\n", value));
  }
  return boost::optional<IWORKColor>();
}

boost::optional<IWORKPosition> KEY1StringConverter<IWORKPosition>::convert(const char *value)
{
  std::string val(value);
  std::string::const_iterator it = val.begin(), end = val.end();
  std::pair<double, double> d;
  boost::optional<IWORKPosition> res;
  if (qi::phrase_parse(it, end, qi::double_ >> qi::double_, ascii::space, d) && it==end)
  {
    res=IWORKPosition();
    res->m_x=d.first;
    res->m_y=d.second;
  }
  else
  {
    ETONYEK_DEBUG_MSG(("KEY1StringConverter<IWORKPosition>: can not parse %s\n", value));
  }
  return res;
}

boost::optional<IWORKSize> KEY1StringConverter<IWORKSize>::convert(const char *value)
{
  std::string val(value);
  std::string::const_iterator it = val.begin(), end = val.end();
  std::pair<int, int> d;
  boost::optional<IWORKSize> res;
  if (qi::phrase_parse(it, end, qi::double_ >> qi::double_, ascii::space, d) && it==end)
  {
    res=IWORKSize();
    res->m_width=d.first;
    res->m_height=d.second;
  }
  else
  {
    ETONYEK_DEBUG_MSG(("KEY1StringConverter<IWORKSize>: can not parse %s\n", value));
  }
  return res;
}

}
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
