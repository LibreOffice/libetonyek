/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKListstyleElement.h"

#include <utility>

#include <boost/make_shared.hpp>

#include "IWORKDictionary.h"
#include "IWORKListLabelGeometriesProperty.h"
#include "IWORKListLabelIndentsProperty.h"
#include "IWORKListLabelTypesProperty.h"
#include "IWORKListTextIndentsProperty.h"
#include "IWORKProperties.h"
#include "IWORKPropertyMap.h"
#include "IWORKPropertyMapElement.h"
#include "IWORKStyle.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKListstyleElement::IWORKListstyleElement(IWORKXMLParserState &state, IWORKListStyle_t &style)
  : IWORKXMLElementContextBase(state)
  , m_style(style)
{
}

IWORKXMLContextPtr_t IWORKListstyleElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::property_map))
    return makeContext<IWORKPropertyMapElement>(getState(), *this);
  return IWORKXMLContextPtr_t();
}

void IWORKListstyleElement::endOfElement()
{
  using namespace property;
  using std::max;

  const std::size_t levels = (max)((max)(m_labelGeometries.size(), m_typeInfos.size()),
                                   (max)(m_labelIndents.size(), m_textIndents.size()));
  std::deque<IWORKPropertyMap> levelProps(levels);
  for (std::size_t i = 0; i != levels; ++i)
  {
    if (i < m_labelGeometries.size())
      levelProps[i].put<ListLabelGeometry>(m_labelGeometries[i]);
    if (i < m_typeInfos.size())
      levelProps[i].put<ListLabelTypeInfo>(m_typeInfos[i]);
    if (i < m_labelIndents.size())
      levelProps[i].put<ListLabelIndent>(m_labelIndents[i]);
    if (i < m_textIndents.size())
      levelProps[i].put<ListTextIndent>(m_textIndents[i]);
  }
  for (std::size_t i = 0; i != levels; ++i)
    m_style[i] = boost::make_shared<IWORKStyle>(levelProps[i], boost::none, boost::none);

  if (getId())
    getState().getDictionary().m_listStyles[get(getId())] = m_style;
}

IWORKXMLContextPtr_t IWORKListstyleElement::handle(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::listLabelGeometries :
    return makeContext<IWORKListLabelGeometriesProperty>(getState(), m_labelGeometries);
  case IWORKToken::NS_URI_SF | IWORKToken::listLabelIndents :
    return makeContext<IWORKListLabelIndentsProperty>(getState(), m_labelIndents);
  case IWORKToken::NS_URI_SF | IWORKToken::listLabelTypes :
    return makeContext<IWORKListLabelTypesProperty>(getState(), m_typeInfos);
  case IWORKToken::NS_URI_SF | IWORKToken::listTextIndents :
    return makeContext<IWORKListTextIndentsProperty>(getState(), m_textIndents);
  }
  return IWORKXMLContextPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
