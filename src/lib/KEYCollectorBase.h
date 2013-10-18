/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYCOLLECTORBASE_H_INCLUDED
#define KEYCOLLECTORBASE_H_INCLUDED

#include <deque>
#include <stack>

#include "KEYCollector.h"
#include "KEYObject.h"
#include "KEYStyle.h"
#include "KEYStylesheet.h"
#include "KEYText_fwd.h"

namespace libkeynote
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
    KEYGeometryPtr_t geometry;
    KEYGraphicStylePtr_t graphicStyle;

    Level();
  };

public:
  KEYCollectorBase(KEYDictionary &dict, const KEYDefaults &defaults);
  ~KEYCollectorBase();

  // collector functions

  virtual void collectCellStyle(const boost::optional<ID_t> &id,
                                const boost::optional<KEYPropertyMap> &props,
                                const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                bool ref, bool anonymous);
  virtual void collectCharacterStyle(const boost::optional<ID_t> &id,
                                     const boost::optional<KEYPropertyMap> &props,
                                     const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                     bool ref, bool anonymous);
  virtual void collectConnectionStyle(const boost::optional<ID_t> &id,
                                      const boost::optional<KEYPropertyMap> &props,
                                      const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                      bool ref, bool anonymous);
  virtual void collectGraphicStyle(const boost::optional<ID_t> &id,
                                   const boost::optional<KEYPropertyMap> &props,
                                   const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                   bool ref, bool anonymous);
  virtual void collectLayoutStyle(const boost::optional<ID_t> &id,
                                  const boost::optional<KEYPropertyMap> &props,
                                  const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                  bool ref, bool anonymous);
  virtual void collectListStyle(const boost::optional<ID_t> &id,
                                const boost::optional<KEYPropertyMap> &props,
                                const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                bool ref, bool anonymous);
  virtual void collectParagraphStyle(const boost::optional<ID_t> &id,
                                     const boost::optional<KEYPropertyMap> &props,
                                     const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                     bool ref, bool anonymous);
  virtual void collectPlaceholderStyle(const boost::optional<ID_t> &id,
                                       const boost::optional<KEYPropertyMap> &props,
                                       const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                       bool ref, bool anonymous);
  virtual void collectSlideStyle(const boost::optional<ID_t> &id,
                                 const boost::optional<KEYPropertyMap> &props,
                                 const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                 bool ref, bool anonymous);
  virtual void collectTabularStyle(const boost::optional<ID_t> &id,
                                   const boost::optional<KEYPropertyMap> &props,
                                   const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                   bool ref, bool anonymous);
  virtual void collectVectorStyle(const boost::optional<ID_t> &id,
                                  const boost::optional<KEYPropertyMap> &props,
                                  const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                  bool ref, bool anonymous);

  virtual void collectGeometry(boost::optional<ID_t> &id,
                               boost::optional<KEYSize> &naturalSize, boost::optional<KEYPosition> &position,
                               boost::optional<double> &angle,
                               boost::optional<double> &shearXAngle, boost::optional<double> &shearYAngle,
                               boost::optional<bool> &horizontalFlip, boost::optional<bool> &verticalFlip,
                               boost::optional<bool> &aspectRatioLocked, boost::optional<bool> &sizesLocked);

  virtual void collectBezier(const boost::optional<ID_t> &id, const KEYPathPtr_t &path, bool ref);
  virtual void collectGroup(const boost::optional<ID_t> &id, const KEYGroupPtr_t &group);
  virtual void collectImage(const boost::optional<ID_t> &id, const KEYImagePtr_t &image);
  virtual void collectLine(const boost::optional<ID_t> &id, const KEYLinePtr_t &line);
  virtual void collectShape(const boost::optional<ID_t> &id);

  virtual void collectBezierPath(const boost::optional<ID_t> &id);
  virtual void collectPolygonPath(const boost::optional<ID_t> &id, const KEYSize &size, unsigned edges);
  virtual void collectRoundedRectanglePath(const boost::optional<ID_t> &id, const KEYSize &size, double radius);
  virtual void collectArrowPath(const boost::optional<ID_t> &id, const KEYSize &size, double headWidth, double stemRelYPos, bool doubleSided);
  virtual void collectStarPath(const boost::optional<ID_t> &id, const KEYSize &size, unsigned points, double innerRadius);
  virtual void collectConnectionPath(const boost::optional<ID_t> &id, const KEYSize &size, double middleX, double middleY);
  virtual void collectCalloutPath(const boost::optional<ID_t> &id, const KEYSize &size, double radius, double tailSize, double tailX, double tailY, bool quoteBubble);

  virtual void collectData(const boost::optional<ID_t> &id, const WPXInputStreamPtr_t &stream, const boost::optional<std::string> &displayName, const boost::optional<unsigned> &type, bool ref);
  virtual void collectUnfiltered(const boost::optional<ID_t> &id, const boost::optional<KEYSize> &size, bool ref);
  virtual void collectFiltered(const boost::optional<ID_t> &id, const boost::optional<KEYSize> &size);
  virtual void collectLeveled(const boost::optional<ID_t> &id, const boost::optional<KEYSize> &size);
  virtual void collectFilteredImage(const boost::optional<ID_t> &id, bool ref);
  virtual void collectMovieMedia(const boost::optional<ID_t> &id);
  virtual void collectMedia(const boost::optional<ID_t> &id);

  virtual void collectLayer(const boost::optional<ID_t> &id, bool ref);
  virtual void collectStylesheet(const boost::optional<ID_t> &id, const boost::optional<ID_t> &parent);

  virtual void collectText(const boost::optional<ID_t> &style, const std::string &text);
  virtual void collectTab();
  virtual void collectLineBreak();

  virtual void collectTextPlaceholder(const boost::optional<ID_t> &id, bool title, bool ref);

  virtual void startLayer();
  virtual void endLayer();
  virtual void startGroup();
  virtual void endGroup();

  virtual void startParagraph(const boost::optional<ID_t> &style);
  virtual void endParagraph();
  virtual void startText();
  virtual void endText();

  virtual void startLevel();
  virtual void endLevel();

protected:
  bool isCollecting() const;
  void setCollecting(bool collecting);

  const KEYDefaults &getDefaults() const;

  const KEYLayerPtr_t &getLayer() const;

private:
  KEYDictionary &m_dict;
  const KEYDefaults &m_defaults;

  KEYLayerPtr_t m_currentLayer;

  std::stack<Level> m_levelStack;
  std::stack<KEYObjectList_t> m_objectsStack;
  KEYPathPtr_t m_currentPath;

  KEYTextPtr_t m_currentText;

  KEYPlaceholderStylePtr_t m_currentPlaceholderStyle;

  KEYStylesheetPtr_t m_currentStylesheet;
  std::deque<KEYStylePtr_t> m_newStyles;

  KEYDataPtr_t m_currentData;
  KEYMediaContentPtr_t m_currentUnfiltered;
  KEYMediaContentPtr_t m_currentFiltered;
  KEYMediaContentPtr_t m_currentLeveled;
  KEYMediaContentPtr_t m_currentContent;

  bool m_collecting;
  bool m_layerOpened;
  int m_groupLevel;
};

} // namespace libkeynote

#endif // KEYCOLLECTORBASE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
