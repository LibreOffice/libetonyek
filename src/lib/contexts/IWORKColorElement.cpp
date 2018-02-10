/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKColorElement.h"

#include <boost/lexical_cast.hpp>

#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

using boost::lexical_cast;

IWORKColorElement::IWORKColorElement(IWORKXMLParserState &state, boost::optional<IWORKColor> &color)
  : IWORKXMLEmptyContextBase(state)
  , m_color(color)
  , m_type("")
  , m_r(0)
  , m_g(0)
  , m_b(0)
  , m_w(0)
  , m_c(0)
  , m_m(0)
  , m_y(0)
  , m_k(0)
  , m_a(1)
{
}

void IWORKColorElement::attribute(const int name, const char *const value)
{
  try
  {
    switch (name)
    {
    // TODO: check xsi:type too
    case IWORKToken::NS_URI_SFA | IWORKToken::a :
      m_a = lexical_cast<double>(value);
      break;
    case IWORKToken::NS_URI_SFA | IWORKToken::b :
      m_b = lexical_cast<double>(value);
      break;
    case IWORKToken::NS_URI_SFA | IWORKToken::c :
      m_c = lexical_cast<double>(value);
      break;
    case IWORKToken::NS_URI_SFA | IWORKToken::g :
      m_g = lexical_cast<double>(value);
      break;
    case IWORKToken::NS_URI_SFA | IWORKToken::k :
      m_k = lexical_cast<double>(value);
      break;
    case IWORKToken::NS_URI_SFA | IWORKToken::m :
      m_m = lexical_cast<double>(value);
      break;
    case IWORKToken::NS_URI_SFA | IWORKToken::r :
      m_r = lexical_cast<double>(value);
      break;
    case IWORKToken::NS_URI_SFA | IWORKToken::w :
      m_w = lexical_cast<double>(value);
      break;
    case IWORKToken::NS_URI_SFA | IWORKToken::y :
      m_y = lexical_cast<double>(value);
      break;
    case IWORKToken::NS_URI_XSI | IWORKToken::type :
      m_type = value;
      break;
    default:
      break;
    }
  }
  catch (const boost::bad_lexical_cast &)
  {
    ETONYEK_DEBUG_MSG(("IWORKColorElement::attribute: invalid color value: %s\n", value));
  }
}

IWORKXMLContextPtr_t IWORKColorElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::custom_space_color | IWORKToken::NS_URI_SFA :
  {
    static bool first=true;
    if (first)
    {
      ETONYEK_DEBUG_MSG(("IWORKColorElement::element: found a custom color element\n"));
      first=false;
    }
    return IWORKXMLContextPtr_t();
  }
  default:
    ETONYEK_DEBUG_MSG(("IWORKColorElement::element: found unknown element\n"));
  }
  return IWORKXMLEmptyContextBase::element(name);
}

void IWORKColorElement::endOfElement()
{
  if (m_type=="sfa:calibrated-white-color-type" || m_type=="sfa:device-white-color-type")
    m_color = IWORKColor(m_w, m_w, m_w, m_a);
  // sfa:named-color-type seems a basic rgb-color-type with a sfa:name and a sfa:catalog
  else if (m_type=="sfa:calibrated-rgb-color-type" || m_type=="sfa:device-rgb-color-type" ||
           m_type=="sfa:named-color-type")
    m_color = IWORKColor(m_r, m_g, m_b, m_a);
  else if (m_type=="sfa:calibrated-cmyk-color-type" || m_type=="sfa:device-cmyk-color-type")
  {
    // checkme
    double w=1-m_k;
    m_color = IWORKColor((1-m_c) * w,(1-m_m) * w,(1-m_y) * w, m_a);
  }
  else
  {
    ETONYEK_DEBUG_MSG(("IWORKColorElement::endOfElement: unknown type: %s\n", m_type.c_str()));
    m_color = IWORKColor(m_r, m_g, m_b, m_a);
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
