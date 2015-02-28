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
#include "IWORKStyleContext.h"
#include "IWORKStylesheet.h"
#include "IWORKTransformation.h"
#include "KEYCollector.h"
#include "KEYText_fwd.h"
#include "KEYTypes.h"

namespace libetonyek
{

struct KEYDictionary;

/** Implementation of common parts of styles and content collector.
  *
  * It collects styles and drawable objects.
  */
class KEYCollectorBase : public KEYCollector
{
  struct Level
  {
    IWORKGeometryPtr_t geometry;
    KEYGraphicStylePtr_t graphicStyle;
    IWORKTransformation m_trafo;

    Level();
  };

public:
  KEYCollectorBase(KEYDictionary &dict, const KEYDefaults &defaults);
  ~KEYCollectorBase();

  // collector functions

  virtual void collectCellStyle(const boost::optional<ID_t> &id,
                                const boost::optional<IWORKPropertyMap> &props,
                                const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                bool ref, bool anonymous);
  virtual void collectCharacterStyle(const boost::optional<ID_t> &id,
                                     const boost::optional<IWORKPropertyMap> &props,
                                     const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                     bool ref, bool anonymous);
  virtual void collectConnectionStyle(const boost::optional<ID_t> &id,
                                      const boost::optional<IWORKPropertyMap> &props,
                                      const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                      bool ref, bool anonymous);
  virtual void collectGraphicStyle(const boost::optional<ID_t> &id,
                                   const boost::optional<IWORKPropertyMap> &props,
                                   const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                   bool ref, bool anonymous);
  virtual void collectLayoutStyle(const boost::optional<ID_t> &id,
                                  const boost::optional<IWORKPropertyMap> &props,
                                  const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                  bool ref, bool anonymous);
  virtual void collectListStyle(const boost::optional<ID_t> &id,
                                const boost::optional<IWORKPropertyMap> &props,
                                const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                bool ref, bool anonymous);
  virtual void collectParagraphStyle(const boost::optional<ID_t> &id,
                                     const boost::optional<IWORKPropertyMap> &props,
                                     const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                     bool ref, bool anonymous);
  virtual void collectPlaceholderStyle(const boost::optional<ID_t> &id,
                                       const boost::optional<IWORKPropertyMap> &props,
                                       const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                       bool ref, bool anonymous);
  virtual void collectSlideStyle(const boost::optional<ID_t> &id,
                                 const boost::optional<IWORKPropertyMap> &props,
                                 const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                 bool ref, bool anonymous);
  virtual void collectTabularStyle(const boost::optional<ID_t> &id,
                                   const boost::optional<IWORKPropertyMap> &props,
                                   const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                   bool ref, bool anonymous);
  virtual void collectVectorStyle(const boost::optional<ID_t> &id,
                                  const boost::optional<IWORKPropertyMap> &props,
                                  const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                  bool ref, bool anonymous);

  virtual void collectGeometry(const boost::optional<ID_t> &id,
                               boost::optional<IWORKSize> &naturalSize, boost::optional<IWORKSize> &size,
                               boost::optional<IWORKPosition> &position, boost::optional<double> &angle,
                               boost::optional<double> &shearXAngle, boost::optional<double> &shearYAngle,
                               boost::optional<bool> &horizontalFlip, boost::optional<bool> &verticalFlip,
                               boost::optional<bool> &aspectRatioLocked, boost::optional<bool> &sizesLocked);

  virtual void collectBezier(const boost::optional<ID_t> &id, const IWORKPathPtr_t &path, bool ref);
  virtual void collectGroup(const boost::optional<ID_t> &id, const KEYGroupPtr_t &group);
  virtual void collectImage(const boost::optional<ID_t> &id, const KEYImagePtr_t &image);
  virtual void collectLine(const boost::optional<ID_t> &id, const KEYLinePtr_t &line);
  virtual void collectShape(const boost::optional<ID_t> &id);

