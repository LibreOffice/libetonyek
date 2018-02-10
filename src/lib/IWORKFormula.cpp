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
  optional<Coord> m_column;
  optional<Coord> m_row;
  optional<string> m_table;
};

typedef std::pair<Address, Address> AddressRange;

struct TrueOrFalseFunc
{
  string m_name;
};

struct Function;
struct PrefixOp;
struct InfixOp;
struct PostfixOp;
struct PExpr;

typedef variant<double, string, TrueOrFalseFunc, Address, AddressRange, recursive_wrapper<PrefixOp>, recursive_wrapper<InfixOp>, recursive_wrapper<PostfixOp>, recursive_wrapper<Function>, recursive_wrapper<PExpr> > Expression;

struct PrefixOp
{
  char m_op;
  Expression m_expr;
};

struct InfixOp
{
  string m_op;
  Expression m_left;
  Expression m_right;
};

struct PostfixOp
{
  char m_op;
  Expression m_expr;
};

struct Function
{
  string m_name;
  vector<Expression> m_args;
};

struct PExpr
{
  Expression m_expr;
};

}

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::Coord,
  (bool, m_absolute)
  (unsigned, m_coord)
)

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::TrueOrFalseFunc,
  (std::string, m_name)
)

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::Address,
  (optional<std::string>, m_table)
  (optional<libetonyek::Coord>, m_column)
  (optional<libetonyek::Coord>, m_row)
)

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::PrefixOp,
  (char, m_op)
  (libetonyek::Expression, m_expr)
)

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::InfixOp,
  (libetonyek::Expression, m_left)
  (std::string, m_op)
  (libetonyek::Expression, m_right)
)

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::PostfixOp,
  (libetonyek::Expression, m_expr)
  (char, m_op)
)

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::Function,
  (std::string, m_name)
  (std::vector<libetonyek::Expression>, m_args)
)

BOOST_FUSION_ADAPT_STRUCT(
  libetonyek::PExpr,
  (libetonyek::Expression, m_expr)
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

  for (char it : columnName)
    columnNumber = 26 * columnNumber + unsigned(toupper(it) - 'A') + 1;

  return columnNumber;
}

#if defined _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100) // unreferenced formal parameter in boost
#endif

template<typename Iterator>
struct FormulaGrammar : public qi::grammar<Iterator, Expression()>
{
  FormulaGrammar()
    : FormulaGrammar::base_type(formula, "formula")
  {
    using ascii::char_;
    using ascii::string;
    using boost::none;
    using phoenix::bind;
    using qi::_1;
    using qi::_a;
    using qi::_val;
    using qi::alpha;
    using qi::alnum;
    using qi::attr;
    using qi::double_;
    using qi::lit;
    using qi::no_case;
    using qi::uint_;

    createFunctionNameMap();

    number %= double_;
    str %= lit('\"') >> +(char_ - '\"') >> '\"';
    trueOrFalseFunctionLit %= string("TRUE")|string("FALSE");
    prefixLit %= char_('+') | char_('-');
    infixLit %=
      char_('+') | char_('-') | char_('*') | char_('/')
      | string("<>") | string("<=") | string(">=")
      | char_('^') | char_('=') | char_('>') | char_('<')
      ;
    postfixLit %= char_('%');

    row %=
      lit('$') >> attr(true) >> uint_
      | attr(false) >> uint_
      ;

    column %=
      lit('$') >> attr(true) >> columnName
      | attr(false) >> columnName
      ;

    columnName = (+alpha)[_val = bind(parseRowName, _1)];

    // TODO: improve
    table %= +(char_ - ":");

    address %=
      table >> "::" >> column >> row
      | attr(none) >> column >> row
      ;

    addressSpecialColumn %=
      table >> "::" >> column >> attr(none)
      | attr(none) >> column >> attr(none)
      ;

    addressSpecialRow %=
      table >> "::" >> attr(none) >> row
      | attr(none) >> attr(none) >> row
      ;

    rangeSpecial %= addressSpecialColumn[_a = _1] >> attr(_a) >> !row;

    range %=
      address >> ':' >> address
      | addressSpecialColumn >> ':' >> addressSpecialColumn
      | addressSpecialRow >> ':' >> addressSpecialRow
      | rangeSpecial
      ;

    prefixOp %= prefixLit >> term;
    infixOp %= term >> infixLit >> expression;
    trueOrFalseFunction %= trueOrFalseFunctionLit;
    postfixOp %= term >> postfixLit;

    function %= (no_case[mappedName] | +alnum) >> '(' >> -(expression % ',') >> ')';

    pExpr %= '(' >> expression >> ')';

    term %=
      str
      | function
      | trueOrFalseFunction
      | range
      | address
      | number
      | prefixOp
      | pExpr
      ;

    expression %=
      infixOp
      | postfixOp
      | term
      ;

    formula %= lit('=') >> expression;

    number.name("number");
    str.name("str");
    trueOrFalseFunction.name("trueOrFalseFunction");
    prefixOp.name("prefixOp");
    infixOp.name("infixOp");
    row.name("row");
    column.name("column");
    columnName.name("columnName");
    table.name("table");
    address.name("address");
    range.name("range");
    function.name("function");
    expression.name("expression");
    term.name("term");
    formula.name("formula");
    pExpr.name("pExpr");
    rangeSpecial.name("rangeSpecial");
  }

