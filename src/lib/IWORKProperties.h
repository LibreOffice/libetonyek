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

#include "IWORKEnum.h"
#include "IWORKPropertyInfo.h"
#include "IWORKTypes.h"

namespace libetonyek
{

IWORK_DECLARE_PROPERTY(Alignment, IWORKAlignment);
IWORK_DECLARE_PROPERTY(Baseline, IWORKBaseline);
IWORK_DECLARE_PROPERTY(BaselineShift, double);
IWORK_DECLARE_PROPERTY(Bold, bool);
IWORK_DECLARE_PROPERTY(Capitalization, IWORKCapitalization);
IWORK_DECLARE_PROPERTY(FirstLineIndent, double);
IWORK_DECLARE_PROPERTY(FontColor, IWORKColor);
IWORK_DECLARE_PROPERTY(FontName, std::string);
IWORK_DECLARE_PROPERTY(FontSize, double);
IWORK_DECLARE_PROPERTY(Geometry, IWORKGeometryPtr_t);
IWORK_DECLARE_PROPERTY(Italic, bool);
IWORK_DECLARE_PROPERTY(KeepLinesTogether, bool);
IWORK_DECLARE_PROPERTY(KeepWithNext, bool);
IWORK_DECLARE_PROPERTY(LeftIndent, double);
IWORK_DECLARE_PROPERTY(LineSpacing, IWORKLineSpacing);
IWORK_DECLARE_PROPERTY(Outline, bool);
IWORK_DECLARE_PROPERTY(ParagraphFill, IWORKColor);
IWORK_DECLARE_PROPERTY(RightIndent, double);
IWORK_DECLARE_PROPERTY(SpaceAfter, double);
IWORK_DECLARE_PROPERTY(SpaceBefore, double);
IWORK_DECLARE_PROPERTY(Strikethru, bool);
IWORK_DECLARE_PROPERTY(Tabs, IWORKTabStops_t);
IWORK_DECLARE_PROPERTY(TextBackground, IWORKColor);
IWORK_DECLARE_PROPERTY(Underline, bool);
IWORK_DECLARE_PROPERTY(WidowControl, bool);

}

#endif // IWORKPROPERTIES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
