/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKMetadataElement.h"

#include "IWORKCollector.h"
#include "IWORKStringElement.h"
#include "IWORKToken.h"
#include "IWORKTypes.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

using boost::optional;

using std::string;

namespace
{

class StringContext : public IWORKXMLElementContextBase
{
public:
  explicit StringContext(IWORKXMLParserState &state, optional<string> &value);

private:
  IWORKXMLContextPtr_t element(int name) override;

private:
  optional<string> &m_value;
};

StringContext::StringContext(IWORKXMLParserState &state, optional<string> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
{
}

IWORKXMLContextPtr_t StringContext::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::string))
    return makeContext<IWORKStringElement>(getState(), m_value);
  return IWORKXMLContextPtr_t();
}

}

IWORKMetadataElement::IWORKMetadataElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_author()
  , m_title()
  , m_keywords()
  , m_comment()
{
}

IWORKXMLContextPtr_t IWORKMetadataElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::authors :
    return makeContext<StringContext>(getState(), m_author);
  case IWORKToken::NS_URI_SF | IWORKToken::comment :
    return makeContext<StringContext>(getState(), m_comment);
  case IWORKToken::NS_URI_SF | IWORKToken::keywords :
    return makeContext<StringContext>(getState(), m_keywords);
  case IWORKToken::NS_URI_SF | IWORKToken::title :
    return makeContext<StringContext>(getState(), m_title);
  }

  return IWORKXMLContextPtr_t();
}

void IWORKMetadataElement::endOfElement()
{
  IWORKMetadata metadata;

  if (m_author)
    metadata.m_author = get(m_author);
  if (m_title)
    metadata.m_title = get(m_title);
  if (m_keywords)
    metadata.m_keywords = get(m_keywords);
  if (m_comment)
    metadata.m_comment = get(m_comment);

  if (isCollector())
    getCollector().collectMetadata(metadata);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
