/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "NUM1Parser.h"

#include "libetonyek_xml.h"
#include "IWORKChainedTokenizer.h"
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

class DocumentElement : public NUM1XMLElementContextBase
{
public:
  explicit DocumentElement(NUM1ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

};

DocumentElement::DocumentElement(NUM1ParserState &state)
  : NUM1XMLElementContextBase(state)
{
}

void DocumentElement::attribute(const int name, const char *const value)
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

IWORKXMLContextPtr_t DocumentElement::element(const int /*name*/)
{

  return IWORKXMLContextPtr_t();
}

void DocumentElement::endOfElement()
{
}

}

namespace
{

class XMLDocument : public NUM1XMLElementContextBase
{
public:
  explicit XMLDocument(NUM1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

XMLDocument::XMLDocument(NUM1ParserState &state)
  : NUM1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t XMLDocument::element(const int name)
{
  switch (name)
  {
  case NUM1Token::NS_URI_LS | NUM1Token::document :
    return makeContext<DocumentElement>(m_state);
  }

  return IWORKXMLContextPtr_t();
}

}

NUM1Parser::NUM1Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, NUMCollector *const collector, NUMDictionary *const dict)
  : IWORKParser(input, package)
  , m_state(*this, collector, *dict)
  , m_version(0)
{
}

NUM1Parser::~NUM1Parser()
{
}

IWORKXMLContextPtr_t NUM1Parser::createDocumentContext()
{
  return makeContext<XMLDocument>(m_state);
}

const IWORKTokenizer &NUM1Parser::getTokenizer() const
{
  static IWORKChainedTokenizer tokenizer(NUM1Token::getTokenizer(), IWORKToken::getTokenizer());
  return tokenizer;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
