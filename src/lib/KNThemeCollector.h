/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNTHEMECOLLECTOR_H_INCLUDED
#define KNTHEMECOLLECTOR_H_INCLUDED

#include "KNCollectorBase.h"

namespace libkeynote
{

struct KNDictionary;
struct KNStyle;

class KNThemeCollector : public KNCollectorBase
{
public:
  KNThemeCollector(KNDictionary &dict, KNLayerMap_t &masterPages, KNSize &size, const KNDefaults &defaults);
  virtual ~KNThemeCollector();

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
  KNDictionary &m_dict;
  KNLayerMap_t &m_masterPages;
  KNSize &m_size;
};

} // namespace libkeynote

#endif // KNTHEMECOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
