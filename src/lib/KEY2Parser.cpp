/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY2Parser.h"

#include <cassert>

#include "libetonyek_utils.h"
#include "libetonyek_xml.h"
#include "IWORKChainedTokenizer.h"
#include "IWORKDiscardContext.h"
#include "IWORKGeometryElement.h"
#include "IWORKGroupElement.h"
#include "IWORKImageElement.h"
#include "IWORKLineElement.h"
#include "IWORKMediaElement.h"
#include "IWORKPath.h"
#include "IWORKPathElement.h"
#include "IWORKPositionElement.h"
#include "IWORKRefContext.h"
#include "IWORKShapeContext.h"
#include "IWORKSizeElement.h"
#include "IWORKStringElement.h"
#include "IWORKStyle.h"
#include "IWORKStyleContainer.h"
#include "IWORKStyleRefContext.h"
#include "IWORKStylesContext.h"
#include "IWORKStylesheetBase.h"
#include "IWORKTableInfoElement.h"
#include "IWORKTabularInfoElement.h"
#include "IWORKText.h"
#include "IWORKTextElement.h"
#include "IWORKTextBodyElement.h"
#include "IWORKTextStorageElement.h"
#include "IWORKToken.h"
#include "IWORKTypes.h"
#include "KEY2Dictionary.h"
#include "KEY2ParserState.h"
#include "KEY2StyleContext.h"
#include "KEY2Token.h"
#include "KEY2XMLContextBase.h"
#include "KEYCollector.h"
#include "KEYTypes.h"

using boost::optional;

using std::string;

namespace libetonyek
{

namespace
{

unsigned getVersion(const int token)
{
  switch (token)
  {
  case KEY2Token::VERSION_STR_2 :
    return 2;
  case KEY2Token::VERSION_STR_3 :
    return 3;
  case KEY2Token::VERSION_STR_4 :
    return 4;
  case KEY2Token::VERSION_STR_5 :
    return 5;
  default:
    break;
  }

  return 0;
}

}

namespace
{

class StringContentContext : public KEY2XMLElementContextBase
{
public:
  StringContentContext(KEY2ParserState &state, optional<string> &value);

private:
  IWORKXMLContextPtr_t element(int name) override;

private:
  optional<string> &m_value;
};

StringContentContext::StringContentContext(KEY2ParserState &state, optional<string> &value)
  : KEY2XMLElementContextBase(state)
  , m_value(value)
{
}

IWORKXMLContextPtr_t StringContentContext::element(const int name)
{
  if (name == (KEY2Token::NS_URI_KEY | KEY2Token::string))
    return makeContext<IWORKStringElement>(getState(), m_value);
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class MetadataElement : public KEY2XMLElementContextBase
{
public:
  explicit MetadataElement(KEY2ParserState &state);

protected:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  boost::optional<std::string> m_author;
  boost::optional<std::string> m_title;
  boost::optional<std::string> m_keywords;
  boost::optional<std::string> m_comment;
};

MetadataElement::MetadataElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_author()
  , m_title()
  , m_keywords()
  , m_comment()
{
}

IWORKXMLContextPtr_t MetadataElement::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::authors :
    return makeContext<StringContentContext>(getState(), m_author);
  case KEY2Token::NS_URI_KEY | KEY2Token::comment :
    return makeContext<StringContentContext>(getState(), m_comment);
  case KEY2Token::NS_URI_KEY | KEY2Token::keywords :
    return makeContext<StringContentContext>(getState(), m_keywords);
  case KEY2Token::NS_URI_KEY | KEY2Token::title :
    return makeContext<StringContentContext>(getState(), m_title);
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void MetadataElement::endOfElement()
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

namespace
{

class StylesContext : public KEY2XMLContextBase<IWORKStylesContext>
{
public:
  StylesContext(KEY2ParserState &state, bool anonymous);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

StylesContext::StylesContext(KEY2ParserState &state, const bool anonymous)
  : KEY2XMLContextBase<IWORKStylesContext>(state, anonymous)
{
}

IWORKXMLContextPtr_t StylesContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::placeholder_style :
    return makeContext<IWORKStyleContext>(getState(), &getState().getDictionary().m_placeholderStyles);
  case KEY2Token::NS_URI_KEY | KEY2Token::slide_style : // v5
  case IWORKToken::NS_URI_SF | IWORKToken::slide_style : // v2-v4
    return makeContext<KEY2StyleContext>(getState(), &getState().getDictionary().m_slideStyles);
  default:
    break;
  }

  return KEY2XMLContextBase<IWORKStylesContext>::element(name);
}

}

namespace
{

class StylesheetElement : public KEY2XMLContextBase<IWORKStylesheetBase>
{
public:
  explicit StylesheetElement(KEY2ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  optional<ID_t> m_parent;
};

StylesheetElement::StylesheetElement(KEY2ParserState &state)
  : KEY2XMLContextBase<IWORKStylesheetBase>(state)
  , m_parent()
{
}

IWORKXMLContextPtr_t StylesheetElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::styles :
    return makeContext<StylesContext>(getState(), false);
  case IWORKToken::NS_URI_SF | IWORKToken::anon_styles :
    return makeContext<StylesContext>(getState(), true);
  case IWORKToken::NS_URI_SF | IWORKToken::parent_ref :
    return makeContext<IWORKRefContext>(getState(), m_parent);
  default:
    break;
  }

