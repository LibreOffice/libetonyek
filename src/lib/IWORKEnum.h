/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKENUM_H_INCLUDED
#define IWORKENUM_H_INCLUDED

namespace libetonyek
{

enum IWORKAlignment
{
  IWORK_ALIGNMENT_LEFT,
  IWORK_ALIGNMENT_RIGHT,
  IWORK_ALIGNMENT_CENTER,
  IWORK_ALIGNMENT_JUSTIFY
};

enum IWORKBaseline
{
  IWORK_BASELINE_NORMAL,
  IWORK_BASELINE_SUB,
  IWORK_BASELINE_SUPER
};

enum IWORKBorderType
{
  IWORK_BORDER_TYPE_NONE,
  IWORK_BORDER_TYPE_TOP,
  IWORK_BORDER_TYPE_BOTTOM,
  IWORK_BORDER_TYPE_TOP_AND_BOTTOM,
  IWORK_BORDER_TYPE_ALL
};

enum IWORKBorderStroke
{
  IWORK_BORDER_STROKE_NONE,
  IWORK_BORDER_STROKE_SOLID,
  IWORK_BORDER_STROKE_DOTTED,
  IWORK_BORDER_STROKE_DASHED
};

enum IWORKCapitalization
{
  IWORK_CAPITALIZATION_NONE,
  IWORK_CAPITALIZATION_ALL_CAPS,
  IWORK_CAPITALIZATION_SMALL_CAPS,
  IWORK_CAPITALIZATION_TITLE
};

enum IWORKLineCap
{
  IWORK_LINE_CAP_NONE,
  IWORK_LINE_CAP_BUTT
};

enum IWORKLineJoin
{
  IWORK_LINE_JOIN_NONE,
  IWORK_LINE_JOIN_MITER
};

enum IWORKCellType
{
  IWORK_CELL_TYPE_NUMBER,
  IWORK_CELL_TYPE_TEXT,
  IWORK_CELL_TYPE_DATE_TIME,
  IWORK_CELL_TYPE_DURATION
};

}

#endif //  IWORKENUM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