  virtual void collectBezierPath(const boost::optional<ID_t> &id);
  virtual void collectPolygonPath(const boost::optional<ID_t> &id, const IWORKSize &size, unsigned edges);
  virtual void collectRoundedRectanglePath(const boost::optional<ID_t> &id, const IWORKSize &size, double radius);
  virtual void collectArrowPath(const boost::optional<ID_t> &id, const IWORKSize &size, double headWidth, double stemRelYPos, bool doubleSided);
  virtual void collectStarPath(const boost::optional<ID_t> &id, const IWORKSize &size, unsigned points, double innerRadius);
  virtual void collectConnectionPath(const boost::optional<ID_t> &id, const IWORKSize &size, double middleX, double middleY);
  virtual void collectCalloutPath(const boost::optional<ID_t> &id, const IWORKSize &size, double radius, double tailSize, double tailX, double tailY, bool quoteBubble);

  virtual void collectData(const boost::optional<ID_t> &id, const RVNGInputStreamPtr_t &stream, const boost::optional<std::string> &displayName, const boost::optional<unsigned> &type, bool ref);
  virtual void collectUnfiltered(const boost::optional<ID_t> &id, const boost::optional<IWORKSize> &size, bool ref);
  virtual void collectFiltered(const boost::optional<ID_t> &id, const boost::optional<IWORKSize> &size);
  virtual void collectLeveled(const boost::optional<ID_t> &id, const boost::optional<IWORKSize> &size);
  virtual void collectFilteredImage(const boost::optional<ID_t> &id, bool ref);
  virtual void collectMovieMedia(const boost::optional<ID_t> &id);
  virtual void collectMedia(const boost::optional<ID_t> &id);

  virtual void collectLayer(const boost::optional<ID_t> &id, bool ref);
  virtual void collectStylesheet(const boost::optional<ID_t> &id, const boost::optional<ID_t> &parent);

  virtual void collectText(const boost::optional<ID_t> &style, const std::string &text);
  virtual void collectTab();
  virtual void collectLineBreak();

  virtual void collectTextPlaceholder(const boost::optional<ID_t> &id, bool title, bool ref);

  virtual void collectTableSizes(const KEYTable::RowSizes_t &rowSizes, const KEYTable::ColumnSizes_t &columnSizes);
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

  virtual void startParagraph(const boost::optional<ID_t> &style);
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

  const KEYDefaults &getDefaults() const;

  const KEYLayerPtr_t &getLayer() const;
  const IWORKObjectList_t &getNotes() const;
  const KEYStickyNotes_t &getStickyNotes() const;

  const IWORKTransformation &getTransformation() const;

private:
  void pushStyle(const IWORKStylePtr_t &style);

private:
  KEYDictionary &m_dict;
  const KEYDefaults &m_defaults;

  KEYLayerPtr_t m_currentLayer;

  std::stack<Level> m_levelStack;
  IWORKStyleContext m_styleContext;
  std::stack<IWORKObjectList_t> m_objectsStack;
  IWORKPathPtr_t m_currentPath;

  KEYTextPtr_t m_currentText;

  KEYPlaceholderStylePtr_t m_currentPlaceholderStyle;

  IWORKStylesheetPtr_t m_currentStylesheet;
  std::deque<IWORKStylePtr_t> m_newStyles;

  KEYDataPtr_t m_currentData;
  KEYMediaContentPtr_t m_currentUnfiltered;
  KEYMediaContentPtr_t m_currentFiltered;
  KEYMediaContentPtr_t m_currentLeveled;
  KEYMediaContentPtr_t m_currentContent;

  KEYTable m_currentTable;

  IWORKObjectList_t m_notes;

  KEYStickyNotes_t m_stickyNotes;

  bool m_collecting;
  bool m_layerOpened;
  int m_groupLevel;
};

} // namespace libetonyek

#endif // KEYCOLLECTORBASE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
