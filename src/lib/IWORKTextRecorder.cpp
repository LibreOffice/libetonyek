/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTextRecorder.h"

#include <deque>

#include <boost/variant.hpp>

#include "libetonyek_utils.h"
#include "IWORKText.h"

namespace libetonyek
{

namespace
{

struct PushBaseLayoutStyle
{
  explicit PushBaseLayoutStyle(const IWORKStylePtr_t &style)
    : m_style(style)
  {
  }

  const IWORKStylePtr_t m_style;
};

struct PushBaseParagraphStyle
{
  explicit PushBaseParagraphStyle(const IWORKStylePtr_t &style)
    : m_style(style)
  {
  }

  const IWORKStylePtr_t m_style;
};

struct SetLayoutStyle
{
  explicit SetLayoutStyle(const IWORKStylePtr_t &style)
    : m_style(style)
  {
  }

  const IWORKStylePtr_t m_style;
};

struct FlushLayout
{
};

struct SetListStyle
{
  explicit SetListStyle(const IWORKStylePtr_t &style)
    : m_style(style)
  {
  }

  const IWORKStylePtr_t m_style;
};

struct SetListLevel
{
  explicit SetListLevel(const unsigned level)
    : m_level(level)
  {
  }

  const unsigned m_level;
};

struct FlushList
{
};

struct SetParagraphStyle
{
  explicit SetParagraphStyle(const IWORKStylePtr_t &style)
    : m_style(style)
  {
  }

  const IWORKStylePtr_t m_style;
};

struct FlushParagraph
{
};

struct SetSpanStyle
{
  explicit SetSpanStyle(const IWORKStylePtr_t &style)
    : m_style(style)
  {
  }

  const IWORKStylePtr_t m_style;
};

struct SetLanguage
{
  explicit SetLanguage(const IWORKStylePtr_t &style)
    : m_style(style)
  {
  }

  const IWORKStylePtr_t m_style;
};

struct FlushSpan
{
};

struct InsertField
{
  explicit InsertField(IWORKFieldType type)
    : m_type(type)
  {
  }

  const IWORKFieldType m_type;
};

struct OpenLink
{
  explicit OpenLink(const std::string &url)
    : m_url(url)
  {
  }

  const std::string m_url;
};

struct CloseLink
{
};

struct InsertText
{
  explicit InsertText(const std::string &text)
    : m_text(text)
  {
  }

  const std::string m_text;
};

struct InsertTab
{
};

struct InsertSpace
{
};

struct InsertBreak
{
  explicit InsertBreak(IWORKBreakType type)
    : m_type(type)
  {
  }
  IWORKBreakType m_type;
};

typedef boost::variant
< PushBaseLayoutStyle
, PushBaseParagraphStyle
, SetLayoutStyle
, FlushLayout
, SetListStyle
, SetListLevel
, FlushList
, SetParagraphStyle
, FlushParagraph
, SetSpanStyle
, SetLanguage
, FlushSpan
, InsertField
, OpenLink
, CloseLink
, InsertText
, InsertTab
, InsertSpace
, InsertBreak
>
Element_t;

}

namespace
{

struct Sender : public boost::static_visitor<void>
{
  explicit Sender(IWORKText &text)
    : m_text(text)
  {
  }

  void operator()(const PushBaseLayoutStyle &value) const
  {
    m_text.pushBaseLayoutStyle(value.m_style);
  }

  void operator()(const PushBaseParagraphStyle &value) const
  {
    m_text.pushBaseParagraphStyle(value.m_style);
  }

  void operator()(const SetLayoutStyle &value) const
  {
    m_text.setLayoutStyle(value.m_style);
  }

  void operator()(const FlushLayout &) const
  {
    m_text.flushLayout();
  }

  void operator()(const SetListStyle &value) const
  {
    m_text.setListStyle(value.m_style);
  }

  void operator()(const SetListLevel &value) const
  {
    m_text.setListLevel(value.m_level);
  }

  void operator()(const FlushList &) const
  {
    m_text.flushList();
  }

  void operator()(const SetParagraphStyle &value) const
  {
    m_text.setParagraphStyle(value.m_style);
  }

  void operator()(const FlushParagraph &) const
  {
    m_text.flushParagraph();
  }

  void operator()(const SetSpanStyle &value) const
  {
    m_text.setSpanStyle(value.m_style);
  }

  void operator()(const SetLanguage &value) const
  {
    m_text.setLanguage(value.m_style);
  }

  void operator()(const FlushSpan &) const
  {
    m_text.flushSpan();
  }

  void operator()(const InsertField &value) const
  {
    m_text.insertField(value.m_type);
  }

