/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include <librevenge-stream/librevenge-stream.h>

#include <libxml/xmlreader.h>

#include "KEYXMLReader.h"

namespace libetonyek
{

namespace
{

extern "C" int readFromStream(void *context, char *buffer, int len)
{
  try
  {
    librevenge::RVNGInputStream *const input = reinterpret_cast<librevenge::RVNGInputStream *>(context);

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

namespace
{

template<class T>
int implGetId(const T &token)
{
  const int name = getNameId(token);
  const int ns = getNamespaceId(token);
  assert((0 == ns) || (ns > name));

  return name | ns;
}

}

struct KEYXMLReader::Impl
{
  xmlTextReaderPtr reader;
  TokenizerFunction_t tokenizer;

  Impl();
  ~Impl();

  // disable copying
  Impl(const Impl &other);
  Impl &operator=(const Impl &other);
};

KEYXMLReader::Impl::Impl()
  : reader(0)
  , tokenizer()
{
}

KEYXMLReader::Impl::~Impl()
{
  if (0 != reader)
  {
    xmlTextReaderClose(reader);
    xmlFreeTextReader(reader);
  }
}

KEYXMLReader::NodeIterator::NodeIterator(const KEYXMLReader &reader, const int types)
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

bool KEYXMLReader::NodeIterator::next()
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

KEYXMLReader::Impl *KEYXMLReader::NodeIterator::getImpl() const
{
  return m_impl;
}

bool KEYXMLReader::NodeIterator::test() const
{
  const int type = xmlTextReaderNodeType(m_impl->reader);

  if ((TYPE_ELEMENT & m_types) && ((XML_READER_TYPE_ELEMENT == type) || (XML_READER_TYPE_END_ELEMENT == type)))
    return true;
  if ((TYPE_TEXT & m_types) && (XML_READER_TYPE_TEXT == type))
    return true;

  return false;
}

KEYXMLReader::AttributeIterator::AttributeIterator(const KEYXMLReader &reader)
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

bool KEYXMLReader::AttributeIterator::next()
{
  if (m_last)
    return false;

  while (move() && !test())
    ;

  return !m_last;
}

bool KEYXMLReader::AttributeIterator::move()
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

bool KEYXMLReader::AttributeIterator::test()
{
  const xmlChar *const prefix = xmlTextReaderConstPrefix(m_impl->reader);
  if (prefix)
    return strcmp("xmlns", reinterpret_cast<const char *>(prefix));

  const xmlChar *const name = xmlTextReaderConstLocalName(m_impl->reader);
  return strcmp("xmlns", reinterpret_cast<const char *>(name));
}

const char *KEYXMLReader::AttributeIterator::getName() const
{
  return reinterpret_cast<const char *>(xmlTextReaderConstLocalName(m_impl->reader));
}

const char *KEYXMLReader::AttributeIterator::getNamespace() const
{
  return reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(m_impl->reader));
}

const char *KEYXMLReader::AttributeIterator::getValue() const
{
  return reinterpret_cast<const char *>(xmlTextReaderConstValue(m_impl->reader));
}

int KEYXMLReader::AttributeIterator::getToken(const char *const token) const
{
  return m_impl->tokenizer(token);
}


KEYXMLReader::ElementIterator::ElementIterator(const KEYXMLReader &reader)
  : m_iterator(reader, TYPE_ELEMENT)
{
}

bool KEYXMLReader::ElementIterator::next()
{
  return m_iterator.next();
}

const char *KEYXMLReader::ElementIterator::getName() const
{
  return reinterpret_cast<const char *>(xmlTextReaderConstLocalName(m_iterator.getImpl()->reader));
}

const char *KEYXMLReader::ElementIterator::getNamespace() const
{
  return reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(m_iterator.getImpl()->reader));
}

const char *KEYXMLReader::ElementIterator::getText() const
{
  return reinterpret_cast<const char *>(xmlTextReaderConstValue(m_iterator.getImpl()->reader));
}

bool KEYXMLReader::ElementIterator::isEmpty() const
{
  return xmlTextReaderIsEmptyElement(m_iterator.getImpl()->reader);
}

const KEYXMLReader::NodeIterator &KEYXMLReader::ElementIterator::getNodeIterator() const
{
  return m_iterator;
}

int KEYXMLReader::ElementIterator::getToken(const char *token) const
{
  return m_iterator.getImpl()->tokenizer(token);
}

KEYXMLReader::MixedIterator::MixedIterator(const KEYXMLReader &reader)
  : m_iterator(reader, TYPE_ELEMENT | TYPE_TEXT)
{
}

bool KEYXMLReader::MixedIterator::next()
{
  return m_iterator.next();
}

bool KEYXMLReader::MixedIterator::isElement() const
{
  return XML_READER_TYPE_ELEMENT == xmlTextReaderNodeType(m_iterator.getImpl()->reader);
}

bool KEYXMLReader::MixedIterator::isText() const
{
  return XML_READER_TYPE_TEXT == xmlTextReaderNodeType(m_iterator.getImpl()->reader);
}

const char *KEYXMLReader::MixedIterator::getName() const
{
  assert(isElement());
  return reinterpret_cast<const char *>(xmlTextReaderConstLocalName(m_iterator.getImpl()->reader));
}

const char *KEYXMLReader::MixedIterator::getNamespace() const
{
  assert(isElement());
  return reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(m_iterator.getImpl()->reader));
}

const char *KEYXMLReader::MixedIterator::getText() const
{
  assert(isText());
  return reinterpret_cast<const char *>(xmlTextReaderConstValue(m_iterator.getImpl()->reader));
}

bool KEYXMLReader::MixedIterator::isEmpty() const
{
  assert(isElement());
  return xmlTextReaderIsEmptyElement(m_iterator.getImpl()->reader);
}

int KEYXMLReader::MixedIterator::getToken(const char *token) const
{
  return m_iterator.getImpl()->tokenizer(token);
}

const KEYXMLReader::NodeIterator &KEYXMLReader::MixedIterator::getNodeIterator() const
{
  return m_iterator;
}

KEYXMLReader::KEYXMLReader(librevenge::RVNGInputStream *const input)
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
    throw;
  }
}

