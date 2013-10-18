/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYCONTENTCOLLECTOR_H_INCLUDED
#define KEYCONTENTCOLLECTOR_H_INCLUDED

#include "KEYCollectorBase.h"
#include "KEYTypes.h"

namespace libwpg
{

class WPGPaintInterface;

} // namespace libwpg

namespace libkeynote
{

struct KEYDictionary;

class KEYContentCollector : public KEYCollectorBase
{
  // disable copying
  KEYContentCollector(const KEYContentCollector &other);
  KEYContentCollector &operator=(const KEYContentCollector &other);

public:
  KEYContentCollector(::libwpg::WPGPaintInterface *painter, KEYDictionary &dict, const KEYLayerMap_t &masterPages, const KEYSize &size, const KEYDefaults &defaults);
  virtual ~KEYContentCollector();

  // collector functions

  virtual void collectPresentation(const boost::optional<KEYSize> &size);

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

  void resolveStyle(KEYStyle &style);

  void drawLayer(const KEYLayerPtr_t &layer);

private:
  libwpg::WPGPaintInterface *m_painter;

  const KEYLayerMap_t &m_masterPages;
  const KEYSize m_size;

  bool m_pageOpened;
  bool m_layerOpened;
};

} // namespace libkeynote

#endif // KEYCONTENTCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
