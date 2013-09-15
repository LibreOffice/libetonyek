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

#include <stack>

#include "KNCollector.h"
#include "KNObject.h"
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
public:
  explicit KNCollectorBase(KNDictionary &dict);
  ~KNCollectorBase();

  // collector functions

  virtual void collectCharacterStyle(const boost::optional<ID_t> &id, const KNCharacterStylePtr_t &style, bool ref);
  virtual void collectGraphicStyle(const boost::optional<ID_t> &id, const KNGraphicStylePtr_t &style, bool ref);
  virtual void collectLayoutStyle(const boost::optional<ID_t> &id, const KNLayoutStylePtr_t &style, bool ref);
  virtual void collectParagraphStyle(const boost::optional<ID_t> &id, const KNParagraphStylePtr_t &style, bool ref);

  virtual void collectBezier(const boost::optional<ID_t> &id, const KNPathPtr_t &path, bool ref);
  virtual void collectGeometry(const boost::optional<ID_t> &id, const KNGeometryPtr_t &geometry);
  virtual void collectGroup(const boost::optional<ID_t> &id, const KNGroupPtr_t &group);
  virtual void collectImage(const boost::optional<ID_t> &id, const KNImagePtr_t &image);
  virtual void collectLine(const boost::optional<ID_t> &id, const KNLinePtr_t &line);
  virtual void collectMedia(const boost::optional<ID_t> &id, const KNMediaPtr_t &media);
  virtual void collectShape(const boost::optional<ID_t> &id);

  virtual void collectBezierPath(const boost::optional<ID_t> &id);
  virtual void collectPolygonPath(const boost::optional<ID_t> &id, const KNSize &size, unsigned edges);
  virtual void collectRoundedRectanglePath(const boost::optional<ID_t> &id, const KNSize &size, double radius);
  virtual void collectArrowPath(const boost::optional<ID_t> &id, const KNSize &size, double headWidth, double stemRelYPos, bool doubleSided);
  virtual void collectStarPath(const boost::optional<ID_t> &id, const KNSize &size, unsigned points, double innerRadius);
  virtual void collectConnectionPath(const boost::optional<ID_t> &id, const KNSize &size, double middleX, double middleY);
  virtual void collectCalloutPath(const boost::optional<ID_t> &id, const KNSize &size, double radius, double tailSize, double tailX, double tailY, bool quoteBubble);

  virtual void collectLayer(const boost::optional<ID_t> &id, bool ref);

  virtual void collectText(const boost::optional<ID_t> &style, const std::string &text);
  virtual void collectTab();
  virtual void collectLineBreak();

  virtual void collectSlideText(const boost::optional<ID_t> &id, bool title);

  virtual void startLayer();
  virtual void endLayer();
  virtual void startGroup();
  virtual void endGroup();

  virtual void startParagraph(const boost::optional<ID_t> &style);
  virtual void endParagraph();
  virtual void startTextLayout(const boost::optional<ID_t> &style);
  virtual void endTextLayout();

  bool isCollecting() const;
  void setCollecting(bool collecting);

  const KNLayerPtr_t &getLayer() const;

private:
  KNDictionary &m_dict;
  std::stack<KNObjectList_t> m_objectsStack;
  KNGeometryPtr_t m_currentGeometry;
  KNPathPtr_t m_currentPath;
  KNLayerPtr_t m_currentLayer;

  KNTextPtr_t m_currentText;

  bool m_collecting;
  bool m_layerOpened;
  int m_groupLevel;
};

} // namespace libkeynote

#endif // KNCOLLECTORBASE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
