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

#include "KNCollectorBase.h"

namespace libwpg
{

class WPGPaintInterface;

} // namespace libwpg

namespace libkeynote
{

struct KNDictionary;

class KNContentCollector : public KNCollectorBase
{
  // disable copying
  KNContentCollector(const KNContentCollector &other);
  KNContentCollector &operator=(const KNContentCollector &other);

public:
  KNContentCollector(::libwpg::WPGPaintInterface *painter, KNDictionary &dict, const KNLayerMap_t &masterPages, const KNSize &size, const KNDefaults &defaults);
  virtual ~KNContentCollector();

  // collector functions

  virtual void collectPresentation(const boost::optional<KNSize> &size);

  virtual void collectLayer(const boost::optional<ID_t> &id, bool ref);
  virtual void collectPage(const boost::optional<ID_t> &id);

  virtual void startSlides();
  virtual void endSlides();
  virtual void startThemes();
  virtual void endThemes();

  virtual void startPage();
  virtual void endPage();
  virtual void startLayer();
  virtual void endLayer();

private:
  // helper functions

  void resolveStyle(KNStyle &style);

  void drawLayer(const KNLayerPtr_t &layer);

private:
  libwpg::WPGPaintInterface *m_painter;

  const KNLayerMap_t &m_masterPages;

  bool m_pageOpened;
  bool m_layerOpened;
};

} // namespace libkeynote

#endif // KNCONTENTCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
