/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKListLabelGeometriesProperty.h"

#include "IWORKContainerContext.h"
#include "IWORKDictionary.h"
#include "IWORKListLabelGeometryElement.h"
#include "IWORKPushCollector.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

namespace
{

typedef IWORKContainerContext<IWORKListLabelGeometry, IWORKListLabelGeometryElement, IWORKPushCollector, IWORKToken::NS_URI_SF | IWORKToken::list_label_geometry, IWORKToken::NS_URI_SF | IWORKToken::list_label_geometry_ref> ArrayElement;

}

IWORKListLabelGeometriesProperty::IWORKListLabelGeometriesProperty(IWORKXMLParserState &state, std::deque<IWORKListLabelGeometry> &elements)
  : IWORKXMLElementContextBase(state)
  , m_elements(elements)
{
}

IWORKXMLContextPtr_t IWORKListLabelGeometriesProperty::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::array))
    return makeContext<ArrayElement>(getState(), m_elements);
  return IWORKXMLContextPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
