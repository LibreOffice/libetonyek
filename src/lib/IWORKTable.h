/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTABLE_H_INCLUDED
#define IWORKTABLE_H_INCLUDED

#include <boost/optional.hpp>

#include <deque>

#include <glm/glm.hpp>

#include "IWORKTypes_fwd.h"
#include "IWORKOutputElements.h"

namespace libetonyek
{

class IWORKTable
{
  struct Cell
  {
    IWORKOutputElements m_content;
    unsigned m_columnSpan;
    unsigned m_rowSpan;
    bool m_covered;

    Cell();
  };

  typedef std::deque<Cell> Row_t;
  typedef std::deque<Row_t> Table_t;

public:
  IWORKTable();

  void setSizes(const IWORKColumnSizes_t &columnSizes, const IWORKRowSizes_t &rowSizes);
  void insertCell(unsigned column, unsigned row, const IWORKOutputElements &content = IWORKOutputElements(), unsigned columnSpan = 1, unsigned rowSpan = 1);
  void insertCoveredCell(unsigned column, unsigned row);

  void setGeometry(const IWORKGeometryPtr_t &geometry);

  void draw(const glm::dmat3 &trafo, IWORKOutputElements &elements) const;

private:
  Table_t m_table;
  IWORKColumnSizes_t m_columnSizes;
  IWORKRowSizes_t m_rowSizes;
  IWORKGeometryPtr_t m_geometry;
};

}

#endif // IWORKTABLE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
