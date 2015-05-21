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

#include "KEYTypes_fwd.h"

#include <deque>

#include <boost/optional.hpp>

#include "libetonyek_utils.h"
#include "IWORKPath_fwd.h"
#include "IWORKStyle_fwd.h"
#include "IWORKText_fwd.h"
#include "IWORKTypes.h"

namespace libetonyek
{

struct KEYLayer
{
  boost::optional<std::string> m_type;
  boost::optional<IWORKZoneID_t> m_zoneId;

  KEYLayer();
};

struct KEYPlaceholder
{
  boost::optional<bool> m_title;
  boost::optional<bool> m_empty;
  IWORKStylePtr_t m_style;
  IWORKGeometryPtr_t m_geometry;
  IWORKTextPtr_t m_text;

  KEYPlaceholder();
};

struct KEYStickyNote
{
  IWORKGeometryPtr_t m_geometry;
  IWORKTextPtr_t m_text;

  KEYStickyNote();
};

typedef std::deque<KEYStickyNote> KEYStickyNotes_t;

}

#endif //  KEYTYPES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
