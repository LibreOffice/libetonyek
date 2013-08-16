/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNCONTENTCOLLECTOR_H_INCLUDED
#define KNCONTENTCOLLECTOR_H_INCLUDED

#include "KNCollector.h"
#include "KNPath.h"
#include "KNStyle.h"
#include "KNTypes.h"

namespace libwpg
{

class WPGPaintInterface;

} // namespace libwpg

namespace libkeynote
{

class KNContentCollector : public KNCollector
{
public:
  KNContentCollector(::libwpg::WPGPaintInterface *painter, const KNStyleSheet &masterStyles, const KNLayerMap_t &masterPages);
  virtual ~KNContentCollector();

  // collector functions

  virtual void collectCharacterStyle(const std::string &id, const KNStyle &style);
  virtual void collectGraphicStyle(const std::string &id, const KNStyle &style);
  virtual void collectHeadlineStyle(const std::string &id, const KNStyle &style);
  virtual void collectLayoutStyle(const std::string &id, const KNStyle &style);
  virtual void collectParagraphStyle(const std::string &id, const KNStyle &style);

  virtual void collectGeometry(const std::string &id, const KNGeometry &geometry);
  virtual void collectGroup(const std::string &id, const KNGroup &group);
  virtual void collectImage(const std::string &id, const KNImage &image);
  virtual void collectLine(const std::string &id, const KNLine &line);
  virtual void collectMedia(const std::string &id, const KNMedia &media);
  virtual void collectPath(const std::string &id, const KNPath &path);

  virtual void startSize();
  virtual void endSize();
  virtual void startSlides();
  virtual void endSlides();
  virtual void startThemes();
  virtual void endThemes();

private:
  // helper functions

  void resolveStyle(KNStyle &style);

private:
  libwpg::WPGPaintInterface *m_painter;

  const KNStyleSheet &m_masterStyles;
  const KNLayerMap_t &m_masterPages;

  KNStyleSheet m_currentStyles;
  KNLayer m_currentLayer;

  KNGeometryMap_t m_currentGeometries;
  KNGroupMap_t m_currentGroups;
  KNImageMap_t m_currentImages;
  KNLineMap_t m_currentLines;
  KNMediaMap_t m_currentMedia;
  KNPathMap_t m_currentPaths;

  bool m_collecting;
};

} // namespace libkeynote

#endif // KNCONTENTCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
