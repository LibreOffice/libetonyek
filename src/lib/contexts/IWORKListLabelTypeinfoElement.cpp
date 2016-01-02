/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKListLabelTypeinfoElement.h"

#include "IWORKBinaryElement.h"
#include "IWORKDictionary.h"
#include "IWORKRefContext.h"
#include "IWORKTextLabelElement.h"
#include "IWORKToken.h"
#include "IWORKTokenizer.h"
#include "IWORKXMLParserState.h"
#include "KEY2Token.h"
#include "libetonyek_utils.h"
#include "libetonyek_xml.h"

namespace libetonyek
{

IWORKListLabelTypeinfoElement::IWORKListLabelTypeinfoElement(IWORKXMLParserState &state, boost::optional<IWORKListLabelTypeInfo_t> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
  , m_text()
  , m_textRef()
  , m_image()
  , m_imageRef()
  , m_isText(false)
  , m_isImage(false)
{
}

void IWORKListLabelTypeinfoElement::attribute(const int name, const char *const value)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::type))
  {
    switch (getState().getTokenizer().getId(value))
    {
    case IWORKToken::image :
      m_isImage = true;
      break;
    case IWORKToken::none :
      break;
    case IWORKToken::text :
    case KEY2Token::text : // NOTE: The hierachical tokenizer can't differentiate these two
      m_isText = true;
      break;
    default :
      ETONYEK_DEBUG_MSG(("IWORKListLabelTypeinfoElement::attribute: unknown label type \"%s\"\n", value));
      break;
    }
  }
  return IWORKXMLElementContextBase::attribute(name, value);
}

IWORKXMLContextPtr_t IWORKListLabelTypeinfoElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::binary :
    return makeContext<IWORKBinaryElement>(getState(), m_image);
  case IWORKToken::NS_URI_SF | IWORKToken::binary_ref :
    return makeContext<IWORKRefContext>(getState(), m_imageRef);
  case IWORKToken::NS_URI_SF | IWORKToken::text_label :
    return makeContext<IWORKTextLabelElement>(getState(), m_text);
  case IWORKToken::NS_URI_SF | IWORKToken::text_label_ref :
    return makeContext<IWORKRefContext>(getState(), m_textRef);
  }
  return IWORKXMLContextPtr_t();
}

void IWORKListLabelTypeinfoElement::endOfElement()
{
  if ((m_isText && !m_text && !m_textRef) || (m_isImage && !m_image && !m_imageRef))
  {
    ETONYEK_DEBUG_MSG(("IWORKListLabelTypeinfoElement::endOfElement: no label def. found\n"));
  }

  if (m_isImage)
  {
    if (bool(m_image))
    {
      m_value = m_image;
    }
    else if (m_imageRef)
    {
      const IWORKMediaContentMap_t::const_iterator it = getState().getDictionary().m_binaries.find(get(m_imageRef));
      if (it != getState().getDictionary().m_binaries.end())
        m_value = it->second;
    }
  }
  else if (m_isText)
  {
    if (m_text)
    {
      m_value = get(m_text);
    }
    else if (m_textRef)
    {
      const IWORKListLabelTypeInfoMap_t::const_iterator it = getState().getDictionary().m_textLabels.find(get(m_textRef));
      if (it != getState().getDictionary().m_textLabels.end())
        m_value = it->second;
    }
  }

  if (getId())
    getState().getDictionary().m_listLabelTypeInfos[get(getId())] = get_optional_value_or(m_value, IWORKListLabelTypeInfo_t());
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