  void createFunctionNameMap()
  {
  }

  qi::rule<Iterator, Function()> function;
  qi::rule<Iterator, TrueOrFalseFunc()> trueOrFalseFunction;
  qi::rule<Iterator, Expression()> expression, formula, term;
  qi::rule<Iterator, PExpr()> pExpr;
  qi::rule<Iterator, Address()> address, addressSpecialColumn, addressSpecialRow;
  qi::rule<Iterator, AddressRange()> range;
  qi::rule<Iterator, unsigned()> columnName;
  qi::rule<Iterator, Coord()> column, row;
  qi::rule<Iterator, double()> number;
  qi::rule<Iterator, string()> str, table, infixLit, trueOrFalseFunctionLit;
  qi::rule<Iterator, PrefixOp()> prefixOp;
  qi::rule<Iterator, InfixOp()> infixOp;
  qi::rule<Iterator, PostfixOp()> postfixOp;
  qi::rule<Iterator, char()> prefixLit, postfixLit;
  qi::rule<Iterator, qi::locals<Address> > rangeSpecial;

  qi::symbols<char, string> mappedName;
};

#if defined _MSC_VER
#pragma warning(pop)
#endif

}

namespace
{

using std::ostringstream;

struct Printer : public boost::static_visitor<void>
{
  Printer(ostringstream &out, int offsetColumn, int offsetRow)
    : m_out(out)
    , m_offsetColumn(offsetColumn)
    , m_offsetRow(offsetRow)
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
    m_out << '[';
    formatAddress(val.first);
    m_out << ':';
    formatAddress(val.second);
    m_out << ']';
  }

  void operator()(const TrueOrFalseFunc &val) const
  {
    m_out << val.m_name << "()";
  }

  void operator()(const recursive_wrapper<PrefixOp> &val) const
  {
    m_out << val.get().m_op;
    apply_visitor(Printer(m_out, m_offsetColumn, m_offsetRow), val.get().m_expr);
  }

  void operator()(const recursive_wrapper<InfixOp> &val) const
  {
    apply_visitor(Printer(m_out, m_offsetColumn, m_offsetRow), val.get().m_left);
    m_out << val.get().m_op;
    apply_visitor(Printer(m_out, m_offsetColumn, m_offsetRow), val.get().m_right);
  }

  void operator()(const recursive_wrapper<PostfixOp> &val) const
  {
    apply_visitor(Printer(m_out, m_offsetColumn, m_offsetRow), val.get().m_expr);
    m_out << val.get().m_op;
  }

  void operator()(const recursive_wrapper<Function> &val) const
  {
    m_out << val.get().m_name << '(';
    for (auto it = val.get().m_args.begin(); it != val.get().m_args.end(); ++it)
    {
      if (it != val.get().m_args.begin())
        m_out << ';';
      apply_visitor(Printer(m_out, m_offsetColumn, m_offsetRow), *it);
    }
    m_out << ')';
  }

  void operator()(const recursive_wrapper<PExpr> &val) const
  {
    m_out << '(';
    apply_visitor(Printer(m_out, m_offsetColumn, m_offsetRow), val.get().m_expr);
    m_out << ')';
  }

private:
  void formatAddress(const Address &val) const
  {
    if (val.m_table)
      m_out << get(val.m_table);
    m_out  << '.';
    if (val.m_column && int(get(val.m_column).m_coord)+m_offsetColumn>=0)
    {
      if (get(val.m_column).m_absolute)
        m_out << '$';

      unsigned column = unsigned(int(get(val.m_column).m_coord)+m_offsetColumn);
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
    }
    if (val.m_row && int(get(val.m_row).m_coord)+m_offsetRow>=0)
    {
      if (get(val.m_row).m_absolute)
        m_out << '$';
      m_out << int(get(val.m_row).m_coord)+m_offsetRow;
    }
  }

private:
  ostringstream &m_out;
  int m_offsetColumn, m_offsetRow;
};

}

namespace
{

struct Collector : public boost::static_visitor<>
{

