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

#include <IWORKFormula.h>

namespace libetonyek
{

class IWORKChart
{

  typedef std::deque<IWORKFormula> Formula_t;

public:
  IWORKChart();

  // void draw(const librevenge::RVNGPropertyList &tableProps, IWORKOutputElements &elements);

  unsigned m_chartType;
  std::deque<std::string> m_rowNames;
  std::deque<std::string> m_columnNames;
  std::string m_chartName;
  std::string m_valueTitle;
  std::string m_categoryTitle;
  Formula_t m_dataFormulas;
  Formula_t m_rowLabelFormulas;
  Formula_t m_columnLabelFormulas;

};

}

#endif // IWORKCHART_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
