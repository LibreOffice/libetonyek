/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKImageElement.h"

#include "libetonyek_xml.h"
#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKGeometryElement.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKImageElement::IWORKImageElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_image(new IWORKImage())
{
}

void IWORKImageElement::startOfElement()
{
  if (isCollector())
    getCollector().startLevel();
}

void IWORKImageElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::locked :
    m_image->m_locked = bool_cast(value);
    break;
  default :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t IWORKImageElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  default:
  {
    static bool first=true;
    if (first)
    {
      first=false;
      ETONYEK_DEBUG_MSG(("IWORKImageElement::element: find some unknown elements\n"));
    }
    break;
  }
  }

  return IWORKXMLContextPtr_t();
}

void IWORKImageElement::endOfElement()
{
  if (getId())
    getState().getDictionary().m_images[get(getId())] = m_image;
  if (isCollector())
  {
    getCollector().collectImage(m_image);
    getCollector().endLevel();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
