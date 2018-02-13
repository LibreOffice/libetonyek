/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTabsElement.h"

#include <boost/optional.hpp>

#include "libetonyek_xml.h"
#include "IWORKDictionary.h"
#include "IWORKToken.h"
#include "IWORKTokenizer.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

using boost::optional;

namespace
{

class TabstopElement : public IWORKXMLEmptyContextBase
{
public:
  TabstopElement(IWORKXMLParserState &state, optional<IWORKTabStop> &tab);

private:
  void attribute(int name, const char *value) override;

private:
  optional<IWORKTabStop> &m_tab;
};

TabstopElement::TabstopElement(IWORKXMLParserState &state, optional<IWORKTabStop> &tab)
  : IWORKXMLEmptyContextBase(state)
  , m_tab(tab)
{
  tab=IWORKTabStop();
}

void TabstopElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::align :
    switch (getState().getTokenizer().getId(value))
    {
    case IWORKToken::center :
      get(m_tab).m_align = IWORK_TABULATION_CENTER;
      break;
    case IWORKToken::decimal :
      get(m_tab).m_align = IWORK_TABULATION_DECIMAL;
      break;
    case IWORKToken::left :
      get(m_tab).m_align = IWORK_TABULATION_LEFT;
      break;
    case IWORKToken::right :
      get(m_tab).m_align = IWORK_TABULATION_RIGHT;
      break;
    default:
      ETONYEK_DEBUG_MSG(("TabstopElement::attribute: unknown alignment %s\n", value));
    }
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::pos :
    get(m_tab).m_pos = double_cast(value);
    break;
  default :
    break;
  }
}

}

IWORKTabsElement::IWORKTabsElement(IWORKXMLParserState &state, IWORKTabStops_t &tabs)
  : IWORKXMLElementContextBase(state)
  , m_tabs(tabs)
  , m_current()
{
}

IWORKXMLContextPtr_t IWORKTabsElement::element(const int name)
{
  if (m_current)
  {
    m_tabs.push_back(get(m_current));
    m_current.reset();
  }

  if ((IWORKToken::NS_URI_SF | IWORKToken::tabstop) == name)
    return std::make_shared<TabstopElement>(getState(), m_current);

  return IWORKXMLContextPtr_t();
}

void IWORKTabsElement::endOfElement()
{
  if (m_current)
    m_tabs.push_back(get(m_current));

  if (getId())
    getState().getDictionary().m_tabs[get(getId())] = m_tabs;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
