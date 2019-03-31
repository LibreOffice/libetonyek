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
  AuthorStorage = 212,
  CustomDateTimeFormat = 222,
  // 401: StyleRoot, ie. list of different styles id
  Text = 2001,
  ShapeField = 2003,
  NoteStart = 2004, // find at the beginning of a note
  Note = 2008,
  DrawableShape = 2011,
  CommentField = 2013,
  StickyNote = 2014,
  CharacterStyle = 2021,
  ParagraphStyle = 2022,
  ListStyle = 2023,
  SectionStyle = 2024,
  GraphicStyle = 2025,
  // 2031: placeholder field
  Link = 2032,
  DateTimeField = 2034,
  PageField = 2043,
  Image = 3005,
  Group = 3008,
  ConnectionLine = 3009,
  MediaStyle = 3016,
  // 3047: related to columns ?
  Comment = 3056,
  TabularInfo = 6000,
  TabularModel = 6001,
  Tile = 6002,
  TableStyle = 6003,
  CellStyle = 6004,
  DataList = 6005,
  Headers = 6006,

  TextRef = 6218,

  GridLines = 6305,
  GridLine = 6306,

  PageMaster = 10011,
  PageMasterData = 10016,
  HeadersAndFooters = 10143
};

}

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
