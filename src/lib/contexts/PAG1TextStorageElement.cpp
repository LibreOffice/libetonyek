/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAG1TextStorageElement.h"

#include <cassert>
#include <string>

#include <boost/optional.hpp>

#include <librevenge/librevenge.h>

#include "IWORKGroupElement.h"
#include "IWORKLayoutElement.h"
#include "IWORKLinkElement.h"
#include "IWORKMediaElement.h"
#include "IWORKPElement.h"
#include "IWORKPositionElement.h"
#include "IWORKRefContext.h"
#include "IWORKSizeElement.h"
#include "IWORKShapeContext.h"
#include "IWORKSpanElement.h"
#include "IWORKTabularInfoElement.h"
#include "IWORKText.h"
#include "IWORKTextBodyElement.h"
#include "IWORKToken.h"
#include "PAG1AnnotationElement.h"
#include "PAG1Dictionary.h"
#include "PAG1FootnotesElement.h"
#include "PAG1ParserState.h"
#include "PAGCollector.h"
#include "libetonyek_xml.h"

namespace libetonyek
{

using boost::optional;

using std::string;

namespace
{

struct ContainerFrame
{
  ContainerFrame();

  optional<double> m_w;
  optional<double> m_h;
  optional<double> m_x;
  optional<double> m_y;
};

ContainerFrame::ContainerFrame()
  : m_w()
  , m_h()
  , m_x()
  , m_y()
{
}

}

namespace
{

class AttachmentElement : public PAG1XMLElementContextBase
{
public:
  explicit AttachmentElement(PAG1ParserState &state);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  bool m_known;
  bool m_block;
  optional<IWORKPosition> m_position;
  optional<IWORKSize> m_originalSize;
  std::shared_ptr<IWORKText> m_savedText;
};

AttachmentElement::AttachmentElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
  , m_known(false)
  , m_block(false)
  , m_position()
  , m_originalSize()
  , m_savedText()
{
  // saved the current zone of text
  m_savedText=getState().m_currentText;
  getState().m_currentText.reset();
}

void AttachmentElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::sfclass :
  case IWORKToken::NS_URI_SF | IWORKToken::kind :
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    return PAG1XMLElementContextBase::attribute(name, value);
  default:
    ETONYEK_DEBUG_MSG(("AttachmentElement::attribute[PAG1TextStorageElement]: find some unknown attribute\n"));
    break;
  }
}
IWORKXMLContextPtr_t AttachmentElement::element(const int name)
{
  IWORKXMLContextPtr_t context;

  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::drawable_shape :
  {
    static bool first=true;
    if (first)
    {
      ETONYEK_DEBUG_MSG(("AttachmentElement::attribute[PAG1TextStorageElement]: find some shapes attached in textbox, not implemented\n"));
      first=false;
    }
    //m_block = true;
    //context = makeContext<IWORKShapeContext>(getState());
    break;
  }
  case IWORKToken::NS_URI_SF | IWORKToken::group :
  {
    static bool first=true;
    if (first)
    {
      ETONYEK_DEBUG_MSG(("AttachmentElement::attribute[PAG1TextStorageElement]: find some groups attached in textbox, not implemented\n"));
      first=false;
    }
    //m_block = true;
    //context = makeContext<IWORKGroupElement>(getState());
    break;
  }
  case IWORKToken::NS_URI_SF | IWORKToken::media :
    m_block = false;
    context = makeContext<IWORKMediaElement>(getState());
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::original_size :
    return makeContext<IWORKSizeElement>(getState(), m_originalSize);
  case IWORKToken::NS_URI_SF | IWORKToken::position :
    return makeContext<IWORKPositionElement>(getState(), m_position);
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_info :
    m_block = true;
    context = makeContext<IWORKTabularInfoElement>(getState());
    break;
  default:
    ETONYEK_DEBUG_MSG(("AttachmentElement::element[PAG1TextStorageElement]: find some unknown element\n"));
  }

  if (bool(context))
  {
    m_known = true;
    if (isCollector())
      getCollector().getOutputManager().push();
  }

  return context;
}