  return KEY2XMLContextBase<IWORKStylesheetBase>::element(name);
}

void StylesheetElement::endOfElement()
{
  if (m_parent)
  {
    assert(getId() != m_parent);

    const IWORKStylesheetMap_t::const_iterator it = getState().getDictionary().m_stylesheets.find(get(m_parent));
    if (getState().getDictionary().m_stylesheets.end() != it)
      getState().m_stylesheet->parent = it->second;
  }

  KEY2XMLContextBase<IWORKStylesheetBase>::endOfElement();
}

}

namespace
{

class ProxyMasterLayerElement : public KEY2XMLElementContextBase
{
public:
  explicit ProxyMasterLayerElement(KEY2ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  optional<ID_t> m_ref;
};

ProxyMasterLayerElement::ProxyMasterLayerElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_ref()
{
}

IWORKXMLContextPtr_t ProxyMasterLayerElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layer_ref :
    return makeContext<IWORKRefContext>(getState(), m_ref);
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void ProxyMasterLayerElement::endOfElement()
{
  if (m_ref && isCollector())
  {
    const KEYLayerMap_t::const_iterator it = getState().getDictionary().m_layers.find(get(m_ref));
    if (getState().getDictionary().m_layers.end() == it)
    {
      ETONYEK_DEBUG_MSG(("ProxyMasterLayerElement::endOfElement[KEY2Parser.cpp]: can not find layer %s\n", get(m_ref).c_str()));
    }
  }
}

}

namespace
{

enum PlaceholderRefType
{
  PLACEHOLDER_BODY, PLACEHOLDER_OBJECT, PLACEHOLDER_SLIDENUMBER, PLACEHOLDER_TITLE
};
class PlaceholderRefContext : public KEY2XMLEmptyContextBase
{
public:
  PlaceholderRefContext(KEY2ParserState &state, const PlaceholderRefType kind);

private:
  void endOfElement() override;

private:
  const PlaceholderRefType m_kind;
};

PlaceholderRefContext::PlaceholderRefContext(KEY2ParserState &state, const PlaceholderRefType kind)
  : KEY2XMLEmptyContextBase(state)
  , m_kind(kind)
{
}

void PlaceholderRefContext::endOfElement()
{
  if (getRef() && isCollector() && getState().getVersion()!=2)
  {
    KEY2Dictionary &dict = getState().getDictionary();
    KEYPlaceholderMap_t &placeholderMap =
      m_kind==PLACEHOLDER_BODY ? dict.m_bodyPlaceholders :
      m_kind==PLACEHOLDER_OBJECT ? dict.m_objectPlaceholders :
      m_kind==PLACEHOLDER_SLIDENUMBER ? dict.m_slidenumberPlaceholders : dict.m_titlePlaceholders;
    const KEYPlaceholderMap_t::const_iterator it = placeholderMap.find(get(getRef()));
    if (placeholderMap.end() != it)
      getCollector().insertTextPlaceholder(it->second);
    else
    {
      ETONYEK_DEBUG_MSG(("PlaceholderRefContext::endOfElement[KEY2Parser.cpp]: can not find placeHolder %s\n", get(getRef()).c_str()));
    }
  }
}

}

namespace
{

class ConnectionLineElement : public KEY2XMLElementContextBase
{
public:
  explicit ConnectionLineElement(KEY2ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;
};

ConnectionLineElement::ConnectionLineElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t ConnectionLineElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::path :
    return makeContext<IWORKPathElement>(getState());
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void ConnectionLineElement::endOfElement()
{
  if (isCollector())
    getCollector().collectShape();
}

}

namespace
{

class HeadlineElement : public KEY2XMLElementContextBase
{
public:
  explicit HeadlineElement(KEY2ParserState &state);

private:
  void attribute(int name, const char *value) override;
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

