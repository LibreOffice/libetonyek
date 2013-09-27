/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNCOLLECTORBASE_H_INCLUDED
#define KNCOLLECTORBASE_H_INCLUDED

#include <deque>
#include <stack>

#include "KNCollector.h"
#include "KNObject.h"
#include "KNStyle.h"
#include "KNStylesheet.h"
#include "KNText_fwd.h"

namespace libkeynote
{

struct KNDictionary;

/** Implementation of common parts of styles and content collector.
  *
  * It collects styles and drawable objects.
  */
class KNCollectorBase : public KNCollector
{
  struct Level
  {
    KNGeometryPtr_t geometry;
    KNGraphicStylePtr_t graphicStyle;

    Level();
  };

public:
  KNCollectorBase(KNDictionary &dict, const KNDefaults &defaults);
  ~KNCollectorBase();

  // collector functions

  virtual void collectCellStyle(const boost::optional<ID_t> &id,
                                const boost::optional<KNPropertyMap> &props,
                                const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                bool ref, bool anonymous);
  virtual void collectCharacterStyle(const boost::optional<ID_t> &id,
                                     const boost::optional<KNPropertyMap> &props,
                                     const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                     bool ref, bool anonymous);
  virtual void collectConnectionStyle(const boost::optional<ID_t> &id,
                                      const boost::optional<KNPropertyMap> &props,
                                      const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                      bool ref, bool anonymous);
  virtual void collectGraphicStyle(const boost::optional<ID_t> &id,
                                   const boost::optional<KNPropertyMap> &props,
                                   const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                   bool ref, bool anonymous);
  virtual void collectLayoutStyle(const boost::optional<ID_t> &id,
                                  const boost::optional<KNPropertyMap> &props,
                                  const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                  bool ref, bool anonymous);
  virtual void collectListStyle(const boost::optional<ID_t> &id,
                                const boost::optional<KNPropertyMap> &props,
                                const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                bool ref, bool anonymous);
  virtual void collectParagraphStyle(const boost::optional<ID_t> &id,
                                     const boost::optional<KNPropertyMap> &props,
                                     const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                     bool ref, bool anonymous);
  virtual void collectPlaceholderStyle(const boost::optional<ID_t> &id,
                                       const boost::optional<KNPropertyMap> &props,
                                       const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                       bool ref, bool anonymous);
  virtual void collectSlideStyle(const boost::optional<ID_t> &id,
                                 const boost::optional<KNPropertyMap> &props,
                                 const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                 bool ref, bool anonymous);
  virtual void collectTabularStyle(const boost::optional<ID_t> &id,
                                   const boost::optional<KNPropertyMap> &props,
                                   const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                   bool ref, bool anonymous);
  virtual void collectVectorStyle(const boost::optional<ID_t> &id,
                                  const boost::optional<KNPropertyMap> &props,
                                  const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                  bool ref, bool anonymous);

  virtual void collectGeometry(boost::optional<ID_t> &id,
                               boost::optional<KNSize> &naturalSize, boost::optional<KNPosition> &position,
                               boost::optional<double> &angle,
                               boost::optional<double> &shearXAngle, boost::optional<double> &shearYAngle,
                               boost::optional<bool> &horizontalFlip, boost::optional<bool> &verticalFlip,
                               boost::optional<bool> &aspectRatioLocked, boost::optional<bool> &sizesLocked);

  virtual void collectBezier(const boost::optional<ID_t> &id, const KNPathPtr_t &path, bool ref);
  virtual void collectGroup(const boost::optional<ID_t> &id, const KNGroupPtr_t &group);
  virtual void collectImage(const boost::optional<ID_t> &id, const KNImagePtr_t &image);
  virtual void collectLine(const boost::optional<ID_t> &id, const KNLinePtr_t &line);
  virtual void collectShape(const boost::optional<ID_t> &id);

  virtual void collectBezierPath(const boost::optional<ID_t> &id);
  virtual void collectPolygonPath(const boost::optional<ID_t> &id, const KNSize &size, unsigned edges);
  virtual void collectRoundedRectanglePath(const boost::optional<ID_t> &id, const KNSize &size, double radius);
  virtual void collectArrowPath(const boost::optional<ID_t> &id, const KNSize &size, double headWidth, double stemRelYPos, bool doubleSided);
  virtual void collectStarPath(const boost::optional<ID_t> &id, const KNSize &size, unsigned points, double innerRadius);
  virtual void collectConnectionPath(const boost::optional<ID_t> &id, const KNSize &size, double middleX, double middleY);
  virtual void collectCalloutPath(const boost::optional<ID_t> &id, const KNSize &size, double radius, double tailSize, double tailX, double tailY, bool quoteBubble);

  virtual void collectData(const boost::optional<ID_t> &id, const WPXInputStreamPtr_t &stream, const boost::optional<std::string> &displayName, const boost::optional<unsigned> &type, bool ref);
  virtual void collectUnfiltered(const boost::optional<ID_t> &id, const boost::optional<KNSize> &size, bool ref);
  virtual void collectFilteredImage(const boost::optional<ID_t> &id, bool ref);
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
  virtual void startTextLayout(const boost::optional<ID_t> &style);
  virtual void endTextLayout();

  virtual void startLevel();
  virtual void endLevel();

protected:
  bool isCollecting() const;
  void setCollecting(bool collecting);

  const KNDefaults &getDefaults() const;

  const KNLayerPtr_t &getLayer() const;

private:
  KNDictionary &m_dict;
  const KNDefaults &m_defaults;

  KNLayerPtr_t m_currentLayer;

  std::stack<Level> m_levelStack;
  std::stack<KNObjectList_t> m_objectsStack;
  KNPathPtr_t m_currentPath;

  KNTextPtr_t m_currentText;

  KNPlaceholderStylePtr_t m_currentPlaceholderStyle;

  KNStylesheetPtr_t m_currentStylesheet;
  std::deque<KNStylePtr_t> m_newStyles;

  bool m_collecting;
  bool m_layerOpened;
  int m_groupLevel;
};

} // namespace libkeynote

#endif // KNCOLLECTORBASE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
