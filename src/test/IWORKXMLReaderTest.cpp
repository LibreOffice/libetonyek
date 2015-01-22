/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstring>

#include "IWORKMemoryStream.h"
#include "IWORKXMLReader.h"

#include "IWORKXMLReaderTest.h"

using libetonyek::IWORKMemoryStream;
using libetonyek::IWORKXMLReader;

using std::strcmp;
using std::strlen;

namespace test
{

namespace
{

struct Tokenizer
{
  Tokenizer();

  int operator()(const char *token) const;
};

Tokenizer::Tokenizer()
{
}

int Tokenizer::operator()(const char *const token) const
{
  if (0 == strcmp(token, "a"))
    return 1;
  else if (0 == strcmp(token, "b"))
    return 2;
  else if (0 == strcmp(token, "c"))
    return 3;
  else if (0 == strcmp(token, "d"))
    return 4;
  else if (0 == strcmp(token, "m"))
    return 8;
  else if (0 == strcmp(token, "n"))
    return 16;
  else if (0 == strcmp(token, ""))
    return 32;

  return 0;
}

}

void IWORKXMLReaderTest::setUp()
{
}

void IWORKXMLReaderTest::tearDown()
{
}

void IWORKXMLReaderTest::testEmptyAttributeList()
{
  const unsigned char xml[] = "<?xml version='1.0'?><element/>";
  IWORKMemoryStream input(xml, sizeof(xml) - 1);

  IWORKXMLReader reader(&input);
  IWORKXMLReader::AttributeIterator attr(reader);

  CPPUNIT_ASSERT(!attr.next());
}

void IWORKXMLReaderTest::testOnlyXmlnsAttributes()
{
  const unsigned char xml[] = "<?xml version='1.0'?><element xmlns='urn:foo' xmlns:bar='urn:bar'/>";
  IWORKMemoryStream input(xml, sizeof(xml) - 1);

  IWORKXMLReader reader(&input);
  IWORKXMLReader::AttributeIterator attr(reader);

  CPPUNIT_ASSERT(!attr.next());
}

void IWORKXMLReaderTest::testIterateAttributes()
{
  const unsigned char xml[] = "<?xml version='1.0'?><element a='1' xmlns:foo='urn:foo' b='2' foo:c='3'/>";
  IWORKMemoryStream input(xml, sizeof(xml) - 1);

  IWORKXMLReader reader(&input);
  IWORKXMLReader::AttributeIterator attr(reader);

  CPPUNIT_ASSERT(attr.next());
  CPPUNIT_ASSERT(0 == attr.getNamespace());
  CPPUNIT_ASSERT(0 == strcmp(attr.getName(), "a"));
  CPPUNIT_ASSERT(0 == strcmp(attr.getValue(), "1"));

  CPPUNIT_ASSERT(attr.next());
  CPPUNIT_ASSERT(0 == attr.getNamespace());
  CPPUNIT_ASSERT(0 == strcmp(attr.getName(), "b"));
  CPPUNIT_ASSERT(0 == strcmp(attr.getValue(), "2"));

  CPPUNIT_ASSERT(attr.next());
  CPPUNIT_ASSERT(0 == strcmp(attr.getNamespace(), "urn:foo"));
  CPPUNIT_ASSERT(0 == strcmp(attr.getName(), "c"));
  CPPUNIT_ASSERT(0 == strcmp(attr.getValue(), "3"));

  CPPUNIT_ASSERT(!attr.next());
}

void IWORKXMLReaderTest::testEmptyElement()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element/>";
    IWORKMemoryStream input(xml, sizeof(xml) - 1);

    IWORKXMLReader reader(&input);
    IWORKXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element xmlns='urn:foo' xmlns:bar='urn:bar'/>";
    IWORKMemoryStream input(xml, sizeof(xml) - 1);

    IWORKXMLReader reader(&input);
    IWORKXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element></element>";
    IWORKMemoryStream input(xml, sizeof(xml) - 1);

    IWORKXMLReader reader(&input);
    IWORKXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(!element.next());
  }
}

void IWORKXMLReaderTest::testIterateElements()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub/></element>";
    IWORKMemoryStream input(xml, sizeof(xml) - 1);

    IWORKXMLReader reader(&input);
    IWORKXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(element.isEmpty());
    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub1/><sub2/></element>";
    IWORKMemoryStream input(xml, sizeof(xml) - 1);

    IWORKXMLReader reader(&input);
    IWORKXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub1"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(element.isEmpty());
    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub2"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(element.isEmpty());
    CPPUNIT_ASSERT(!element.next());
  }
}

