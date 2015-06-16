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

#include <glm/glm.hpp>

#include "libetonyek_utils.h"
#include "IWORKPath_fwd.h"
#include "IWORKStyle.h"
#include "IWORKStyleStack.h"
#include "IWORKStylesheet.h"
#include "IWORKTable.h"
#include "IWORKText_fwd.h"
#include "IWORKTransformation.h"
#include "IWORKTypes.h"
#include "IWORKOutputManager.h"

namespace libetonyek
{

class IWORKDocumentInterface;
class IWORKPropertyMap;
struct IWORKSize;

class IWORKCollector
{
private:
  struct Level
  {
    IWORKGeometryPtr_t m_geometry;
    IWORKStylePtr_t m_graphicStyle;
    glm::dmat3 m_trafo;

    Level();
  };

public:
  explicit IWORKCollector(IWORKDocumentInterface *document);
  ~IWORKCollector();

  // collector functions

  void collectStyle(const IWORKStylePtr_t &style);

  void setGraphicStyle(const IWORKStylePtr_t &style);

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

  void collectStylesheet(const IWORKStylesheetPtr_t &stylesheet);

  void collectText(const std::string &text);
  void collectTab();
  void collectLineBreak();

  void collectTableSizes(const IWORKRowSizes_t &rowSizes, const IWORKColumnSizes_t &columnSizes);
  void collectTableBorders(const IWORKGridLineList_t &verticalLines, const IWORKGridLineList_t &horizontalLines);
  void collectTableCell(unsigned row, unsigned column, const boost::optional<std::string> &content, unsigned rowSpan, unsigned columnSpan);
  void collectCoveredTableCell(unsigned row, unsigned column);
  void collectTableRow();
  void collectTable();

  void collectMetadata(const IWORKMetadata &metadata);

  void collectHeader(const std::string &name);
  void collectFooter(const std::string &name);

  void startDocument();
  void endDocument();

  void startGroup();
  void endGroup();

  void startParagraph(const IWORKStylePtr_t &style);
  void endParagraph();
  void openSpan(const IWORKStylePtr_t &style);
  void closeSpan();
  void openLink(const std::string &url);
  void closeLink();
  void startText();
  void endText();

  void startLevel();
  void endLevel();

  void pushStyle();
  void popStyle();

  void pushStylesheet(const IWORKStylesheetPtr_t &stylesheet);
  void popStylesheet();

  IWORKOutputManager &getOutputManager();

protected:
  void fillMetadata(librevenge::RVNGPropertyList &props);

private:
  void pushStyle(const IWORKStylePtr_t &style);
  void resolveStyle(IWORKStyle &style);

  void collectHeaderFooter(const std::string &name, IWORKHeaderFooterMap_t &map);

  virtual void drawTable() = 0;

protected:
  IWORKDocumentInterface *m_document;

  std::stack<Level> m_levelStack;
  IWORKStyleStack m_styleStack;
  std::stack<IWORKStylesheetPtr_t> m_stylesheetStack;
  IWORKOutputManager m_outputManager;

  std::deque<IWORKStylePtr_t> m_newStyles;

  IWORKTextPtr_t m_currentText;
  IWORKTable m_currentTable;

  IWORKHeaderFooterMap_t m_headers;
  IWORKHeaderFooterMap_t m_footers;

private:
  IWORKPathPtr_t m_currentPath;
  IWORKDataPtr_t m_currentData;
  IWORKMediaContentPtr_t m_currentUnfiltered;
  IWORKMediaContentPtr_t m_currentFiltered;
  IWORKMediaContentPtr_t m_currentLeveled;
  IWORKMediaContentPtr_t m_currentContent;

  IWORKMetadata m_metadata;

  int m_groupLevel;
};

} // namespace libetonyek

#endif // IWORKCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
