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
#include "IWORKCellCommentDrawableInfoElement.h"
#include "IWORKChainedTokenizer.h"
#include "IWORKChartInfoElement.h"
#include "IWORKDiscardContext.h"
#include "IWORKGeometryElement.h"
#include "IWORKGroupElement.h"
#include "IWORKImageElement.h"
#include "IWORKMediaElement.h"
#include "IWORKMetadataElement.h"
#include "IWORKPathElement.h"
#include "IWORKRefContext.h"
#include "IWORKShapeContext.h"
#include "IWORKStyleContainer.h"
#include "IWORKStyleContext.h"
#include "IWORKStylesContext.h"
#include "IWORKStylesheetBase.h"
#include "IWORKTabularInfoElement.h"
#include "IWORKTextElement.h"
#include "IWORKToken.h"
#include "NUMCollector.h"
#include "NUM1Dictionary.h"
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
  default:
    break;
  }

  return 0;
}

}

namespace
{
class StickyNoteElement : public NUM1XMLElementContextBase
{
public:
  explicit StickyNoteElement(NUM1ParserState &state);

private:
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

  IWORKStylePtr_t m_graphicStyle;
};

StickyNoteElement::StickyNoteElement(NUM1ParserState &state)
  : NUM1XMLElementContextBase(state)
  , m_graphicStyle()
{
}

void StickyNoteElement::startOfElement()
{
  if (isCollector())
  {
    assert(!getState().m_currentText);
    getState().m_currentText = getCollector().createText(getState().m_langManager);
    getCollector().startLevel();
  }
}

typedef IWORKStyleContainer<IWORKToken::NS_URI_SF | IWORKToken::graphic_style, IWORKToken::NS_URI_SF | IWORKToken::graphic_style_ref> GraphicStyleContext;

IWORKXMLContextPtr_t StickyNoteElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return std::make_shared<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::path : // use me
    return std::make_shared<IWORKPathElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::style : // use me
    return std::make_shared<GraphicStyleContext>(getState(), m_graphicStyle, getState().getDictionary().m_graphicStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::text :
    return std::make_shared<IWORKTextElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::wrap : // README
    return IWORKXMLContextPtr_t();
  default:
    ETONYEK_DEBUG_MSG(("StickyNoteElement::element[NUM1Parser.cpp]: unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void StickyNoteElement::endOfElement()
{
  if (isCollector())
  {
    getCollector().collectText(getState().m_currentText);
    getState().m_currentText.reset();
    getCollector().collectStickyNote();

    getCollector().endLevel();
  }
}
}
namespace
{

class DrawablesElement : public NUM1XMLElementContextBase
{
public:
  explicit DrawablesElement(NUM1ParserState &state);

private:
  void startOfElement() override;
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;
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
  //   return std::make_shared<PlaceholderRefContext>(getState(), false);
  case IWORKToken::NS_URI_SF | IWORKToken::cell_comment_drawable_info:
    return std::make_shared<IWORKCellCommentDrawableInfoElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::chart_info :
    return std::make_shared<IWORKChartInfoElement>(getState());
  // case IWORKToken::NS_URI_SF | IWORKToken::connection_line :
  //   return std::make_shared<ConnectionLineElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::group :
    return std::make_shared<IWORKGroupElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::image :
    return std::make_shared<IWORKImageElement>(getState());
  // case IWORKToken::NS_URI_SF | IWORKToken::line :
  //   return std::make_shared<LineElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::media :
    return std::make_shared<IWORKMediaElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::shape :
    return std::make_shared<IWORKShapeContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::sticky_note :
    return std::make_shared<StickyNoteElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_info :
    return std::make_shared<IWORKTabularInfoElement>(getState());
  // case IWORKToken::NS_URI_SF | IWORKToken::title_placeholder_ref :
  //   return std::make_shared<PlaceholderRefContext>(getState(), true);
  default:
    break;
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
  IWORKXMLContextPtr_t element(int name) override;
};

LayerElement::LayerElement(NUM1ParserState &state)
  : NUM1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t LayerElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::drawables :
    return std::make_shared<DrawablesElement>(getState());
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class LayersElement : public NUM1XMLElementContextBase
{
public:
  explicit LayersElement(NUM1ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
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
    return std::make_shared<LayerElement>(getState());
  default:
    break;
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
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;
};

PageInfoElement::PageInfoElement(NUM1ParserState &state)
  : NUM1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t PageInfoElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layers :
    return std::make_shared<LayersElement>(getState());
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void PageInfoElement::endOfElement()
{
}

}

namespace
{

class StylesContext : public NUM1XMLContextBase<IWORKStylesContext>
{
public:
  StylesContext(NUM1ParserState &state, bool anonymous);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

StylesContext::StylesContext(NUM1ParserState &state, const bool anonymous)
  : NUM1XMLContextBase<IWORKStylesContext>(state, anonymous)
{
}

IWORKXMLContextPtr_t StylesContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::workspace_style :
  case NUM1Token::NS_URI_LS | NUM1Token::workspace_style :
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_workspaceStyles);
  default:
    break;
  }

  return NUM1XMLContextBase<IWORKStylesContext>::element(name);
}

}

namespace
{

class StylesheetElement : public NUM1XMLContextBase<IWORKStylesheetBase>
{
public:
  explicit StylesheetElement(NUM1ParserState &state);
  IWORKXMLContextPtr_t element(int name) override;
};

StylesheetElement::StylesheetElement(NUM1ParserState &state)
  : NUM1XMLContextBase<IWORKStylesheetBase>(state)
{
}

IWORKXMLContextPtr_t StylesheetElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::styles :
    return std::make_shared<StylesContext>(getState(), false);
  case IWORKToken::NS_URI_SF | IWORKToken::anon_styles :
    return std::make_shared<StylesContext>(getState(), true);
  default:
    break;
  }
  return NUM1XMLContextBase<IWORKStylesheetBase>::element(name);
}
}

