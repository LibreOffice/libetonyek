/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/lexical_cast.hpp>

#include "IWORKDataElement.h"

#include "IWORKDictionary.h"
#include "IWORKParser.h"
#include "IWORKToken.h"
#include "IWORKTokenizer.h"
#include "IWORKTypes.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKDataElement::IWORKDataElement(IWORKXMLParserState &state, IWORKDataPtr_t &data)
  : IWORKXMLEmptyContextBase(state)
  , m_data(data)
  , m_displayName()
  , m_stream()
  , m_mimeType()
{
}

void IWORKDataElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::displayname :
    m_displayName = value;
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::hfs_type :
  {
    switch (getState().getTokenizer().getId(value))
    {
    case IWORKToken::_1246774599 :
      m_mimeType = "image/jpeg";
      break;
    case IWORKToken::_1299148630 :
      m_mimeType = "video/quicktime";
      break;
    case IWORKToken::_1346651680 :
      m_mimeType = "application/pdf";
      break;
    case IWORKToken::_1347307366 :
      m_mimeType = "image/png";
      break;
    case IWORKToken::_1414088262 :
      m_mimeType = "image/tiff";
      break;
    default :
      break;
    }
    break;
  }
  case IWORKToken::NS_URI_SF | IWORKToken::path :
    m_stream.reset(getState().getParser().getPackage()->getSubStreamByName(value));
    break;
  default :
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  }
}

void IWORKDataElement::endOfElement()
{
  if (bool(m_stream))
  {
    m_data.reset(new IWORKData());
    m_data->m_stream = m_stream;
    m_data->m_displayName = m_displayName;
    if (m_mimeType)
      m_data->m_mimeType = get(m_mimeType);

    if (getId())
      getState().getDictionary().m_data[get(getId())] = m_data;
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
