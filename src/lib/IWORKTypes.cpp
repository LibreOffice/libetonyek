/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTypes.h"

namespace libetonyek
{

IWORKSize::IWORKSize()
  : width(0)
  , height(0)
{
}

IWORKSize::IWORKSize(const double w, const double h)
  : width(w)
  , height(h)
{
}

IWORKPosition::IWORKPosition()
  : x(0)
  , y(0)
{
}

IWORKPosition::IWORKPosition(const double x_, const double y_)
  : x(x_)
  , y(y_)
{
}

IWORKGeometry::IWORKGeometry()
  : naturalSize()
  , size()
  , position()
  , angle()
  , shearXAngle()
  , shearYAngle()
  , horizontalFlip()
  , verticalFlip()
  , aspectRatioLocked()
  , sizesLocked()
{
}

IWORKColor::IWORKColor()
  : red()
  , green()
  , blue()
  , alpha()
{
}

IWORKColor::IWORKColor(const double r, const double g, const double b, const double a)
  : red(r)
  , green(g)
  , blue(b)
  , alpha(a)
{
}

IWORKPadding::IWORKPadding()
  : top()
  , right()
  , bottom()
  , left()
{
}

IWORKTabStop::IWORKTabStop(double pos_)
  : pos(pos_)
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
