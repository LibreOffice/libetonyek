/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTABLERECORDER_H_INCLUDED
#define IWORKTABLERECORDER_H_INCLUDED

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include "IWORKStyle_fwd.h"
#include "IWORKTable.h"
#include "IWORKTypes.h"

namespace libetonyek
{

class IWORKText;

class IWORKTableRecorder
{
public:
  IWORKTableRecorder();

  void replay(IWORKTable &table) const;

  void setSize(unsigned columns, unsigned rows);
  void setHeaders(unsigned headerColumns, unsigned headerRows, unsigned footerRows);
  void setBandedRows(bool banded);
  void setRepeated(bool columns, bool rows);

  void setStyle(const IWORKStylePtr_t &style);
  void setSizes(const IWORKColumnSizes_t &columnSizes, const IWORKRowSizes_t &rowSizes);
  void setBorders(const IWORKGridLineList_t &verticalLines, const IWORKGridLineList_t &horizontalLines);
  void insertCell(unsigned column, unsigned row, const boost::optional<std::string> &value, const boost::shared_ptr<IWORKText> &content, unsigned columnSpan, unsigned rowSpan, const boost::optional<IWORKFormula> &formula, const IWORKStylePtr_t &style, IWORKCellType type);
  void insertCoveredCell(unsigned column, unsigned row);

  void setDefaultCellStyle(IWORKTable::CellType type, const IWORKStylePtr_t &style);
  void setDefaultLayoutStyle(IWORKTable::CellType type, const IWORKStylePtr_t &style);
  void setDefaultParagraphStyle(IWORKTable::CellType type, const IWORKStylePtr_t &style);

private:
  struct Impl;

private:
  boost::shared_ptr<Impl> m_impl;
};

}

#endif // IWORKTABLERECORDER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
