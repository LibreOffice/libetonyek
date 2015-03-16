/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYCOLLECTOR_H_INCLUDED
#define KEYCOLLECTOR_H_INCLUDED

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
#include "KEYTypes.h"
#include "KEYTypes_fwd.h"

namespace libetonyek
{

class IWORKDocumentInterface;
class IWORKPropertyMap;
struct IWORKSize;

class KEYCollector
{
  struct Level
  {
    IWORKGeometryPtr_t m_geometry;
    IWORKStylePtr_t m_graphicStyle;
    IWORKTransformation m_trafo;

    Level();
  };

public:
  explicit KEYCollector(IWORKDocumentInterface *document);
  ~KEYCollector();

  // collector functions

  void collectCellStyle(const IWORKStylePtr_t &style, bool anonymous);
  void collectCharacterStyle(const IWORKStylePtr_t &style, bool anonymous);
  void collectConnectionStyle(const IWORKStylePtr_t &style, bool anonymous);
  void collectGraphicStyle(const IWORKStylePtr_t &style, bool anonymous);
  void collectLayoutStyle(const IWORKStylePtr_t &style, bool anonymous);
  void collectListStyle(const IWORKStylePtr_t &style, bool anonymous);
  void collectParagraphStyle(const IWORKStylePtr_t &style, bool anonymous);
  void collectPlaceholderStyle(const IWORKStylePtr_t &style, bool anonymous);
  void collectSlideStyle(const IWORKStylePtr_t &style, bool anonymous);
  void collectTabularStyle(const IWORKStylePtr_t &style, bool anonymous);
  void collectVectorStyle(const IWORKStylePtr_t &style, bool anonymous);

  void collectGeometry(const IWORKGeometryPtr_t &geometry);

  void collectBezier(const IWORKPathPtr_t &path);
  void collectGroup(const IWORKGroupPtr_t &group);
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

  void collectData(const IWORKDataPtr_t &data);
  IWORKMediaContentPtr_t collectUnfiltered(const boost::optional<IWORKSize> &size);
  void insertUnfiltered(const IWORKMediaContentPtr_t &content);
  void collectFiltered(const boost::optional<IWORKSize> &size);
  void collectLeveled(const boost::optional<IWORKSize> &size);
  IWORKMediaContentPtr_t collectFilteredImage();
  void insertFilteredImage(const IWORKMediaContentPtr_t &content);
  void collectMovieMedia();
  void collectMedia();

  void collectPresentation(const boost::optional<IWORKSize> &size);

  KEYLayerPtr_t collectLayer();
  void insertLayer(const KEYLayerPtr_t &layer);
  void collectPage();
  IWORKStylesheetPtr_t collectStylesheet(const IWORKStylesheetPtr_t &parent);

  void collectText(const IWORKStylePtr_t &style, const std::string &text);
  void collectTab();
  void collectLineBreak();

  KEYPlaceholderPtr_t collectTextPlaceholder(const IWORKStylePtr_t &style, bool title);
  void insertTextPlaceholder(const KEYPlaceholderPtr_t &placeholder);

  void collectTableSizes(const IWORKTable::RowSizes_t &rowSizes, const IWORKTable::ColumnSizes_t &columnSizes);
  void collectTableCell(unsigned row, unsigned column, const boost::optional<std::string> &content, unsigned rowSpan, unsigned columnSpan);
  void collectCoveredTableCell(unsigned row, unsigned column);
  void collectTableRow();
  void collectTable();

  void collectNote();

  void collectStickyNote();

  void startSlides();
  void endSlides();
  void startThemes();
  void endThemes();

  void startPage();
  void endPage();
  void startLayer();
  void endLayer();
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

private:
  void pushStyle(const IWORKStylePtr_t &style);
  void resolveStyle(IWORKStyle &style);

  void drawNotes();
  void drawStickyNotes();

  void pushZone();
  void popZone();

private:
  IWORKDocumentInterface *m_document;
  std::stack<Level> m_levelStack;
  IWORKStyleStack m_styleStack;
  IWORKPathPtr_t m_currentPath;

  IWORKTextPtr_t m_currentText;

  IWORKStylesheetPtr_t m_currentStylesheet;
  std::deque<IWORKStylePtr_t> m_newStyles;

  IWORKDataPtr_t m_currentData;
  IWORKMediaContentPtr_t m_currentUnfiltered;
  IWORKMediaContentPtr_t m_currentFiltered;
  IWORKMediaContentPtr_t m_currentLeveled;
  IWORKMediaContentPtr_t m_currentContent;

  IWORKTable m_currentTable;

  IWORKZoneList_t m_zoneList;
  std::stack<IWORKZoneID_t> m_zoneStack;
  IWORKOutputElements *m_currentZone;
  IWORKOutputElements m_notes;

  KEYStickyNotes_t m_stickyNotes;

  IWORKSize m_size;

  bool m_paint;
  bool m_pageOpened;
  bool m_layerOpened;
  int m_groupLevel;
  int m_layerCount;
};

} // namespace libetonyek

#endif // KEYCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
