/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKHEADERFOOTERCONTEXT_H_INCLUDED
#define IWORKHEADERFOOTERCONTEXT_H_INCLUDED

#include <functional>
#include <string>

#include <boost/optional.hpp>

#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKHeaderFooterContext : public IWORKXMLElementContextBase
{
public:
  typedef std::function<void(const std::string &)> CollectFunction_t;

  IWORKHeaderFooterContext(IWORKXMLParserState &state, const CollectFunction_t &collect);

private:
  virtual void startOfElement();
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  const CollectFunction_t m_collect;
  boost::optional<std::string> m_name;
};

}

#endif // IWORKHEADERFOOTERCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
