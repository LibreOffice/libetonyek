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
#include <boost/shared_ptr.hpp>

#include "libetonyek_utils.h"
#include "IWORKOutputElements.h"
#include "IWORKPath_fwd.h"
#include "IWORKStyle_fwd.h"
#include "IWORKText_fwd.h"
#include "IWORKTypes.h"

namespace libetonyek
{

struct KEYLayer
{
  boost::optional<std::string> m_type;
  boost::optional<IWORKOutputID_t> m_outputId;

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

struct KEYSlide
{
  boost::optional<std::string> m_name;
  IWORKOutputElements m_content;
  IWORKStylePtr_t m_style;
  std::shared_ptr<KEYSlide> m_masterSlide;

  KEYSlide();
};

}

#endif //  KEYTYPES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
