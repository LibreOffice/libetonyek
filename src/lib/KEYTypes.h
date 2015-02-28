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
#include "IWORKObject.h"
#include "IWORKPath_fwd.h"
#include "IWORKStyle.h"
#include "IWORKTypes_fwd.h"
#include "KEYText_fwd.h"
#include "KEYTypes_fwd.h"

namespace libetonyek
{

class IWORKTransformation;

struct KEYLine
{
  IWORKGeometryPtr_t geometry;
  IWORKStylePtr_t style;
  boost::optional<double> x1;
  boost::optional<double> y1;
  boost::optional<double> x2;
  boost::optional<double> y2;

  KEYLine();
};

struct KEYData
{
  RVNGInputStreamPtr_t stream;
  boost::optional<std::string> displayName;
  boost::optional<int> type;

  KEYData();
};

struct KEYMediaContent
{
  boost::optional<IWORKSize> size;
  KEYDataPtr_t data;

  KEYMediaContent();
};

struct KEYBinary
{
  boost::optional<IWORKSize> size;
  boost::optional<std::string> path;
  boost::optional<std::string> type;
  boost::optional<unsigned> dataSize;

  KEYBinary();
};

struct KEYImage
{
  boost::optional<bool> locked;
  IWORKGeometryPtr_t geometry;
  boost::optional<KEYBinary> binary;

  KEYImage();
};

struct KEYMedia
{
  IWORKGeometryPtr_t geometry;
  KEYGraphicStylePtr_t style;
  boost::optional<bool> placeholder;
  boost::optional<IWORKSize> placeholderSize;
  KEYMediaContentPtr_t content;

  KEYMedia();
};

struct KEYWrap
{
  IWORKPathPtr_t path;
  IWORKGeometryPtr_t geometry;

  KEYWrap();
};

struct KEYGroup
{
  IWORKObjectList_t objects;

  KEYGroup();
};

struct KEYLayer
{
  boost::optional<std::string> type;
  IWORKObjectList_t objects;

  KEYLayer();
};

struct KEYPlaceholder
{
  boost::optional<bool> title;
  boost::optional<bool> empty;
  KEYPlaceholderStylePtr_t style;
  IWORKGeometryPtr_t geometry;
  KEYTextPtr_t text;

  KEYPlaceholder();
};

struct KEYStickyNote
{
  IWORKGeometryPtr_t geometry;
  KEYTextPtr_t text;

  KEYStickyNote();
  KEYStickyNote(const IWORKGeometryPtr_t &geometry, const KEYTextPtr_t &text);
};

typedef std::deque<KEYStickyNote> KEYStickyNotes_t;

IWORKObjectPtr_t makeObject(const KEYGroupPtr_t &group);
IWORKObjectPtr_t makeObject(const KEYImagePtr_t &image, const IWORKTransformation &trafo);
IWORKObjectPtr_t makeObject(const KEYLinePtr_t &line, const IWORKTransformation &trafo);
IWORKObjectPtr_t makeObject(const KEYMediaPtr_t &media, const IWORKTransformation &trafo);
IWORKObjectPtr_t makeObject(const KEYPlaceholderPtr_t &placeholder, const IWORKTransformation &trafo);

}

#endif //  KEYTYPES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
