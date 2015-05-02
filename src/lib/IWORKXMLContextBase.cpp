/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKXMLContextBase.h"

#include "libetonyek_utils.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKXMLContextMinimal::IWORKXMLContextMinimal()
{
}

void IWORKXMLContextMinimal::startOfElement()
{
}

void IWORKXMLContextMinimal::endOfAttributes()
{
}

void IWORKXMLContextMinimal::endOfElement()
{
}

IWORKXMLContextElement::IWORKXMLContextElement(IWORKXMLParserState &)
  : IWORKXMLContextMinimal()
{
}

void IWORKXMLContextElement::attribute(const int name, const char *const value)
{
  if ((IWORKToken::NS_URI_SFA | IWORKToken::ID) == name)
    m_id = value;
}

void IWORKXMLContextElement::text(const char *)
{
  ETONYEK_DEBUG_MSG(("text content is not expected at this element\n"));
}

const boost::optional<ID_t> &IWORKXMLContextElement::getId() const
{
  return m_id;
}

IWORKXMLContextText::IWORKXMLContextText(IWORKXMLParserState &)
  : IWORKXMLContextMinimal()
{
}

IWORKXMLContextPtr_t IWORKXMLContextText::element(int)
{
  ETONYEK_DEBUG_MSG(("no subelement is expected at this element\n"));
  return IWORKXMLContextPtr_t();
}

IWORKXMLContextMixed::IWORKXMLContextMixed(IWORKXMLParserState &)
  : IWORKXMLContextMinimal()
{
}

IWORKXMLContextEmpty::IWORKXMLContextEmpty(IWORKXMLParserState &)
  : IWORKXMLContextMinimal()
{
}

void IWORKXMLContextEmpty::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    m_id = value;
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::IDREF :
    m_ref = value;
    break;
  }
}

IWORKXMLContextPtr_t IWORKXMLContextEmpty::element(int)
{
  ETONYEK_DEBUG_MSG(("no subelement is expected at this element\n"));
  return IWORKXMLContextPtr_t();
}

void IWORKXMLContextEmpty::text(const char *)
{
  ETONYEK_DEBUG_MSG(("text content is not expected at this element\n"));
}

const boost::optional<ID_t> &IWORKXMLContextEmpty::getId() const
{
  return m_id;
}

const boost::optional<ID_t> &IWORKXMLContextEmpty::getRef() const
{
  return m_ref;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