  void operator()(const OpenLink &value) const
  {
    m_text.openLink(value.m_url);
  }

  void operator()(const CloseLink &) const
  {
    m_text.closeLink();
  }

  void operator()(const InsertText &value) const
  {
    m_text.insertText(value.m_text);
  }

  void operator()(const InsertTab &) const
  {
    m_text.insertTab();
  }

  void operator()(const InsertSpace &) const
  {
    m_text.insertSpace();
  }

  void operator()(const InsertBreak &value) const
  {
    switch (value.m_type)
    {
    case IWORK_BREAK_NONE :
      break;
    case IWORK_BREAK_COLUMN:
      m_text.insertColumnBreak();
      break;
    case IWORK_BREAK_LINE :
      m_text.insertLineBreak();
      break;
    case IWORK_BREAK_PAGE:
      m_text.insertPageBreak();
      break;
    default:
      ETONYEK_DEBUG_MSG(("Sender::operator(InsertBreak)[IWORKTextRecorder.cpp]: unexpected break\n"));
      break;
    }
  }

private:
  IWORKText &m_text;
};

}

struct IWORKTextRecorder::Impl
{
  Impl();

  std::deque<Element_t> m_elements;
};

IWORKTextRecorder::Impl::Impl()
  : m_elements()
{
}

IWORKTextRecorder::IWORKTextRecorder()
  : m_impl(new Impl())
{
}

void IWORKTextRecorder::replay(IWORKText &text) const
{
  Sender sender(text);
  for (std::deque<Element_t>::const_iterator it = m_impl->m_elements.begin(); it != m_impl->m_elements.end(); ++it)
    boost::apply_visitor(sender, *it);
}

void IWORKTextRecorder::pushBaseLayoutStyle(const IWORKStylePtr_t &style)
{
  m_impl->m_elements.push_back(PushBaseLayoutStyle(style));
}

void IWORKTextRecorder::pushBaseParagraphStyle(const IWORKStylePtr_t &style)
{
  m_impl->m_elements.push_back(PushBaseParagraphStyle(style));
}

void IWORKTextRecorder::setLayoutStyle(const IWORKStylePtr_t &style)
{
  m_impl->m_elements.push_back(SetLayoutStyle(style));
}

void IWORKTextRecorder::flushLayout()
{
  m_impl->m_elements.push_back(FlushLayout());
}

void IWORKTextRecorder::setListStyle(const IWORKStylePtr_t &style)
{
  m_impl->m_elements.push_back(SetListStyle(style));
}

void IWORKTextRecorder::setListLevel(const unsigned level)
{
  m_impl->m_elements.push_back(SetListLevel(level));
}

void IWORKTextRecorder::flushList()
{
  m_impl->m_elements.push_back(FlushList());
}

void IWORKTextRecorder::setParagraphStyle(const IWORKStylePtr_t &style)
{
  m_impl->m_elements.push_back(SetParagraphStyle(style));
}

void IWORKTextRecorder::flushParagraph()
{
  m_impl->m_elements.push_back(FlushParagraph());
}

void IWORKTextRecorder::setSpanStyle(const IWORKStylePtr_t &style)
{
  m_impl->m_elements.push_back(SetSpanStyle(style));
}

void IWORKTextRecorder::setLanguage(const IWORKStylePtr_t &style)
{
  m_impl->m_elements.push_back(SetLanguage(style));
}

void IWORKTextRecorder::flushSpan()
{
  m_impl->m_elements.push_back(FlushSpan());
}

void IWORKTextRecorder::insertField(IWORKFieldType type)
{
  m_impl->m_elements.push_back(InsertField(type));
}

void IWORKTextRecorder::openLink(const std::string &url)
{
  m_impl->m_elements.push_back(OpenLink(url));
}

void IWORKTextRecorder::closeLink()
{
  m_impl->m_elements.push_back(CloseLink());
}

void IWORKTextRecorder::insertText(const std::string &text)
{
  m_impl->m_elements.push_back(InsertText(text));
}

void IWORKTextRecorder::insertTab()
{
  m_impl->m_elements.push_back(InsertTab());
}

void IWORKTextRecorder::insertSpace()
{
  m_impl->m_elements.push_back(InsertSpace());
}

void IWORKTextRecorder::insertColumnBreak()
{
  m_impl->m_elements.push_back(InsertBreak(IWORK_BREAK_COLUMN));
}

void IWORKTextRecorder::insertLineBreak()
{
  m_impl->m_elements.push_back(InsertBreak(IWORK_BREAK_LINE));
}

void IWORKTextRecorder::insertPageBreak()
{
  m_impl->m_elements.push_back(InsertBreak(IWORK_BREAK_PAGE));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