  explicit Collector(librevenge::RVNGPropertyListVector &propsVector, const IWORKTableNameMapPtr_t &tableNameMap,
                     int offsetColumn, int offsetRow)
    : m_propsVector(propsVector)
    , m_tableNameMap(tableNameMap)
    , m_offsetColumn(offsetColumn)
    , m_offsetRow(offsetRow)
  { }

  void operator()(double val) const
  {
    librevenge::RVNGPropertyList props;
    props.insert("librevenge:type", "librevenge-number");
    props.insert("librevenge:number", val, librevenge::RVNG_GENERIC);
    m_propsVector.append(props);
  }

  void operator()(const std::string &val) const
  {
    librevenge::RVNGPropertyList props;
    props.insert("librevenge:type", "librevenge-text");
    props.insert("librevenge:text", val.c_str());
    m_propsVector.append(props);
  }

  void operator()(const TrueOrFalseFunc &val) const
  {
    librevenge::RVNGPropertyList props1;
    props1.insert("librevenge:type", "librevenge-function");
    props1.insert("librevenge:function", val.m_name.c_str());
    m_propsVector.append(props1);
    librevenge::RVNGPropertyList props2;
    props2.insert("librevenge:type", "librevenge-operator");
    props2.insert("librevenge:operator", "(");
    m_propsVector.append(props2);
    props2.insert("librevenge:operator", ")");
    m_propsVector.append(props2);
  }

  void operator()(const Address &val) const
  {

    librevenge::RVNGPropertyList props;
    props.insert("librevenge:type", "librevenge-cell");

    if (val.m_table)
    {
      const IWORKTableNameMap_t::const_iterator it = m_tableNameMap->find(get(val.m_table));
      if (m_tableNameMap->end() != it)
        props.insert("librevenge:sheet-name", (it->second).c_str());
      else
        props.insert("librevenge:sheet-name", get(val.m_table).c_str());
    }

    if (val.m_column && int(get(val.m_column).m_coord)+m_offsetColumn>0)
    {
      props.insert("librevenge:column-absolute", get(val.m_column).m_absolute);
      props.insert("librevenge:column", int(get(val.m_column).m_coord)-1+m_offsetColumn);
    }

    if (val.m_row && int(get(val.m_row).m_coord)+m_offsetRow>0)
    {
      props.insert("librevenge:row-absolute", get(val.m_row).m_absolute);
      props.insert("librevenge:row", int(get(val.m_row).m_coord)-1+m_offsetRow);
    }

    m_propsVector.append(props);
  }

  void operator()(const AddressRange &val) const
  {
    librevenge::RVNGPropertyList props;
    props.insert("librevenge:type", "librevenge-cells");

    if (val.first.m_column && int(get(val.first.m_column).m_coord)+m_offsetColumn>0)
    {
      props.insert("librevenge:start-column-absolute", get(val.first.m_column).m_absolute);
      props.insert("librevenge:start-column", int(get(val.first.m_column).m_coord)-1+m_offsetColumn);
    }
    if (val.first.m_row && int(get(val.first.m_row).m_coord)+m_offsetRow>0)
    {
      props.insert("librevenge:start-row-absolute", get(val.first.m_row).m_absolute);
      props.insert("librevenge:start-row", int(get(val.first.m_row).m_coord)-1+m_offsetRow);
    }
    if (val.second.m_column && int(get(val.second.m_column).m_coord)+m_offsetColumn>0)
    {
      props.insert("librevenge:end-column-absolute", get(val.second.m_column).m_absolute);
      props.insert("librevenge:end-column", int(get(val.second.m_column).m_coord)-1+m_offsetColumn);
    }
    if (val.second.m_row && int(get(val.second.m_row).m_coord)+m_offsetRow>0)
    {
      props.insert("librevenge:end-row-absolute", get(val.second.m_row).m_absolute);
      props.insert("librevenge:end-row", int(get(val.second.m_row).m_coord)-1+m_offsetRow);
    }
    m_propsVector.append(props);
  }

  void operator()(const recursive_wrapper<PrefixOp> &val) const
  {
    librevenge::RVNGPropertyList props;
    props.insert("librevenge:type", "librevenge-operator");
    props.insert("librevenge:operator", val.get().m_op);
    m_propsVector.append(props);
    apply_visitor(Collector(m_propsVector, m_tableNameMap, m_offsetColumn, m_offsetRow), val.get().m_expr);
  }

  void operator()(const recursive_wrapper<InfixOp> &val) const
  {
    apply_visitor(Collector(m_propsVector, m_tableNameMap, m_offsetColumn, m_offsetRow), val.get().m_left);
    librevenge::RVNGPropertyList props;
    props.insert("librevenge:type", "librevenge-operator");
    props.insert("librevenge:operator", val.get().m_op.c_str());
    m_propsVector.append(props);
    apply_visitor(Collector(m_propsVector, m_tableNameMap, m_offsetColumn, m_offsetRow), val.get().m_right);
  }

