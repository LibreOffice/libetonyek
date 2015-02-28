/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYCONTENTCOLLECTOR_H_INCLUDED
#define KEYCONTENTCOLLECTOR_H_INCLUDED

#include "KEYCollectorBase.h"
#include "KEYTypes.h"

namespace libetonyek
{

struct KEYDictionary;

class KEYContentCollector : public KEYCollectorBase
{
  // disable copying
  KEYContentCollector(const KEYContentCollector &other);
  KEYContentCollector &operator=(const KEYContentCollector &other);

public:
  KEYContentCollector(librevenge::RVNGPresentationInterface *painter, KEYDictionary &dict, const KEYLayerMap_t &masterPages, const IWORKSize &size, const KEYDefaults &defaults);
  virtual ~KEYContentCollector();

  // collector functions

  virtual void collectPresentation(const boost::optional<IWORKSize> &size);

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

  void resolveStyle(IWORKStyle &style);

  void drawLayer(const KEYLayerPtr_t &layer);
  void drawNotes(const IWORKObjectList_t &notes);
  void drawStickyNotes(const KEYStickyNotes_t &stickyNotes);

private:
  librevenge::RVNGPresentationInterface *m_painter;

  const KEYLayerMap_t &m_masterPages;
  const IWORKSize m_size;

  bool m_pageOpened;
  bool m_layerOpened;

  int m_layerCount;
};

} // namespace libetonyek

#endif // KEYCONTENTCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
