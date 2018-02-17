/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWAOBJECTTYPE_H_INCLUDED
#define IWAOBJECTTYPE_H_INCLUDED

namespace libetonyek
{

namespace IWAObjectType
{

enum
{
  Text = 2001,
  // 2003: line or shape
  DrawableShape = 2011,
  StickyNote = 2014,
  CharacterStyle = 2021,
  ParagraphStyle = 2022,
  ListStyle = 2023,
  GraphicStyle = 2025,
  Link = 2032,
  DateTimeField = 2034,
  PageField = 2043,
  Image = 3005,
  Group = 3008,
  Comment = 3056,
  TabularInfo = 6000,
  TabularModel = 6001,
  Tile = 6002,
  TableStyle = 6003,
  CellStyle = 6004,
  DataList = 6005,
  Headers = 6006
};

}

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