  void operator()(const recursive_wrapper<PostfixOp> &val) const
  {
    apply_visitor(Collector(m_propsVector, m_tableNameMap, m_offsetColumn, m_offsetRow), val.get().m_expr);
    librevenge::RVNGPropertyList props;
    props.insert("librevenge:type", "librevenge-operator");
    std::string op;
    op+=val.get().m_op;
    props.insert("librevenge:operator", op.c_str());
    m_propsVector.append(props);
  }

  void operator()(const recursive_wrapper<Function> &val) const
  {
    librevenge::RVNGPropertyList props1;
    props1.insert("librevenge:type", "librevenge-function");
    props1.insert("librevenge:function", val.get().m_name.c_str());
    m_propsVector.append(props1);

    librevenge::RVNGPropertyList props2;
    props2.insert("librevenge:type", "librevenge-operator");
    props2.insert("librevenge:operator", "(");
    m_propsVector.append(props2);

    librevenge::RVNGPropertyList props;
    props.insert("librevenge:type", "librevenge-operator");
    props.insert("librevenge:operator", ";");

    for (auto it = val.get().m_args.begin(); it != val.get().m_args.end(); ++it)
    {
      if (it != val.get().m_args.begin())
        m_propsVector.append(props);
      apply_visitor(Collector(m_propsVector, m_tableNameMap, m_offsetColumn, m_offsetRow), *it);
    }
    librevenge::RVNGPropertyList props3;
    props3.insert("librevenge:type", "librevenge-operator");
    props3.insert("librevenge:operator", ")");
    m_propsVector.append(props3);
  }

  void operator()(const recursive_wrapper<PExpr> &val) const
  {
    librevenge::RVNGPropertyList props;
    props.insert("librevenge:type", "librevenge-operator");
    props.insert("librevenge:operator", "(");
    m_propsVector.append(props);

    apply_visitor(Collector(m_propsVector, m_tableNameMap, m_offsetColumn, m_offsetRow), val.get().m_expr);

    librevenge::RVNGPropertyList props1;
    props1.insert("librevenge:type", "librevenge-operator");
    props1.insert("librevenge:operator", ")");
    m_propsVector.append(props1);
  }


private:
  librevenge::RVNGPropertyListVector &m_propsVector;
  const IWORKTableNameMapPtr_t &m_tableNameMap;
  int m_offsetColumn, m_offsetRow;
};

}

struct IWORKFormula::Impl
{
  Expression m_formula;
};

IWORKFormula::IWORKFormula(const boost::optional<unsigned> &hc)
  : m_impl(new Impl())
  , m_hc(hc)
{
}

bool IWORKFormula::parse(const std::string &formula)
{
  FormulaGrammar<string::const_iterator> grammar;
  string::const_iterator it = formula.begin();
  string::const_iterator end = formula.end();
  const bool r = qi::phrase_parse(it, end, grammar, ascii::space, m_impl->m_formula);
  if (!r || it!=end)
  {
    ETONYEK_DEBUG_MSG(("IWORKFormula::parse: can not parse %s\n", formula.c_str()));
    return false;
  }
  return true;
}

const std::string IWORKFormula::str(const boost::optional<unsigned> &hc) const
{
  ostringstream out;
  out << '=';
  int offsetCol=0, offsetRow=0;
  if (!computeOffset(hc, offsetCol, offsetRow))
    offsetCol=offsetRow=0;
  apply_visitor(Printer(out, offsetCol, offsetRow), m_impl->m_formula);
  return out.str();
}

void IWORKFormula::write(const boost::optional<unsigned> &hc, librevenge::RVNGPropertyListVector &formula, const IWORKTableNameMapPtr_t &tableNameMap) const
{
  int offsetCol=0, offsetRow=0;
  if (!computeOffset(hc, offsetCol, offsetRow))
    offsetCol=offsetRow=0;
  apply_visitor(Collector(formula, tableNameMap, offsetCol, offsetRow), m_impl->m_formula);
}

bool IWORKFormula::computeOffset(const boost::optional<unsigned> &hc, int &offsetColumn, int &offsetRow) const
{
  offsetColumn=offsetRow=0;
  if (!m_hc && !hc)
    return true;
  if (!m_hc || !hc)
  {
    ETONYEK_DEBUG_MSG(("IWORKFormula::parse: called without cell positions\n"));
    return false;
  }
  if (get(m_hc)==get(hc)) return true;
  int prevRow=(int) get(m_hc)/256, prevColumn=(int) get(m_hc)%256;
  int row=(int) get(hc)/256, column=(int) get(hc)%256;
  offsetColumn=column-prevColumn;
  offsetRow=row-prevRow;
  return true;
}

} // namespace libetonyek

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
