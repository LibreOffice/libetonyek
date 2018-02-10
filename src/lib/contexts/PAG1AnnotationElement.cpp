/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAG1AnnotationElement.h"

#include "libetonyek_xml.h"
#include "IWORKToken.h"
#include "PAGCollector.h"
#include "PAG1ParserState.h"

namespace libetonyek
{

PAG1AnnotationElement::PAG1AnnotationElement(PAG1ParserState &state, IWORKXMLContext &container, bool isRef)
  : PAG1XMLElementContextBase(state)
  , m_container(container)
  , m_isRef(isRef)
  , m_id()
{
}

void PAG1AnnotationElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::ID : // annotation
    m_id = value;
    break;
  default:
    break;
  }

}

IWORKXMLContextPtr_t PAG1AnnotationElement::element(const int name)
{
  return m_container.element(name);
}

void PAG1AnnotationElement::text(const char *const value)
{
  return m_container.text(value);
}

void PAG1AnnotationElement::endOfElement()
{
  if (m_isRef || !bool(getState().m_currentText))
    return;
  if (!m_id)
  {
    ETONYEK_DEBUG_MSG(("PAG1AnnotationElement::endOfElement: oops call without id\n"));
  }
  else if (isCollector())
  {
    getCollector().collectText(getState().m_currentText);
    getCollector().sendAnnotation(get(m_id));
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
