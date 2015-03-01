/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYCOLLECTORBASE_H_INCLUDED
#define KEYCOLLECTORBASE_H_INCLUDED

#include <deque>
#include <stack>

#include "IWORKObject.h"
#include "IWORKStyle.h"
#include "IWORKStylesheet.h"
#include "IWORKStyleStack.h"
#include "IWORKText_fwd.h"
#include "IWORKTransformation.h"
#include "KEYCollector.h"
#include "KEYTypes.h"

namespace libetonyek
{

/** Implementation of common parts of styles and content collector.
  *
  * It collects styles and drawable objects.
  */
class KEYCollectorBase : public KEYCollector
{
  struct Level
  {
    IWORKGeometryPtr_t m_geometry;
    IWORKStylePtr_t m_graphicStyle;
    IWORKTransformation m_trafo;

    Level();
  };

public:
  KEYCollectorBase();
  ~KEYCollectorBase();

  // collector functions

  virtual void collectCellStyle(const IWORKStylePtr_t &style, bool anonymous);
  virtual void collectCharacterStyle(const IWORKStylePtr_t &style, bool anonymous);
  virtual void collectConnectionStyle(const IWORKStylePtr_t &style, bool anonymous);
  virtual void collectGraphicStyle(const IWORKStylePtr_t &style, bool anonymous);
  virtual void collectLayoutStyle(const IWORKStylePtr_t &style, bool anonymous);
  virtual void collectListStyle(const IWORKStylePtr_t &style, bool anonymous);
  virtual void collectParagraphStyle(const IWORKStylePtr_t &style, bool anonymous);
  virtual void collectPlaceholderStyle(const IWORKStylePtr_t &style, bool anonymous);
  virtual void collectSlideStyle(const IWORKStylePtr_t &style, bool anonymous);
  virtual void collectTabularStyle(const IWORKStylePtr_t &style, bool anonymous);
  virtual void collectVectorStyle(const IWORKStylePtr_t &style, bool anonymous);

  virtual void collectGeometry(const IWORKGeometryPtr_t &geometry);

  virtual void collectBezier(const IWORKPathPtr_t &path);
  virtual void collectGroup(const IWORKGroupPtr_t &group);
  virtual void collectImage(const IWORKImagePtr_t &image);
  virtual void collectLine(const IWORKLinePtr_t &line);
  virtual void collectShape();

  virtual void collectBezierPath();
  virtual void collectPolygonPath(const IWORKSize &size, unsigned edges);
  virtual void collectRoundedRectanglePath(const IWORKSize &size, double radius);
  virtual void collectArrowPath(const IWORKSize &size, double headWidth, double stemRelYPos, bool doubleSided);
  virtual void collectStarPath(const IWORKSize &size, unsigned points, double innerRadius);
  virtual void collectConnectionPath(const IWORKSize &size, double middleX, double middleY);
  virtual void collectCalloutPath(const IWORKSize &size, double radius, double tailSize, double tailX, double tailY, bool quoteBubble);

  virtual void collectData(const IWORKDataPtr_t &data);
  virtual IWORKMediaContentPtr_t collectUnfiltered(const boost::optional<IWORKSize> &size);
  virtual void insertUnfiltered(const IWORKMediaContentPtr_t &content);
  virtual void collectFiltered(const boost::optional<IWORKSize> &size);
  virtual void collectLeveled(const boost::optional<IWORKSize> &size);
  virtual IWORKMediaContentPtr_t collectFilteredImage();
  virtual void insertFilteredImage(const IWORKMediaContentPtr_t &content);
  virtual void collectMovieMedia();
  virtual void collectMedia();

  virtual KEYLayerPtr_t collectLayer();
  virtual IWORKStylesheetPtr_t collectStylesheet(const IWORKStylesheetPtr_t &parent);

  virtual void collectText(const IWORKStylePtr_t &style, const std::string &text);
  virtual void collectTab();
  virtual void collectLineBreak();

  virtual KEYPlaceholderPtr_t collectTextPlaceholder(const IWORKStylePtr_t &style, bool title);
  virtual void insertTextPlaceholder(const KEYPlaceholderPtr_t &placeholder);

  virtual void collectTableSizes(const IWORKTable::RowSizes_t &rowSizes, const IWORKTable::ColumnSizes_t &columnSizes);
  virtual void collectTableCell(unsigned row, unsigned column, const boost::optional<std::string> &content, unsigned rowSpan, unsigned columnSpan);
  virtual void collectCoveredTableCell(unsigned row, unsigned column);
  virtual void collectTableRow();
  virtual void collectTable();

  virtual void collectNote();

  virtual void collectStickyNote();

  virtual void startPage();
  virtual void endPage();
  virtual void startLayer();
  virtual void endLayer();
  virtual void startGroup();
  virtual void endGroup();

  virtual void startParagraph(const IWORKStylePtr_t &style);
  virtual void endParagraph();
  virtual void startText(bool object);
  virtual void endText();

  virtual void startLevel();
  virtual void endLevel();

  void pushStyle();
  void popStyle();

protected:
  bool isCollecting() const;
  void setCollecting(bool collecting);

  const IWORKObjectList_t &getNotes() const;
  const KEYStickyNotes_t &getStickyNotes() const;

  const IWORKTransformation &getTransformation() const;

private:
  void pushStyle(const IWORKStylePtr_t &style);

private:
  std::stack<Level> m_levelStack;
  IWORKStyleStack m_styleStack;
  std::stack<IWORKObjectList_t> m_objectsStack;
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

  IWORKObjectList_t m_notes;

  KEYStickyNotes_t m_stickyNotes;

  bool m_collecting;
  bool m_layerOpened;
  int m_groupLevel;
};

} // namespace libetonyek

#endif // KEYCOLLECTORBASE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
