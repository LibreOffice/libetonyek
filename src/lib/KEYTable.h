/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYTABLE_H_INCLUDED
#define KEYTABLE_H_INCLUDED

#include <deque>

#include "KEYObject.h"

namespace libetonyek
{

class KEYTable
{
  struct Cell
  {
    KEYObjectPtr_t content;
    unsigned columnSpan;
    unsigned rowSpan;
    bool covered;

    Cell();
  };

  typedef std::deque<Cell> Row_t;
  typedef std::deque<Row_t> Table_t;

public:
  typedef std::deque<double> ColumnSizes_t;
  typedef std::deque<double> RowSizes_t;

public:
  KEYTable();

  void setSizes(const ColumnSizes_t &columnSizes, const RowSizes_t &rowSizes);
  void insertCell(unsigned column, unsigned row, const KEYObjectPtr_t &content = KEYObjectPtr_t(), unsigned columnSpan = 1, unsigned rowSpan = 1);
  void insertCoveredCell(unsigned column, unsigned row);

  void setGeometry(const KEYGeometryPtr_t &geometry);

  void draw(const KEYOutput &output) const;

private:
  Table_t m_table;
  ColumnSizes_t m_columnSizes;
  RowSizes_t m_rowSizes;
  KEYGeometryPtr_t m_geometry;
};

KEYObjectPtr_t makeObject(const KEYTable &table);

}

#endif //  KEYTABLE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
