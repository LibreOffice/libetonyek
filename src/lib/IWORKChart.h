/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKCHART_H_INCLUDED
#define IWORKCHART_H_INCLUDED

#include <deque>
#include <string>

#include <boost/optional.hpp>

namespace libetonyek
{

class IWORKChart
{

public:
  IWORKChart();

  unsigned m_chartType;
  std::deque<std::string> m_rowNames;
  std::deque<std::string> m_columnNames;
  boost::optional<std::string> m_chartName;
  boost::optional<std::string> m_valueTitle;
  boost::optional<std::string> m_categoryTitle;

};

}

#endif // IWORKCHART_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
