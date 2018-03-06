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
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_graphicStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::headline_style :
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_headlineStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle :
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_listStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::cell_style :
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::chart_style :
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_chartStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::chart_series_style :
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_chartSeriesStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::graphic_style :
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_graphicStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::characterstyle :
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_characterStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle :
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_layoutStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle :
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_paragraphStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::table_style :
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_tableStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::table_cell_style :
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_tableCellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::table_vector_style :
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_tableVectorStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_style :
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_tabularStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::tocstyle :
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_tocStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::vector_style :
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_vectorStyles);

  case IWORKToken::NS_URI_SF | IWORKToken::cell_style_ref :
    return std::make_shared<IWORKStyleRefContext>(getState(), getState().getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::characterstyle_ref :
    return std::make_shared<IWORKStyleRefContext>(getState(), getState().getDictionary().m_characterStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::chart_style_ref :
    return std::make_shared<IWORKStyleRefContext>(getState(), getState().getDictionary().m_chartStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::chart_series_style_ref :
    return std::make_shared<IWORKStyleRefContext>(getState(), getState().getDictionary().m_chartSeriesStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::connection_style_ref :
    return std::make_shared<IWORKStyleRefContext>(getState(), getState().getDictionary().m_graphicStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::graphic_style_ref :
    return std::make_shared<IWORKStyleRefContext>(getState(), getState().getDictionary().m_graphicStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle_ref :
    return std::make_shared<IWORKStyleRefContext>(getState(), getState().getDictionary().m_layoutStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle_ref :
    return std::make_shared<IWORKStyleRefContext>(getState(), getState().getDictionary().m_listStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle_ref :
    return std::make_shared<IWORKStyleRefContext>(getState(), getState().getDictionary().m_paragraphStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::table_style_ref :
    return std::make_shared<IWORKStyleRefContext>(getState(), getState().getDictionary().m_tableStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::table_cell_style_ref :
    return std::make_shared<IWORKStyleRefContext>(getState(), getState().getDictionary().m_tableCellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::table_vector_style_ref :
    return std::make_shared<IWORKStyleRefContext>(getState(), getState().getDictionary().m_tableVectorStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_style_ref :
    return std::make_shared<IWORKStyleRefContext>(getState(), getState().getDictionary().m_tabularStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::tocstyle_ref :
    return std::make_shared<IWORKStyleRefContext>(getState(), getState().getDictionary().m_tocStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::vector_style_ref :
    return std::make_shared<IWORKStyleRefContext>(getState(), getState().getDictionary().m_vectorStyles);
  default:
    ETONYEK_DEBUG_MSG(("IWORKStylesContext::element: find some unknown element %d\n", int(name)));
  }

  return IWORKXMLContextPtr_t();
}

void IWORKStylesContext::endOfElement()
{
}


}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
