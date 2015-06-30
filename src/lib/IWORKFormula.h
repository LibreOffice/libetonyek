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

#include <string>

#include <boost/optional/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include <libetonyek_utils.h>

#include "IWORKOutputElements.h"
#include "IWORKTypes.h"

namespace libetonyek
{

class IWORKFormula
{
  struct Impl;

public:
  IWORKFormula();

  bool parse(const std::string &formula, boost::optional<IWORKTableNameMap_t &> tableNameMap = boost::optional<IWORKTableNameMap_t &>());

  const std::string toString() const;

  void write(librevenge::RVNGPropertyListVector &formula);

private:
  boost::shared_ptr<Impl> m_impl;
  IWORKOutputElements m_elements;
};

} // namespace libetonyek

#endif // IWORKFORMULA_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
