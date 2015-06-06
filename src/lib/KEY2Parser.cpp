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
#include "IWORKMediaElement.h"
#include "IWORKPath.h"
#include "IWORKPathElement.h"
#include "IWORKPositionElement.h"
#include "IWORKRefContext.h"
#include "IWORKStyle.h"
#include "IWORKSizeElement.h"
#include "IWORKStringElement.h"
#include "IWORKStyleRefContext.h"
#include "IWORKStylesContext.h"
#include "IWORKTabularInfoElement.h"
#include "IWORKTextBodyElement.h"
#include "IWORKTextStorageElement.h"
#include "IWORKToken.h"
#include "IWORKTypes.h"
#include "KEY2ParserState.h"
#include "KEY2StyleContext.h"
#include "KEY2Token.h"
#include "KEY2XMLContextBase.h"
#include "KEYCollector.h"
#include "KEYDictionary.h"
#include "KEYTypes.h"

using boost::get_optional_value_or;
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
  virtual IWORKXMLContextPtr_t element(int name);

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
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

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
  virtual IWORKXMLContextPtr_t element(int name);
};

StylesContext::StylesContext(KEY2ParserState &state, const bool anonymous)
  : KEY2XMLContextBase<IWORKStylesContext>(state, anonymous)
{
}

IWORKXMLContextPtr_t StylesContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle :
    return makeContext<KEY2StyleContext>(getState(), &getState().getDictionary().m_layoutStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::placeholder_style :
    return makeContext<KEY2StyleContext>(getState(), &getState().getDictionary().m_placeholderStyles);

  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle_ref :
    return makeContext<IWORKStyleRefContext>(getState(), getState().getDictionary().m_layoutStyles, false, m_anonymous);
  }

  return KEY2XMLContextBase<IWORKStylesContext>::element(name);
}

}

namespace
{

class StylesheetElement : public KEY2XMLElementContextBase
{
public:
  explicit StylesheetElement(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<ID_t> m_parent;
};

StylesheetElement::StylesheetElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t StylesheetElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::styles :
    return makeContext<StylesContext>(getState(), false);
  case IWORKToken::NS_URI_SF | IWORKToken::anon_styles :
    return makeContext<StylesContext>(getState(), false);
  case IWORKToken::NS_URI_SF | IWORKToken::parent_ref :
    return makeContext<IWORKRefContext>(getState(), m_parent);
  }

  return IWORKXMLContextPtr_t();
}

void StylesheetElement::endOfElement()
{
  if (isCollector())
  {
    IWORKStylesheetPtr_t parent;

    if (m_parent)
    {
      assert(getId() != m_parent);

      const IWORKStylesheetMap_t::const_iterator it = getState().getDictionary().m_stylesheets.find(get(m_parent));
      if (getState().getDictionary().m_stylesheets.end() != it)
        parent = it->second;
    }

    const IWORKStylesheetPtr_t stylesheet = getCollector().collectStylesheet(parent);
    if (bool(stylesheet) && getId())
      getState().getDictionary().m_stylesheets[get(getId())] = stylesheet;
  }
}

}

namespace
{

class ProxyMasterLayerElement : public KEY2XMLElementContextBase
{
public:
  explicit ProxyMasterLayerElement(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

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
  }

  return IWORKXMLContextPtr_t();
}

void ProxyMasterLayerElement::endOfElement()
{
  if (m_ref && isCollector())
  {
    const KEYLayerMap_t::const_iterator it = getState().getDictionary().m_layers.find(get(m_ref));
    if (getState().getDictionary().m_layers.end() != it)
      getCollector().insertLayer(it->second);
  }
}

}

namespace
{

// NOTE: isn't it wonderful that there are two text elements in two
// different namespaces, but with the same schema?
class TextElement : public KEY2XMLElementContextBase
{
public:
  explicit TextElement(KEY2ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
};

TextElement::TextElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void TextElement::attribute(const int name, const char *)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle :
    // TODO: handle
    if (isCollector())
      getCollector().collectStyle(IWORKStylePtr_t(), false);
    break;
  }
}

IWORKXMLContextPtr_t TextElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::text_storage :
    return makeContext<IWORKTextStorageElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class ShapeElement : public KEY2XMLElementContextBase
{
public:
  explicit ShapeElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

ShapeElement::ShapeElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void ShapeElement::startOfElement()
{
  if (isCollector())
  {
    getCollector().startLevel();
    getCollector().startText();
  }
}

IWORKXMLContextPtr_t ShapeElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::path :
    return makeContext<IWORKPathElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::text :
    return makeContext<TextElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void ShapeElement::endOfElement()
{
  if (isCollector())
  {
    getCollector().collectShape();
    getCollector().endText();
    getCollector().endLevel();
  }
}

}

namespace
{

class ImageElement : public KEY2XMLElementContextBase
{
public:
  explicit ImageElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  IWORKImagePtr_t m_image;
};

ImageElement::ImageElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_image(new IWORKImage())
{
}

void ImageElement::startOfElement()
{
  if (isCollector())
    getCollector().startLevel();
}

void ImageElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::locked :
    m_image->m_locked = bool_cast(value);
    break;
  default :
    KEY2XMLElementContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t ImageElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void ImageElement::endOfElement()
{
  if (getId())
    getState().getDictionary().m_images[get(getId())] = m_image;
  if (isCollector())
  {
    getCollector().collectImage(m_image);
    getCollector().endLevel();
  }
}

}

