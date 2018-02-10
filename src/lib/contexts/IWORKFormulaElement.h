/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKFORMULAELEMENT_H_INCLUDED
#define IWORKFORMULAELEMENT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKXMLContextBase.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{
//! class to parse the sf:fo element, ie. formula element
class IWORKFoElement : public IWORKXMLEmptyContextBase
{
public:
  explicit IWORKFoElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
  const boost::optional<ID_t> &getId() const;

private:
  boost::optional<ID_t> m_id;
  boost::optional<std::string> m_formula;
  boost::optional<unsigned> m_hc;
};

//! class to parse the sf:formula element
class IWORKFormulaElement : public IWORKXMLEmptyContextBase
{
public:
  explicit IWORKFormulaElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
  const boost::optional<ID_t> &getId() const;

private:
  boost::optional<ID_t> m_id;
  boost::optional<std::string> m_formula;
  boost::optional<unsigned> m_hc;
};

//! class to parse the sf:TableCellFormula element
class IWORKTableCellFormulaElement : public IWORKXMLEmptyContextBase
{
public:
  explicit IWORKTableCellFormulaElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
  const boost::optional<ID_t> &getId() const;

private:
  boost::optional<ID_t> m_id;
  boost::optional<std::string> m_formula;
  boost::optional<std::string> m_tableId;
};

//! class to parse the sf:of element, ie formula offset element
class IWORKOfElement : public IWORKXMLEmptyContextBase
{
public:
  explicit IWORKOfElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

  boost::optional<ID_t> m_ref;
  boost::optional<unsigned> m_hc;
};
}

#endif // IWORKFORMULAELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
