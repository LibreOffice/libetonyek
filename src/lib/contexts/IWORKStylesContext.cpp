/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKStylesContext.h"

#include "IWORKDictionary.h"
#include "IWORKStyle_fwd.h"
#include "IWORKStyleContext.h"
#include "IWORKStyleRefContext.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

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
  case IWORKToken::NS_URI_SF | IWORKToken::connection_style :
  case IWORKToken::NS_URI_SF | IWORKToken::headline_style :
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle :
    return makeContext<IWORKStyleContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::cell_style :
    return makeContext<IWORKStyleContext>(getState(), &getState().getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::graphic_style :
    return makeContext<IWORKStyleContext>(getState(), &getState().getDictionary().m_graphicStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::characterstyle :
    return makeContext<IWORKStyleContext>(getState(), &getState().getDictionary().m_characterStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle :
    return makeContext<IWORKStyleContext>(getState(), &getState().getDictionary().m_layoutStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle :
    return makeContext<IWORKStyleContext>(getState(), &getState().getDictionary().m_paragraphStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_style :
    return makeContext<IWORKStyleContext>(getState(), &getState().getDictionary().m_tabularStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::vector_style :
    return makeContext<IWORKStyleContext>(getState(), &getState().getDictionary().m_vectorStyles);

  case IWORKToken::NS_URI_SF | IWORKToken::liststyle_ref :
    return IWORKXMLContextPtr_t();
  case IWORKToken::NS_URI_SF | IWORKToken::cell_style_ref :
    return makeContext<IWORKStyleRefContext>(getState(), getState().getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::characterstyle_ref :
    return makeContext<IWORKStyleRefContext>(getState(), getState().getDictionary().m_characterStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle_ref :
    return makeContext<IWORKStyleRefContext>(getState(), getState().getDictionary().m_layoutStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle_ref :
    return makeContext<IWORKStyleRefContext>(getState(), getState().getDictionary().m_paragraphStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_style_ref :
    return makeContext<IWORKStyleRefContext>(getState(), getState().getDictionary().m_tabularStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::vector_style_ref :
    return makeContext<IWORKStyleRefContext>(getState(), getState().getDictionary().m_vectorStyles);
  }

  return IWORKXMLContextPtr_t();
}

void IWORKStylesContext::endOfElement()
{
}


}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
