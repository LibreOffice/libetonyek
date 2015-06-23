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

unsigned parseRowName(const vector<char> &columnName)
{
  unsigned columnNumber = 0;

  for(unsigned i=0; i<columnName.size(); ++i)
      columnNumber = 26*columnNumber + toupper(columnName[i]) - 64;

  return columnNumber;
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

} // namespace libetonyek

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
