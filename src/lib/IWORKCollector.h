/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKCOLLECTOR_H_INCLUDED
#define IWORKCOLLECTOR_H_INCLUDED

#include <deque>
#include <stack>
#include <string>

#include <boost/optional.hpp>

#include "libetonyek_utils.h"
#include "IWORKObject.h"
#include "IWORKPath_fwd.h"
#include "IWORKStyle.h"
#include "IWORKStyleStack.h"
#include "IWORKStylesheet.h"
#include "IWORKTable.h"
#include "IWORKText_fwd.h"
#include "IWORKTransformation.h"
#include "IWORKTypes.h"
#include "IWORKZoneManager.h"

namespace libetonyek
{

class IWORKDocumentInterface;
class IWORKPropertyMap;
struct IWORKSize;

class IWORKCollector
{
  struct Level
  {
    IWORKGeometryPtr_t m_geometry;
    IWORKStylePtr_t m_graphicStyle;
    IWORKTransformation m_trafo;

    Level();
  };

public:
  explicit IWORKCollector(IWORKDocumentInterface *document);
  ~IWORKCollector();

  // collector functions

  void collectStyle(const IWORKStylePtr_t &style, bool anonymous);

  void setGraphicStyle(const IWORKStylePtr_t &style);
  void setLayoutStyle(const IWORKStylePtr_t &style);

  void collectGeometry(const IWORKGeometryPtr_t &geometry);

  void collectBezier(const IWORKPathPtr_t &path);
  void collectImage(const IWORKImagePtr_t &image);
  void collectLine(const IWORKLinePtr_t &line);
  void collectShape();

  void collectBezierPath();
  void collectPolygonPath(const IWORKSize &size, unsigned edges);
  void collectRoundedRectanglePath(const IWORKSize &size, double radius);
  void collectArrowPath(const IWORKSize &size, double headWidth, double stemRelYPos, bool doubleSided);
  void collectStarPath(const IWORKSize &size, unsigned points, double innerRadius);
  void collectConnectionPath(const IWORKSize &size, double middleX, double middleY);
  void collectCalloutPath(const IWORKSize &size, double radius, double tailSize, double tailX, double tailY, bool quoteBubble);

  void collectMedia(const IWORKMediaContentPtr_t &content);

  IWORKStylesheetPtr_t collectStylesheet(const IWORKStylesheetPtr_t &parent);

  void collectText(const IWORKStylePtr_t &style, const std::string &text);
  void collectTab();
  void collectLineBreak();

  void collectTableSizes(const IWORKTable::RowSizes_t &rowSizes, const IWORKTable::ColumnSizes_t &columnSizes);
  void collectTableCell(unsigned row, unsigned column, const boost::optional<std::string> &content, unsigned rowSpan, unsigned columnSpan);
  void collectCoveredTableCell(unsigned row, unsigned column);
  void collectTableRow();
  void collectTable();

  void startGroup();
  void endGroup();

  void startParagraph(const IWORKStylePtr_t &style);
  void endParagraph();
  void startText(bool object);
  void endText();

  void startLevel();
  void endLevel();

  void pushStyle();
  void popStyle();

protected:
  IWORKZoneManager &getZoneManager();

private:
  void pushStyle(const IWORKStylePtr_t &style);
  void resolveStyle(IWORKStyle &style);

protected:
  IWORKDocumentInterface *m_document;

  std::stack<Level> m_levelStack;
  IWORKStyleStack m_styleStack;
  IWORKZoneManager m_zoneManager;

  IWORKStylesheetPtr_t m_currentStylesheet;
  std::deque<IWORKStylePtr_t> m_newStyles;

  IWORKTextPtr_t m_currentText;

private:
  IWORKPathPtr_t m_currentPath;
  IWORKDataPtr_t m_currentData;
  IWORKMediaContentPtr_t m_currentUnfiltered;
  IWORKMediaContentPtr_t m_currentFiltered;
  IWORKMediaContentPtr_t m_currentLeveled;
  IWORKMediaContentPtr_t m_currentContent;
  IWORKTable m_currentTable;

  int m_groupLevel;
};

} // namespace libetonyek

#endif // IWORKCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
