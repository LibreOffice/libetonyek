/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKListLabelTypeinfoElement.h"

#include "IWORKDictionary.h"
#include "IWORKToken.h"
#include "IWORKTokenizer.h"
#include "IWORKXMLParserState.h"
#include "libetonyek_utils.h"

namespace libetonyek
{

IWORKListLabelTypeinfoElement::IWORKListLabelTypeinfoElement(IWORKXMLParserState &state, boost::optional<IWORKListLabelTypeInfo_t> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
  , m_bullet()
  , m_text()
  , m_image()
{
}

void IWORKListLabelTypeinfoElement::attribute(const int name, const char *const value)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::type))
    m_label = getState().getTokenizer().getId(value) != IWORKToken::none;
  return IWORKXMLElementContextBase::attribute(name, value);
}

IWORKXMLContextPtr_t IWORKListLabelTypeinfoElement::element(const int name)
{
  (void) name;
#if 0
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::text_label :
    return TextLabelElement(getState(), m_text);
  }
#endif
  return IWORKXMLContextPtr_t();
}

void IWORKListLabelTypeinfoElement::endOfElement()
{
  if ((m_bullet && m_text) || (m_bullet && m_image) || (m_text && m_image))
  {
    ETONYEK_DEBUG_MSG(("IWORKListLabelTypeinfoElement::endOfElement: more than one label type found\n"));
  }
  if (m_label && !m_bullet && !m_text && !m_image)
  {
    ETONYEK_DEBUG_MSG(("IWORKListLabelTypeinfoElement::endOfElement: no label type found\n"));
  }

  if (m_label)
  {
    if (m_image)
      m_value = get(m_image);
    else if (m_text)
      m_value = get(m_text);
    else if (m_bullet)
      m_value = get(m_bullet);
  }

  if (getId())
    getState().getDictionary().m_listLabelTypeInfos[get(getId())] = get_optional_value_or(m_value, IWORKListLabelTypeInfo_t());
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
