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
#include <string>

#include "IWORKTypes_fwd.h"
#include "libetonyek_utils.h"

namespace libetonyek
{

class IWORKFormula
{
  struct Impl;

public:
  IWORKFormula();

  bool parse(const std::string &formula);

  const std::string str() const;

  void write(librevenge::RVNGPropertyListVector &formula, const IWORKTableNameMapPtr_t &tableNameMap) const;

private:
  std::shared_ptr<Impl> m_impl;
};

} // namespace libetonyek

#endif // IWORKFORMULA_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
