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

#include "IWORKText.h"

namespace libetonyek
{

namespace
{

struct PushBaseLayoutStyle
{
  PushBaseLayoutStyle(const IWORKStylePtr_t &style)
    : m_style(style)
  {
  }

  const IWORKStylePtr_t m_style;
};

struct PushBaseParagraphStyle
{
  PushBaseParagraphStyle(const IWORKStylePtr_t &style)
    : m_style(style)
  {
  }

  const IWORKStylePtr_t m_style;
};

struct SetLayoutStyle
{
  SetLayoutStyle(const IWORKStylePtr_t &style)
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
  SetListStyle(const IWORKStylePtr_t &style)
    : m_style(style)
  {
  }

  const IWORKStylePtr_t m_style;
};

struct SetListLevel
{
  SetListLevel(const unsigned level)
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
  SetParagraphStyle(const IWORKStylePtr_t &style)
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
  SetSpanStyle(const IWORKStylePtr_t &style)
    : m_style(style)
  {
  }

  const IWORKStylePtr_t m_style;
};

struct SetLanguage
{
  SetLanguage(const IWORKStylePtr_t &style)
    : m_style(style)
  {
  }

  const IWORKStylePtr_t m_style;
};

struct FlushSpan
{
};

struct OpenLink
{
  OpenLink(const std::string &url)
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
  InsertText(const std::string &text)
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

struct InsertLineBreak
{
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
, OpenLink
, CloseLink
, InsertText
, InsertTab
, InsertSpace
, InsertLineBreak
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

  void operator()(const InsertLineBreak &) const
  {
    m_text.insertLineBreak();
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

void IWORKTextRecorder::insertLineBreak()
{
  m_impl->m_elements.push_back(InsertLineBreak());
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
