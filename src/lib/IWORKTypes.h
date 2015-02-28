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
#include "IWORKObject.h"
#include "IWORKPath_fwd.h"
#include "IWORKStyle_fwd.h"
#include "IWORKTypes_fwd.h"

namespace libetonyek
{

class IWORKTransformation;

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

struct IWORKLine
{
  IWORKGeometryPtr_t geometry;
  IWORKStylePtr_t style;
  boost::optional<double> x1;
  boost::optional<double> y1;
  boost::optional<double> x2;
  boost::optional<double> y2;

  IWORKLine();
};

struct IWORKData
{
  RVNGInputStreamPtr_t stream;
  boost::optional<std::string> displayName;
  boost::optional<int> type;

  IWORKData();
};

struct IWORKMediaContent
{
  boost::optional<IWORKSize> size;
  IWORKDataPtr_t data;

  IWORKMediaContent();
};

struct IWORKBinary
{
  boost::optional<IWORKSize> size;
  boost::optional<std::string> path;
  boost::optional<std::string> type;
  boost::optional<unsigned> dataSize;

  IWORKBinary();
};

struct IWORKImage
{
  boost::optional<bool> locked;
  IWORKGeometryPtr_t geometry;
  boost::optional<IWORKBinary> binary;

  IWORKImage();
};

struct IWORKMedia
{
  IWORKGeometryPtr_t geometry;
  IWORKStylePtr_t style;
  boost::optional<bool> placeholder;
  boost::optional<IWORKSize> placeholderSize;
  IWORKMediaContentPtr_t content;

  IWORKMedia();
};

struct IWORKWrap
{
  IWORKPathPtr_t path;
  IWORKGeometryPtr_t geometry;

  IWORKWrap();
};

struct IWORKGroup
{
  IWORKObjectList_t objects;

  IWORKGroup();
};

IWORKObjectPtr_t makeObject(const IWORKGroupPtr_t &group);
IWORKObjectPtr_t makeObject(const IWORKImagePtr_t &image, const IWORKTransformation &trafo);
IWORKObjectPtr_t makeObject(const IWORKLinePtr_t &line, const IWORKTransformation &trafo);
IWORKObjectPtr_t makeObject(const IWORKMediaPtr_t &media, const IWORKTransformation &trafo);

}

#endif //  IWORKTYPES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
