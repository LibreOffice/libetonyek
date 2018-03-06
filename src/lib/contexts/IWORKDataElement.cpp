/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKDataElement.h"

#include <memory>

#include "IWORKDictionary.h"
#include "IWORKParser.h"
#include "IWORKToken.h"
#include "IWORKTokenizer.h"
#include "IWORKTypes.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKDataElement::IWORKDataElement(IWORKXMLParserState &state, IWORKDataPtr_t &data, boost::optional<IWORKColor> &fillColor)
  : IWORKXMLEmptyContextBase(state)
  , m_data(data)
  , m_fillColor(fillColor)
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
    if (!m_stream)
    {
      // basic theme files can be absent, try to recover some
      std::string val(value);
      auto lastSlash = val.find_last_of('/');
      if (std::string::npos != lastSlash)
        val.erase(0,lastSlash + 1);
      auto len=val.length();
      if (len>15 && val.compare(0, 11, "tile_paper_")==0)
      {
        auto lastPoint = val.find_last_of('.');
        if (std::string::npos != lastPoint && lastPoint>12)
        {
          std::map<std::string, IWORKColor> fileColor=
          {
            { "blue", IWORKColor(0,0,1,1) },
            { "gray", IWORKColor(0.5,0.5,0.5,1) },
            { "green", IWORKColor(0,1,0,1) },
            { "purple", IWORKColor(0.5,0,0.5,1) },
            { "red", IWORKColor(1,0,0,1) },
            { "yellow", IWORKColor(1,1,0,1) }
          };
          auto it=fileColor.find(val.substr(11,lastPoint-11));
          if (it!=fileColor.end())
          {
            m_fillColor=it->second;
            break;
          }
        }
      }
      // find also "Girl and Snowcone", "Happy Girls", or Shared/*(~430 picture here)
      ETONYEK_DEBUG_MSG(("IWORKDataElement::attribute: can not find %s\n",value));
    }
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
    m_data = std::make_shared<IWORKData>();
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
