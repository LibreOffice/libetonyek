/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKPROPERTIES_H_INCLUDED
#define IWORKPROPERTIES_H_INCLUDED

#include <map>

#include "IWORKEnum.h"
#include "IWORKPropertyInfo.h"
#include "IWORKTypes.h"

namespace libetonyek
{

IWORK_DECLARE_PROPERTY(Alignment, IWORKAlignment);
IWORK_DECLARE_PROPERTY(Baseline, IWORKBaseline);
IWORK_DECLARE_PROPERTY(BaselineShift, double);
IWORK_DECLARE_PROPERTY(Bold, bool);
IWORK_DECLARE_PROPERTY(BottomBorder, IWORKStroke);
IWORK_DECLARE_PROPERTY(Capitalization, IWORKCapitalization);
IWORK_DECLARE_PROPERTY(Columns, IWORKColumns);
IWORK_DECLARE_PROPERTY(Fill, IWORKFill);
IWORK_DECLARE_PROPERTY(FirstLineIndent, double);
IWORK_DECLARE_PROPERTY(FollowingLayoutStyle, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(FollowingParagraphStyle, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(FontColor, IWORKColor);
IWORK_DECLARE_PROPERTY(FontName, std::string);
IWORK_DECLARE_PROPERTY(FontSize, double);
IWORK_DECLARE_PROPERTY(Geometry, IWORKGeometryPtr_t);
IWORK_DECLARE_PROPERTY(HeadLineEnd, IWORKMarker);
IWORK_DECLARE_PROPERTY(Hyphenate, bool);
IWORK_DECLARE_PROPERTY(Italic, bool);
IWORK_DECLARE_PROPERTY(KeepLinesTogether, bool);
IWORK_DECLARE_PROPERTY(KeepWithNext, bool);
IWORK_DECLARE_PROPERTY(LabelCharacterStyle, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(Language, std::string);
IWORK_DECLARE_PROPERTY(LayoutMargins, IWORKPadding);
IWORK_DECLARE_PROPERTY(LayoutParagraphStyle, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(LayoutStyle, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(LeftBorder, IWORKStroke);
IWORK_DECLARE_PROPERTY(LeftIndent, double);
IWORK_DECLARE_PROPERTY(LineSpacing, IWORKLineSpacing);
IWORK_DECLARE_PROPERTY(ListLabelGeometry, IWORKListLabelGeometry);
IWORK_DECLARE_PROPERTY(ListLabelGeometries, std::deque<IWORKListLabelGeometry>);
IWORK_DECLARE_PROPERTY(ListLabelIndent, double);
IWORK_DECLARE_PROPERTY(ListLabelIndents, std::deque<double>);
IWORK_DECLARE_PROPERTY(ListLabelTypeInfo, IWORKListLabelTypeInfo_t);
IWORK_DECLARE_PROPERTY(ListLabelTypes, std::deque<IWORKListLabelTypeInfo_t>);
IWORK_DECLARE_PROPERTY(ListLevelStyles, IWORKListLevels_t);
IWORK_DECLARE_PROPERTY(ListStyle, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(ListTextIndent, double);
IWORK_DECLARE_PROPERTY(ListTextIndents, std::deque<double>);
IWORK_DECLARE_PROPERTY(Opacity, double);
IWORK_DECLARE_PROPERTY(Outline, bool);
IWORK_DECLARE_PROPERTY(Padding, IWORKPadding);
IWORK_DECLARE_PROPERTY(PageBreakBefore, bool);
IWORK_DECLARE_PROPERTY(ParagraphBorderType, IWORKBorderType);
IWORK_DECLARE_PROPERTY(ParagraphFill, IWORKColor);
IWORK_DECLARE_PROPERTY(ParagraphStroke, IWORKStroke);
IWORK_DECLARE_PROPERTY(RightBorder, IWORKStroke);
IWORK_DECLARE_PROPERTY(RightIndent, double);
IWORK_DECLARE_PROPERTY(SFSeries, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTAutoResizeProperty, bool);
IWORK_DECLARE_PROPERTY(SFC2DAreaFillProperty,IWORKFill);
IWORK_DECLARE_PROPERTY(SFC2DColumnFillProperty, IWORKFill);
IWORK_DECLARE_PROPERTY(SFC2DMixedColumnFillProperty, IWORKFill);
IWORK_DECLARE_PROPERTY(SFC2DPieFillProperty, IWORKFill);
IWORK_DECLARE_PROPERTY(SFTableCellStylePropertyFill, IWORKFill);
IWORK_DECLARE_PROPERTY(SFTableStylePropertyCellStyle, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTableStylePropertyHeaderColumnCellStyle, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTableStylePropertyHeaderRowCellStyle, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTCellStylePropertyDateTimeFormat, IWORKDateTimeFormat);
IWORK_DECLARE_PROPERTY(SFTCellStylePropertyDurationFormat, IWORKDurationFormat);
IWORK_DECLARE_PROPERTY(SFTCellStylePropertyNumberFormat, IWORKNumberFormat);
IWORK_DECLARE_PROPERTY(SFTCellStylePropertyLayoutStyle, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTCellStylePropertyParagraphStyle, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTDefaultBodyCellStyleProperty, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTDefaultBodyVectorStyleProperty, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTDefaultBorderVectorStyleProperty, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTDefaultFooterBodyVectorStyleProperty, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTDefaultFooterRowCellStyleProperty, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTDefaultFooterSeparatorVectorStyleProperty, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTDefaultGroupingLevelVectorStyleProperty, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTDefaultGroupingRowCellStyleProperty, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTDefaultHeaderBodyVectorStyleProperty, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTDefaultHeaderColumnCellStyleProperty, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTDefaultHeaderRowCellStyleProperty, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTDefaultHeaderSeparatorVectorStyleProperty, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTHeaderColumnRepeatsProperty, bool);
IWORK_DECLARE_PROPERTY(SFTHeaderRowRepeatsProperty, bool);
IWORK_DECLARE_PROPERTY(SFTStrokeProperty, IWORKStroke);
IWORK_DECLARE_PROPERTY(SFTTableBandedCellFillProperty, IWORKFill);
IWORK_DECLARE_PROPERTY(SFTTableBandedRowsProperty, bool);
IWORK_DECLARE_PROPERTY(SFTTableNameStylePropertyLayoutStyle, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(SFTTableNameStylePropertyParagraphStyle, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(Shadow, IWORKShadow);
IWORK_DECLARE_PROPERTY(SpaceAfter, double);
IWORK_DECLARE_PROPERTY(SpaceBefore, double);
IWORK_DECLARE_PROPERTY(Strikethru, bool);
IWORK_DECLARE_PROPERTY(Stroke, IWORKStroke);
IWORK_DECLARE_PROPERTY(Tabs, IWORKTabStops_t);
IWORK_DECLARE_PROPERTY(TailLineEnd, IWORKMarker);
IWORK_DECLARE_PROPERTY(TextBackground, IWORKColor);
IWORK_DECLARE_PROPERTY(TopBorder, IWORKStroke);
IWORK_DECLARE_PROPERTY(TocStyle, IWORKStylePtr_t);
IWORK_DECLARE_PROPERTY(Tracking, double);
IWORK_DECLARE_PROPERTY(Underline, bool);
IWORK_DECLARE_PROPERTY(VerticalAlignment, IWORKVerticalAlignment);
IWORK_DECLARE_PROPERTY(WidowControl, bool);

}

#endif // IWORKPROPERTIES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
