/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNSTYLESCOLLECTOR_H_INCLUDED
#define KNSTYLESCOLLECTOR_H_INCLUDED

#include "KNCollector.h"
#include "KNPath.h"
#include "KNStyle.h"

namespace libkeynote
{

class KNStylesCollector : public KNCollector
{
public:
  KNStylesCollector(KNStyleSheet &masterStyles, KNLayerMap_t &masterPages, KNSize &size);
  virtual ~KNStylesCollector();

  // collector functions

  virtual void collectCharacterStyle(const ID_t &id, const KNStyle &style);
  virtual void collectGraphicStyle(const ID_t &id, const KNStyle &style);
  virtual void collectHeadlineStyle(const ID_t &id, const KNStyle &style);
  virtual void collectLayoutStyle(const ID_t &id, const KNStyle &style);
  virtual void collectParagraphStyle(const ID_t &id, const KNStyle &style);

  virtual void collectGeometry(const ID_t &id, const KNGeometry &geometry);
  virtual void collectGroup(const ID_t &id, const KNGroup &group);
  virtual void collectImage(const ID_t &id, const KNImage &image);
  virtual void collectLine(const ID_t &id, const KNLine &line);
  virtual void collectMedia(const ID_t &id, const KNMedia &media);
  virtual void collectPath(const ID_t &id, const KNPath &path);

  virtual void collectSize(const KNSize &size);

  virtual void collectLayer(const ID_t &id, bool ref);
  virtual void collectPage(const ID_t &id);

  virtual void startSlides();
  virtual void endSlides();
  virtual void startThemes();
  virtual void endThemes();

  virtual void startPage();
  virtual void endPage();
  virtual void startLayer();
  virtual void endLayer();

private:
  KNStyleSheet &m_currentStyles;
  KNLayerMap_t &m_masterPages;
  KNSize &m_size;

  KNGeometryMap_t m_currentGeometries;
  KNGroupMap_t m_currentGroups;
  KNImageMap_t m_currentImages;
  KNLineMap_t m_currentLines;
  KNMediaMap_t m_currentMedia;
  KNPathMap_t m_currentPaths;

  KNLayer m_currentLayer;

  bool m_collecting;
};

} // namespace libkeynote

#endif // KNSTYLESCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
