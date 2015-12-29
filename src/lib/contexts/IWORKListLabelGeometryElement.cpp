/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKListLabelGeometryElement.h"

#include "IWORKDictionary.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"
#include "libetonyek_xml.h"

namespace libetonyek
{

IWORKListLabelGeometryElement::IWORKListLabelGeometryElement(IWORKXMLParserState &state, boost::optional<IWORKListLabelGeometry> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
  , m_scale()
  , m_offset()
  , m_scaleWithText()
{
}

void IWORKListLabelGeometryElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::scale :
    m_scale = try_double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::offset :
    m_offset = try_double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::scale_with_text :
    m_scaleWithText = try_bool_cast(value);
    break;
  }
  IWORKXMLEmptyContextBase::attribute(name, value);
}

void IWORKListLabelGeometryElement::endOfElement()
{
  IWORKListLabelGeometry value;
  if (m_scale)
    value.m_scale = get(m_scale);
  if (m_offset)
    value.m_offset = get(m_offset);
  if (m_scaleWithText)
    value.m_scaleWithText = get(m_scaleWithText);
  m_value = value;
  if (getId())
    getState().getDictionary().m_listLabelGeometries[get(getId())] = value;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
