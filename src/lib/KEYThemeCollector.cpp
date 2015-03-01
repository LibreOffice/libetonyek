/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYThemeCollector.h"

#include "libetonyek_utils.h"
#include "KEYDictionary.h"

using boost::optional;

namespace libetonyek
{

KEYThemeCollector::KEYThemeCollector(IWORKSize &size)
  : KEYCollectorBase()
  , m_size(size)
{
}

KEYThemeCollector::~KEYThemeCollector()
{
}

void KEYThemeCollector::collectPresentation(const boost::optional<IWORKSize> &size)
{
  if (size)
    m_size = get(size);
}

void KEYThemeCollector::insertLayer(const KEYLayerPtr_t &)
{
}

void KEYThemeCollector::collectPage()
{
  // TODO: implement me
}

void KEYThemeCollector::startSlides()
{
}

void KEYThemeCollector::endSlides()
{
}

void KEYThemeCollector::startThemes()
{
  setCollecting(true);
}

void KEYThemeCollector::endThemes()
{
  setCollecting(false);
}

void KEYThemeCollector::startPage()
{
}

void KEYThemeCollector::endPage()
{
}

void KEYThemeCollector::startLayer()
{
  KEYCollectorBase::startLayer();
}

void KEYThemeCollector::endLayer()
{
  KEYCollectorBase::endLayer();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
