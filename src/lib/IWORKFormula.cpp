/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKFormula.h"

#include <sstream>
#include <utility>
#include <vector>

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/variant/recursive_variant.hpp>

namespace libetonyek
{

using boost::apply_visitor;
using boost::optional;
using boost::recursive_wrapper;
using boost::variant;

using std::string;
using std::vector;

struct Coord
{
  unsigned m_coord;
  bool m_absolute;
};

struct Address
{
  Coord m_column;
  Coord m_row;
  optional<string> m_table;
  optional<string> m_worksheet;
};

typedef std::pair<Address, Address> AddressRange;

struct Function;
struct UnaryOp;
struct BinaryOp;

typedef variant<double, string, Address, AddressRange, recursive_wrapper<UnaryOp>, recursive_wrapper<BinaryOp>, recursive_wrapper<Function> > Expression;

struct UnaryOp
{
  char m_op;
  Expression m_expr;
};

struct BinaryOp
{
  char m_op;
  Expression m_left;
  Expression m_right;
};

struct Function
{
  string m_name;
  vector<Expression> m_args;
};

}

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::Coord,
  (bool, m_absolute)
  (unsigned, m_coord)
)

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::Address,
  (optional<std::string>, m_worksheet)
  (optional<std::string>, m_table)
  (libetonyek::Coord, m_column)
  (libetonyek::Coord, m_row)
)

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::UnaryOp,
  (char, m_op)
  (libetonyek::Expression, m_expr)
)

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::BinaryOp,
  (libetonyek::Expression, m_left)
  (char, m_op)
  (libetonyek::Expression, m_right)
)

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::Function,
  (std::string, m_name)
  (std::vector<libetonyek::Expression>, m_args)
)

namespace libetonyek
{

namespace
{

namespace ascii = boost::spirit::ascii;
namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;

unsigned parseRowName(const vector<char> &)
{
  // TODO: implement
  return 0;
}

template<typename Iterator>
struct FormulaGrammar : public qi::grammar<Iterator, Expression()>
{
  FormulaGrammar()
  : FormulaGrammar::base_type(formula)
{
  using ascii::char_;
  using boost::none;
  using phoenix::bind;
  using qi::_1;
  using qi::_val;
  using qi::alpha;
  using qi::attr;
  using qi::double_;
  using qi::lit;
  using qi::uint_;

  number %= double_;
  str %= lit('\'') >> +(char_ - '\'') >> '\'';
  unaryLit %= char_('+') | char_('-');
  binaryLit %= char_('+') | char_('-') | char_('*') | char_('/') | char_('%');

  row %=
    lit('$') >> attr(true) >> uint_
    | attr(false) >> uint_
    ;

  column %=
    lit('$') >> attr(true) >> columnName
    | attr(false) >> columnName
    ;

  columnName = (+alpha)[_val = bind(parseRowName, _1)];

  worksheet = table.alias();
  // TODO: improve
  table %= +(char_ - '.');

  address %=
    worksheet >> '.' >> table >> '.' >> column >> row
    | attr(none) >> table >> '.' >> column >> row
    | attr(none) >> attr(none) >> column >> row
    ;

  range %= address >> ':' >> address;

  unaryOp %= unaryLit >> term;
  binaryOp %= term >> binaryLit >> expression;

  function %= +alpha >> '(' >> -(expression % ';') >> ')';

  term %=
    number
    | str
    | address
    | range
    | unaryOp
    | function
    ;

  expression %= term
                | binaryOp
                ;

  formula %= lit('=') >> expression;

  number.name("number");
  str.name("string");
  unaryOp.name("unary operator");
  binaryOp.name("binary operator");
  row.name("row");
  column.name("column");
  columnName.name("column name");
  table.name("table name");
  worksheet.name("worksheet name");
  address.name("address");
  range.name("address range");
  function.name("function");
  expression.name("expression");
  term.name("term");
  formula.name("formula");
}

qi::rule<Iterator, Function()> function;
qi::rule<Iterator, Expression()> expression, formula, term;
qi::rule<Iterator, Address()> address;
qi::rule<Iterator, AddressRange()> range;
qi::rule<Iterator, unsigned()> columnName;
qi::rule<Iterator, Coord()> column, row;
qi::rule<Iterator, double()> number;
qi::rule<Iterator, string()> str, table, worksheet;
qi::rule<Iterator, UnaryOp()> unaryOp;
qi::rule<Iterator, BinaryOp()> binaryOp;
qi::rule<Iterator, char()> unaryLit, binaryLit;
};

}

namespace
{

struct printer : public boost::static_visitor<string>
{
  string operator()(double val) const
  {
    return boost::lexical_cast<string>(val);
  }

