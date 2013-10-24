/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYTYPES_H_INCLUDED
#define KEYTYPES_H_INCLUDED

#include <deque>

#include <boost/optional.hpp>

#include "libetonyek_utils.h"
#include "KEYObject.h"
#include "KEYPath_fwd.h"
#include "KEYStyle.h"
#include "KEYText_fwd.h"
#include "KEYTypes_fwd.h"

namespace libetonyek
{

struct KEYSize
{
  double width;
  double height;

  KEYSize();
  KEYSize(double w, double h);
};

struct KEYPosition
{
  double x;
  double y;

  KEYPosition();
  KEYPosition(double x_, double y_);
};

struct KEYGeometry
{
  KEYSize naturalSize;
  KEYPosition position;
  boost::optional<double> angle;
  boost::optional<double> shearXAngle;
  boost::optional<double> shearYAngle;
  boost::optional<bool> horizontalFlip;
  boost::optional<bool> verticalFlip;
  boost::optional<bool> aspectRatioLocked;
  boost::optional<bool> sizesLocked;

  KEYGeometry();
};

struct KEYColor
{
  double red;
  double green;
  double blue;
  double alpha;

  KEYColor();
  KEYColor(double r, double g, double b, double a);
};

struct KEYPadding
{
  boost::optional<int> top;
  boost::optional<int> right;
  boost::optional<int> bottom;
  boost::optional<int> left;

  KEYPadding();
};

struct KEYLine
{
  KEYGeometryPtr_t geometry;
  KEYStylePtr_t style;
  boost::optional<double> x1;
  boost::optional<double> y1;
  boost::optional<double> x2;
  boost::optional<double> y2;

  KEYLine();
};

struct KEYData
{
  WPXInputStreamPtr_t stream;
  boost::optional<std::string> displayName;
  boost::optional<int> type;

  KEYData();
};

struct KEYMediaContent
{
  boost::optional<KEYSize> size;
  KEYDataPtr_t data;

  KEYMediaContent();
};

struct KEYBinary
{
  boost::optional<KEYSize> size;
  boost::optional<std::string> path;
  boost::optional<std::string> type;
  boost::optional<unsigned> dataSize;

  KEYBinary();
};

struct KEYImage
{
  boost::optional<bool> locked;
  KEYGeometryPtr_t geometry;
  boost::optional<KEYBinary> binary;

  KEYImage();
};

struct KEYMedia
{
  KEYGeometryPtr_t geometry;
  KEYGraphicStylePtr_t style;
  boost::optional<bool> placeholder;
  boost::optional<KEYSize> placeholderSize;
  KEYMediaContentPtr_t content;

  KEYMedia();
};

struct KEYWrap
{
  KEYPathPtr_t path;
  KEYGeometryPtr_t geometry;

  KEYWrap();
};

struct KEYGroup
{
  KEYObjectList_t objects;

  KEYGroup();
};

struct KEYLayer
{
  boost::optional<std::string> type;
  KEYObjectList_t objects;

  KEYLayer();
};

struct KEYPlaceholder
{
  boost::optional<bool> title;
  boost::optional<bool> empty;
  KEYPlaceholderStylePtr_t style;
  KEYGeometryPtr_t geometry;
  KEYTextPtr_t text;

  KEYPlaceholder();
};

KEYObjectPtr_t makeObject(const KEYGroupPtr_t &group);
KEYObjectPtr_t makeObject(const KEYImagePtr_t &image);
KEYObjectPtr_t makeObject(const KEYLinePtr_t &line);
KEYObjectPtr_t makeObject(const KEYMediaPtr_t &media);
KEYObjectPtr_t makeObject(const KEYPlaceholderPtr_t &placeholder);

}

#endif //  KEYTYPES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
