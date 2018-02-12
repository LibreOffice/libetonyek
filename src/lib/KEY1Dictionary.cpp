/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY1Dictionary.h"

namespace libetonyek
{

KEY1Dictionary::KEY1Dictionary()
  : IWORKDictionary()
  , m_masterSlides()
  , m_slides()
  , m_styleContexts()
  , m_styleContextsMap()
{
  m_styleContexts.push_back(StylesContext());
}

KEY1Dictionary::StylesContext &KEY1Dictionary::getCurrentStylesContext()
{
  return m_styleContexts.back();
}

void KEY1Dictionary::pushStylesContext()
{
  m_styleContexts.push_back(StylesContext());
}

void KEY1Dictionary::popStylesContext()
{
  if (m_styleContexts.size()>1)
    m_styleContexts.pop_back();
  else
  {
    ETONYEK_DEBUG_MSG(("KEY1Dictionary::popCurrentStylesContext: oops style contexts must not be empty\n"));
  }
}

void KEY1Dictionary::collectStylesContext(const std::string &contextName)
{
  if (m_styleContexts.size()>1)
    m_styleContextsMap.insert(StylesContextMap_t::value_type(contextName,m_styleContexts.back()));
  else
  {
    ETONYEK_DEBUG_MSG(("KEY1Dictionary::popCurrentStylesContext: oops style contexts must not be empty\n"));
  }
}

void KEY1Dictionary::linkStylesContext(const boost::optional<std::string> &master)
{
  if (!master)
    getCurrentStylesContext().link(m_styleContexts.front());
  else if (m_styleContextsMap.find(get(master))!=m_styleContextsMap.end())
    getCurrentStylesContext().link(m_styleContextsMap.find(get(master))->second);
  else
  {
    ETONYEK_DEBUG_MSG(("KEY1Dictionary::linkStylesContext: oops can not find context %s\n", get(master).c_str()));
    getCurrentStylesContext().link(m_styleContexts.front());
  }
}

void KEY1Dictionary::storeImageStyle(IWORKStylePtr_t style, bool definition)
{
  getCurrentStylesContext().m_imageContext.storeStyle(style, definition);
}

void KEY1Dictionary::storeLineStyle(IWORKStylePtr_t style, bool definition)
{
  getCurrentStylesContext().m_lineContext.storeStyle(style, definition);
}

void KEY1Dictionary::storeShapeStyle(IWORKStylePtr_t style, bool definition)
{
  getCurrentStylesContext().m_shapeContext.storeStyle(style, definition);
}

void KEY1Dictionary::storeTextboxStyle(IWORKStylePtr_t style, bool definition)
{
  getCurrentStylesContext().m_textboxContext.storeStyle(style, definition);
}

KEYPlaceholderPtr_t KEY1Dictionary::getBodyPlaceholder()
{
  return getCurrentStylesContext().m_bodyPlaceholder;
}

KEYPlaceholderPtr_t KEY1Dictionary::getPageNumberPlaceholder()
{
  return getCurrentStylesContext().m_pageNumberPlaceholder;
}

KEYPlaceholderPtr_t KEY1Dictionary::getTitlePlaceholder()
{
  return getCurrentStylesContext().m_titlePlaceholder;
}

void KEY1Dictionary::storeBodyPlaceholder(KEYPlaceholderPtr_t placeHolder)
{
  getCurrentStylesContext().m_bodyPlaceholder=placeHolder;
}

void KEY1Dictionary::storePageNumberPlaceholder(KEYPlaceholderPtr_t placeHolder)
{
  getCurrentStylesContext().m_pageNumberPlaceholder=placeHolder;
}

void KEY1Dictionary::storeTitlePlaceholder(KEYPlaceholderPtr_t placeHolder)
{
  getCurrentStylesContext().m_titlePlaceholder=placeHolder;
}

KEY1Dictionary::StyleContext::StyleContext()
  : m_style()
  , m_styles()
{
}

void KEY1Dictionary::StyleContext::storeStyle(IWORKStylePtr_t style, bool definition)
{
  if (!style) return;
  if (definition)
    m_style=style;
  else
    m_styles.push_back(style);
}

void KEY1Dictionary::StyleContext::link(const KEY1Dictionary::StyleContext &parentContext)
{
  if (m_style)
    m_style->setParent(parentContext.m_style);
  else
    m_style=parentContext.m_style;
  if (m_style)
  {
    for (std::deque<IWORKStylePtr_t>::const_iterator it=m_styles.begin(); it!=m_styles.end(); ++it)
      (*it)->setParent(m_style);
  }
  m_styles.clear();
}

KEY1Dictionary::StylesContext::StylesContext()
  : m_imageContext()
  , m_lineContext()
  , m_shapeContext()
  , m_textboxContext()
  , m_bodyPlaceholder()
  , m_pageNumberPlaceholder()
  , m_titlePlaceholder()
{
}

void KEY1Dictionary::StylesContext::link(const KEY1Dictionary::StylesContext &parentContext)
{
  m_imageContext.link(parentContext.m_imageContext);
  m_lineContext.link(parentContext.m_lineContext);
  m_shapeContext.link(parentContext.m_shapeContext);
  m_textboxContext.link(parentContext.m_textboxContext);

  link(m_bodyPlaceholder, parentContext.m_bodyPlaceholder);
  link(m_pageNumberPlaceholder, parentContext.m_pageNumberPlaceholder);
  link(m_titlePlaceholder, parentContext.m_titlePlaceholder);
}

void KEY1Dictionary::StylesContext::link(KEYPlaceholderPtr_t &placeholder, const KEYPlaceholderPtr_t &parentHolder)
{
  if (!parentHolder)
    return;
  if (!placeholder)
  {
    placeholder=parentHolder;
    return;
  }
  if (!placeholder->m_geometry)
    placeholder->m_geometry=parentHolder->m_geometry;
  if (!placeholder->m_text)
    placeholder->m_text=parentHolder->m_text;
  if (!placeholder->m_visible)
    placeholder->m_visible=parentHolder->m_visible;
  if (placeholder->m_bulletIndentations.empty())
    placeholder->m_bulletIndentations=parentHolder->m_bulletIndentations;
  if (placeholder->m_style)
    placeholder->m_style->setParent(parentHolder->m_style);
  else
    placeholder->m_style=parentHolder->m_style;
  if (placeholder->m_paragraphStyle)
    placeholder->m_paragraphStyle->setParent(parentHolder->m_paragraphStyle);
  else
    placeholder->m_paragraphStyle=parentHolder->m_paragraphStyle;
  if (placeholder->m_spanStyle)
    placeholder->m_spanStyle->setParent(parentHolder->m_spanStyle);
  else
    placeholder->m_spanStyle=parentHolder->m_spanStyle;
}
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
