/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTypes.h"

#include "IWORKDocumentInterface.h"

namespace libetonyek
{

IWORKSize::IWORKSize()
  : m_width(0)
  , m_height(0)
{
}

IWORKSize::IWORKSize(const double w, const double h)
  : m_width(w)
  , m_height(h)
{
}

IWORKPosition::IWORKPosition()
  : m_x(0)
  , m_y(0)
{
}

IWORKPosition::IWORKPosition(const double x_, const double y_)
  : m_x(x_)
  , m_y(y_)
{
}

IWORKGeometry::IWORKGeometry()
  : m_naturalSize()
  , m_size()
  , m_position()
  , m_angle()
  , m_shearXAngle()
  , m_shearYAngle()
  , m_horizontalFlip()
  , m_verticalFlip()
  , m_aspectRatioLocked()
  , m_sizesLocked()
{
}

IWORKColor::IWORKColor()
  : m_red()
  , m_green()
  , m_blue()
  , m_alpha()
{
}

IWORKColor::IWORKColor(const double r, const double g, const double b, const double a)
  : m_red(r)
  , m_green(g)
  , m_blue(b)
  , m_alpha(a)
{
}

IWORKPadding::IWORKPadding()
  : m_top()
  , m_right()
  , m_bottom()
  , m_left()
{
}

IWORKTabStop::IWORKTabStop(double pos_)
  : m_pos(pos_)
{
}

IWORKLine::IWORKLine()
  : m_geometry()
  , m_style()
  , m_x1()
  , m_y1()
  , m_x2()
  , m_y2()
{
}

IWORKData::IWORKData()
  : m_stream()
  , m_displayName()
  , m_mimeType()
{
}

IWORKMediaContent::IWORKMediaContent()
  : m_size()
  , m_data()
{
}

IWORKBinary::IWORKBinary()
  : m_size()
  , m_path()
  , m_type()
  , m_dataSize()
{
}

IWORKImage::IWORKImage()
  : m_locked()
  , m_geometry()
  , m_binary()
{
}

IWORKMedia::IWORKMedia()
  : m_geometry()
  , m_style()
  , m_placeholder()
  , m_placeholderSize()
  , m_content()
{
}

IWORKWrap::IWORKWrap()
  : m_path()
  , m_geometry()
{
}

IWORKLineSpacing::IWORKLineSpacing(const double amount, const bool relative)
  : m_amount(amount)
  , m_relative(relative)
{
}

IWORKTableData::IWORKTableData()
  : m_columnSizes()
  , m_rowSizes()
  , m_column(0)
  , m_row(0)
  , m_numColumns()
  , m_numRows()
  , m_columnSpan()
  , m_rowSpan()
  , m_cellMove()
  , m_content()
  , m_formula()
  , m_horizontalLines()
  , m_verticalLines()
  , m_style()
  , m_type(IWORK_CELL_TYPE_TEXT)
{
}

IWORKStroke::IWORKStroke()
  : m_type(IWORK_STROKE_TYPE_NONE)
  , m_width(0)
  , m_color()
  , m_join(IWORK_LINE_JOIN_NONE)
  , m_cap(IWORK_LINE_CAP_NONE)
  , m_pattern()
{
}

IWORKGradientStop::IWORKGradientStop()
  : m_color()
  , m_fraction()
  , m_inflection()
{
}

IWORKGradient::IWORKGradient()
  : m_type(IWORK_GRADIENT_TYPE_LINEAR)
  , m_stops()
  , m_angle(0)
{
}

IWORKFillImage::IWORKFillImage()
  : m_type(IWORK_FILL_IMAGE_TYPE_ORIGINAL_SIZE)
  , m_color()
  , m_size()
  , m_stream()
{
}

IWORKShadow::IWORKShadow()
  : m_color()
  , m_angle(0)
  , m_offset(0)
  , m_opacity(0)
{
}

IWORKMetadata::IWORKMetadata()
  : m_title()
  , m_author()
  , m_keywords()
  , m_comment()
{
}

IWORKColumns::Column::Column()
  : m_width(0)
  , m_spacing(0)
{
}

IWORKColumns::IWORKColumns()
  : m_equal(true)
  , m_columns()
{
}

IWORKNumberFormat::IWORKNumberFormat()
  : m_type(IWORK_CELL_NUMBER_TYPE_DOUBLE)
  , m_string()
  , m_decimalPlaces(0)
  , m_currencyCode()
  , m_negativeStyle(0)
  , m_thousandsSeperator(false)
  , m_fractionAccuracy(0)
  , m_accountingStyle(false)
  , m_base(0)
  , m_basePlaces(0)
{
}

IWORKDateTimeFormat::IWORKDateTimeFormat()
  : m_format()
{
}

IWORKDurationFormat::IWORKDurationFormat()
  : m_format()
{
}

IWORKFilterDescriptor::IWORKFilterDescriptor()
  : m_isShadow(false)
{
}

IWORKListLabelGeometry::IWORKListLabelGeometry()
  : m_align(IWORK_ALIGNMENT_LEFT)
  , m_scale(1)
  , m_offset(0)
  , m_scaleWithText(false)
{
}

IWORKTextLabelFormat::IWORKTextLabelFormat()
  : m_prefix(IWORK_LABEL_NUM_FORMAT_SURROUNDING_NONE)
  , m_format(IWORK_LABEL_NUM_FORMAT_NUMERIC)
  , m_suffix(IWORK_LABEL_NUM_FORMAT_SURROUNDING_NONE)
{
}

IWORKTextLabel::IWORKTextLabel()
  : m_format()
  , m_tiered(false)
  , m_first(1)
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
