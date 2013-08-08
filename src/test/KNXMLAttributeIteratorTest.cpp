/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstring>

#include "KNXMLAttributeIterator.h"

#include "KNXMLAttributeIteratorTest.h"

using libkeynote::KNXMLAttributeIterator;

using std::strcmp;
using std::strlen;

namespace test
{

void KNXMLAttributeIteratorTest::setUp()
{
}

void KNXMLAttributeIteratorTest::tearDown()
{
}

void KNXMLAttributeIteratorTest::testEmpty()
{
  const char *const xml = "<?xml version='1.0'?><element/>";
  xmlTextReaderPtr reader = xmlReaderForMemory(xml, strlen(xml), "", "utf-8", XML_PARSE_NONET);

  CPPUNIT_ASSERT(1 == xmlTextReaderRead(reader));
  CPPUNIT_ASSERT(0 == strcmp(reinterpret_cast<const char *>(xmlTextReaderConstLocalName(reader)), "element"));

  KNXMLAttributeIterator attr(reader);
  CPPUNIT_ASSERT(!attr.next());
  CPPUNIT_ASSERT(attr.last());

  xmlTextReaderClose(reader);
  xmlFreeTextReader(reader);
}

void KNXMLAttributeIteratorTest::testOnlyXmlns()
{
  const char *const xml = "<?xml version='1.0'?><element xmlns='urn:foo' xmlns:bar='urn:bar'/>";
  xmlTextReaderPtr reader = xmlReaderForMemory(xml, strlen(xml), "", "utf-8", XML_PARSE_NONET);

  CPPUNIT_ASSERT(1 == xmlTextReaderRead(reader));
  CPPUNIT_ASSERT(0 == strcmp(reinterpret_cast<const char *>(xmlTextReaderConstLocalName(reader)), "element"));

  KNXMLAttributeIterator attr(reader);
  CPPUNIT_ASSERT(!attr.next());
  CPPUNIT_ASSERT(attr.last());

  xmlTextReaderClose(reader);
  xmlFreeTextReader(reader);
}

void KNXMLAttributeIteratorTest::testIterate()
{
  const char *const xml = "<?xml version='1.0'?><element a='1' xmlns:foo='urn:foo' b='2' foo:c='3'/>";
  xmlTextReaderPtr reader = xmlReaderForMemory(xml, strlen(xml), "", "utf-8", XML_PARSE_NONET);

  CPPUNIT_ASSERT(1 == xmlTextReaderRead(reader));
  CPPUNIT_ASSERT(0 == strcmp(reinterpret_cast<const char *>(xmlTextReaderConstLocalName(reader)), "element"));

  KNXMLAttributeIterator attr(reader);

  CPPUNIT_ASSERT(attr.next());
  CPPUNIT_ASSERT(!attr.last());
  CPPUNIT_ASSERT(0 == attr->ns);
  CPPUNIT_ASSERT(0 == strcmp(reinterpret_cast<const char *>(attr->name), "a"));
  CPPUNIT_ASSERT(0 == strcmp(reinterpret_cast<const char *>(attr->value), "1"));

  CPPUNIT_ASSERT(attr.next());
  CPPUNIT_ASSERT(!attr.last());
  CPPUNIT_ASSERT(0 == attr->ns);
  CPPUNIT_ASSERT(0 == strcmp(reinterpret_cast<const char *>(attr->name), "b"));
  CPPUNIT_ASSERT(0 == strcmp(reinterpret_cast<const char *>(attr->value), "2"));

  CPPUNIT_ASSERT(attr.next());
  CPPUNIT_ASSERT(!attr.last());
  CPPUNIT_ASSERT(0 == strcmp(reinterpret_cast<const char *>(attr->ns), "urn:foo"));
  CPPUNIT_ASSERT(0 == strcmp(reinterpret_cast<const char *>(attr->name), "c"));
  CPPUNIT_ASSERT(0 == strcmp(reinterpret_cast<const char *>(attr->value), "3"));

  CPPUNIT_ASSERT(!attr.next());
  CPPUNIT_ASSERT(attr.last());

  xmlTextReaderClose(reader);
  xmlFreeTextReader(reader);
}

CPPUNIT_TEST_SUITE_REGISTRATION(KNXMLAttributeIteratorTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