  string operator()(const std::string &val) const
  {
    return val;
  }

  string operator()(const Address &val) const
  {
    std::ostringstream out;

    if (val.m_worksheet)
      out << get(val.m_worksheet) << '.';
    if (val.m_table)
      out << get(val.m_table) << '.';
    if (val.m_column.m_absolute)
      out << '$';
    out << val.m_column.m_coord;
    if (val.m_row.m_absolute)
      out << '$';
    out << val.m_row.m_coord;

    return out.str();
  }

  string operator()(const AddressRange &val) const
  {
    return operator()(val.first) + ':' + operator()(val.second);
  }

  string operator()(const recursive_wrapper<UnaryOp> &val) const
  {
    return val.get().m_op + apply_visitor(printer(), val.get().m_expr);
  }

  string operator()(const recursive_wrapper<BinaryOp> &val) const
  {
    return apply_visitor(printer(), val.get().m_left) + val.get().m_op + apply_visitor(printer(), val.get().m_right);
  }

  string operator()(const recursive_wrapper<Function> &val) const
  {
    std::ostringstream out;

    out << val.get().m_name << '(';
    for (vector<Expression>::const_iterator it = val.get().m_args.begin(); it != val.get().m_args.end(); ++it)
      out << apply_visitor(printer(), *it);
    out << ')';

    return out.str();
  }
};

}

namespace
{

struct collector : public boost::static_visitor<librevenge::RVNGPropertyListVector>
{
  librevenge::RVNGPropertyListVector operator()(double val) const
  {
    librevenge::RVNGPropertyListVector propsVector;
    librevenge::RVNGPropertyList props;
    props.insert("librevenge:type", "librevenge:number");
    props.insert("librevenge:number", val);
    propsVector.append(props);
    return propsVector;
  }

  librevenge::RVNGPropertyListVector operator()(const std::string &val) const
  {
    librevenge::RVNGPropertyListVector propsVector;
    librevenge::RVNGPropertyList props;
    props.insert("librevenge:type", "librevenge:text");
    props.insert("librevenge:text", val.c_str());
    propsVector.append(props);
    return propsVector;
  }

  librevenge::RVNGPropertyListVector operator()(const Address &val) const
  {
    librevenge::RVNGPropertyListVector propsVector;
    if (val.m_worksheet)
    {
      librevenge::RVNGPropertyList props;
      props.insert("librevenge:type", "librevenge:sheet");
      props.insert("librevenge:sheet", val.m_worksheet);
      propsVector.append(props);
    }
    if (val.m_table)
    {
      librevenge::RVNGPropertyList props;
      props.insert("librevenge:type", "librevenge:table");
      props.insert("librevenge:sheet", val.m_table);
      propsVector.append(props);
    }

    librevenge::RVNGPropertyList props;
    props.insert("librevenge:type", "librevenge-cell");

    if (val.m_column.m_absolute)
      props.insert("librevenge:column-absolute", true);
    props.insert("librevenge:column", int(val.m_column.m_coord));

    if (val.m_row.m_absolute)
      props.insert("librevenge:row-absolute", true);
    props.insert("librevenge:row", int(val.m_row.m_coord));

    propsVector.append(props);

    return propsVector;
  }