void AttachmentElement::endOfElement()
{
  if (m_known)
  {
    if (isCollector())
    {
      if (m_position)
        getCollector().collectAttachmentPosition(get(m_position));
      if (getId())
        getState().getDictionary().m_attachments[get(getId())] = PAGAttachment(getCollector().getOutputManager().save(), m_block);
      getCollector().getOutputManager().pop();
    }
  }
  // reset the current zone of text
  getState().m_currentText=m_savedText;
}

}

namespace
{

class AttachmentsElement : public PAG1XMLElementContextBase
{
public:
  explicit AttachmentsElement(PAG1ParserState &state);

private:
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;
};

AttachmentsElement::AttachmentsElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

void AttachmentsElement::startOfElement()
{
  if (isCollector())
    getCollector().openAttachments();
}

IWORKXMLContextPtr_t AttachmentsElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::attachment))
    return makeContext<AttachmentElement>(getState());
  return IWORKXMLContextPtr_t();
}

void AttachmentsElement::endOfElement()
{
  if (isCollector())
    getCollector().closeAttachments();
}

}

namespace
{
class AttachmentRef  : public PAG1XMLElementContextBase
{
public:
  AttachmentRef(PAG1ParserState &state);

private:
  virtual void attribute(const int name, const char *const value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

  boost::optional<ID_t> m_ref;
  boost::optional<std::string> m_kind;
};

AttachmentRef::AttachmentRef(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
  , m_ref()
  , m_kind()
{
}

void AttachmentRef::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::IDREF :
    m_ref = value;
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::kind :
    m_kind = value;
    break;
  default:
    ETONYEK_DEBUG_MSG(("AttachmentRef::attribute[PAG1TextStorageElement]: find unexpected attribute\n"));
  }
}

IWORKXMLContextPtr_t AttachmentRef::element(int /*name*/)
{
  ETONYEK_DEBUG_MSG(("AttachmentRef::element[PAG1TextStorageElement]: find unexpected element\n"));
  return IWORKXMLContextPtr_t();
}

void AttachmentRef::endOfElement()
{
  if (!isCollector())
    return;
  if (!m_ref)
  {
    ETONYEK_DEBUG_MSG(("AttachmentRef::endOfElement[PAG1TextStorageElement]: called without ref\n"));
    return;
  }
  if (!getState().m_currentText)
  {
    ETONYEK_DEBUG_MSG(("AttachmentRef::endOfElement[PAG1TextStorageElement]: can not find attachment current text\n"));
    return;
  }

  const PAGAttachmentMap_t::const_iterator it = getState().getDictionary().m_attachments.find(get(m_ref));
  if (it != getState().getDictionary().m_attachments.end())
  {
    const IWORKOutputElements &content = getCollector().getOutputManager().get(it->second.m_id);
    if (it->second.m_block)
      getState().m_currentText->insertBlockContent(content);
    else
      getState().m_currentText->insertInlineContent(content);
  }
  else
  {
    ETONYEK_DEBUG_MSG(("AttachmentRef::endOfElement[PAG1TextStorageElement]: can not find attachment %s\n", get(m_ref).c_str()));
  }
}

}

namespace
{

class FootnoteElement : public PAG1XMLEmptyContextBase
{
public:
  explicit FootnoteElement(PAG1ParserState &state);

private:
  void endOfElement() override;
};

FootnoteElement::FootnoteElement(PAG1ParserState &state)
  : PAG1XMLEmptyContextBase(state)
{
}

void FootnoteElement::endOfElement()
{
  if (getState().m_footnoteState.m_nextFootnote != getState().m_footnoteState.m_footnotes.end())
  {
    if (bool(getState().m_currentText))
      getState().m_currentText->insertInlineContent(*getState().m_footnoteState.m_nextFootnote);
    ++getState().m_footnoteState.m_nextFootnote;
  }
}

}

