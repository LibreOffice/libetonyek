/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY1TABLEELEMENT_H_INCLUDED
#define KEY1TABLEELEMENT_H_INCLUDED

#include <string>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include "KEY1XMLContextBase.h"

#include "IWORKTypes.h"

namespace libetonyek
{
class IWORKText;
class KEY1ParserState;

class KEY1TableElement : public KEY1XMLElementContextBase
{
public:
  struct TableData;

  explicit KEY1TableElement(KEY1ParserState &state, boost::optional<IWORKSize> &size);

protected:
  virtual void attribute(int name, const char *value);
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  boost::optional<std::string> m_key;
  boost::optional<IWORKSize> &m_size;
  boost::shared_ptr<TableData> m_tableData;
};

}

#endif // KEY1TABLEELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