void IWORKXMLReaderTest::testIterateNestedElements()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub1><sub2/></sub1></element>";
    IWORKMemoryStream input(xml, sizeof(xml) - 1);

    IWORKXMLReader reader(&input);
    IWORKXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub1"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(!element.isEmpty());

    {
      IWORKXMLReader::ElementIterator sub1(reader);

      CPPUNIT_ASSERT(sub1.next());
      CPPUNIT_ASSERT(0 == strcmp(sub1.getName(), "sub2"));
      CPPUNIT_ASSERT(0 == sub1.getNamespace());
      CPPUNIT_ASSERT(sub1.isEmpty());
      CPPUNIT_ASSERT(!sub1.next());
    }

    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub1><sub2/><sub3/></sub1><sub4/></element>";
    IWORKMemoryStream input(xml, sizeof(xml) - 1);

    IWORKXMLReader reader(&input);
    IWORKXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub1"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(!element.isEmpty());

    {
      IWORKXMLReader::ElementIterator sub1(reader);

      CPPUNIT_ASSERT(sub1.next());
      CPPUNIT_ASSERT(0 == strcmp(sub1.getName(), "sub2"));
      CPPUNIT_ASSERT(0 == sub1.getNamespace());
      CPPUNIT_ASSERT(sub1.isEmpty());
      CPPUNIT_ASSERT(sub1.next());
      CPPUNIT_ASSERT(0 == strcmp(sub1.getName(), "sub3"));
      CPPUNIT_ASSERT(0 == sub1.getNamespace());
      CPPUNIT_ASSERT(sub1.isEmpty());
      CPPUNIT_ASSERT(!sub1.next());
    }

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub4"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(element.isEmpty());
    CPPUNIT_ASSERT(!element.next());
  }
}

void IWORKXMLReaderTest::testIterateElementsWithAttributes()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element attr1='value1' attr2='value2'><sub attr3='value3'/></element>";
    IWORKMemoryStream input(xml, sizeof(xml) - 1);

    IWORKXMLReader reader(&input);
    IWORKXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(element.isEmpty());
    CPPUNIT_ASSERT(!element.next());
  }
}

void IWORKXMLReaderTest::testEmptyMixed()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element/>";
    IWORKMemoryStream input(xml, sizeof(xml) - 1);

    IWORKXMLReader reader(&input);
    IWORKXMLReader::MixedIterator element(reader);

    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element xmlns='urn:foo' xmlns:bar='urn:bar'/>";
    IWORKMemoryStream input(xml, sizeof(xml) - 1);

    IWORKXMLReader reader(&input);
    IWORKXMLReader::MixedIterator element(reader);

    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element></element>";
    IWORKMemoryStream input(xml, sizeof(xml) - 1);

    IWORKXMLReader reader(&input);
    IWORKXMLReader::MixedIterator element(reader);

    CPPUNIT_ASSERT(!element.next());
  }
}

void IWORKXMLReaderTest::testIterateMixed()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub/></element>";
    IWORKMemoryStream input(xml, sizeof(xml) - 1);

    IWORKXMLReader reader(&input);
    IWORKXMLReader::MixedIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(element.isElement());
    CPPUNIT_ASSERT(!element.isText());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(element.isEmpty());
    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element>hello</element>";
    IWORKMemoryStream input(xml, sizeof(xml) - 1);

    IWORKXMLReader reader(&input);
    IWORKXMLReader::MixedIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(!element.isElement());
    CPPUNIT_ASSERT(element.isText());
    CPPUNIT_ASSERT(0 == strcmp(element.getText(), "hello"));
    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub1/>hello</element>";
    IWORKMemoryStream input(xml, sizeof(xml) - 1);

    IWORKXMLReader reader(&input);
    IWORKXMLReader::MixedIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(element.isElement());
    CPPUNIT_ASSERT(!element.isText());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub1"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(element.isEmpty());
    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(!element.isElement());
    CPPUNIT_ASSERT(element.isText());
    CPPUNIT_ASSERT(0 == strcmp(element.getText(), "hello"));
    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub1/>hello <sub2/>world</element>";
    IWORKMemoryStream input(xml, sizeof(xml) - 1);

    IWORKXMLReader reader(&input);
    IWORKXMLReader::MixedIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(element.isElement());
    CPPUNIT_ASSERT(!element.isText());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub1"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(element.isEmpty());
    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(!element.isElement());
    CPPUNIT_ASSERT(element.isText());
    CPPUNIT_ASSERT(0 == strcmp(element.getText(), "hello "));
    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(element.isElement());
    CPPUNIT_ASSERT(!element.isText());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub2"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(element.isEmpty());
    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(!element.isElement());
    CPPUNIT_ASSERT(element.isText());
    CPPUNIT_ASSERT(0 == strcmp(element.getText(), "world"));
    CPPUNIT_ASSERT(!element.next());
  }
}

