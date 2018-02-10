/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKDictionary.h"

namespace libetonyek
{

IWORKDictionary::IWORKDictionary()
  : m_cellStyles()
  , m_characterStyles()
  , m_graphicStyles()
  , m_layoutStyles()
  , m_listStyles()
  , m_paragraphStyles()
  , m_tabularStyles()
  , m_vectorStyles()
  , m_stylesheets()
  , m_beziers()
  , m_formulas()
  , m_data()
  , m_binaries()
  , m_unfiltereds()
  , m_filteredImages()
  , m_images()
  , m_gradientStops()
  , m_patterns()
  , m_strokes()
  , m_tables()
  , m_tabs()
  , m_filterDescriptors()
  , m_listLabelTypeInfos()
  , m_textLabels()
  , m_listLabelGeometries()
  , m_doubleArrays()
  , m_listLabelGeometriesArrays()
  , m_listLabelTypesArrays()
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
