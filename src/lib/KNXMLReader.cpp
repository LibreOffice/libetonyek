/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include <libwpd-stream/libwpd-stream.h>

#include <libxml/xmlreader.h>

#include "KNXMLReader.h"

namespace libkeynote
{

namespace
{

extern "C" int readFromStream(void *context, char *buffer, int len)
{
  try
  {
    WPXInputStream *const input = reinterpret_cast<WPXInputStream *>(context);

    unsigned long bytesRead = 0;
    const unsigned char *const bytes = input->read(len, bytesRead);

    std::memcpy(buffer, bytes, static_cast<int>(bytesRead));
    return static_cast<int>(bytesRead);
  }
  catch (...)
  {
  }

  return -1;
}

extern "C" int closeStream(void * /* context */)
{
  return 0;
}

}

namespace
{

struct XMLException {};

}

struct KNXMLReader::Impl
{
  xmlTextReaderPtr reader;
  TokenizerFunction_t tokenizer;

  Impl();
  ~Impl();
};

KNXMLReader::Impl::Impl()
  : reader(0)
  , tokenizer()
{
}

KNXMLReader::Impl::~Impl()
{
  if (0 != reader)
  {
    xmlTextReaderClose(reader);
    xmlFreeTextReader(reader);
  }
}

KNXMLReader::NodeIterator::NodeIterator(const KNXMLReader &reader, const int types)
  : m_impl(reader.m_impl)
  , m_types(types)
  , m_level(0)
  , m_last(false)
{
  if (-1 == xmlTextReaderMoveToElement(m_impl->reader))
    throw XMLException();
  assert(XML_READER_TYPE_ELEMENT == xmlTextReaderNodeType(m_impl->reader));

  m_level = xmlTextReaderDepth(m_impl->reader);
  if (-1 == m_level)
    throw XMLException();

  m_last = xmlTextReaderIsEmptyElement(m_impl->reader);
}

bool KNXMLReader::NodeIterator::next()
{
  if (m_last)
    return false;

  do
  {
    const int ret = xmlTextReaderRead(m_impl->reader);
    if (ret == -1)
      throw XMLException();
    else if (ret == 0)
      return false;
  }
  while (!test());

  if (XML_READER_TYPE_END_ELEMENT == xmlTextReaderNodeType(m_impl->reader))
  {
    m_last = true;
    assert(m_level == xmlTextReaderDepth(m_impl->reader));
  }

  return !m_last;
}

KNXMLReader::Impl *KNXMLReader::NodeIterator::getImpl() const
{
  return m_impl;
}

bool KNXMLReader::NodeIterator::test() const
{
  const int type = xmlTextReaderNodeType(m_impl->reader);

  if ((TYPE_ELEMENT & m_types) && ((XML_READER_TYPE_ELEMENT == type) || (XML_READER_TYPE_END_ELEMENT == type)))
    return true;
  if ((TYPE_TEXT & m_types) && (XML_READER_TYPE_TEXT == type))
    return true;

  return false;
}

KNXMLReader::AttributeIterator::AttributeIterator(const KNXMLReader &reader)
  : m_impl(reader.m_impl)
  , m_first(true)
  , m_last(false)
{
  assert(XML_READER_TYPE_ELEMENT == xmlTextReaderNodeType(m_impl->reader));

  const int ret = xmlTextReaderHasAttributes(m_impl->reader);
  if (0 == ret) // no attributes
    m_last = true;
  else if (-1 == ret)
    throw XMLException();
}

bool KNXMLReader::AttributeIterator::next()
{
  if (m_last)
    return false;

  while (move() && !test())
    ;

  return !m_last;
}

bool KNXMLReader::AttributeIterator::move()
{
  int ret = 0;
  if (m_first)
  {
    ret = xmlTextReaderMoveToFirstAttribute(m_impl->reader);
    m_first = false;
  }
  else
    ret = xmlTextReaderMoveToNextAttribute(m_impl->reader);

  if (0 == ret)
    m_last = true;
  else if (-1 == ret)
    throw XMLException();

  return !m_last;
}

bool KNXMLReader::AttributeIterator::test()
{
  const xmlChar *const prefix = xmlTextReaderConstPrefix(m_impl->reader);
  if (prefix)
    return strcmp("xmlns", reinterpret_cast<const char *>(prefix));

  const xmlChar *const name = xmlTextReaderConstLocalName(m_impl->reader);
  return strcmp("xmlns", reinterpret_cast<const char *>(name));
}

const char *KNXMLReader::AttributeIterator::getName() const
{
  return reinterpret_cast<const char *>(xmlTextReaderConstLocalName(m_impl->reader));
}

const char *KNXMLReader::AttributeIterator::getNamespace() const
{
  return reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(m_impl->reader));
}

const char *KNXMLReader::AttributeIterator::getValue() const
{
  return reinterpret_cast<const char *>(xmlTextReaderConstValue(m_impl->reader));
}

int KNXMLReader::AttributeIterator::getToken(const char *const token) const
{
  return m_impl->tokenizer(token);
}


KNXMLReader::ElementIterator::ElementIterator(const KNXMLReader &reader)
  : m_iterator(reader, TYPE_ELEMENT)
{
}

bool KNXMLReader::ElementIterator::next()
{
  return m_iterator.next();
}

