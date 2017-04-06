/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKDISCARDCONTEXT_H_INCLUDED
#define IWORKDISCARDCONTEXT_H_INCLUDED

#include <memory>

#include "IWORKXMLContext.h"

namespace libetonyek
{

class IWORKXMLParserState;

class IWORKDiscardContext : public IWORKXMLContext, public std::enable_shared_from_this<IWORKDiscardContext>
{
  struct Data;

public:
  explicit IWORKDiscardContext(IWORKXMLParserState &state);

protected:
  virtual void startOfElement();
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void text(const char *value);
  virtual void endOfElement();

private:
  IWORKXMLParserState &m_state;
  unsigned m_level;
  bool m_enableCollector;
  std::shared_ptr<Data> m_data;
};

}

#endif // IWORKDISCARDCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