namespace
{

class FootnotebrElement : public PAG1XMLEmptyContextBase
{
public:
  explicit FootnotebrElement(PAG1ParserState &state);

private:
  void endOfElement() override;
};

FootnotebrElement::FootnotebrElement(PAG1ParserState &state)
  : PAG1XMLEmptyContextBase(state)
{
}

void FootnotebrElement::endOfElement()
{
  getState().m_footnoteState.m_pending = true;
}

}

namespace
{

class FootnoteMarkElement : public PAG1XMLEmptyContextBase
{
public:
  explicit FootnoteMarkElement(PAG1ParserState &state);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;
};

FootnoteMarkElement::FootnoteMarkElement(PAG1ParserState &state)
  : PAG1XMLEmptyContextBase(state)
{
}

void FootnoteMarkElement::attribute(const int name, const char *const value)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::mark))
    m_state.m_footnoteState.m_mark = value;
  else
  {
    ETONYEK_DEBUG_MSG(("FootnoteMarkElement::attribute[PAG1TextStorageElement]: find some unknown attribute\n"));
  }
}

void FootnoteMarkElement::endOfElement()
{
  m_state.m_footnoteState.m_firstTextAfterMark = true;
}

}

namespace
{

class FootnoteHelper
{
public:
  FootnoteHelper(PAG1ParserState &state);

  IWORKXMLContextPtr_t element(int name);
  const char *text(const char *value);

private:
  PAG1ParserState &m_state;
};

FootnoteHelper::FootnoteHelper(PAG1ParserState &state)
  : m_state(state)
{
}

IWORKXMLContextPtr_t FootnoteHelper::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::footnote :
    return makeContext<FootnoteElement>(m_state);
  case IWORKToken::NS_URI_SF | IWORKToken::footnotebr :
    return makeContext<FootnotebrElement>(m_state);
  case IWORKToken::NS_URI_SF | IWORKToken::footnote_mark :
    return makeContext<FootnoteMarkElement>(m_state);
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

const char *FootnoteHelper::text(const char *const value)
{
  if (m_state.m_footnoteState.m_firstTextAfterMark && value && (value[0] == ' '))
  {
    m_state.m_footnoteState.m_firstTextAfterMark = false;
    return value + 1;
  }
  return value;
}

}

namespace
{

class SpanElement : public PAG1XMLContextBase<IWORKSpanElement>
{
public:
  explicit SpanElement(PAG1ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void text(const char *value) override;

private:
  FootnoteHelper m_footnoteHelper;
};

SpanElement::SpanElement(PAG1ParserState &state)
  : PAG1XMLContextBase<IWORKSpanElement>(state)
  , m_footnoteHelper(state)
{
}

IWORKXMLContextPtr_t SpanElement::element(const int name)
{
  const IWORKXMLContextPtr_t context = m_footnoteHelper.element(name);
  if (bool(context))
    return context;
  if (name==(IWORKToken::NS_URI_SF | IWORKToken::attachment_ref))
    return makeContext<AttachmentRef>(getState());
  return IWORKSpanElement::element(name);
}

void SpanElement::text(const char *value)
{
  PAG1XMLContextBase<IWORKSpanElement>::text(m_footnoteHelper.text(value));
}

}

namespace
{

class LinkElement : public PAG1XMLContextBase<IWORKLinkElement>
{
public:
  LinkElement(PAG1ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void text(const char *value) override;

private:
  FootnoteHelper m_footnoteHelper;
};

LinkElement::LinkElement(PAG1ParserState &state)
  : PAG1XMLContextBase<IWORKLinkElement>(state)
  , m_footnoteHelper(state)
{
}

IWORKXMLContextPtr_t LinkElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::span))
    return makeContext<SpanElement>(m_state);
  const IWORKXMLContextPtr_t context = m_footnoteHelper.element(name);
  if (bool(context))
    return context;
  return IWORKLinkElement::element(name);
}

void LinkElement::text(const char *value)
{
  PAG1XMLContextBase<IWORKLinkElement>::text(m_footnoteHelper.text(value));
}

}

