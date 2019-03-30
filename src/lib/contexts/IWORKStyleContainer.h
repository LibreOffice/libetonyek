/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKSTYLECONTAINER_H_INCLUDED
#define IWORKSTYLECONTAINER_H_INCLUDED

#include <memory>
#include <string>

#include <boost/optional.hpp>

#include "IWORKStyle_fwd.h"
#include "IWORKStyleContext.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

#include "IWORKXMLContextBase.h"

namespace libetonyek
{

template<int TokenId, int RefTokenId, int TokenId2=0, int RefTokenId2=0>
class IWORKStyleContainer : public IWORKXMLElementContextBase
{
public:
  IWORKStyleContainer(IWORKXMLParserState &state, IWORKStylePtr_t &style, IWORKStyleMap_t &styleMap);
  IWORKStyleContainer(IWORKXMLParserState &state, IWORKStylePtr_t &style, IWORKStyleMap_t &styleMap, IWORKStyleMap_t &styleMap2);

protected:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKStyleContainer(IWORKStyleContainer const &);
  IWORKStyleContainer &operator=(IWORKStyleContainer const &);

  IWORKStylePtr_t &m_style;
  IWORKStyleMap_t &m_styleMap;
  std::shared_ptr<IWORKStyleContext> m_context;
  boost::optional<ID_t> m_ref;
  IWORKStyleMap_t *m_styleMap2;
  boost::optional<ID_t> m_ref2;
};

template<int TokenId, int RefTokenId, int TokenId2, int RefTokenId2>
IWORKStyleContainer<TokenId, RefTokenId, TokenId2, RefTokenId2>::IWORKStyleContainer(IWORKXMLParserState &state, IWORKStylePtr_t &style, IWORKStyleMap_t &styleMap)
  : IWORKXMLElementContextBase(state)
  , m_style(style)
  , m_styleMap(styleMap)
  , m_context()
  , m_ref()
  , m_styleMap2(nullptr)
  , m_ref2()
{
}

template<int TokenId, int RefTokenId, int TokenId2, int RefTokenId2>
IWORKStyleContainer<TokenId, RefTokenId, TokenId2, RefTokenId2>::IWORKStyleContainer(IWORKXMLParserState &state, IWORKStylePtr_t &style, IWORKStyleMap_t &styleMap, IWORKStyleMap_t &styleMap2)
  : IWORKXMLElementContextBase(state)
  , m_style(style)
  , m_styleMap(styleMap)
  , m_context()
  , m_ref()
  , m_styleMap2(&styleMap2)
  , m_ref2()
{
}

template<int TokenId, int RefTokenId, int TokenId2, int RefTokenId2>
IWORKXMLContextPtr_t IWORKStyleContainer<TokenId, RefTokenId, TokenId2, RefTokenId2>::element(const int name)
{
  switch (name)
  {
  case TokenId :
    m_context = std::make_shared<IWORKStyleContext>(getState(), &m_styleMap);
    return m_context;
  case RefTokenId :
    return std::make_shared<IWORKRefContext>(getState(), m_ref);
  case IWORKToken::NS_URI_SF | IWORKToken::null:
    return IWORKXMLContextPtr_t();
  case IWORKToken::INVALID_TOKEN: // TokenId2 and RefTokenId2 are optional, so avoid unintentional match
    break;
  default:
    if (!name) break;
    if (name==TokenId2)
    {
      m_context = std::make_shared<IWORKStyleContext>(getState(), m_styleMap2);
      return m_context;
    }
    if (name==RefTokenId2)
      return std::make_shared<IWORKRefContext>(getState(), m_ref2);
    break;
  }
  ETONYEK_DEBUG_MSG(("IWORKStyleContainer<...>::element: unknown element %d\n", name));
  return IWORKXMLContextPtr_t();
}

template<int TokenId, int RefTokenId, int TokenId2, int RefTokenId2>
void IWORKStyleContainer<TokenId, RefTokenId, TokenId2, RefTokenId2>::endOfElement()
{
  if (m_ref)
    m_style = getState().getStyleByName(get(m_ref).c_str(), m_styleMap);
  else if (m_ref2 && m_styleMap2)
    m_style = getState().getStyleByName(get(m_ref2).c_str(), *m_styleMap2);
  else if (m_context)
    m_style=m_context->getStyle();
}

}

#endif // IWORKSTYLECONTAINER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
