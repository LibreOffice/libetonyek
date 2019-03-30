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
  IWORKStyleMap_t m_cellCommentStyles;
  IWORKStyleMap_t m_characterStyles;
  IWORKStyleMap_t m_chartStyles;
  IWORKStyleMap_t m_chartSeriesStyles;
  IWORKStyleMap_t m_graphicStyles; // graphic-style or connection-style
  IWORKStyleMap_t m_headlineStyles;
  IWORKStyleMap_t m_layoutStyles;
  IWORKStyleMap_t m_listStyles;
  IWORKStyleMap_t m_paragraphStyles;
  IWORKStyleMap_t m_slideStyles;
  IWORKStyleMap_t m_tableStyles;
  IWORKStyleMap_t m_tableCellStyles;
  IWORKStyleMap_t m_tableVectorStyles;
  IWORKStyleMap_t m_tabularStyles;
  IWORKStyleMap_t m_tocStyles;
  IWORKStyleMap_t m_vectorStyles;

  IWORKStylesheetMap_t m_stylesheets;

  IWORKPathMap_t m_beziers;
  IWORKMediaContentMap_t m_binaries;
  IWORKColumnsMap_t m_columnSets;
  IWORKShadowMap_t m_coreImageFilterInfos;
  IWORKDataMap_t m_data;
  IWORKDateTimeFormatMap_t m_dateTimeFormats;
  IWORKDurationFormatMap_t m_durationFormats;
  IWORKExternalTextWrapMap_t m_externalTextWraps;
  IWORKFillMap_t m_fills;
  IWORKFilterDescriptorMap_t m_filterDescriptors;
  IWORKMediaContentMap_t m_filteredImages;
  IWORKFiltersMap_t m_filters;
  IWORKFormulaMap_t m_formulas;
  IWORKGradientMap_t m_gradients;
  IWORKGradientStopMap_t m_gradientStops;
  IWORKMediaContentMap_t m_images;
  IWORKLineSpacingMap_t m_lineSpacings;
  IWORKListLabelGeometryMap_t m_listLabelGeometries;
  IWORKListLabelTypeInfoMap_t m_listLabelTypeInfos;
  IWORKNumberFormatMap_t m_numberFormats;
  IWORKPaddingMap_t m_paddings;
  IWORKPatternMap_t m_patterns;
  IWORKShadowMap_t m_shadows;
  IWORKStrokeMap_t m_strokes;
  IWORKTableCellMap_t m_tableCells;
  IWORKTableVectorMap_t m_tableVectors;
  IWORKTabStopsMap_t m_tabs;
  IWORKTableMap_t m_tabulars;
  IWORKListLabelTypeInfoMap_t m_textLabels;
  IWORKMediaContentMap_t m_texturedFillImages;
  IWORKMediaContentMap_t m_unfiltereds;

  std::unordered_map<ID_t, std::deque<double> > m_doubleArrays;
  std::unordered_map<ID_t, std::deque<IWORKListLabelGeometry> > m_listLabelGeometriesArrays;
  std::unordered_map<ID_t, std::deque<IWORKListLabelTypeInfo_t> > m_listLabelTypesArrays;
};

}

#endif // IWORKDICTIONARY_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
