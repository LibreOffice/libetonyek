/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKPElement.h"

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
  virtual void endOfElement();

  void ensureOpened();

private:
  IWORKStylePtr_t m_style;
  bool m_opened;
};

SpanElement::SpanElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_style()
  , m_opened(false)
{
}

void SpanElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::style :
  {
    const IWORKStyleMap_t::const_iterator it = getState().getDictionary().m_characterStyles.find(value);
    if (getState().getDictionary().m_characterStyles.end() != it)
      m_style = it->second;
    break;
  }
  }
}

IWORKXMLContextPtr_t SpanElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::crbr :
  case IWORKToken::NS_URI_SF | IWORKToken::intratopicbr :
  case IWORKToken::NS_URI_SF | IWORKToken::lnbr :
    ensureOpened();
    return makeContext<BrContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tab :
    ensureOpened();
    return makeContext<TabElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void SpanElement::text(const char *const value)
{
  ensureOpened();
  getCollector()->collectText(value);
}

void SpanElement::endOfElement()
{
  if (m_opened)
    getCollector()->closeSpan();
}

void SpanElement::ensureOpened()
{
  if (!m_opened)
  {
    getCollector()->openSpan(m_style);
    m_opened = true;
  }
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
  virtual void endOfElement();

private:
  bool m_opened;
};

LinkElement::LinkElement(IWORKXMLParserState &state)
  : IWORKXMLMixedContextBase(state)
  , m_opened(false)
{
}

void LinkElement::attribute(const int name, const char *const value)
{
  if (IWORKToken::href == name)
  {
    getCollector()->openLink(value);
    m_opened = true;
  }
}

IWORKXMLContextPtr_t LinkElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::span :
    return makeContext<SpanElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void LinkElement::text(const char *const value)
{
  getCollector()->collectText(value);
}

void LinkElement::endOfElement()
{
  if (m_opened)
    getCollector()->closeLink();
}

}

IWORKPElement::IWORKPElement(IWORKXMLParserState &state)
  : IWORKXMLMixedContextBase(state)
  , m_style()
  , m_opened(false)
{
}

void IWORKPElement::attribute(const int name, const char *const value)
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

IWORKXMLContextPtr_t IWORKPElement::element(const int name)
{
  ensureOpened();

  switch (name)
  {
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

void IWORKPElement::text(const char *const value)
{
  ensureOpened();
  getCollector()->collectText(value);
}

void IWORKPElement::endOfElement()
{
  ensureOpened();
  getCollector()->endParagraph();
}

void IWORKPElement::ensureOpened()
{
  if (!m_opened)
  {
    getCollector()->startParagraph(m_style);
    m_opened = true;
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
