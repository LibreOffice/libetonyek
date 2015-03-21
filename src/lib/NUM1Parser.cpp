/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "NUM1Parser.h"

#include "IWORKXMLContexts.h"
#include "IWORKToken.h"
#include "NUMCollector.h"
#include "NUM1Token.h"
#include "NUM1XMLContextBase.h"

namespace libetonyek
{

namespace
{

unsigned getVersion(const int token)
{
  switch (token)
  {
  case NUM1Token::VERSION_STR_2 :
    return 2;
  }

  return 0;
}

}

namespace
{

class DocumentContext : public NUM1XMLElementContextBase
{
public:
  explicit DocumentContext(NUM1ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

};

DocumentContext::DocumentContext(NUM1ParserState &state)
  : NUM1XMLElementContextBase(state)
{
}

void DocumentContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case NUM1Token::NS_URI_LS | NUM1Token::version :
  {
    const unsigned version = getVersion(getToken(value));
    if (0 == version)
    {
      ETONYEK_DEBUG_MSG(("unknown version %s\n", value));
    }
  }
  break;
  }
}

IWORKXMLContextPtr_t DocumentContext::element(const int /*name*/)
{

  return IWORKXMLContextPtr_t();
}

void DocumentContext::endOfElement()
{
}

}

namespace
{

class XMLDocumentContext : public NUM1XMLElementContextBase
{
public:
  explicit XMLDocumentContext(NUM1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

XMLDocumentContext::XMLDocumentContext(NUM1ParserState &state)
  : NUM1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t XMLDocumentContext::element(const int name)
{
  switch (name)
  {
  case NUM1Token::NS_URI_LS | NUM1Token::document :
    return makeContext<DocumentContext>(m_state);
  }

  return IWORKXMLContextPtr_t();
}

}

NUM1Parser::NUM1Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, NUMCollector *const collector, NUMDictionary *const dict)
  : IWORKParser(input, package, 0)
  , m_state(*this, collector, *dict, getTokenizer())
  , m_version(0)
{
}

NUM1Parser::~NUM1Parser()
{
}

IWORKXMLContextPtr_t NUM1Parser::createDocumentContext()
{
  return makeContext<XMLDocumentContext>(m_state);
}

TokenizerFunction_t NUM1Parser::getTokenizer() const
{
  return ChainedTokenizer(NUM1Tokenizer(), IWORKTokenizer());
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
