/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKDICTIONARY_H_INCLUDED
#define IWORKDICTIONARY_H_INCLUDED

#include <deque>
#include <unordered_map>

#include "IWORKPath.h"
#include "IWORKStyle.h"
#include "IWORKStylesheet.h"
#include "IWORKTypes.h"

namespace libetonyek
{

/** A dictionary of all referenceable entities in the presentation.
  *
  * In other words, all entities that might be included through a -ref
  * element in the format. This is purely a file format concept; all
  * that matters when an entity is inserted by a reference is that there
  * has been an entity of the same type and with the same content
  * inserted previously (or at least that is how I understand it. I
  * might be wrong about it). That means we need to know all entities in
  * the whole presentation to resolve references.
  *
  * @note The dictionary contains (among other things) all master
  * styles, therefore we do not handle them separately. (We might do
  * that in the future, to ensure correctness of the format.)
  */
struct IWORKDictionary
{
  IWORKDictionary();

  IWORKStyleMap_t m_cellStyles;
  IWORKStyleMap_t m_characterStyles;
  IWORKStyleMap_t m_graphicStyles;
  IWORKStyleMap_t m_layoutStyles;
  IWORKStyleMap_t m_listStyles;
  IWORKStyleMap_t m_paragraphStyles;
  IWORKStyleMap_t m_tabularStyles;
  IWORKStyleMap_t m_vectorStyles;

  IWORKStylesheetMap_t m_stylesheets;

  IWORKPathMap_t m_beziers;

  IWORKFormulaMap_t m_formulas;

  IWORKDataMap_t m_data;
  IWORKMediaContentMap_t m_binaries;
  IWORKMediaContentMap_t m_unfiltereds;
  IWORKMediaContentMap_t m_filteredImages;
  IWORKMediaContentMap_t m_images;
  IWORKGradientStopMap_t m_gradientStops;
  IWORKLineSpacingMap_t m_lineSpacings;
  IWORKPatternMap_t m_patterns;
  IWORKStrokeMap_t m_strokes;
  IWORKTableMap_t m_tables;

  IWORKTabStopsMap_t m_tabs;

  IWORKFilterDescriptorMap_t m_filterDescriptors;

  IWORKListLabelTypeInfoMap_t m_listLabelTypeInfos;
  IWORKListLabelTypeInfoMap_t m_textLabels;
  IWORKListLabelGeometryMap_t m_listLabelGeometries;

  std::unordered_map<ID_t, std::deque<double> > m_doubleArrays;
  std::unordered_map<ID_t, std::deque<IWORKListLabelGeometry> > m_listLabelGeometriesArrays;
  std::unordered_map<ID_t, std::deque<IWORKListLabelTypeInfo_t> > m_listLabelTypesArrays;
};

}

#endif // IWORKDICTIONARY_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
