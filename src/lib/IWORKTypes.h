/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTYPES_H_INCLUDED
#define IWORKTYPES_H_INCLUDED

#include <deque>

#include <boost/optional.hpp>

#include "libetonyek_utils.h"
#include "IWORKTypes_fwd.h"

namespace libetonyek
{

struct IWORKSize
{
  double width;
  double height;

  IWORKSize();
  IWORKSize(double w, double h);
};

struct IWORKPosition
{
  double x;
  double y;

  IWORKPosition();
  IWORKPosition(double x_, double y_);
};

struct IWORKGeometry
{
  IWORKSize naturalSize;
  IWORKSize size;
  IWORKPosition position;
  boost::optional<double> angle;
  boost::optional<double> shearXAngle;
  boost::optional<double> shearYAngle;
  boost::optional<bool> horizontalFlip;
  boost::optional<bool> verticalFlip;
  boost::optional<bool> aspectRatioLocked;
  boost::optional<bool> sizesLocked;

  IWORKGeometry();
};

struct IWORKColor
{
  double red;
  double green;
  double blue;
  double alpha;

  IWORKColor();
  IWORKColor(double r, double g, double b, double a);
};

struct IWORKPadding
{
  boost::optional<int> top;
  boost::optional<int> right;
  boost::optional<int> bottom;
  boost::optional<int> left;

  IWORKPadding();
};

struct IWORKTabStop
{
  double pos;

  explicit IWORKTabStop(double pos_);
};

typedef std::deque<IWORKTabStop> IWORKTabStops_t;

}

#endif //  IWORKTYPES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