KEYXMLReader::KEYXMLReader(librevenge::RVNGInputStream *const input, const TokenizerFunction_t tokenizer)
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
    throw;
  }
}

KEYXMLReader::KEYXMLReader(const ElementIterator &iterator)
  : m_impl(iterator.getNodeIterator().getImpl())
  , m_owner(false)
{
}

KEYXMLReader::KEYXMLReader(const MixedIterator &iterator)
  : m_impl(iterator.getNodeIterator().getImpl())
  , m_owner(false)
{
  assert(iterator.isElement());
}

KEYXMLReader::~KEYXMLReader()
{
  if (m_owner)
    delete m_impl;
}

const char *KEYXMLReader::getName() const
{
  return reinterpret_cast<const char *>(xmlTextReaderConstLocalName(m_impl->reader));
}

const char *KEYXMLReader::getNamespace() const
{
  return reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(m_impl->reader));
}

int KEYXMLReader::getToken(const char *token) const
{
  return m_impl->tokenizer(token);
}

void KEYXMLReader::initialize(librevenge::RVNGInputStream *const input)
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

int getNameId(const KEYXMLReader::AttributeIterator &attribute)
{
  return attribute.getToken(attribute.getName());
}

int getNamespaceId(const KEYXMLReader::AttributeIterator &attribute)
{
  return attribute.getToken(attribute.getNamespace() ? attribute.getNamespace() : "");
}

int getId(const KEYXMLReader::AttributeIterator &attribute)
{
  return implGetId(attribute);
}

int getValueId(const KEYXMLReader::AttributeIterator &attribute)
{
  return attribute.getToken(attribute.getValue());
}

int getNameId(const KEYXMLReader::ElementIterator &element)
{
  return element.getToken(element.getName());
}

int getNamespaceId(const KEYXMLReader::ElementIterator &element)
{
  return element.getToken(element.getNamespace() ? element.getNamespace() : "");
}

int getId(const KEYXMLReader::ElementIterator &element)
{
  return implGetId(element);
}

int getTextId(const KEYXMLReader::ElementIterator &element)
{
  return element.getToken(element.getText());
}

int getNameId(const KEYXMLReader &reader)
{
  return reader.getToken(reader.getName());
}

int getNamespaceId(const KEYXMLReader &reader)
{
  return reader.getToken(reader.getNamespace() ? reader.getNamespace() : "");
}

int getId(const KEYXMLReader &reader)
{
  return implGetId(reader);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
