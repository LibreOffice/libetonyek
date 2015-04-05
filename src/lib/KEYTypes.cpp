/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYTypes.h"

#include <librevenge/librevenge.h>

#include "libetonyek_utils.h"
#include "IWORKDocumentInterface.h"
#include "IWORKText.h"
#include "IWORKTransformation.h"

namespace libetonyek
{

KEYLayer::KEYLayer()
  : m_type()
  , m_zoneId()
{
}

KEYPlaceholder::KEYPlaceholder()
  : m_title()
  , m_empty()
  , m_style()
  , m_geometry()
  , m_text()
{
}

KEYStickyNote::KEYStickyNote()
  : m_geometry()
  , m_text()
{
}

KEYStickyNote::KEYStickyNote(const IWORKGeometryPtr_t &geometry_, const IWORKTextPtr_t &text_)
  : m_geometry(geometry_)
  , m_text(text_)
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
