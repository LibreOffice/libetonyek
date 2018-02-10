/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKFilteredImageElement.h"

#include "IWORKDictionary.h"
#include "IWORKImageContext.h"
#include "IWORKRefContext.h"
#include "IWORKSizeElement.h"
#include "IWORKToken.h"
#include "IWORKUnfilteredElement.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

namespace
{

typedef IWORKImageContext IWORKFilteredElement;
typedef IWORKImageContext LeveledElement;

}


IWORKFilteredImageElement::IWORKFilteredImageElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content)
  : IWORKXMLElementContextBase(state)
  , m_content(content)
  , m_unfilteredId()
  , m_unfiltered()
  , m_filtered()
  , m_leveled()
{
}

IWORKXMLContextPtr_t IWORKFilteredImageElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::unfiltered :
    return makeContext<IWORKUnfilteredElement>(getState(), m_unfiltered);
  case IWORKToken::NS_URI_SF | IWORKToken::unfiltered_ref :
    return makeContext<IWORKRefContext>(getState(), m_unfilteredId);
  case IWORKToken::NS_URI_SF | IWORKToken::filtered :
    return makeContext<IWORKFilteredElement>(getState(), m_filtered);
  case IWORKToken::NS_URI_SF | IWORKToken::leveled :
    return makeContext<LeveledElement>(getState(), m_leveled);
  case IWORKToken::NS_URI_SF | IWORKToken::extent : // TODO readme
  case IWORKToken::NS_URI_SF | IWORKToken::filter_properties :
    break;
  default:
    ETONYEK_DEBUG_MSG(("IWORKFilteredImageElement::element: unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void IWORKFilteredImageElement::endOfElement()
{
  if (m_unfilteredId && !m_unfiltered)
  {
    const IWORKMediaContentMap_t::const_iterator it = getState().getDictionary().m_unfiltereds.find(get(m_unfilteredId));
    if (getState().getDictionary().m_unfiltereds.end() != it)
      m_unfiltered = it->second;
    else
    {
      ETONYEK_DEBUG_MSG(("IWORKFilteredImageElement::endOfElement: can not find image %s\n", get(m_unfilteredId).c_str()));
    }
  }

  // If a filter is applied to an image, the new image is saved next
  // to the original. So all we need is to pick the right one. We
  // can happily ignore the whole filter-properties section :-)
  // NOTE: Leveled is apparently used to save the result of using
  // the "Enhance" button.
  if (bool(m_filtered))
    m_content = m_filtered;
  else if (bool(m_leveled))
    m_content = m_leveled;
  else
    m_content = m_unfiltered;
  if (bool(m_content) && getId())
    getState().getDictionary().m_filteredImages[get(getId())] = m_content;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
