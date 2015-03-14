/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAGParser.h"
#include "IWORKXMLContexts.h"
#include "IWORKToken.h"
#include "PAGCollector.h"
#include "PAGToken.h"
#include "PAGXMLContextBase.h"

using boost::optional;

namespace libetonyek
{

namespace
{

unsigned getVersion(const int token)
{
  switch (token)
  {
  case PAGToken::VERSION_STR_4 :
    return 4;
  }

  return 0;
}

}

namespace
{

class FootersContext : public PAGXMLElementContextBase
{
public:
  explicit FootersContext(PAGParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

FootersContext::FootersContext(PAGParserState &state)
  : PAGXMLElementContextBase(state)
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

class HeadersContext : public PAGXMLElementContextBase
{
public:
  explicit HeadersContext(PAGParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

HeadersContext::HeadersContext(PAGParserState &state)
  : PAGXMLElementContextBase(state)
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

class StylesheetContext : public PAGXMLElementContextBase
{
public:
  explicit StylesheetContext(PAGParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

StylesheetContext::StylesheetContext(PAGParserState &state)
  : PAGXMLElementContextBase(state)
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

class SectionPrototypesContext : public PAGXMLElementContextBase
{
public:
  explicit SectionPrototypesContext(PAGParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

SectionPrototypesContext::SectionPrototypesContext(PAGParserState &state)
  : PAGXMLElementContextBase(state)
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

class MetadataContext : public PAGXMLElementContextBase
{
public:
  explicit MetadataContext(PAGParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

MetadataContext::MetadataContext(PAGParserState &state)
  : PAGXMLElementContextBase(state)
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

class DocumentContext : public PAGXMLElementContextBase
{
public:
  explicit DocumentContext(PAGParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKSize> m_size;
};

DocumentContext::DocumentContext(PAGParserState &state)
  : PAGXMLElementContextBase(state)
  , m_size()
{
}

void DocumentContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case PAGToken::NS_URI_SL | PAGToken::version :
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
  case PAGToken::NS_URI_SL | PAGToken::section_prototypes :
    return makeContext<SectionPrototypesContext>(getState());
  case PAGToken::NS_URI_SL | PAGToken::stylesheet :
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

PAGParser::PAGParser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, PAGCollector *const /*collector*/, PAGDictionary *const dict)
  : IWORKParser(input, package, 0)
  , m_state(*this, *dict, getTokenizer())
  , m_version(0)
{
}

PAGParser::~PAGParser()
{
}

IWORKXMLContextPtr_t PAGParser::createDocumentContext()
{
  return makeContext<DocumentContext>(m_state);
}

TokenizerFunction_t PAGParser::getTokenizer() const
{
  return ChainedTokenizer(PAGTokenizer(), IWORKTokenizer());
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
