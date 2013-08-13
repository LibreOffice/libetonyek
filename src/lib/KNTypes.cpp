/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KNTypes.h"

namespace libkeynote
{

KNSize::KNSize()
  : width(0)
  , height(0)
{
}

KNSize::KNSize(const double w, const double h)
  : width(w)
  , height(h)
{
}

KNPosition::KNPosition()
  : x(0)
  , y(0)
{
}

KNPosition::KNPosition(const double x_, const double y_)
  : x(x_)
  , y(y_)
{
}

KNGeometry::KNGeometry()
  : naturalSize()
  , size()
  , position()
  , angle(0)
  , shearXAngle(0)
  , shearYAngle(0)
  , horizontalFlip(false)
  , verticalFlip(false)
  , aspectRatioLocked(false)
  , sizesLocked(false)
{
}

KNColor::KNColor()
  : red()
  , green()
  , blue()
  , alpha()
{
}

KNColor::KNColor(const double r, const double g, const double b, const double a)
  : red(r)
  , green(g)
  , blue(b)
  , alpha(a)
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
