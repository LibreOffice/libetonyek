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

#include <string>

#include <boost/optional.hpp>

#include "libetonyek_utils.h"
#include "IWORKPath_fwd.h"
#include "IWORKStylesheet.h"
#include "IWORKTable.h"
#include "IWORKTypes.h"
#include "KEYTypes_fwd.h"

namespace libetonyek
{

class IWORKPropertyMap;
struct IWORKSize;

class KEYCollector
{
public:
  virtual ~KEYCollector() = 0;

  // collector functions

  virtual void collectCellStyle(const IWORKStylePtr_t &style, bool anonymous) = 0;
  virtual void collectCharacterStyle(const IWORKStylePtr_t &style, bool anonymous) = 0;
  virtual void collectConnectionStyle(const IWORKStylePtr_t &style, bool anonymous) = 0;
  virtual void collectGraphicStyle(const IWORKStylePtr_t &style, bool anonymous) = 0;
  virtual void collectLayoutStyle(const IWORKStylePtr_t &style, bool anonymous) = 0;
  virtual void collectListStyle(const IWORKStylePtr_t &style, bool anonymous) = 0;
  virtual void collectParagraphStyle(const IWORKStylePtr_t &style, bool anonymous) = 0;
  virtual void collectPlaceholderStyle(const IWORKStylePtr_t &style, bool anonymous) = 0;
  virtual void collectSlideStyle(const IWORKStylePtr_t &style, bool anonymous) = 0;
  virtual void collectTabularStyle(const IWORKStylePtr_t &style, bool anonymous) = 0;
  virtual void collectVectorStyle(const IWORKStylePtr_t &style, bool anonymous) = 0;

  virtual void collectGeometry(const IWORKGeometryPtr_t &geometry) = 0;

  virtual void collectBezier(const IWORKPathPtr_t &path) = 0;
  virtual void collectGroup(const IWORKGroupPtr_t &group) = 0;
  virtual void collectImage(const IWORKImagePtr_t &image) = 0;
  virtual void collectLine(const IWORKLinePtr_t &line) = 0;
  virtual void collectShape() = 0;

  virtual void collectBezierPath() = 0;
  virtual void collectPolygonPath(const IWORKSize &size, unsigned edges) = 0;
  virtual void collectRoundedRectanglePath(const IWORKSize &size, double radius) = 0;
  virtual void collectArrowPath(const IWORKSize &size, double headWidth, double stemRelYPos, bool doubleSided) = 0;
  virtual void collectStarPath(const IWORKSize &size, unsigned points, double innerRadius) = 0;
  virtual void collectConnectionPath(const IWORKSize &size, double middleX, double middleY) = 0;
  virtual void collectCalloutPath(const IWORKSize &size, double radius, double tailSize, double tailX, double tailY, bool quoteBubble) = 0;

  // TODO: assemble in parser
  virtual void collectData(const IWORKDataPtr_t &data) = 0;
  virtual IWORKMediaContentPtr_t collectUnfiltered(const boost::optional<IWORKSize> &size) = 0;
  virtual void insertUnfiltered(const IWORKMediaContentPtr_t &content) = 0;
  virtual void collectFiltered(const boost::optional<IWORKSize> &size) = 0;
  virtual void collectLeveled(const boost::optional<IWORKSize> &size) = 0;
  virtual IWORKMediaContentPtr_t collectFilteredImage() = 0;
  virtual void insertFilteredImage(const IWORKMediaContentPtr_t &content) = 0;
  virtual void collectMovieMedia() = 0;
  virtual void collectMedia() = 0;

  virtual void collectPresentation(const boost::optional<IWORKSize> &size) = 0;

  virtual KEYLayerPtr_t collectLayer() = 0;
  virtual void insertLayer(const KEYLayerPtr_t &layer) = 0;
  virtual void collectPage() = 0;
  virtual IWORKStylesheetPtr_t collectStylesheet(const IWORKStylesheetPtr_t &parent) = 0;

  virtual void collectText(const IWORKStylePtr_t &style, const std::string &text) = 0;
  virtual void collectTab() = 0;
  virtual void collectLineBreak() = 0;

  virtual KEYPlaceholderPtr_t collectTextPlaceholder(const IWORKStylePtr_t &style, bool title) = 0;
  virtual void insertTextPlaceholder(const KEYPlaceholderPtr_t &placeholder) = 0;

  virtual void collectTableSizes(const IWORKTable::RowSizes_t &rowSizes, const IWORKTable::ColumnSizes_t &columnSizes) = 0;
  virtual void collectTableCell(unsigned row, unsigned column, const boost::optional<std::string> &content, unsigned rowSpan, unsigned columnSpan) = 0;
  virtual void collectCoveredTableCell(unsigned row, unsigned column) = 0;
  virtual void collectTableRow() = 0;
  virtual void collectTable() = 0;

  virtual void collectNote() = 0;

  virtual void collectStickyNote() = 0;

  virtual void startPage() = 0;
  virtual void endPage() = 0;
  virtual void startLayer() = 0;
  virtual void endLayer() = 0;
  virtual void startGroup() = 0;
  virtual void endGroup() = 0;

  virtual void startParagraph(const IWORKStylePtr_t &style) = 0;
  virtual void endParagraph() = 0;

  /** Start of a block that can contain text.
    *
    * This can be:
    * * a text shape
    * * a sticky note
    * * a placeholder
    * * a note.
    */
  virtual void startText(bool object) = 0;

  /** End of a text block.
    */
  virtual void endText() = 0;

  // helper functions

  virtual void startSlides() = 0;
  virtual void endSlides() = 0;
  virtual void startThemes() = 0;
  virtual void endThemes() = 0;

  virtual void startLevel() = 0;
  virtual void endLevel() = 0;
};

} // namespace libetonyek

#endif // KEYCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
