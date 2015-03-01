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

#include "IWORKTypes_fwd.h"

#include <deque>

#include <boost/optional.hpp>

#include "libetonyek_utils.h"
#include "IWORKObject.h"
#include "IWORKPath_fwd.h"
#include "IWORKStyle_fwd.h"

namespace libetonyek
{

class IWORKTransformation;

struct IWORKSize
{
  double m_width;
  double m_height;

  IWORKSize();
  IWORKSize(double w, double h);
};

struct IWORKPosition
{
  double m_x;
  double m_y;

  IWORKPosition();
  IWORKPosition(double x_, double y_);
};

struct IWORKGeometry
{
  IWORKSize m_naturalSize;
  IWORKSize m_size;
  IWORKPosition m_position;
  boost::optional<double> m_angle;
  boost::optional<double> m_shearXAngle;
  boost::optional<double> m_shearYAngle;
  boost::optional<bool> m_horizontalFlip;
  boost::optional<bool> m_verticalFlip;
  boost::optional<bool> m_aspectRatioLocked;
  boost::optional<bool> m_sizesLocked;

  IWORKGeometry();
};

struct IWORKColor
{
  double m_red;
  double m_green;
  double m_blue;
  double m_alpha;

  IWORKColor();
  IWORKColor(double r, double g, double b, double a);
};

struct IWORKPadding
{
  boost::optional<int> m_top;
  boost::optional<int> m_right;
  boost::optional<int> m_bottom;
  boost::optional<int> m_left;

  IWORKPadding();
};

struct IWORKTabStop
{
  double m_pos;

  explicit IWORKTabStop(double pos_);
};

typedef std::deque<IWORKTabStop> IWORKTabStops_t;

struct IWORKLine
{
  IWORKGeometryPtr_t m_geometry;
  IWORKStylePtr_t m_style;
  boost::optional<double> m_x1;
  boost::optional<double> m_y1;
  boost::optional<double> m_x2;
  boost::optional<double> m_y2;

  IWORKLine();
};

struct IWORKData
{
  RVNGInputStreamPtr_t m_stream;
  boost::optional<std::string> m_displayName;
  boost::optional<int> m_type;

  IWORKData();
};

struct IWORKMediaContent
{
  boost::optional<IWORKSize> m_size;
  IWORKDataPtr_t m_data;

  IWORKMediaContent();
};

struct IWORKBinary
{
  boost::optional<IWORKSize> m_size;
  boost::optional<std::string> m_path;
  boost::optional<std::string> m_type;
  boost::optional<unsigned> m_dataSize;

  IWORKBinary();
};

struct IWORKImage
{
  boost::optional<bool> m_locked;
  IWORKGeometryPtr_t m_geometry;
  boost::optional<IWORKBinary> m_binary;

  IWORKImage();
};

struct IWORKMedia
{
  IWORKGeometryPtr_t m_geometry;
  IWORKStylePtr_t m_style;
  boost::optional<bool> m_placeholder;
  boost::optional<IWORKSize> m_placeholderSize;
  IWORKMediaContentPtr_t m_content;

  IWORKMedia();
};

struct IWORKWrap
{
  IWORKPathPtr_t m_path;
  IWORKGeometryPtr_t m_geometry;

  IWORKWrap();
};

struct IWORKGroup
{
  IWORKObjectList_t m_objects;

  IWORKGroup();
};

IWORKObjectPtr_t makeObject(const IWORKGroupPtr_t &group);
IWORKObjectPtr_t makeObject(const IWORKImagePtr_t &image, const IWORKTransformation &trafo);
IWORKObjectPtr_t makeObject(const IWORKLinePtr_t &line, const IWORKTransformation &trafo);
IWORKObjectPtr_t makeObject(const IWORKMediaPtr_t &media, const IWORKTransformation &trafo);

}

#endif //  IWORKTYPES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