  librevenge::RVNGPropertyListVector operator()(const AddressRange &val) const
  {
    librevenge::RVNGPropertyListVector propsVector;

    librevenge::RVNGPropertyList props;
    props.insert("librevenge:type", "librevenge-cells");

    if (val.first.m_column.m_absolute)
      props.insert("librevenge:start-column-absolute", true);
    props.insert("librevenge:start-column", int(val.first.m_column.m_coord));

    if (val.first.m_row.m_absolute)
      props.insert("librevenge:start-row-absolute", true);
    props.insert("librevenge:start-row", int(val.first.m_row.m_coord));

    if (val.second.m_column.m_absolute)
      props.insert("librevenge:end-column-absolute", true);
    props.insert("librevenge:end-column", int(val.second.m_column.m_coord));

    if (val.second.m_row.m_absolute)
      props.insert("librevenge:end-row-absolute", true);
    props.insert("librevenge:end-row", int(val.second.m_row.m_coord));

    propsVector.append(props);

    return propsVector;
  }

  librevenge::RVNGPropertyListVector operator()(const recursive_wrapper<UnaryOp> &val) const
  {
    librevenge::RVNGPropertyListVector propsVector;
    librevenge::RVNGPropertyList props;
    props.insert("librevenge:type", "librevenge-operator");
    props.insert("librevenge:operator", val.get().m_op);
    propsVector.append(props);
    propsVector.append(apply_visitor(collector(), val.get().m_expr));
    return propsVector;
  }

  librevenge::RVNGPropertyListVector operator()(const recursive_wrapper<BinaryOp> &val) const
  {
    librevenge::RVNGPropertyListVector propsVector;
    propsVector.append(apply_visitor(collector(), val.get().m_left));
    librevenge::RVNGPropertyList props;
    props.insert("librevenge:type", "librevenge-operator");
    props.insert("librevenge:operator", val.get().m_op);
    propsVector.append(props);
    propsVector.append(apply_visitor(collector(), val.get().m_right));
    return propsVector;
  }

  librevenge::RVNGPropertyListVector operator()(const recursive_wrapper<Function> &val) const
  {
    librevenge::RVNGPropertyListVector propsVector;

    librevenge::RVNGPropertyList props1;
    props1.insert("librevenge:type", "librevenge-function");
    props1.insert("librevenge:function", val.get().m_name.c_str());
    propsVector.append(props1);

    librevenge::RVNGPropertyList props2;
    props2.insert("librevenge:type", "librevenge-operator");
    props2.insert("librevenge:operator", "(");
    propsVector.append(props2);

    for (vector<Expression>::const_iterator it = val.get().m_args.begin(); it != val.get().m_args.end(); ++it)
      propsVector.append(apply_visitor(collector(), *it));

    librevenge::RVNGPropertyList props3;
    props3.insert("librevenge:type", "librevenge-operator");
    props3.insert("librevenge:operator", ")");
    propsVector.append(props3);

    return propsVector;
  }
};

}

struct IWORKFormula::Impl
{
  Expression m_formula;
};

IWORKFormula::IWORKFormula()
  : m_impl(new Impl())
{
}

bool IWORKFormula::parse(const std::string &formula)
{
  FormulaGrammar<string::const_iterator> grammar;
  string::const_iterator it = formula.begin();
  string::const_iterator end = formula.end();
  const bool r = qi::phrase_parse(it, end, grammar, ascii::space, m_impl->m_formula);
  return r && (it == end);
}

const std::string IWORKFormula::toString() const
{
  return '=' + apply_visitor(printer(), m_impl->m_formula);
}

void IWORKFormula::collectFormula(librevenge::RVNGPropertyListVector &formula)
{
  formula.append(apply_visitor(collector(), m_impl->m_formula));
}

} // namespace libetonyek

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
