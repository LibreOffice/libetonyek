/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAG1AnnotationContext.h"

#include <cassert>

#include "PAGCollector.h"
#include "IWORKTextElement.h"
#include "IWORKToken.h"
#include "PAG1ParserState.h"

namespace libetonyek
{

PAG1AnnotationContext::PAG1AnnotationContext(PAG1ParserState &state, const CollectFunction_t &collect)
  : PAG1XMLElementContextBase(state)
  , m_collect(collect)
  , m_target()
{
}

void PAG1AnnotationContext::startOfElement()
{
  if (isCollector())
  {
    assert(!getState().m_currentText);
    getState().m_currentText = getCollector().createText(getState().m_langManager, true);
  }
}

void PAG1AnnotationContext::attribute(const int name, const char *const value)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::target))
    m_target = value;
  else
    PAG1XMLElementContextBase::attribute(name, value);
}

IWORKXMLContextPtr_t PAG1AnnotationContext::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::text))
    return makeContext<IWORKTextElement>(getState());
  return IWORKXMLContextPtr_t();
}

void PAG1AnnotationContext::endOfElement()
{
  if (isCollector())
  {
    getCollector().collectText(getState().m_currentText);
    getState().m_currentText.reset();
    if (m_target)
      m_collect(get(m_target));
    else
    {
      ETONYEK_DEBUG_MSG(("PAG1AnnotationContext::endOfElement: called without target\n"));
    }
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
