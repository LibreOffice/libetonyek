/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKChart.h"

namespace libetonyek
{

IWORKChart::IWORKChart()
  : m_chartType()
  , m_rowNames()
  , m_columnNames()
  , m_chartName()
  , m_valueTitle()
  , m_categoryTitle()
{
}

void IWORKChart::draw(const librevenge::RVNGPropertyList &/*chartProps*/, IWORKOutputElements &/*elements*/)
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
