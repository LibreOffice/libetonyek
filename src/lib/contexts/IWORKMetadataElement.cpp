/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKMetadataElement.h"

#include "IWORKStringElement.h"
#include "IWORKToken.h"

namespace libetonyek
{

using boost::optional;

using std::string;

namespace
{

class TitleElement : public IWORKXMLElementContextBase
{
public:
  explicit TitleElement(IWORKXMLParserState &state, optional<string> &value);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  optional<string> &m_value;
};

TitleElement::TitleElement(IWORKXMLParserState &state, optional<string> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
{
}

IWORKXMLContextPtr_t TitleElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::string))
    return makeContext<IWORKStringElement>(getState(), m_value);
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class AuthorsElement : public IWORKXMLElementContextBase
{
public:
  AuthorsElement(IWORKXMLParserState &state, optional<string> &value);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  optional<string> &m_value;
};

AuthorsElement::AuthorsElement(IWORKXMLParserState &state, optional<string> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
{
}

IWORKXMLContextPtr_t AuthorsElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::string))
    return makeContext<IWORKStringElement>(getState(), m_value);
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class KeywordsElement : public IWORKXMLElementContextBase
{
public:
  KeywordsElement(IWORKXMLParserState &state, optional<string> &value);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  optional<string> &m_value;
};

KeywordsElement::KeywordsElement(IWORKXMLParserState &state, optional<string> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
{
}

IWORKXMLContextPtr_t KeywordsElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::string))
    return makeContext<IWORKStringElement>(getState(), m_value);
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class CommentElement : public IWORKXMLElementContextBase
{
public:
  explicit CommentElement(IWORKXMLParserState &state, optional<string> &m_value);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  optional<string> &m_value;
};

CommentElement::CommentElement(IWORKXMLParserState &state, optional<string> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
{
}

IWORKXMLContextPtr_t CommentElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::string))
    return makeContext<IWORKStringElement>(getState(), m_value);
  return IWORKXMLContextPtr_t();
}

}

IWORKMetadataElement::IWORKMetadataElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_metadata()
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
    return makeContext<AuthorsElement>(getState(), m_author);
  case IWORKToken::NS_URI_SF | IWORKToken::comment :
    return makeContext<CommentElement>(getState(), m_comment);
  case IWORKToken::NS_URI_SF | IWORKToken::keywords :
    return makeContext<KeywordsElement>(getState(), m_keywords);
  case IWORKToken::NS_URI_SF | IWORKToken::title :
    return makeContext<TitleElement>(getState(), m_title);
  }

  return IWORKXMLContextPtr_t();
}

void IWORKMetadataElement::endOfElement()
{
  if (m_author)
    m_metadata.m_author = get(m_author);
  if (m_title)
    m_metadata.m_title = get(m_title);
  if (m_keywords)
    m_metadata.m_keywords = get(m_keywords);
  if (m_comment)
    m_metadata.m_comment = get(m_comment);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