namespace
{

class WorkSpaceElement : public NUM1XMLElementContextBase
{
public:
  explicit WorkSpaceElement(NUM1ParserState &state);

private:
  void attribute(const int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

  boost::optional<std::string> m_spaceName;
  bool m_opened;
};

WorkSpaceElement::WorkSpaceElement(NUM1ParserState &state)
  : NUM1XMLElementContextBase(state)
  , m_spaceName()
  , m_opened(false)
{
}

void WorkSpaceElement::attribute(const int name, const char *value)
{
  switch (name)
  {
  case NUM1Token::NS_URI_LS | NUM1Token::workspace_name:
    m_spaceName = value;
    break;
  default:
    NUM1XMLElementContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t WorkSpaceElement::element(const int name)
{
  if (isCollector() && !m_opened)
  {
    m_opened=true;
    getCollector().startWorkSpace(m_spaceName);
  }
  switch (name)
  {
  case NUM1Token::NS_URI_LS | NUM1Token::page_info:
    return std::make_shared<PageInfoElement>(getState());
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void WorkSpaceElement::endOfElement()
{
  if (isCollector() && m_opened)
    getCollector().endWorkSpace(getState().m_tableNameMap);
}

}

namespace
{

class WorkSpaceArrayElement : public NUM1XMLElementContextBase
{
public:
  explicit WorkSpaceArrayElement(NUM1ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
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
    return std::make_shared<WorkSpaceElement>(getState());
  default:
    break;
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
  void startOfElement() override;
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;
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
  default:
    break;
  }
}

IWORKXMLContextPtr_t DocumentElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::metadata :
    return std::make_shared<IWORKMetadataElement>(getState());
  case NUM1Token::NS_URI_LS | NUM1Token::stylesheet :
    return std::make_shared<StylesheetElement>(getState());
  case NUM1Token::NS_URI_LS | NUM1Token::workspace_array :
    return std::make_shared<WorkSpaceArrayElement>(getState());
  default:
    break;
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
  IWORKXMLContextPtr_t element(int name) override;
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
    return std::make_shared<DocumentElement>(m_state);
  default:
    break;
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
  IWORKXMLContextPtr_t element(int name) override;
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
    return std::make_shared<StylesheetElement>(getState());
  case NUM1Token::NS_URI_LS | NUM1Token::workspace_style :
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_workspaceStyles);
  default:
    break;
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
  return std::make_shared<XMLDocument>(m_state);
}

IWORKXMLContextPtr_t NUM1Parser::createDiscardContext()
{
  return std::make_shared<DiscardContext>(m_state);
}

const IWORKTokenizer &NUM1Parser::getTokenizer() const
{
  static IWORKChainedTokenizer tokenizer(NUM1Token::getTokenizer(), IWORKToken::getTokenizer());
  return tokenizer;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