namespace
{

class LineElement : public KEY2XMLElementContextBase
{
public:
  explicit LineElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKPosition> m_head;
  optional<IWORKPosition> m_tail;
};

LineElement::LineElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void LineElement::startOfElement()
{
  if (isCollector())
    getCollector().startLevel();
}

IWORKXMLContextPtr_t LineElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::head :
    return makeContext<IWORKPositionElement>(getState(), m_head);
  case IWORKToken::NS_URI_SF | IWORKToken::tail :
    return makeContext<IWORKPositionElement>(getState(), m_tail);
  }

  return IWORKXMLContextPtr_t();
}

void LineElement::endOfElement()
{
  IWORKLinePtr_t line(new IWORKLine());
  if (m_head)
  {
    line->m_x1 = get(m_head).m_x;
    line->m_y1 = get(m_head).m_y;
  }
  if (m_tail)
  {
    line->m_x2 = get(m_tail).m_x;
    line->m_y2 = get(m_tail).m_y;
  }
  if (isCollector())
  {
    getCollector().collectLine(line);
    getCollector().endLevel();
  }
}

}

namespace
{

class GroupElement : public KEY2XMLElementContextBase
{
public:
  explicit GroupElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

GroupElement::GroupElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void GroupElement::startOfElement()
{
  if (isCollector())
  {
    getCollector().startLevel();
    getCollector().startGroup();
  }
}

IWORKXMLContextPtr_t GroupElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::group :
    return makeContext<GroupElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::image :
    return makeContext<ImageElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::line :
    return makeContext<LineElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::media :
    return makeContext<IWORKMediaElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::shape :
    return makeContext<ShapeElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void GroupElement::endOfElement()
{
  if (isCollector())
  {
    getCollector().endGroup();
    getCollector().endLevel();
  }
}

}

namespace
{

class PlaceholderRefContext : public KEY2XMLEmptyContextBase
{
public:
  PlaceholderRefContext(KEY2ParserState &state, bool title);

private:
  virtual void endOfElement();

private:
  const bool m_title;
};

PlaceholderRefContext::PlaceholderRefContext(KEY2ParserState &state, const bool title)
  : KEY2XMLEmptyContextBase(state)
  , m_title(title)
{
}

void PlaceholderRefContext::endOfElement()
{
  if (getRef() && isCollector())
  {
    KEYDictionary &dict = getState().getDictionary();
    KEYPlaceholderMap_t &placeholderMap = m_title ? dict.m_titlePlaceholders : dict.m_bodyPlaceholders;
    const KEYPlaceholderMap_t::const_iterator it = placeholderMap.find(get(getRef()));
    if (placeholderMap.end() != it)
      getCollector().insertTextPlaceholder(it->second);
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
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
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

class StickyNoteElement : public KEY2XMLElementContextBase
{
public:
  explicit StickyNoteElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

StickyNoteElement::StickyNoteElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void StickyNoteElement::startOfElement()
{
  if (isCollector())
  {
    getCollector().startText();
    getCollector().startLevel();
  }
}

IWORKXMLContextPtr_t StickyNoteElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::text :
    return makeContext<TextElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void StickyNoteElement::endOfElement()
{
  if (isCollector())
  {
    getCollector().collectStickyNote();

    getCollector().endLevel();
    getCollector().endText();
  }
}

}

namespace
{

class DrawablesElement : public KEY2XMLElementContextBase
{
public:
  explicit DrawablesElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
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
    return makeContext<PlaceholderRefContext>(getState(), false);
  case IWORKToken::NS_URI_SF | IWORKToken::connection_line :
    return makeContext<ConnectionLineElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::group :
    return makeContext<GroupElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::image :
    return makeContext<ImageElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::line :
    return makeContext<LineElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::media :
    return makeContext<IWORKMediaElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::shape :
    return makeContext<ShapeElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::sticky_note :
    return makeContext<StickyNoteElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_info :
    return makeContext<IWORKTabularInfoElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::title_placeholder_ref :
    return makeContext<PlaceholderRefContext>(getState(), true);
  case KEY2Token::NS_URI_KEY | KEY2Token::sticky_note :
    return makeContext<StickyNoteElement>(getState());
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
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
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
  virtual IWORKXMLContextPtr_t element(int name);
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
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  // TODO: use size
  optional<IWORKSize> m_size;
};

PageElement::PageElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
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
  virtual IWORKXMLContextPtr_t element(int name);

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
  PlaceholderContext(KEY2ParserState &state, bool title);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  const bool m_title;
  optional<ID_t> m_ref;
};

PlaceholderContext::PlaceholderContext(KEY2ParserState &state, const bool title)
  : KEY2XMLElementContextBase(state)
  , m_title(title)
  , m_ref()
{
}

void PlaceholderContext::startOfElement()
{
  if (isCollector())
    getCollector().startText();
}

IWORKXMLContextPtr_t PlaceholderContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    // ignore; the real geometry comes from style
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::style :
    return makeContext<StyleElement>(getState(), m_ref);
  case KEY2Token::NS_URI_KEY | KEY2Token::text :
    return makeContext<TextElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void PlaceholderContext::endOfElement()
{
  if (isCollector())
  {
    IWORKStylePtr_t style;
    if (m_ref)
    {
      const IWORKStyleMap_t::const_iterator it = getState().getDictionary().m_placeholderStyles.find(get(m_ref));
      if (getState().getDictionary().m_placeholderStyles.end() != it)
        style = it->second;
    }

    const KEYPlaceholderPtr_t placeholder = getCollector().collectTextPlaceholder(style, m_title);
    if (bool(placeholder) && getId())
    {
      KEYDictionary &dict = getState().getDictionary();
      KEYPlaceholderMap_t &placeholderMap = m_title ? dict.m_titlePlaceholders : dict.m_bodyPlaceholders;
      placeholderMap[get(getId())] = placeholder;
    }
    getCollector().endText();
  }
}

}

namespace
{

class MasterSlideElement : public KEY2XMLElementContextBase
{
public:
  explicit MasterSlideElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

MasterSlideElement::MasterSlideElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void MasterSlideElement::startOfElement()
{
  if (isCollector())
    getCollector().startPage();
}

IWORKXMLContextPtr_t MasterSlideElement::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::page :
    return makeContext<PageElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::stylesheet :
    return makeContext<StylesheetElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::title_placeholder :
    return makeContext<PlaceholderContext>(getState(), true);
  case KEY2Token::NS_URI_KEY | KEY2Token::body_placeholder :
    return makeContext<PlaceholderContext>(getState(), false);
  case KEY2Token::NS_URI_KEY | KEY2Token::sticky_notes :
    return makeContext<StickyNoteElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void MasterSlideElement::endOfElement()
{
  if (isCollector())
  {
    getCollector().collectPage();
    getCollector().endPage();
  }
}

}

namespace
{

class MasterSlidesElement : public KEY2XMLElementContextBase
{
public:
  explicit MasterSlidesElement(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

MasterSlidesElement::MasterSlidesElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t MasterSlidesElement::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::master_slide :
    return makeContext<MasterSlideElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class ThemeElement : public KEY2XMLElementContextBase
{
public:
  explicit ThemeElement(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  // TODO: use size
  optional<IWORKSize> m_size;
};

ThemeElement::ThemeElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
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
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
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

class NotesElement : public KEY2XMLElementContextBase
{
public:
  explicit NotesElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

NotesElement::NotesElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void NotesElement::startOfElement()
{
  if (isCollector())
    getCollector().startText();
}

IWORKXMLContextPtr_t NotesElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::text_storage | IWORKToken::NS_URI_SF :
    return makeContext<IWORKTextStorageElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void NotesElement::endOfElement()
{
  if (isCollector())
  {
    getCollector().collectNote();
    getCollector().endText();
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
  virtual IWORKXMLContextPtr_t element(int name);
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
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class SlideElement : public KEY2XMLElementContextBase
{
public:
  explicit SlideElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

SlideElement::SlideElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
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
  case KEY2Token::NS_URI_KEY | KEY2Token::notes :
    return makeContext<NotesElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::page :
    return makeContext<PageElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::stylesheet :
    return makeContext<StylesheetElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::title_placeholder :
    return makeContext<PlaceholderContext>(getState(), true);
  case KEY2Token::NS_URI_KEY | KEY2Token::body_placeholder :
    return makeContext<PlaceholderContext>(getState(), false);
  case KEY2Token::NS_URI_KEY | KEY2Token::sticky_notes :
    return makeContext<StickyNotesElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void SlideElement::endOfElement()
{
  if (isCollector())
  {
    getCollector().collectPage();
    getCollector().endPage();
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
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
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
    return makeContext<SlideElement>(getState());
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

class PresentationElement : public KEY2XMLElementContextBase
{
public:
  explicit PresentationElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

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
      ETONYEK_DEBUG_MSG(("unknown version %s\n", value));
    }
  }
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
  }

  return IWORKXMLContextPtr_t();
}

void PresentationElement::endOfElement()
{
  if (isCollector())
    getCollector().endDocument();
}

}

namespace
{

class XMLDocument : public KEY2XMLElementContextBase
{
public:
  explicit XMLDocument(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
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

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

DiscardContext::DiscardContext(KEY2ParserState &state)
  : KEY2XMLContextBase<IWORKDiscardContext>(state)
{
}

IWORKXMLContextPtr_t DiscardContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle :
    return makeContext<KEY2StyleContext>(getState(), &getState().getDictionary().m_layoutStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::placeholder_style :
    return makeContext<KEY2StyleContext>(getState(), &getState().getDictionary().m_placeholderStyles);
  }

  return KEY2XMLContextBase<IWORKDiscardContext>::element(name);
}

}

KEY2Parser::KEY2Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, KEYCollector &collector, KEYDictionary &dict)
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
