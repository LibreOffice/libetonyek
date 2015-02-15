/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "libetonyek_utils.h"
#include "IWORKToken.h"
#include "IWORKXMLContextBase.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKXMLContextBase::IWORKXMLContextBase(IWORKXMLParserState &state)
  : m_state(state)
{
}

void IWORKXMLContextBase::startOfElement()
{
}

void IWORKXMLContextBase::endOfAttributes()
{
}

void IWORKXMLContextBase::endOfElement()
{
}

KEYCollector *IWORKXMLContextBase::getCollector() const
{
  return m_state.getCollector();
}

const KEYDefaults &IWORKXMLContextBase::getDefaults() const
{
  return m_state.getDefaults();
}

int IWORKXMLContextBase::getToken(const char *const value) const
{
  return m_state.getTokenizer()(value);
}

IWORKXMLParserState &IWORKXMLContextBase::getState()
{
  return m_state;
}

IWORKXMLElementContextBase::IWORKXMLElementContextBase(IWORKXMLParserState &state)
  : IWORKXMLContextBase(state)
{
}

void IWORKXMLElementContextBase::attribute(const int name, const char *const value)
{
  if ((IWORKToken::NS_URI_SFA | IWORKToken::ID) == name)
    m_id = value;
}

void IWORKXMLElementContextBase::text(const char *)
{
  ETONYEK_DEBUG_MSG(("text content is not expected at this element\n"));
}

const boost::optional<ID_t> &IWORKXMLElementContextBase::getId() const
{
  return m_id;
}

IWORKXMLTextContextBase::IWORKXMLTextContextBase(IWORKXMLParserState &state)
  : IWORKXMLContextBase(state)
{
}

IWORKXMLContextPtr_t IWORKXMLTextContextBase::element(int)
{
  ETONYEK_DEBUG_MSG(("no subelement is expected at this element\n"));
  return IWORKXMLContextPtr_t();
}

IWORKXMLMixedContextBase::IWORKXMLMixedContextBase(IWORKXMLParserState &state)
  : IWORKXMLContextBase(state)
{
}

IWORKXMLEmptyContextBase::IWORKXMLEmptyContextBase(IWORKXMLParserState &state)
  : IWORKXMLContextBase(state)
{
}

void IWORKXMLEmptyContextBase::attribute(const int name, const char *const value)
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

IWORKXMLContextPtr_t IWORKXMLEmptyContextBase::element(int)
{
  ETONYEK_DEBUG_MSG(("no subelement is expected at this element\n"));
  return IWORKXMLContextPtr_t();
}

void IWORKXMLEmptyContextBase::text(const char *)
{
  ETONYEK_DEBUG_MSG(("text content is not expected at this element\n"));
}

const boost::optional<ID_t> &IWORKXMLEmptyContextBase::getId() const
{
  return m_id;
}

const boost::optional<ID_t> &IWORKXMLEmptyContextBase::getRef() const
{
  return m_ref;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