namespace
{

class PElement : public PAG1XMLContextBase<IWORKPElement>
{
public:
  explicit PElement(PAG1ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void text(const char *value) override;
  void endOfElement() override;

private:
  FootnoteHelper m_footnoteHelper;
};

PElement::PElement(PAG1ParserState &state)
  : PAG1XMLContextBase<IWORKPElement>(state)
  , m_footnoteHelper(state)
{
}

IWORKXMLContextPtr_t PElement::element(const int name)
{
  ensureOpened();

  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::attachment_ref :
    return makeContext<AttachmentRef>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::link :
    return makeContext<LinkElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::span :
    return makeContext<SpanElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::annotation_field :
    return makeContext<PAG1AnnotationElement>(getState(),*this);
  case IWORKToken::NS_URI_SF | IWORKToken::annotation_field_ref :
    return makeContext<PAG1AnnotationElement>(getState(),*this,true);
  default:
    break;
  }

  const IWORKXMLContextPtr_t context = m_footnoteHelper.element(name);
  if (bool(context))
    return context;

  return IWORKPElement::element(name);
}

void PElement::text(const char *value)
{
  PAG1XMLContextBase<IWORKPElement>::text(m_footnoteHelper.text(value));
}

void PElement::endOfElement()
{
  IWORKPElement::endOfElement();

  if (getState().m_footnoteState.m_pending)
  {
    PAGFootnoteState &fs = getState().m_footnoteState;
    const bool firstFootnote = fs.m_footnotes.empty();
    fs.m_footnotes.push_back(IWORKOutputElements());
    if (firstFootnote) // We can init. insertion iterator now
      fs.m_nextFootnote = fs.m_footnotes.begin();
    librevenge::RVNGPropertyList props;
    if (!fs.m_mark.empty())
      props.insert("text:label", fs.m_mark.c_str());
    if (getCollector().getFootnoteKind() == PAG_FOOTNOTE_KIND_FOOTNOTE)
      fs.m_footnotes.back().addOpenFootnote(props);
    else
      fs.m_footnotes.back().addOpenEndnote(props);
    getState().m_currentText->draw(fs.m_footnotes.back());
    // prepare for possible next footnote
    // TODO: introduce IN_FOOTNOTES state and move this to startOfElement
    getState().m_currentText = getCollector().createText(getState().m_langManager, false, false);

    if (getCollector().getFootnoteKind() == PAG_FOOTNOTE_KIND_FOOTNOTE)
      fs.m_footnotes.back().addCloseFootnote();
    else
      fs.m_footnotes.back().addCloseEndnote();

    fs.m_pending = false;
    fs.m_firstTextAfterMark = false;
    fs.m_mark.clear();
  }
}

}

namespace
{

class LayoutElement : public PAG1XMLContextBase<IWORKLayoutElement>
{
public:
  explicit LayoutElement(PAG1ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

LayoutElement::LayoutElement(PAG1ParserState &state)
  : PAG1XMLContextBase<IWORKLayoutElement>(state)
{
}

IWORKXMLContextPtr_t LayoutElement::element(const int name)
{
  if (!m_opened)
    open();

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
  SectionElement(PAG1ParserState &state);

private:
  void open();

  void startOfElement() override;
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  bool m_opened;
  optional<string> m_style;
};

SectionElement::SectionElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
  , m_opened(false)
  , m_style()
{
}

void SectionElement::open()
{
  assert(!m_opened);

  if (isCollector())
    getCollector().openSection(get_optional_value_or(m_style, ""));

  m_opened = true;
}

void SectionElement::startOfElement()
{
  if (isCollector())
  {
    // This should not happen in normal files, but better be safe than sorry
    if (bool(getState().m_currentText) && !getState().m_currentText->empty())
    {
      getCollector().collectText(getState().m_currentText);
      getState().m_currentText = getCollector().createText(getState().m_langManager);
      getCollector().collectTextBody();
    }
  }
}

void SectionElement::attribute(const int name, const char *const value)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::style))
    m_style = value;
  else
    PAG1XMLElementContextBase::attribute(name, value);
}