const char *KNXMLReader::ElementIterator::getName() const
{
  return reinterpret_cast<const char *>(xmlTextReaderConstLocalName(m_iterator.getImpl()->reader));
}

const char *KNXMLReader::ElementIterator::getNamespace() const
{
  return reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(m_iterator.getImpl()->reader));
}

const char *KNXMLReader::ElementIterator::getText() const
{
  return reinterpret_cast<const char *>(xmlTextReaderConstValue(m_iterator.getImpl()->reader));
}

bool KNXMLReader::ElementIterator::isEmpty() const
{
  return xmlTextReaderIsEmptyElement(m_iterator.getImpl()->reader);
}

const KNXMLReader::NodeIterator &KNXMLReader::ElementIterator::getNodeIterator() const
{
  return m_iterator;
}

int KNXMLReader::ElementIterator::getToken(const char *token) const
{
  return m_iterator.getImpl()->tokenizer(token);
}

KNXMLReader::MixedIterator::MixedIterator(const KNXMLReader &reader)
  : m_iterator(reader, TYPE_ELEMENT | TYPE_TEXT)
{
}

bool KNXMLReader::MixedIterator::next()
{
  return m_iterator.next();
}

bool KNXMLReader::MixedIterator::isElement() const
{
  return XML_READER_TYPE_ELEMENT == xmlTextReaderNodeType(m_iterator.getImpl()->reader);
}

bool KNXMLReader::MixedIterator::isText() const
{
  return XML_READER_TYPE_TEXT == xmlTextReaderNodeType(m_iterator.getImpl()->reader);
}

const char *KNXMLReader::MixedIterator::getName() const
{
  assert(isElement());
  return reinterpret_cast<const char *>(xmlTextReaderConstLocalName(m_iterator.getImpl()->reader));
}

const char *KNXMLReader::MixedIterator::getNamespace() const
{
  assert(isElement());
  return reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(m_iterator.getImpl()->reader));
}

const char *KNXMLReader::MixedIterator::getText() const
{
  assert(isText());
  return reinterpret_cast<const char *>(xmlTextReaderConstValue(m_iterator.getImpl()->reader));
}

bool KNXMLReader::MixedIterator::isEmpty() const
{
  assert(isElement());
  return xmlTextReaderIsEmptyElement(m_iterator.getImpl()->reader);
}

int KNXMLReader::MixedIterator::getToken(const char *token) const
{
  return m_iterator.getImpl()->tokenizer(token);
}

const KNXMLReader::NodeIterator &KNXMLReader::MixedIterator::getNodeIterator() const
{
  return m_iterator;
}

KNXMLReader::KNXMLReader(WPXInputStream *const input)
  : m_impl(new Impl())
  , m_owner(true)
{
  try
  {
    initialize(input);
  }
  catch (...)
  {
    delete m_impl;
  }
}

KNXMLReader::KNXMLReader(WPXInputStream *const input, const TokenizerFunction_t tokenizer)
  : m_impl(new Impl())
  , m_owner(true)
{
  try
  {
    initialize(input);
    m_impl->tokenizer = tokenizer;
  }
  catch (...)
  {
    delete m_impl;
  }
}

KNXMLReader::KNXMLReader(const ElementIterator &iterator)
  : m_impl(iterator.getNodeIterator().getImpl())
  , m_owner(false)
{
}

KNXMLReader::KNXMLReader(const MixedIterator &iterator)
  : m_impl(iterator.getNodeIterator().getImpl())
  , m_owner(false)
{
  assert(iterator.isElement());
}

KNXMLReader::~KNXMLReader()
{
  if (m_owner)
    delete m_impl;
}

const char *KNXMLReader::getName() const
{
  return reinterpret_cast<const char *>(xmlTextReaderConstLocalName(m_impl->reader));
}

const char *KNXMLReader::getNamespace() const
{
  return reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(m_impl->reader));
}

int KNXMLReader::getToken(const char *token) const
{
  return m_impl->tokenizer(token);
}

void KNXMLReader::initialize(WPXInputStream *const input)
{
  m_impl->reader = xmlReaderForIO(readFromStream, closeStream, input, "", 0, 0);
  if (!bool(m_impl->reader))
    throw XMLException();

  // go to root element
  int ret = 0;
  do
  {
    ret = xmlTextReaderRead(m_impl->reader);
  }
  while ((1 == ret) && (XML_READER_TYPE_ELEMENT != xmlTextReaderNodeType(m_impl->reader)));

  if (1 != ret)
    throw XMLException();
}

int getNameId(const KNXMLReader::AttributeIterator &attribute)
{
  return attribute.getToken(attribute.getName());
}

int getNamespaceId(const KNXMLReader::AttributeIterator &attribute)
{
  return attribute.getToken(attribute.getNamespace());
}

int getValueId(const KNXMLReader::AttributeIterator &attribute)
{
  return attribute.getToken(attribute.getValue());
}

int getNameId(const KNXMLReader::ElementIterator &element)
{
  return element.getToken(element.getName());
}

int getNamespaceId(const KNXMLReader::ElementIterator &element)
{
  return element.getToken(element.getNamespace());
}

int getTextId(const KNXMLReader::ElementIterator &element)
{
  return element.getToken(element.getText());
}

int getNameId(const KNXMLReader &reader)
{
  return reader.getToken(reader.getName());
}

int getNamespaceId(const KNXMLReader &reader)
{
  return reader.getToken(reader.getNamespace());
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
