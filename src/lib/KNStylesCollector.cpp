/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KNDictionary.h"
#include "KNStylesCollector.h"

namespace libkeynote
{

KNStylesCollector::KNStylesCollector(KNDictionary &dict, KNLayerMap_t &masterPages, KNSize &size)
  : KNCollectorBase(dict)
  , m_dict(dict)
  , m_masterPages(masterPages)
  , m_size(size)
{
}

KNStylesCollector::~KNStylesCollector()
{
}

void KNStylesCollector::collectSize(const KNSize &size)
{
  m_size = size;
}

void KNStylesCollector::collectLayer(const ID_t &id, const bool ref)
{
  // TODO: implement me
  (void) id;
  (void) ref;
}

void KNStylesCollector::collectPage(const ID_t &id)
{
  // TODO: implement me
  (void) id;
}

void KNStylesCollector::startSlides()
{
}

void KNStylesCollector::endSlides()
{
}

void KNStylesCollector::startThemes()
{
  setCollecting(true);
}

void KNStylesCollector::endThemes()
{
  setCollecting(false);
}

void KNStylesCollector::startPage()
{
}

void KNStylesCollector::endPage()
{
}

void KNStylesCollector::startLayer()
{
}

void KNStylesCollector::endLayer()
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
