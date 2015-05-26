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
  , m_type()
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

IWORKLineSpacing::IWORKLineSpacing(const double value, const bool relative)
  : m_value(value)
  , m_relative(relative)
{
}

IWORKTableData::IWORKTableData()
  : m_columnSizes()
  , m_rowSizes()
  , m_column(0)
  , m_row(0)
  , m_columnSpan()
  , m_rowSpan()
  , m_cellMove()
  , m_content()
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