void IWORKXMLReaderTest::testElementName()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element/>";
    IWORKMemoryStream input(xml, sizeof(xml) - 1);

    IWORKXMLReader reader(&input);

    CPPUNIT_ASSERT(0 == strcmp(reader.getName(), "element"));
    CPPUNIT_ASSERT(0 == reader.getNamespace());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub1><sub2/></sub1></element>";
    IWORKMemoryStream input(xml, sizeof(xml) - 1);

    IWORKXMLReader reader(&input);

    CPPUNIT_ASSERT(0 == strcmp(reader.getName(), "element"));
    CPPUNIT_ASSERT(0 == reader.getNamespace());

    IWORKXMLReader::ElementIterator element(reader);
    CPPUNIT_ASSERT(element.next());

    {
      IWORKXMLReader nested(element);

      CPPUNIT_ASSERT(0 == strcmp(nested.getName(), "sub1"));
      CPPUNIT_ASSERT(0 == nested.getNamespace());

      IWORKXMLReader::ElementIterator sub1(nested);
      CPPUNIT_ASSERT(sub1.next());

      {
        IWORKXMLReader nested2(element);

        CPPUNIT_ASSERT(0 == strcmp(nested2.getName(), "sub2"));
        CPPUNIT_ASSERT(0 == nested2.getNamespace());
      }
    }
  }
}

void IWORKXMLReaderTest::testTokenizer()
{
  const unsigned char xml[] = "<?xml version='1.0'?><m:a n:b='c' xmlns:m='m' xmlns:n='n'><d/><u:d xmlns:u='u'/></m:a>";
  IWORKMemoryStream input(xml, sizeof(xml) - 1);

  IWORKXMLReader reader(&input, Tokenizer());

  CPPUNIT_ASSERT_EQUAL(1, getNameId(reader));
  CPPUNIT_ASSERT_EQUAL(8, getNamespaceId(reader));
  CPPUNIT_ASSERT_EQUAL(1 + 8, getId(reader));

  IWORKXMLReader::AttributeIterator attrs(reader);
  CPPUNIT_ASSERT(attrs.next());

  CPPUNIT_ASSERT_EQUAL(2, getNameId(attrs));
  CPPUNIT_ASSERT_EQUAL(16, getNamespaceId(attrs));
  CPPUNIT_ASSERT_EQUAL(2 + 16, getId(attrs));
  CPPUNIT_ASSERT_EQUAL(3, getValueId(attrs));

  IWORKXMLReader::ElementIterator elements(reader);
  CPPUNIT_ASSERT(elements.next());

  CPPUNIT_ASSERT_EQUAL(4, getNameId(elements));
  CPPUNIT_ASSERT_EQUAL(32, getNamespaceId(elements));
  CPPUNIT_ASSERT_EQUAL(4 + 32, getId(elements));

  IWORKXMLReader nested(elements);
  CPPUNIT_ASSERT_EQUAL(4, getNameId(nested));
  CPPUNIT_ASSERT_EQUAL(32, getNamespaceId(nested));
  CPPUNIT_ASSERT_EQUAL(4 + 32, getId(nested));

  CPPUNIT_ASSERT(elements.next());
  CPPUNIT_ASSERT_EQUAL(4, getNameId(elements));
  CPPUNIT_ASSERT_EQUAL(0, getNamespaceId(elements));
  CPPUNIT_ASSERT_EQUAL(4, getId(elements));
}

CPPUNIT_TEST_SUITE_REGISTRATION(IWORKXMLReaderTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
