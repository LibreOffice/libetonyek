/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKFORMULA_H_INCLUDED
#define IWORKFORMULA_H_INCLUDED

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include "IWORKTypes_fwd.h"
#include "libetonyek_utils.h"

namespace libetonyek
{

class IWORKFormula
{
  struct Impl;

public:
  struct Token;

  explicit IWORKFormula(const boost::optional<unsigned> &hc);

  bool parse(const std::string &formula);
  bool parse(const std::vector<Token> &formula);

  void write(const boost::optional<unsigned> &hc, librevenge::RVNGPropertyListVector &formula, const IWORKTableNameMapPtr_t &tableNameMap) const;
  const std::string str(const boost::optional<unsigned> &hc) const;

public:
  struct Coord
  {
    Coord()
      : m_coord(0)
      , m_absolute(false)
    {
    }
    int m_coord;
    bool m_absolute;
  };

  struct Address
  {
    Address()
      : m_column()
      , m_row()
      , m_table()
    {
    }
    boost::optional<Coord> m_column;
    boost::optional<Coord> m_row;
    boost::optional<std::string> m_table;
    friend std::ostream &operator<<(std::ostream &s, Address const &ad);
  };

  //! small structure used by IWAParser to create formula
  struct Token
  {
    enum Type
    {
      Cell, Double, Function, Operator, String
    };
    explicit Token(Type type)
      : m_type(type)
      , m_string()
      , m_value(0)
      , m_address()
    {
    }
    explicit Token(double const &val)
      : m_type(Double)
      , m_string()
      , m_value(val)
      , m_address()
    {
    }
    Token(std::string const &name, Type type)
      : m_type(type)
      , m_string(name)
      , m_value(0)
      , m_address()
    {
    }
    explicit Token(IWORKFormula::Address const &address)
      : m_type(Cell)
      , m_string()
      , m_value()
      , m_address(address)
    {
    }
    friend std::ostream &operator<<(std::ostream &s, Token const &dt);
    Type m_type;
    std::string m_string;
    double m_value;
    IWORKFormula::Address m_address;
  };

private:
  bool computeOffset(const boost::optional<unsigned> &hc, int &offsetColumn, int &offsetRow) const;
  std::shared_ptr<Impl> m_impl;
  boost::optional<unsigned> m_hc;
};

} // namespace libetonyek

#endif // IWORKFORMULA_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
