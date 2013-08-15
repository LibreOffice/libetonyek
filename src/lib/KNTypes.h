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
#include <string>

#include <boost/optional.hpp>

namespace libkeynote
{

typedef std::string ID_t;
typedef std::deque<ID_t> IDs_t;

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

struct KNPoint
{
  double x;
  double y;

  KNPoint();
};

struct KNLine
{
  boost::optional<ID_t> geometry;
  boost::optional<KNPoint> head;
  boost::optional<KNPoint> tail;

  KNLine();
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
  boost::optional<ID_t> geometry;
  boost::optional<KNBinary> binary;

  KNImage();
};

struct KNMedia
{
  boost::optional<ID_t> geometry;
  boost::optional<ID_t> style;
  boost::optional<bool> placeholder;
  boost::optional<KNSize> placeholderSize;
  boost::optional<KNGeometry> cropGeometry;
  boost::optional<KNBinary> data;

  KNMedia();
};

struct KNWrap
{
  boost::optional<ID_t> path;
  boost::optional<ID_t> geometry;

  KNWrap();
};

struct KNGroup
{
  IDs_t geometries;
  IDs_t groups;
  IDs_t images;
  IDs_t lines;
  IDs_t media;
  IDs_t shapes;
  IDs_t wraps;

  KNGroup();
};

struct KNLayer
{
  boost::optional<std::string> type;
  IDs_t groups;
  IDs_t images;
  IDs_t lines;
  IDs_t media;
  IDs_t shapes;

  KNLayer();
};

}

#endif //  KNTYPES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
