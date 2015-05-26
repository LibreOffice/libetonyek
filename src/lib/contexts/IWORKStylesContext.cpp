/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKStylesContext.h"

#include "IWORKStyleContext.h"
#include "IWORKStyleRefContext.h"
#include "IWORKToken.h"

namespace libetonyek
{

IWORKStylesContext::IWORKStylesContext(IWORKXMLParserState &state, const bool anonymous)
  : IWORKXMLElementContextBase(state)
  , m_anonymous(anonymous)
{
}

IWORKXMLContextPtr_t IWORKStylesContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::cell_style :
  case IWORKToken::NS_URI_SF | IWORKToken::characterstyle :
  case IWORKToken::NS_URI_SF | IWORKToken::connection_style :
  case IWORKToken::NS_URI_SF | IWORKToken::graphic_style :
  case IWORKToken::NS_URI_SF | IWORKToken::headline_style :
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle :
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle :
  case IWORKToken::NS_URI_SF | IWORKToken::slide_style :
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_style :
  case IWORKToken::NS_URI_SF | IWORKToken::vector_style :
    return makeContext<IWORKStyleContext>(getState(), name);

  case IWORKToken::NS_URI_SF | IWORKToken::cell_style_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::characterstyle_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::vector_style_ref :
    return makeContext<IWORKStyleRefContext>(getState(), name, false, m_anonymous);
  }

  return IWORKXMLContextPtr_t();
}

void IWORKStylesContext::endOfElement()
{
}


}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
