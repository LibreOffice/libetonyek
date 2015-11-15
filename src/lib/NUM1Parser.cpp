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
#include "IWORKChartInfoElement.h"
#include "IWORKDiscardContext.h"
#include "IWORKGroupElement.h"
#include "IWORKMediaElement.h"
#include "IWORKMetadataElement.h"
#include "IWORKShapeContext.h"
#include "IWORKStylesheetBase.h"
#include "IWORKTabularInfoElement.h"
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

class DrawablesElement : public NUM1XMLElementContextBase
{
public:
  explicit DrawablesElement(NUM1ParserState &state);

private:
  virtual void startOfElement();
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

DrawablesElement::DrawablesElement(NUM1ParserState &state)
  : NUM1XMLElementContextBase(state)
{
}

void DrawablesElement::startOfElement()
{
  if (isCollector())
    getCollector().startLevel();
}

void DrawablesElement::attribute(int, const char *)
{
}

IWORKXMLContextPtr_t DrawablesElement::element(const int name)
{
  switch (name)
  {
  // case IWORKToken::NS_URI_SF | IWORKToken::body_placeholder_ref :
  //   return makeContext<PlaceholderRefContext>(getState(), false);
  // case IWORKToken::NS_URI_SF | IWORKToken::connection_line :
  //   return makeContext<ConnectionLineElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::group :
    return makeContext<IWORKGroupElement>(getState());
  // case IWORKToken::NS_URI_SF | IWORKToken::image :
  //   return makeContext<ImageElement>(getState());
  // case IWORKToken::NS_URI_SF | IWORKToken::line :
  //   return makeContext<LineElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::media :
    return makeContext<IWORKMediaElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::shape :
    return makeContext<IWORKShapeContext>(getState());
  // case IWORKToken::NS_URI_SF | IWORKToken::sticky_note :
  //   return makeContext<StickyNoteElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_info :
    return makeContext<IWORKTabularInfoElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::chart_info :
    return makeContext<IWORKChartInfoElement>(getState());
    // case IWORKToken::NS_URI_SF | IWORKToken::title_placeholder_ref :
    //   return makeContext<PlaceholderRefContext>(getState(), true);

  }

  return IWORKXMLContextPtr_t();
}

void DrawablesElement::endOfElement()
{
  if (isCollector())
    getCollector().endLevel();
}

}

namespace
{

class LayerElement : public NUM1XMLElementContextBase
{
public:
  explicit LayerElement(NUM1ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

LayerElement::LayerElement(NUM1ParserState &state)
  : NUM1XMLElementContextBase(state)
{
}

void LayerElement::startOfElement()
{
}

IWORKXMLContextPtr_t LayerElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::drawables :
    return makeContext<DrawablesElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void LayerElement::endOfElement()
{
}

}

namespace
{

class LayersElement : public NUM1XMLElementContextBase
{
public:
  explicit LayersElement(NUM1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

LayersElement::LayersElement(NUM1ParserState &state)
  : NUM1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t LayersElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layer :
    return makeContext<LayerElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class PageInfoElement : public NUM1XMLElementContextBase
{
public:
  explicit PageInfoElement(NUM1ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

PageInfoElement::PageInfoElement(NUM1ParserState &state)
  : NUM1XMLElementContextBase(state)
{
}

void PageInfoElement::startOfElement()
{
}

IWORKXMLContextPtr_t PageInfoElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layers :
    return makeContext<LayersElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void PageInfoElement::endOfElement()
{
}

}

namespace
{

class StylesheetElement : public NUM1XMLContextBase<IWORKStylesheetBase>
{
public:
  explicit StylesheetElement(NUM1ParserState &state);
};

StylesheetElement::StylesheetElement(NUM1ParserState &state)
  : NUM1XMLContextBase<IWORKStylesheetBase>(state)
{
}

}

namespace
{

class WorkSpaceElement : public NUM1XMLElementContextBase
{
public:
  explicit WorkSpaceElement(NUM1ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

WorkSpaceElement::WorkSpaceElement(NUM1ParserState &state)
  : NUM1XMLElementContextBase(state)
{
}

void WorkSpaceElement::startOfElement()
{
}

IWORKXMLContextPtr_t WorkSpaceElement::element(const int name)
{
  switch (name)
  {
  case NUM1Token::NS_URI_LS | NUM1Token::page_info:
    return makeContext<PageInfoElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void WorkSpaceElement::endOfElement()
{
}

}

namespace
{

class WorkSpaceArrayElement : public NUM1XMLElementContextBase
{
public:
  explicit WorkSpaceArrayElement(NUM1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

WorkSpaceArrayElement::WorkSpaceArrayElement(NUM1ParserState &state)
  : NUM1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t WorkSpaceArrayElement::element(const int name)
{
  switch (name)
  {
  case NUM1Token::NS_URI_LS | NUM1Token::workspace:
    return makeContext<WorkSpaceElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class DocumentElement : public NUM1XMLElementContextBase
{
public:
  explicit DocumentElement(NUM1ParserState &state);

private:
  virtual void startOfElement();
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

DocumentElement::DocumentElement(NUM1ParserState &state)
  : NUM1XMLElementContextBase(state)
{
}

void DocumentElement::startOfElement()
{
  if (isCollector())
    getCollector().startDocument();
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

IWORKXMLContextPtr_t DocumentElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::metadata :
    return makeContext<IWORKMetadataElement>(getState());
  case NUM1Token::NS_URI_LS | NUM1Token::stylesheet :
    return makeContext<StylesheetElement>(getState());
  case NUM1Token::NS_URI_LS | NUM1Token::workspace_array :
    return makeContext<WorkSpaceArrayElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void DocumentElement::endOfElement()
{
  if (isCollector())
    getCollector().endDocument();
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

namespace
{

class DiscardContext : public NUM1XMLContextBase<IWORKDiscardContext>
{
public:
  explicit DiscardContext(NUM1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

DiscardContext::DiscardContext(NUM1ParserState &state)
  : NUM1XMLContextBase<IWORKDiscardContext>(state)
{
}

IWORKXMLContextPtr_t DiscardContext::element(const int name)
{
  switch (name)
  {
  case NUM1Token::NS_URI_LS | NUM1Token::stylesheet :
    return makeContext<StylesheetElement>(getState());
  }

  return shared_from_this();
}

}

NUM1Parser::NUM1Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, NUMCollector &collector, NUM1Dictionary *const dict)
  : IWORKParser(input, package)
  , m_state(*this, collector, *dict)
{
}

NUM1Parser::~NUM1Parser()
{
}

IWORKXMLContextPtr_t NUM1Parser::createDocumentContext()
{
  return makeContext<XMLDocument>(m_state);
}

IWORKXMLContextPtr_t NUM1Parser::createDiscardContext()
{
  return makeContext<DiscardContext>(m_state);
}

const IWORKTokenizer &NUM1Parser::getTokenizer() const
{
  static IWORKChainedTokenizer tokenizer(NUM1Token::getTokenizer(), IWORKToken::getTokenizer());
  return tokenizer;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
