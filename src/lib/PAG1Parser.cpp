/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAG1Parser.h"
#include "IWORKXMLContexts.h"
#include "IWORKToken.h"
#include "PAGCollector.h"
#include "PAG1Token.h"
#include "PAG1XMLContextBase.h"

using boost::optional;

namespace libetonyek
{

namespace
{

unsigned getVersion(const int token)
{
  switch (token)
  {
  case PAG1Token::VERSION_STR_4 :
    return 4;
  }

  return 0;
}

}

namespace
{

class FootersContext : public PAG1XMLElementContextBase
{
public:
  explicit FootersContext(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

FootersContext::FootersContext(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t FootersContext::element(int)
{
  // TODO: parse
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class HeadersContext : public PAG1XMLElementContextBase
{
public:
  explicit HeadersContext(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

HeadersContext::HeadersContext(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t HeadersContext::element(int)
{
  // TODO: parse
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class StylesheetContext : public PAG1XMLElementContextBase
{
public:
  explicit StylesheetContext(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

StylesheetContext::StylesheetContext(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t StylesheetContext::element(int)
{
  // TODO: parse
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class SectionPrototypesContext : public PAG1XMLElementContextBase
{
public:
  explicit SectionPrototypesContext(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

SectionPrototypesContext::SectionPrototypesContext(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t SectionPrototypesContext::element(int)
{
  // TODO: parse
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class MetadataContext : public PAG1XMLElementContextBase
{
public:
  explicit MetadataContext(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

MetadataContext::MetadataContext(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t MetadataContext::element(int)
{
  // TODO: parse
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class DocumentContext : public PAG1XMLElementContextBase
{
public:
  explicit DocumentContext(PAG1ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKSize> m_size;
};

DocumentContext::DocumentContext(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
  , m_size()
{
}

void DocumentContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case PAG1Token::NS_URI_SL | PAG1Token::version :
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

IWORKXMLContextPtr_t DocumentContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::metadata :
    return makeContext<MetadataContext>(getState());
  case PAG1Token::NS_URI_SL | PAG1Token::section_prototypes :
    return makeContext<SectionPrototypesContext>(getState());
  case PAG1Token::NS_URI_SL | PAG1Token::stylesheet :
    return makeContext<StylesheetContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::headers :
    return makeContext<HeadersContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::footers :
    return makeContext<FootersContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void DocumentContext::endOfElement()
{
}

}

namespace
{

class XMLDocumentContext : public PAG1XMLElementContextBase
{
public:
  explicit XMLDocumentContext(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

XMLDocumentContext::XMLDocumentContext(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t XMLDocumentContext::element(const int name)
{
  switch (name)
  {
  case PAG1Token::NS_URI_SL | PAG1Token::document :
    return makeContext<DocumentContext>(m_state);
  }

  return IWORKXMLContextPtr_t();
}

}

PAG1Parser::PAG1Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, PAGCollector *const /*collector*/, PAGDictionary *const dict)
  : IWORKParser(input, package, 0)
  , m_state(*this, *dict, getTokenizer())
  , m_version(0)
{
}

PAG1Parser::~PAG1Parser()
{
}

IWORKXMLContextPtr_t PAG1Parser::createDocumentContext()
{
  return makeContext<XMLDocumentContext>(m_state);
}

TokenizerFunction_t PAG1Parser::getTokenizer() const
{
  return ChainedTokenizer(PAG1Tokenizer(), IWORKTokenizer());
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
