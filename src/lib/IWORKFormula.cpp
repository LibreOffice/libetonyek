/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKFormula.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <utility>
#include <vector>

#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
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

  for (vector<char>::const_iterator it = columnName.begin(); it != columnName.end(); ++it)
    columnNumber = 26 * columnNumber + (toupper(*it) - 'A') + 1;

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

  expression %=
    term
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

using std::ostringstream;

struct printer : public boost::static_visitor<void>
{
  printer(ostringstream &out)
    : m_out(out)
  {
  }

  void operator()(double val) const
  {
    m_out << val;
  }

  void operator()(const std::string &val) const
  {
    m_out << val;
  }

  void operator()(const Address &val) const
  {
    m_out << '[';
    formatAddress(val);
    m_out << ']';
  }

  void operator()(const AddressRange &val) const
  {
    printer p(m_out);
    p(val.first);
    m_out << ':';
    p(val.second);
  }

  void operator()(const recursive_wrapper<UnaryOp> &val) const
  {
    m_out << val.get().m_op;
    apply_visitor(printer(m_out), val.get().m_expr);
  }

  void operator()(const recursive_wrapper<BinaryOp> &val) const
  {
    apply_visitor(printer(m_out), val.get().m_left);
    m_out << val.get().m_op;
    apply_visitor(printer(m_out), val.get().m_right);
  }

  void operator()(const recursive_wrapper<Function> &val) const
  {
    m_out << val.get().m_name << '(';
    for (vector<Expression>::const_iterator it = val.get().m_args.begin(); it != val.get().m_args.end(); ++it)
      apply_visitor(printer(m_out), *it);
    m_out << ')';
  }

private:
  void formatAddress(const Address &val) const
  {
    if (val.m_worksheet)
      m_out << get(val.m_worksheet) << '.';
    if (val.m_table)
      m_out << get(val.m_table);
    m_out  << '.';
    if (val.m_column.m_absolute)
      m_out << '$';

    unsigned column = val.m_column.m_coord;
    vector<char> columnNumerals;
    columnNumerals.reserve(4);
    while (column != 0)
    {
      if (column > 0)
        --column;
      columnNumerals.push_back('A' + column % 26);
      column /= 26;
    }
    copy(columnNumerals.rbegin(), columnNumerals.rend(), std::ostream_iterator<char>(m_out));

    if (val.m_row.m_absolute)
      m_out << '$';
    m_out << val.m_row.m_coord;
  }

private:
  ostringstream &m_out;
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
  ostringstream out;
  out << '=';
  apply_visitor(printer(out), m_impl->m_formula);
  return out.str();
}

} // namespace libetonyek

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
