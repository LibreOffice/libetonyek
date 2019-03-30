/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTABULARMODELELEMENT_H_INCLUDED
#define IWORKTABULARMODELELEMENT_H_INCLUDED

#include <string>

#include <boost/optional.hpp>

#include "IWORKStyle_fwd.h"

#include "IWORKXMLContextBase.h"

namespace libetonyek
{
class IWORKTable;

class IWORKTabularModelElement : public IWORKXMLElementContextBase
{
public:
  explicit IWORKTabularModelElement(IWORKXMLParserState &state, bool isDefinition=false);

private:
  void attribute(int name, const char *value) override;
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  void sendStyle(const IWORKStylePtr_t &style, const std::shared_ptr<IWORKTable> &table);

private:
  bool m_isDefinition;
  boost::optional<ID_t> m_id;
  boost::optional<std::string> m_tableName;
  boost::optional<std::string> m_tableId;
  boost::optional<ID_t> m_styleRef;
  boost::optional<unsigned> m_headerColumns;
  boost::optional<unsigned> m_headerRows;
  boost::optional<unsigned> m_footerRows;
};

}

#endif // IWORKTABULARMODELELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
