/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYTHEMECOLLECTOR_H_INCLUDED
#define KEYTHEMECOLLECTOR_H_INCLUDED

#include "KEYCollectorBase.h"

namespace libkeynote
{

struct KEYDictionary;

class KEYThemeCollector : public KEYCollectorBase
{
public:
  KEYThemeCollector(KEYDictionary &dict, KEYLayerMap_t &masterPages, KEYSize &size, const KEYDefaults &defaults);
  virtual ~KEYThemeCollector();

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
  KEYLayerMap_t &m_masterPages;
  KEYSize &m_size;
};

} // namespace libkeynote

#endif // KEYTHEMECOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
