/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include "KN2Token.h"
#include "KNXMLAttributeIterator.h"

namespace libkeynote
{

namespace
{

struct XMLAttributeListException {};

}

KNXMLAttribute::KNXMLAttribute()
  : name(0)
  , ns(0)
  , value(0)
{
}

KNXMLAttributeIterator::KNXMLAttributeIterator(xmlTextReaderPtr reader)
  : m_reader(reader)
  , m_current()
  , m_first(true)
  , m_last(false)
{
  assert(XML_READER_TYPE_ELEMENT == xmlTextReaderNodeType(reader));

  switch (xmlTextReaderHasAttributes(reader))
  {
  case 0 :
    // no attributes
    m_last = true;
    break;
  case 1 :
    break;
  default :
    throw XMLAttributeListException();
  }
}

bool KNXMLAttributeIterator::next()
{
  if (m_last)
    return false;

  while (move() && !test())
    ;

  if (!m_last)
    read();

  return !m_last;
}

bool KNXMLAttributeIterator::last() const
{
  return m_last;
}

const KNXMLAttribute &KNXMLAttributeIterator::operator*() const
{
  return m_current;
}

const KNXMLAttribute *KNXMLAttributeIterator::operator->() const
{
  return &m_current;
}

bool KNXMLAttributeIterator::move()
{
  int ret = 0;
  if (m_first)
  {
    ret = xmlTextReaderMoveToFirstAttribute(m_reader);
    m_first = false;
  }
  else
    ret = xmlTextReaderMoveToNextAttribute(m_reader);

  switch (ret)
  {
  case 0 :
    m_last = true;
    break;
  case 1 :
    break;
  default :
    throw XMLAttributeListException();
  }

  return !m_last;
}

void KNXMLAttributeIterator::read()
{
  m_current.name = xmlTextReaderConstLocalName(m_reader);
  m_current.ns = xmlTextReaderConstNamespaceUri(m_reader);
  m_current.value = xmlTextReaderConstValue(m_reader);
}

bool KNXMLAttributeIterator::test()
{
  const xmlChar *const prefix = xmlTextReaderConstPrefix(m_reader);
  if (prefix)
    return KN2Token::xmlns != getKN2TokenID(prefix);

  return KN2Token::xmlns != getKN2TokenID(xmlTextReaderConstLocalName(m_reader));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