IWORKXMLContextPtr_t SectionElement::element(const int name)
{
  if (!m_opened)
    open();

  if ((IWORKToken::NS_URI_SF | IWORKToken::layout) == name)
    return makeContext<LayoutElement>(getState());

  return IWORKXMLContextPtr_t();
}

void SectionElement::endOfElement()
{
  if (isCollector())
  {
    if (!m_opened)
      open();
    getCollector().collectText(getState().m_currentText);
    // In case there's non-section text following. Again, this should not happen in normal files.
    getState().m_currentText = getCollector().createText(getState().m_langManager);
    getCollector().closeSection();
  }
}

}

namespace
{

class ContainerHintElement : public PAG1XMLEmptyContextBase
{
public:
  ContainerHintElement(PAG1ParserState &state, ContainerFrame &containerFrame);

private:
  void attribute(int name, const char *value) override;

private:
  ContainerFrame &m_containerFrame;
};

ContainerHintElement::ContainerHintElement(PAG1ParserState &state, ContainerFrame &containerFrame)
  : PAG1XMLEmptyContextBase(state)
  , m_containerFrame(containerFrame)
{
}

void ContainerHintElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::frame_h :
    m_containerFrame.m_h = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::frame_w :
    m_containerFrame.m_w = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::frame_x :
    m_containerFrame.m_x = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::frame_y :
    m_containerFrame.m_y = double_cast(value);
    break;
  // also page-index, cindex, sindex, lindex, anchor-loc, nlabel=true/false
  default:
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
  IWORKXMLContextPtr_t element(int name) override;

private:
  ContainerFrame m_containerFrame;
};

TextBodyElement::TextBodyElement(PAG1ParserState &state)
  : PAG1XMLContextBase<IWORKTextBodyElement>(state)
  , m_containerFrame()
{
}

IWORKXMLContextPtr_t TextBodyElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::container_hint :
    return makeContext<ContainerHintElement>(getState(), m_containerFrame);
  case IWORKToken::NS_URI_SF | IWORKToken::p : // for footnotes
    return makeContext<PElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::section :
    return makeContext<SectionElement>(getState());
  default:
    break;
  }

  return IWORKTextBodyElement::element(name);
}

}

PAG1TextStorageElement::PAG1TextStorageElement(PAG1ParserState &state, PAGTextStorageKind kind)
  : PAG1XMLContextBase<IWORKTextStorageElement>(state)
  , m_kind(kind)
  , m_textOpened(false)
{
  if (m_kind==PAG_TEXTSTORAGE_KIND_TEXTBOX)
  {
    m_textOpened=true;
    if (!bool(getState().m_currentText))
    {
      ETONYEK_DEBUG_MSG(("PAG1TextStorageElement::PAG1TextStorageElement: called in textbox without current tetx\n"));
    }
  }
}

IWORKXMLContextPtr_t PAG1TextStorageElement::element(const int name)
{
  sendStylesheet();

  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::attachments :
    return makeContext<AttachmentsElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::footnotes :
    return makeContext<PAG1FootnotesElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::text_body :
    if (!m_textOpened)
    {
      assert(!getState().m_currentText);
      getState().m_currentText = getCollector().createText(getState().m_langManager, m_kind==PAG_TEXTSTORAGE_KIND_TEXTBOX);
      m_textOpened = true;
    }
    return makeContext<TextBodyElement>(getState());
  default:
    break;
  }

  return PAG1XMLContextBase<IWORKTextStorageElement>::element(name);
}

void PAG1TextStorageElement::endOfElement()
{
  if (isCollector() && m_textOpened && m_kind==PAG_TEXTSTORAGE_KIND_BASIC)
  {
    assert(getState().m_currentText);
    if (bool(getState().m_currentText) && !getState().m_currentText->empty())
      getCollector().collectText(getState().m_currentText);
    getCollector().collectTextBody();
  }
  if (m_kind!=PAG_TEXTSTORAGE_KIND_TEXTBOX)
    getState().m_currentText.reset();

  PAG1XMLContextBase<IWORKTextStorageElement>::endOfElement();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
