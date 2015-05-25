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

namespace libetonyek
{

KEYLayer::KEYLayer()
  : m_type()
  , m_outputId()
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

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
