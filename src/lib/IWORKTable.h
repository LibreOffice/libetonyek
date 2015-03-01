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

#include "IWORKObject.h"
#include "IWORKTypes_fwd.h"

namespace libetonyek
{

class IWORKTransformation;

class IWORKTable
{
  struct Cell
  {
    IWORKObjectPtr_t m_content;
    unsigned m_columnSpan;
    unsigned m_rowSpan;
    bool m_covered;

    Cell();
  };

  typedef std::deque<Cell> Row_t;
  typedef std::deque<Row_t> Table_t;

public:
  typedef std::deque<double> ColumnSizes_t;
  typedef std::deque<double> RowSizes_t;

public:
  IWORKTable();

  void setSizes(const ColumnSizes_t &columnSizes, const RowSizes_t &rowSizes);
  void insertCell(unsigned column, unsigned row, const IWORKObjectPtr_t &content = IWORKObjectPtr_t(), unsigned columnSpan = 1, unsigned rowSpan = 1);
  void insertCoveredCell(unsigned column, unsigned row);

  void setGeometry(const IWORKGeometryPtr_t &geometry);

  void draw(librevenge::RVNGPresentationInterface *painter, const IWORKTransformation &trafo) const;

private:
  Table_t m_table;
  ColumnSizes_t m_columnSizes;
  RowSizes_t m_rowSizes;
  IWORKGeometryPtr_t m_geometry;
};

IWORKObjectPtr_t makeObject(const IWORKTable &table, const IWORKTransformation &trafo);

}

#endif //  KEYTABLE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
