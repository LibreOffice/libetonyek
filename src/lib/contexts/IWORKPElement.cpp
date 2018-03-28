/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKPElement.h"

#include "IWORKBrContext.h"
#include "IWORKDictionary.h"
#include "IWORKFieldElement.h"
#include "IWORKLinkElement.h"
#include "IWORKProperties.h"
#include "IWORKSpanElement.h"
#include "IWORKStyle.h"
#include "IWORKTabElement.h"
#include "IWORKText.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"
#include "libetonyek_xml.h"
namespace libetonyek
{

namespace
{
// cell format ?
class GhostTextElement : public IWORKXMLEmptyContextBase
{
public:
  explicit GhostTextElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void text(const char *text) override;
};

GhostTextElement::GhostTextElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

void GhostTextElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::localize | IWORKToken::NS_URI_SF : // a bool
    break;
  case IWORKToken::ID | IWORKToken::NS_URI_SFA :
  case IWORKToken::IDREF | IWORKToken::NS_URI_SFA :
    /* storeme? */
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  default :
    ETONYEK_DEBUG_MSG(("GhostTextElement::attribute[IWORKPElement.cpp]: find some unknown attribute\n"));
    IWORKXMLEmptyContextBase::attribute(name, value);
  }
}

IWORKXMLContextPtr_t GhostTextElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::br : // ok to ignore?
    return IWORKXMLContextPtr_t();
  case IWORKToken::NS_URI_SF | IWORKToken::crbr :
  case IWORKToken::NS_URI_SF | IWORKToken::intratopicbr :
  case IWORKToken::NS_URI_SF | IWORKToken::lnbr :
    return std::make_shared<IWORKBrContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::pgbr :
    return IWORKXMLContextPtr_t();
  case IWORKToken::NS_URI_SF | IWORKToken::span :
    return std::make_shared<IWORKSpanElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tab :
    return std::make_shared<IWORKTabElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::link :
    return std::make_shared<IWORKLinkElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::date_time :
    return std::make_shared<IWORKFieldElement>(getState(),IWORK_FIELD_DATETIME);
  case IWORKToken::NS_URI_SF | IWORKToken::filename :
    return std::make_shared<IWORKFieldElement>(getState(),IWORK_FIELD_FILENAME);
  case IWORKToken::NS_URI_SF | IWORKToken::page_count :
    return std::make_shared<IWORKFieldElement>(getState(),IWORK_FIELD_PAGECOUNT);
  case IWORKToken::NS_URI_SF | IWORKToken::page_number :
    return std::make_shared<IWORKFieldElement>(getState(),IWORK_FIELD_PAGENUMBER);
  default:
    ETONYEK_DEBUG_MSG(("GhostTextElement::element[IWORKPElement.cpp]: find unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void GhostTextElement::text(const char *const value)
{
  if (bool(getState().m_currentText))
    getState().m_currentText->insertText(value);
}
}
}

namespace libetonyek
{

IWORKPElement::IWORKPElement(IWORKXMLParserState &state)
  : IWORKXMLMixedContextBase(state)
  , m_style()
  , m_opened(false)
  , m_delayedPageBreak(false)
  , m_listLevel()
  , m_restartList()
{
}

void IWORKPElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::list_level :
    m_listLevel = try_int_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::restart_list :
    m_restartList = try_bool_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::style :
    m_style = getState().getStyleByName(value, getState().getDictionary().m_paragraphStyles);
    break;
  default:
    ETONYEK_DEBUG_MSG(("IWORKPElement::attribute: find unknown attributes\n"));
    break;
  }
}

IWORKXMLContextPtr_t IWORKPElement::element(const int name)
{
  ensureOpened();

  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::br : // ok to ignore?
    return IWORKXMLContextPtr_t();
  case IWORKToken::NS_URI_SF | IWORKToken::crbr :
  case IWORKToken::NS_URI_SF | IWORKToken::intratopicbr :
  case IWORKToken::NS_URI_SF | IWORKToken::lnbr :
    return std::make_shared<IWORKBrContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::pgbr :
    m_delayedPageBreak=true;
    return IWORKXMLContextPtr_t();
  case IWORKToken::NS_URI_SF | IWORKToken::span :
    return std::make_shared<IWORKSpanElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tab :
    return std::make_shared<IWORKTabElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::link :
    return std::make_shared<IWORKLinkElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::date_time :
    return std::make_shared<IWORKFieldElement>(getState(),IWORK_FIELD_DATETIME);
  case IWORKToken::NS_URI_SF | IWORKToken::filename :
    return std::make_shared<IWORKFieldElement>(getState(),IWORK_FIELD_FILENAME);
  case IWORKToken::NS_URI_SF | IWORKToken::ghost_text :
  case IWORKToken::NS_URI_SF | IWORKToken::ghost_text_ref :
    /* checkme: sf:ghost-text and sf:ghost-text-ref seems similar, but maybe
       sf:ghost-text-ref can also be called without the text data...
     */
    return std::make_shared<GhostTextElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::page_count :
    return std::make_shared<IWORKFieldElement>(getState(),IWORK_FIELD_PAGECOUNT);
  case IWORKToken::NS_URI_SF | IWORKToken::page_number :
    return std::make_shared<IWORKFieldElement>(getState(),IWORK_FIELD_PAGENUMBER);
  default:
    ETONYEK_DEBUG_MSG(("IWORKPElement::element: find unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void IWORKPElement::text(const char *const value)
{
  ensureOpened();
  if (bool(getState().m_currentText))
    getState().m_currentText->insertText(value);
}

void IWORKPElement::endOfElement()
{
  ensureOpened();
  if (bool(getState().m_currentText))
  {
    getState().m_currentText->flushParagraph();
    if (m_delayedPageBreak)
      getState().m_currentText->insertPageBreak();
  }
}

void IWORKPElement::ensureOpened()
{
  if (!m_opened)
  {
    if (bool(getState().m_currentText))
    {
      if (m_restartList && get(m_restartList))
        getState().m_currentText->flushList();
      getState().m_currentText->setParagraphStyle(m_style);
      if (m_listLevel)
        getState().m_currentText->setListLevel(get(m_listLevel));
      if (m_style && m_style->has<property::ListStyle>())
        getState().m_currentText->setListStyle(m_style->get<property::ListStyle>());
      else
        getState().m_currentText->setListStyle(IWORKStylePtr_t());
    }
    m_opened = true;
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
