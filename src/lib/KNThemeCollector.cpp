/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "libkeynote_utils.h"
#include "KNDefaults.h"
#include "KNDictionary.h"
#include "KNThemeCollector.h"

using boost::optional;

namespace libkeynote
{

KNThemeCollector::KNThemeCollector(KNDictionary &dict, KNLayerMap_t &masterPages, KNSize &size, const KNDefaults &defaults)
  : KNCollectorBase(dict, defaults)
  , m_masterPages(masterPages)
  , m_size(size)
{
}

KNThemeCollector::~KNThemeCollector()
{
}

void KNThemeCollector::collectPresentation(const boost::optional<KNSize> &size)
{
  if (isCollecting())
  {
    optional<KNSize> size_(size);
    getDefaults().applyPresentationSize(size_);
    assert(size_);
    m_size = get(size_);
  }
}

void KNThemeCollector::collectLayer(const boost::optional<ID_t> &id, const bool ref)
{
  if (isCollecting())
  {
    KNCollectorBase::collectLayer(id, ref);

    if (ref)
    {
      KN_DEBUG_MSG(("cannot use master page reference %s in a master page\n", id ? get(id).c_str() : ""));
    }
    else
    {
      const KNLayerPtr_t layer = getLayer();
      if (bool(layer) && id)
        m_masterPages.insert(KNLayerMap_t::value_type(get(id), layer));
      else
      {
        KN_DEBUG_MSG(("master style layer is empty\n"));
      }
    }
  }
}

void KNThemeCollector::collectPage(const boost::optional<ID_t> &id)
{
  // TODO: implement me
  (void) id;
}

void KNThemeCollector::startSlides()
{
}

void KNThemeCollector::endSlides()
{
}

void KNThemeCollector::startThemes()
{
  setCollecting(true);
}

void KNThemeCollector::endThemes()
{
  setCollecting(false);
}

void KNThemeCollector::startPage()
{
  KNCollectorBase::startPage();
}

void KNThemeCollector::endPage()
{
  KNCollectorBase::endPage();
}

void KNThemeCollector::startLayer()
{
  KNCollectorBase::startLayer();
}

void KNThemeCollector::endLayer()
{
  KNCollectorBase::endLayer();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