  void ensureOpened();

  optional<ID_t> m_styleRef;
  optional<int> m_depth;
  bool m_opened;
};

HeadlineElement::HeadlineElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_styleRef()
  , m_depth()
  , m_opened(false)
{
}

void HeadlineElement::attribute(int name, const char *value)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::depth :
    m_depth=try_int_cast(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    KEY2XMLElementContextBase::attribute(name, value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("HeadlineElement::attribute[KEY2Parser.cpp]: unknown attribute\n"));
    break;
  }
}

void HeadlineElement::startOfElement()
{
  if (isCollector())
    getCollector().startLevel();
}

IWORKXMLContextPtr_t HeadlineElement::element(const int name)
{
  ensureOpened();
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::style_ref :
    return makeContext<IWORKRefContext>(getState(), m_styleRef);
  case KEY2Token::NS_URI_KEY | KEY2Token::text :
    return makeContext<IWORKTextElement>(getState());
  default:
    ETONYEK_DEBUG_MSG(("HeadlineElement::element[KEY2Parser.cpp]: unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void HeadlineElement::endOfElement()
{
  if (m_opened)
    getState().closeHeadline();
  if (isCollector())
    getCollector().endLevel();
}

void HeadlineElement::ensureOpened()
{
  if (m_opened || !isCollector())
    return;
  if (!m_depth)
  {
    ETONYEK_DEBUG_MSG(("HeadlineElement::element[KEY2Parser.cpp]: oops can not find the depth\n"));
    return;
  }
  getState().openHeadline(get(m_depth));
  m_opened=true;
}
}

namespace
{
typedef IWORKStyleContainer<IWORKToken::NS_URI_SF | IWORKToken::graphic_style, IWORKToken::NS_URI_SF | IWORKToken::graphic_style_ref> GraphicStyleContext;
}

namespace
{

class StickyNoteElement : public KEY2XMLElementContextBase
{
public:
  explicit StickyNoteElement(KEY2ParserState &state);

private:
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

  IWORKStylePtr_t m_graphicStyle;
};

StickyNoteElement::StickyNoteElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
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

IWORKXMLContextPtr_t StickyNoteElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::path : // use me
    return makeContext<IWORKPathElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::style : // use me
    return makeContext<GraphicStyleContext>(getState(), m_graphicStyle, getState().getDictionary().m_graphicStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::text :
    return makeContext<IWORKTextElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::wrap : // README
    return IWORKXMLContextPtr_t();
  default:
    ETONYEK_DEBUG_MSG(("StickyNoteElement::element[KEY2Parser.cpp]: unknown element\n"));
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

class BulletsElement : public KEY2XMLElementContextBase
{
public:
  explicit BulletsElement(KEY2ParserState &state, IWORKTextPtr_t &bodyText,
                          IWORKTextPtr_t &titleText);

private:
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKTextPtr_t &m_bodyText;
  IWORKTextPtr_t &m_titleText;
};

BulletsElement::BulletsElement(KEY2ParserState &state, IWORKTextPtr_t &bodyText, IWORKTextPtr_t &titleText)
  : KEY2XMLElementContextBase(state)
  , m_bodyText(bodyText)
  , m_titleText(titleText)
{
}

void BulletsElement::startOfElement()
{
  getState().openBullets();
}

IWORKXMLContextPtr_t BulletsElement::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::headline :
    return makeContext<HeadlineElement>(getState());
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void BulletsElement::endOfElement()
{
  m_bodyText=getState().getBodyText();
  if (m_bodyText && m_bodyText->empty()) m_bodyText.reset();
  m_titleText=getState().getTitleText();
  if (m_titleText && m_titleText->empty()) m_titleText.reset();
  getState().closeBullets();
}

}

namespace
{

class DrawablesElement : public KEY2XMLElementContextBase
{
public:
  explicit DrawablesElement(KEY2ParserState &state);

private:
  void startOfElement() override;
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;
};

DrawablesElement::DrawablesElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
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
  case IWORKToken::NS_URI_SF | IWORKToken::body_placeholder_ref :
    return makeContext<PlaceholderRefContext>(getState(), PLACEHOLDER_BODY);
  case IWORKToken::NS_URI_SF | IWORKToken::connection_line :
    return makeContext<ConnectionLineElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::group :
    return makeContext<IWORKGroupElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::image :
    return makeContext<IWORKImageElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::line :
    return makeContext<IWORKLineElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::media :
    return makeContext<IWORKMediaElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::shape :
    return makeContext<IWORKShapeContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::sticky_note :
    return makeContext<StickyNoteElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::table_info :
    return makeContext<IWORKTableInfoElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_info :
    return makeContext<IWORKTabularInfoElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::title_placeholder_ref :
    return makeContext<PlaceholderRefContext>(getState(), PLACEHOLDER_TITLE);
  case KEY2Token::NS_URI_KEY | KEY2Token::sticky_note :
    return makeContext<StickyNoteElement>(getState());
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

class LayerElement : public KEY2XMLElementContextBase
{
public:
  explicit LayerElement(KEY2ParserState &state);

private:
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;
};

LayerElement::LayerElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void LayerElement::startOfElement()
{
  if (isCollector())
    getCollector().startLayer();
}

IWORKXMLContextPtr_t LayerElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::drawables :
    return makeContext<DrawablesElement>(getState());
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void LayerElement::endOfElement()
{
  if (isCollector())
  {
    const KEYLayerPtr_t layer(getCollector().collectLayer());
    getCollector().endLayer();
    if (bool(layer))
    {
      if (bool(layer) && getId())
        getState().getDictionary().m_layers[get(getId())] = layer;
      getCollector().insertLayer(layer);
    }
  }
}

}

namespace
{

class LayersElement : public KEY2XMLElementContextBase
{
public:
  explicit LayersElement(KEY2ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

LayersElement::LayersElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t LayersElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layer :
    return makeContext<LayerElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::proxy_master_layer :
    return makeContext<ProxyMasterLayerElement>(getState());
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class PageElement : public KEY2XMLElementContextBase
{
public:
  explicit PageElement(KEY2ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  // TODO: use size
  optional<IWORKSize> m_size;
};

PageElement::PageElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_size()
{
}

IWORKXMLContextPtr_t PageElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeElement>(getState(), m_size);
  case IWORKToken::NS_URI_SF | IWORKToken::layers :
    return makeContext<LayersElement>(getState());
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void PageElement::endOfElement()
{
}

}

namespace
{

class StyleElement : public KEY2XMLElementContextBase
{
public:
  explicit StyleElement(KEY2ParserState &state, optional<ID_t> &ref);

private:
  IWORKXMLContextPtr_t element(int name) override;

private:
  optional<ID_t> &m_ref;
};

StyleElement::StyleElement(KEY2ParserState &state, optional<ID_t> &ref)
  : KEY2XMLElementContextBase(state)
  , m_ref(ref)
{
}

IWORKXMLContextPtr_t StyleElement::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::placeholder_style_ref) == name)
    return makeContext<IWORKRefContext>(getState(), m_ref);

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class PlaceholderContext : public KEY2XMLElementContextBase
{
public:
  PlaceholderContext(KEY2ParserState &state, bool title, boost::optional<ID_t> &ref);

private:
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  const bool m_title;
  optional<ID_t> &m_placeholderRef;
  optional<ID_t> m_styleRef;
};

PlaceholderContext::PlaceholderContext(KEY2ParserState &state, const bool title, boost::optional<ID_t> &ref)
  : KEY2XMLElementContextBase(state)
  , m_title(title)
  , m_placeholderRef(ref)
  , m_styleRef()
{
}

void PlaceholderContext::startOfElement()
{
  if (isCollector() && getState().getVersion()!=2)
  {
    // the content in version 2 node is defined after
    assert(!getState().m_currentText);
    getState().m_currentText = getCollector().createText(getState().m_langManager);
  }
}

IWORKXMLContextPtr_t PlaceholderContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    // ignore; the real geometry comes from style
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::style :
    return makeContext<StyleElement>(getState(), m_styleRef);
  case KEY2Token::NS_URI_KEY | KEY2Token::text :
    return makeContext<IWORKTextElement>(getState());
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void PlaceholderContext::endOfElement()
{
  if (getId()) m_placeholderRef=getId();
  if (isCollector())
  {
    IWORKStylePtr_t style;
    if (m_styleRef)
    {
      const IWORKStyleMap_t::const_iterator it = getState().getDictionary().m_placeholderStyles.find(get(m_styleRef));
      if (getState().getDictionary().m_placeholderStyles.end() != it)
        style = it->second;
    }

    if (bool(getState().m_currentText) && !getState().m_currentText->empty())
      getCollector().collectText(getState().m_currentText);
    getState().m_currentText.reset();

    const KEYPlaceholderPtr_t placeholder = getCollector().collectTextPlaceholder(style, m_title);
    if (bool(placeholder) && getId())
    {
      KEY2Dictionary &dict = getState().getDictionary();
      KEYPlaceholderMap_t &placeholderMap = m_title ? dict.m_titlePlaceholders : dict.m_bodyPlaceholders;
      placeholderMap[get(getId())] = placeholder;
    }
  }
}

}

namespace
{

class NotesElement : public KEY2XMLElementContextBase
{
public:
  explicit NotesElement(KEY2ParserState &state);

private:
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;
};

NotesElement::NotesElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void NotesElement::startOfElement()
{
  if (isCollector())
  {
    assert(!getState().m_currentText);
    getState().m_currentText = getCollector().createText(getState().m_langManager);
  }
}

IWORKXMLContextPtr_t NotesElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::text_storage | IWORKToken::NS_URI_SF :
    return makeContext<IWORKTextStorageElement>(getState());
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void NotesElement::endOfElement()
{
  if (isCollector())
  {
    getCollector().collectText(getState().m_currentText);
    getState().m_currentText.reset();
    getCollector().collectNote();
  }
}

}

namespace
{

class StickyNotesElement : public KEY2XMLElementContextBase
{
public:
  explicit StickyNotesElement(KEY2ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

StickyNotesElement::StickyNotesElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t StickyNotesElement::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::sticky_note :
    return makeContext<StickyNoteElement>(getState());
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class SlideElement : public KEY2XMLElementContextBase
{
public:
  explicit SlideElement(KEY2ParserState &state, bool isMasterSlide);

private:
  void attribute(int name, const char *value) override;
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  bool m_isMasterSlide;
  optional<ID_t> m_styleRef;
  optional<ID_t> m_masterRef;

  boost::optional<std::string> m_name; // master name
  // v2: content of the body and title placeholder
  IWORKTextPtr_t m_bodyText;
  IWORKTextPtr_t m_titleText;

  // v2: change read these ref in the drawables element
  boost::optional<ID_t> m_bodyRef;
  boost::optional<ID_t> m_objectRef;
  boost::optional<ID_t> m_slidenumberRef;
  boost::optional<ID_t> m_titleRef;
};

SlideElement::SlideElement(KEY2ParserState &state, bool isMasterSlide)
  : KEY2XMLElementContextBase(state)
  , m_isMasterSlide(isMasterSlide)
  , m_styleRef()
  , m_masterRef()
  , m_name()
  , m_bodyText()
  , m_titleText()
  , m_bodyRef()
  , m_objectRef()
  , m_slidenumberRef()
  , m_titleRef()
{
}

void SlideElement::attribute(int name, const char *value)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::name :
    m_name=value;
    break;
  case KEY2Token::NS_URI_KEY | KEY2Token::depth :
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    KEY2XMLElementContextBase::attribute(name, value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("SlideElement::attribute[KEY2Parser.cpp]: unknown attribute\n"));
    break;
  }
}

void SlideElement::startOfElement()
{
  if (isCollector())
    getCollector().startPage();
}

IWORKXMLContextPtr_t SlideElement::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::bullets :
    return makeContext<BulletsElement>(getState(), m_bodyText, m_titleText);
  case KEY2Token::NS_URI_KEY | KEY2Token::notes :
    return makeContext<NotesElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::page :
    return makeContext<PageElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::master_ref :
    return makeContext<IWORKRefContext>(getState(), m_masterRef);
  case KEY2Token::NS_URI_KEY | KEY2Token::sticky_notes :
    return makeContext<StickyNotesElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::style_ref :
    return makeContext<IWORKRefContext>(getState(), m_styleRef);
  case KEY2Token::NS_URI_KEY | KEY2Token::stylesheet :
    return makeContext<StylesheetElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::body_placeholder :
    return makeContext<PlaceholderContext>(getState(), PLACEHOLDER_BODY, m_bodyRef);
  case KEY2Token::NS_URI_KEY | KEY2Token::object_placeholder :
    return makeContext<PlaceholderContext>(getState(), PLACEHOLDER_OBJECT, m_objectRef);
  case KEY2Token::NS_URI_KEY | KEY2Token::slide_number_placeholder :
    return makeContext<PlaceholderContext>(getState(), PLACEHOLDER_SLIDENUMBER, m_slidenumberRef);
  case KEY2Token::NS_URI_KEY | KEY2Token::title_placeholder :
    return makeContext<PlaceholderContext>(getState(), PLACEHOLDER_TITLE, m_titleRef);
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void SlideElement::endOfElement()
{
  if (isCollector())
  {
    if (m_styleRef)
    {
      const IWORKStyleMap_t::const_iterator it = getState().getDictionary().m_slideStyles.find(get(m_styleRef));
      if (it != getState().getDictionary().m_slideStyles.end())
        getCollector().setSlideStyle(it->second);
      else
      {
        ETONYEK_DEBUG_MSG(("SlideElement::endOfElement[KEY2Parser.cpp]: unknown style %s\n", get(m_styleRef).c_str()));
      }
    }
    if ((m_bodyText || m_titleText) && getState().getVersion()==2)
    {
      KEY2Dictionary &dict = getState().getDictionary();
      getCollector().startLayer();
      if (m_bodyText)
      {
        KEYPlaceholderMap_t::const_iterator it =
          m_bodyRef ? dict.m_bodyPlaceholders.find(get(m_bodyRef)) : dict.m_bodyPlaceholders.end();
        if (it==dict.m_bodyPlaceholders.end() || !it->second)
        {
          ETONYEK_DEBUG_MSG(("SlideElement::endOfElement[KEY2Parser.cpp]: can not find the body ref\n"));
        }
        else
        {
          KEYPlaceholderPtr_t placeholder=it->second;
          placeholder->m_text=m_bodyText;
          getCollector().insertTextPlaceholder(placeholder);
        }
      }
      if (m_titleText)
      {
        KEYPlaceholderMap_t::const_iterator it =
          m_titleRef ? dict.m_titlePlaceholders.find(get(m_titleRef)) : dict.m_titlePlaceholders.end();
        if (it==dict.m_titlePlaceholders.end() || !it->second)
        {
          ETONYEK_DEBUG_MSG(("SlideElement::endOfElement[KEY2Parser.cpp]: can not find the title ref\n"));
        }
        else
        {
          KEYPlaceholderPtr_t placeholder=it->second;
          placeholder->m_text=m_titleText;
          getCollector().insertTextPlaceholder(placeholder);
        }
      }
      const KEYLayerPtr_t layer(getCollector().collectLayer());
      getCollector().endLayer();
      if (layer)
        getCollector().insertLayer(layer);
    }
    else if (m_bodyText || m_titleText)
    {
      ETONYEK_DEBUG_MSG(("SlideElement::endOfElement[KEY2Parser.cpp]: find unexpected body/title text\n"));
    }

    KEYSlidePtr_t slide=getCollector().collectSlide();
    getCollector().endPage();
    if (!slide)
      return;
    slide->m_name=m_name;
    if (m_masterRef && m_isMasterSlide)
    {
      ETONYEK_DEBUG_MSG(("SlideElement::endOfElement[KEY2Parser.cpp]: find a master slide with masterRef\n"));
    }
    else if (m_masterRef)
    {
      auto const it = getState().getDictionary().m_masterSlides.find(get(m_masterRef));
      if (it != getState().getDictionary().m_masterSlides.end())
        slide->m_masterSlide=it->second;
      else
      {
        ETONYEK_DEBUG_MSG(("SlideElement::endOfElement[KEY2Parser.cpp]: unknown master %s\n", get(m_masterRef).c_str()));
      }
    }
    if (!m_isMasterSlide)
      getState().getDictionary().m_slides.push_back(slide);
    else if (getId())
      getState().getDictionary().m_masterSlides[get(getId())]=slide;
    else
    {
      ETONYEK_DEBUG_MSG(("SlideElement::endOfElement[KEY2Parser.cpp]: can not find a master id\n"));
    }
  }
}

}

namespace
{

class SlideListElement : public KEY2XMLElementContextBase
{
public:
  explicit SlideListElement(KEY2ParserState &state);

private:
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;
};

SlideListElement::SlideListElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void SlideListElement::startOfElement()
{
  if (isCollector())
    getCollector().startSlides();
}

IWORKXMLContextPtr_t SlideListElement::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::slide :
    return makeContext<SlideElement>(getState(), false);
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void SlideListElement::endOfElement()
{
  if (isCollector())
    getCollector().endSlides();
}

}

namespace
{

class MasterSlidesElement : public KEY2XMLElementContextBase
{
public:
  explicit MasterSlidesElement(KEY2ParserState &state);

private:
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;
};

MasterSlidesElement::MasterSlidesElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void MasterSlidesElement::startOfElement()
{
  if (isCollector())
    getCollector().startSlides();
}

IWORKXMLContextPtr_t MasterSlidesElement::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::master_slide :
    return makeContext<SlideElement>(getState(), true);
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void MasterSlidesElement::endOfElement()
{
  if (isCollector())
    getCollector().endSlides();
}
}

namespace
{

class ThemeElement : public KEY2XMLElementContextBase
{
public:
  explicit ThemeElement(KEY2ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;

private:
  // TODO: use size
  optional<IWORKSize> m_size;
};

ThemeElement::ThemeElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_size()
{
}

IWORKXMLContextPtr_t ThemeElement::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | IWORKToken::size :
    return makeContext<IWORKSizeElement>(getState(), m_size);
  case KEY2Token::NS_URI_KEY | KEY2Token::stylesheet :
    return makeContext<StylesheetElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::master_slides :
    return makeContext<MasterSlidesElement>(getState());
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class ThemeListElement : public KEY2XMLElementContextBase
{
public:
  explicit ThemeListElement(KEY2ParserState &state);

private:
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;
};

ThemeListElement::ThemeListElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void ThemeListElement::startOfElement()
{
  if (isCollector())
    getCollector().startThemes();
}

IWORKXMLContextPtr_t ThemeListElement::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::theme :
    return makeContext<ThemeElement>(getState());
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void ThemeListElement::endOfElement()
{
  if (isCollector())
    getCollector().endThemes();
}

}

namespace
{

class PresentationElement : public KEY2XMLElementContextBase
{
public:
  explicit PresentationElement(KEY2ParserState &state);

private:
  void startOfElement() override;
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  optional<IWORKSize> m_size;
  bool m_pendingSize;
};

PresentationElement::PresentationElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_size()
  , m_pendingSize(false)
{
}

void PresentationElement::startOfElement()
{
  if (isCollector())
    getCollector().startDocument();
}

void PresentationElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::version :
  {
    const unsigned version = getVersion(getToken(value));
    if (0 == version)
    {
      ETONYEK_DEBUG_MSG(("PresentationElement::attribute[KEY2Parser.cpp]: unknown version %s\n", value));
    }
    getState().setVersion(version);
    if (isCollector())
      getCollector().setAccumulateTransformTo(version>2);
  }
  break;
  default:
    break;
  }
}

IWORKXMLContextPtr_t PresentationElement::element(const int name)
{
  if (m_pendingSize)
  {
    if (m_size && isCollector())
      getCollector().collectPresentationSize(get(m_size));
    m_pendingSize = false;
  }

  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::metadata :
    return makeContext<MetadataElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::theme_list :
    return makeContext<ThemeListElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::slide_list :
    return makeContext<SlideListElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::size :
    m_pendingSize = true;
    return makeContext<IWORKSizeElement>(getState(), m_size);
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void PresentationElement::endOfElement()
{
  if (isCollector())
  {
    getCollector().sendSlides(getState().getDictionary().m_slides);
    getCollector().endDocument();
  }
}

}

namespace
{

class XMLDocument : public KEY2XMLElementContextBase
{
public:
  explicit XMLDocument(KEY2ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

XMLDocument::XMLDocument(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t XMLDocument::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::presentation :
    return makeContext<PresentationElement>(m_state);
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class DiscardContext : public KEY2XMLContextBase<IWORKDiscardContext>
{
public:
  explicit DiscardContext(KEY2ParserState &state);
  ~DiscardContext() override;

private:
  IWORKXMLContextPtr_t element(int name) override;

private:
  KEY2ParserState &m_state;
  IWORKStylesheetPtr_t m_savedStylesheet;
};

DiscardContext::DiscardContext(KEY2ParserState &state)
  : KEY2XMLContextBase<IWORKDiscardContext>(state)
  , m_state(state)
  , m_savedStylesheet()
{
}

DiscardContext::~DiscardContext()
{
  if (bool(m_savedStylesheet))
    m_state.m_stylesheet = m_savedStylesheet;
}

IWORKXMLContextPtr_t DiscardContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::placeholder_style :
    return makeContext<IWORKStyleContext>(getState(), &getState().getDictionary().m_placeholderStyles);
  case KEY2Token::NS_URI_KEY | KEY2Token::slide_style : // v5
  case IWORKToken::NS_URI_SF | IWORKToken::slide_style : // v2-v4
    return makeContext<KEY2StyleContext>(getState(), &getState().getDictionary().m_slideStyles);
  case KEY2Token::NS_URI_KEY | KEY2Token::stylesheet :
    if (!m_savedStylesheet)
    {
      // this can only happen in a broken document
      m_savedStylesheet = m_state.m_stylesheet;
      m_state.m_stylesheet.reset();
    }
    return makeContext<StylesheetElement>(getState());
  default:
    break;
  }

  return KEY2XMLContextBase<IWORKDiscardContext>::element(name);
}

}

KEY2Parser::KEY2Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, KEYCollector &collector, KEY2Dictionary &dict)
  : IWORKParser(input, package)
  , m_state(*this, collector, dict)
{
}

KEY2Parser::~KEY2Parser()
{
}

IWORKXMLContextPtr_t KEY2Parser::createDocumentContext()
{
  return makeContext<XMLDocument>(m_state);
}

IWORKXMLContextPtr_t KEY2Parser::createDiscardContext()
{
  return makeContext<DiscardContext>(m_state);
}

const IWORKTokenizer &KEY2Parser::getTokenizer() const
{
  static IWORKChainedTokenizer tokenizer(KEY2Token::getTokenizer(), IWORKToken::getTokenizer());
  return tokenizer;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
