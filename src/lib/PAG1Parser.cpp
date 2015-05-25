/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAG1Parser.h"

#include "libetonyek_xml.h"
#include "IWORKChainedTokenizer.h"
#include "IWORKLayoutElement.h"
#include "IWORKPElement.h"
#include "IWORKRefContext.h"
#include "IWORKStylesContext.h"
#include "IWORKTabularInfoElement.h"
#include "IWORKTextBodyElement.h"
#include "IWORKTextStorageElement.h"
#include "IWORKToken.h"
#include "PAGCollector.h"
#include "PAGDictionary.h"
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

struct PageFrame
{
  PageFrame();

  optional<double> m_w;
  optional<double> m_h;
  optional<double> m_x;
  optional<double> m_y;
};

PageFrame::PageFrame()
  : m_w()
  , m_h()
  , m_x()
  , m_y()
{
}

}

namespace
{

class FootersElement : public PAG1XMLElementContextBase
{
public:
  explicit FootersElement(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

FootersElement::FootersElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t FootersElement::element(int)
{
  // TODO: parse
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class HeadersElement : public PAG1XMLElementContextBase
{
public:
  explicit HeadersElement(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

HeadersElement::HeadersElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t HeadersElement::element(int)
{
  // TODO: parse
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class StylesheetElement : public PAG1XMLElementContextBase
{
public:
  explicit StylesheetElement(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

StylesheetElement::StylesheetElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t StylesheetElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::anon_styles :
    return makeContext<IWORKStylesContext>(getState(), true);
  case IWORKToken::NS_URI_SF | IWORKToken::styles :
    return makeContext<IWORKStylesContext>(getState(), false);
  }

  return IWORKXMLContextPtr_t();
}

void StylesheetElement::endOfElement()
{
  getCollector()->collectStylesheet();
}

}

namespace
{

class SectionPrototypesElement : public PAG1XMLElementContextBase
{
public:
  explicit SectionPrototypesElement(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

SectionPrototypesElement::SectionPrototypesElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t SectionPrototypesElement::element(int)
{
  // TODO: parse
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class MetadataElement : public PAG1XMLElementContextBase
{
public:
  explicit MetadataElement(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

MetadataElement::MetadataElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t MetadataElement::element(int)
{
  // TODO: parse
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class AttachmentElement : public PAG1XMLElementContextBase
{
public:
  explicit AttachmentElement(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  bool m_known;
};

AttachmentElement::AttachmentElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
  , m_known(false)
{
}

IWORKXMLContextPtr_t AttachmentElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::tabular_info))
  {
    m_known = true;
    getCollector()->getOutputManager().push();
    return makeContext<IWORKTabularInfoElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void AttachmentElement::endOfElement()
{
  if (m_known)
  {
    if (getId())
      getState().getDictionary().m_attachments[get(getId())] = getCollector()->getOutputManager().save();
    getCollector()->getOutputManager().pop();
  }
}

}

namespace
{

class AttachmentsElement : public PAG1XMLElementContextBase
{
public:
  explicit AttachmentsElement(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

AttachmentsElement::AttachmentsElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t AttachmentsElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::attachment))
    return makeContext<AttachmentElement>(getState());
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class PElement : public PAG1XMLContextBase<IWORKPElement>
{
public:
  explicit PElement(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<ID_t> m_ref;
};

PElement::PElement(PAG1ParserState &state)
  : PAG1XMLContextBase<IWORKPElement>(state)
  , m_ref()
{
}

IWORKXMLContextPtr_t PElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::attachment_ref))
  {
    // It is possible that there can be 2 or more attachments in the same para.
    // In that case the code would have to be adapted to handle that.
    assert(!m_ref);
    return makeContext<IWORKRefContext>(getState(), m_ref);
  }

  return IWORKPElement::element(name);
}

void PElement::endOfElement()
{
  if (m_ref)
  {
    const IWORKOutputMap_t::const_iterator it = getState().getDictionary().m_attachments.find(get(m_ref));
    if (it != getState().getDictionary().m_attachments.end())
      getCollector()->collectAttachment(it->second);
  }

  IWORKPElement::endOfElement();
}

}

namespace
{

class LayoutElement : public PAG1XMLContextBase<IWORKLayoutElement>
{
public:
  explicit LayoutElement(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

LayoutElement::LayoutElement(PAG1ParserState &state)
  : PAG1XMLContextBase<IWORKLayoutElement>(state)
{
}

IWORKXMLContextPtr_t LayoutElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::p))
    return makeContext<PElement>(getState());

  return IWORKLayoutElement::element(name);
}

}

namespace
{

class SectionElement : public PAG1XMLElementContextBase
{
public:
  SectionElement(PAG1ParserState &state, const PageFrame &pageFrame);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  const PageFrame &m_pageFrame;
};

SectionElement::SectionElement(PAG1ParserState &state, const PageFrame &pageFrame)
  : PAG1XMLElementContextBase(state)
  , m_pageFrame(pageFrame)
{
}

void SectionElement::startOfElement()
{
  const double w(get_optional_value_or(m_pageFrame.m_w, 0));
  const double h(get_optional_value_or(m_pageFrame.m_h, 0));
  const double x(get_optional_value_or(m_pageFrame.m_x, 0));
  const double y(get_optional_value_or(m_pageFrame.m_y, 0));

  // TODO: This assumes that the left/right and top/bottom margins are always equal.
  getCollector()->openSection(pt2in(w + 2 * x), pt2in(h + 2 * y), pt2in(x), pt2in(y));
}

IWORKXMLContextPtr_t SectionElement::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::layout) == name)
    return makeContext<LayoutElement>(getState());

  return IWORKXMLContextPtr_t();
}

void SectionElement::endOfElement()
{
  getCollector()->closeSection();
}

}

namespace
{

class ContainerHintElement : public PAG1XMLEmptyContextBase
{
public:
  ContainerHintElement(PAG1ParserState &state, PageFrame &pageFrame);

private:
  virtual void attribute(int name, const char *value);

private:
  PageFrame &m_pageFrame;
};

ContainerHintElement::ContainerHintElement(PAG1ParserState &state, PageFrame &pageFrame)
  : PAG1XMLEmptyContextBase(state)
  , m_pageFrame(pageFrame)
{
}

void ContainerHintElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::frame_h :
    m_pageFrame.m_h = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::frame_w :
    m_pageFrame.m_w = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::frame_x :
    m_pageFrame.m_x = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::frame_y :
    m_pageFrame.m_y = double_cast(value);
    break;
  }
}

}

namespace
{

class TextBodyElement : public PAG1XMLContextBase<IWORKTextBodyElement>
{
public:
  explicit TextBodyElement(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  PageFrame m_pageFrame;
};

TextBodyElement::TextBodyElement(PAG1ParserState &state)
  : PAG1XMLContextBase<IWORKTextBodyElement>(state)
  , m_pageFrame()
{
}

IWORKXMLContextPtr_t TextBodyElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::container_hint :
    return makeContext<ContainerHintElement>(getState(), m_pageFrame);
  case IWORKToken::NS_URI_SF | IWORKToken::section :
    return makeContext<SectionElement>(getState(), m_pageFrame);
  }

  return IWORKTextBodyElement::element(name);
}

}

namespace
{

class TextStorageElement : public PAG1XMLContextBase<IWORKTextStorageElement>
{
public:
  explicit TextStorageElement(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  bool m_textOpened;
};

TextStorageElement::TextStorageElement(PAG1ParserState &state)
  : PAG1XMLContextBase<IWORKTextStorageElement>(state)
  , m_textOpened(false)
{
}

IWORKXMLContextPtr_t TextStorageElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::attachments :
    return makeContext<AttachmentsElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::text_body :
    if (!m_textOpened)
    {
      getCollector()->startText();
      m_textOpened = true;
    }
    return makeContext<TextBodyElement>(getState());
  }

  return PAG1XMLContextBase<IWORKTextStorageElement>::element(name);
}

void TextStorageElement::endOfElement()
{
  IWORKTextStorageElement::endOfElement();

  if (m_textOpened)
  {
    getCollector()->collectTextBody();
    getCollector()->endText();
  }
}

}

namespace
{

class DocumentElement : public PAG1XMLElementContextBase
{
public:
  explicit DocumentElement(PAG1ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKSize> m_size;
};

DocumentElement::DocumentElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
  , m_size()
{
}

void DocumentElement::attribute(const int name, const char *const value)
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

IWORKXMLContextPtr_t DocumentElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::headers :
    return makeContext<HeadersElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::footers :
    return makeContext<FootersElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::metadata :
    return makeContext<MetadataElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::text_storage :
    return makeContext<TextStorageElement>(getState());
  case PAG1Token::NS_URI_SL | PAG1Token::section_prototypes :
    return makeContext<SectionPrototypesElement>(getState());
  case PAG1Token::NS_URI_SL | PAG1Token::stylesheet :
    return makeContext<StylesheetElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void DocumentElement::endOfElement()
{
}

}

namespace
{

class XMLDocument : public PAG1XMLElementContextBase
{
public:
  explicit XMLDocument(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

XMLDocument::XMLDocument(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t XMLDocument::element(const int name)
{
  switch (name)
  {
  case PAG1Token::NS_URI_SL | PAG1Token::document :
    return makeContext<DocumentElement>(m_state);
  }

  return IWORKXMLContextPtr_t();
}

}

PAG1Parser::PAG1Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, PAGCollector *const collector, PAGDictionary *const dict)
  : IWORKParser(input, package)
  , m_state(*this, collector, *dict)
{
}

PAG1Parser::~PAG1Parser()
{
}

IWORKXMLContextPtr_t PAG1Parser::createDocumentContext()
{
  return makeContext<XMLDocument>(m_state);
}

const IWORKTokenizer &PAG1Parser::getTokenizer() const
{
  static IWORKChainedTokenizer tokenizer(PAG1Token::getTokenizer(), IWORKToken::getTokenizer());
  return tokenizer;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
