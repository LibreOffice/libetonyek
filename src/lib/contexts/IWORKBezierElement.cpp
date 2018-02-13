/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKBezierElement.h"

#include <memory>

#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKPath.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKBezierElement::IWORKBezierElement(IWORKXMLParserState &state, IWORKPathPtr_t &path)
  : IWORKXMLEmptyContextBase(state)
  , m_path(path)
{
}

void IWORKBezierElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::path :
    try
    {
      m_path = std::make_shared<IWORKPath>(value);
    }
    catch (const IWORKPath::InvalidException &)
    {
      ETONYEK_DEBUG_MSG(("IWORKBezierElement::attribute: '%s' is not a valid path\n", value));
    }
    break;
  default :
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  }
}

void IWORKBezierElement::endOfElement()
{
  if (getId())
    getState().getDictionary().m_beziers[get(getId())] = m_path;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
