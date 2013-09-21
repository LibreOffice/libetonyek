/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNTYPES_H_INCLUDED
#define KNTYPES_H_INCLUDED

#include <deque>

#include <boost/optional.hpp>

#include "libkeynote_utils.h"
#include "KNObject.h"
#include "KNPath_fwd.h"
#include "KNStyle.h"
#include "KNText_fwd.h"
#include "KNTypes_fwd.h"

namespace libkeynote
{

struct KNSize
{
  double width;
  double height;

  KNSize();
  KNSize(double w, double h);
};

struct KNPosition
{
  double x;
  double y;

  KNPosition();
  KNPosition(double x_, double y_);
};

struct KNGeometry
{
  boost::optional<KNSize> naturalSize;
  boost::optional<KNSize> size;
  boost::optional<KNPosition> position;
  double angle;
  double shearXAngle;
  double shearYAngle;
  bool horizontalFlip;
  bool verticalFlip;
  bool aspectRatioLocked;
  bool sizesLocked;

  KNGeometry();
};

struct KNColor
{
  double red;
  double green;
  double blue;
  double alpha;

  KNColor();
  KNColor(double r, double g, double b, double a);
};

struct KNPadding
{
  boost::optional<int> top;
  boost::optional<int> right;
  boost::optional<int> bottom;
  boost::optional<int> left;

  KNPadding();
};

struct KNLine
{
  KNGeometryPtr_t geometry;
  KNStylePtr_t style;
  boost::optional<double> x1;
  boost::optional<double> y1;
  boost::optional<double> x2;
  boost::optional<double> y2;

  KNLine();
};

struct KNData
{
  WPXInputStreamPtr_t stream;
  boost::optional<std::string> displayName;
  boost::optional<int> type;
};

struct KNUnfiltered
{
  KNSize size;
  boost::optional<KNData> data;
};

struct KNFilteredImage
{
  KNUnfilteredPtr_t unfiltered;
};

struct KNBinary
{
  boost::optional<KNSize> size;
  boost::optional<std::string> path;
  boost::optional<std::string> type;
  boost::optional<unsigned> dataSize;

  KNBinary();
};

struct KNImage
{
  boost::optional<bool> locked;
  KNGeometryPtr_t geometry;
  boost::optional<KNBinary> binary;

  KNImage();
};

struct KNMedia
{
  KNGeometryPtr_t geometry;
  KNStylePtr_t style;
  boost::optional<bool> placeholder;
  boost::optional<KNSize> placeholderSize;
  boost::optional<KNBinary> data;

  KNMedia();
};

struct KNWrap
{
  KNPathPtr_t path;
  KNGeometryPtr_t geometry;

  KNWrap();
};

struct KNGroup
{
  KNObjectList_t objects;

  KNGroup();
};

struct KNLayer
{
  boost::optional<std::string> type;
  KNObjectList_t objects;

  KNLayer();
};

struct KNTextBody
{
  bool title;
  boost::optional<bool> empty;
  KNGeometryPtr_t geometry;
  KNStylePtr_t style;
  KNTextPtr_t text;

  explicit KNTextBody(bool title_);
};

KNObjectPtr_t makeObject(const KNGroupPtr_t &group);
KNObjectPtr_t makeObject(const KNImagePtr_t &image);
KNObjectPtr_t makeObject(const KNLinePtr_t &line);
KNObjectPtr_t makeObject(const KNMediaPtr_t &media);
KNObjectPtr_t makeObject(const KNTextBodyPtr_t &body);

}

#endif //  KNTYPES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
