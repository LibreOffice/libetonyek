/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTextBodyElement.h"

#include <boost/optional.hpp>

#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKStyle.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

namespace
{

class BrContext : public IWORKXMLEmptyContextBase
{
public:
  explicit BrContext(IWORKXMLParserState &state);

private:
  virtual void endOfElement();
};

BrContext::BrContext(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

void BrContext::endOfElement()
{
  getCollector()->collectLineBreak();
}

}

namespace
{

class TabElement : public IWORKXMLEmptyContextBase
{
public:
  explicit TabElement(IWORKXMLParserState &state);

private:
  virtual void endOfElement();
};

TabElement::TabElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

void TabElement::endOfElement()
{
  getCollector()->collectTab();
}

}

namespace
{

class SpanElement : public IWORKXMLElementContextBase
{
public:
  explicit SpanElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void text(const char *value);

private:
  boost::optional<ID_t> m_style;
};

SpanElement::SpanElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void SpanElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::style :
    m_style = value;
    break;
  }
}

IWORKXMLContextPtr_t SpanElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::br :
  case IWORKToken::NS_URI_SF | IWORKToken::crbr :
  case IWORKToken::NS_URI_SF | IWORKToken::intratopicbr :
  case IWORKToken::NS_URI_SF | IWORKToken::lnbr :
    return makeContext<BrContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tab :
    return makeContext<TabElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void SpanElement::text(const char *const value)
{
  IWORKStylePtr_t style;
  if (m_style)
  {
    const IWORKStyleMap_t::const_iterator it = getState().getDictionary().m_characterStyles.find(get(m_style));
    if (getState().getDictionary().m_characterStyles.end() != it)
      style = it->second;
  }
  getCollector()->collectText(style, value);
}

}

namespace
{

class LinkElement : public IWORKXMLMixedContextBase
{
public:
  explicit LinkElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void text(const char *value);
};

LinkElement::LinkElement(IWORKXMLParserState &state)
  : IWORKXMLMixedContextBase(state)
{
}

void LinkElement::attribute(int, const char *)
{
}

IWORKXMLContextPtr_t LinkElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::br :
    return makeContext<BrContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::span :
    return makeContext<SpanElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void LinkElement::text(const char *const value)
{
  getCollector()->collectText(IWORKStylePtr_t(), value);
}

}

namespace
{

class PElement : public IWORKXMLMixedContextBase
{
public:
  explicit PElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
  virtual void text(const char *value);

private:
  void ensureOpened();

private:
  IWORKStylePtr_t m_style;
  bool m_opened;
};

PElement::PElement(IWORKXMLParserState &state)
  : IWORKXMLMixedContextBase(state)
  , m_style()
  , m_opened(false)
{
}

void PElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::style :
  {
    const IWORKStyleMap_t::const_iterator it = getState().getDictionary().m_paragraphStyles.find(value);
    if (getState().getDictionary().m_paragraphStyles.end() != it)
      m_style = it->second;
    break;
  }
  }
}

IWORKXMLContextPtr_t PElement::element(const int name)
{
  ensureOpened();

  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::br :
  case IWORKToken::NS_URI_SF | IWORKToken::crbr :
  case IWORKToken::NS_URI_SF | IWORKToken::intratopicbr :
  case IWORKToken::NS_URI_SF | IWORKToken::lnbr :
    return makeContext<BrContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::span :
    return makeContext<SpanElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tab :
    return makeContext<TabElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::link :
    return makeContext<LinkElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void PElement::text(const char *const value)
{
  ensureOpened();
  getCollector()->collectText(m_style, value);
}

void PElement::endOfElement()
{
  ensureOpened();
  getCollector()->endParagraph();
}

void PElement::ensureOpened()
{
  if (!m_opened)
  {
    getCollector()->startParagraph(m_style);
    m_opened = true;
  }
}

}

namespace
{

class LayoutElement : public IWORKXMLElementContextBase
{
public:
  explicit LayoutElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
};

LayoutElement::LayoutElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void LayoutElement::attribute(const int name, const char *)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::style) == name)
  {
    // TODO: fetch the style
    getCollector()->collectStyle(IWORKStylePtr_t(), false);
  }
}

IWORKXMLContextPtr_t LayoutElement::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::p) == name)
    return makeContext<PElement>(getState());

  return IWORKXMLContextPtr_t();
}

}

IWORKTextBodyElement::IWORKTextBodyElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_layout(false)
  , m_para(false)
{
}

IWORKXMLContextPtr_t IWORKTextBodyElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layout :
    if (m_layout || m_para)
    {
      ETONYEK_DEBUG_MSG(("layout following another element, not allowed, skipping\n"));
    }
    else
    {
      m_layout = true;
      return makeContext<LayoutElement>(getState());
    }
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::p :
    if (m_layout)
    {
      ETONYEK_DEBUG_MSG(("paragraph following layout, not allowed, skipping\n"));
    }
    else if (m_para)
    {
      return makeContext<PElement>(getState());
    }
    else
    {
      m_para = true;
      return makeContext<PElement>(getState());
    }
    break;
  }

  return IWORKXMLContextPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
