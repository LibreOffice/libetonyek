/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstring>

#include "KEYMemoryStream.h"
#include "KEYXMLReader.h"

#include "KEYXMLReaderTest.h"

using libetonyek::KEYMemoryStream;
using libetonyek::KEYXMLReader;

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

void KEYXMLReaderTest::setUp()
{
}

void KEYXMLReaderTest::tearDown()
{
}

void KEYXMLReaderTest::testEmptyAttributeList()
{
  const unsigned char xml[] = "<?xml version='1.0'?><element/>";
  KEYMemoryStream input(xml, sizeof(xml) - 1);

  KEYXMLReader reader(&input);
  KEYXMLReader::AttributeIterator attr(reader);

  CPPUNIT_ASSERT(!attr.next());
}

void KEYXMLReaderTest::testOnlyXmlnsAttributes()
{
  const unsigned char xml[] = "<?xml version='1.0'?><element xmlns='urn:foo' xmlns:bar='urn:bar'/>";
  KEYMemoryStream input(xml, sizeof(xml) - 1);

  KEYXMLReader reader(&input);
  KEYXMLReader::AttributeIterator attr(reader);

  CPPUNIT_ASSERT(!attr.next());
}

void KEYXMLReaderTest::testIterateAttributes()
{
  const unsigned char xml[] = "<?xml version='1.0'?><element a='1' xmlns:foo='urn:foo' b='2' foo:c='3'/>";
  KEYMemoryStream input(xml, sizeof(xml) - 1);

  KEYXMLReader reader(&input);
  KEYXMLReader::AttributeIterator attr(reader);

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

void KEYXMLReaderTest::testEmptyElement()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element/>";
    KEYMemoryStream input(xml, sizeof(xml) - 1);

    KEYXMLReader reader(&input);
    KEYXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element xmlns='urn:foo' xmlns:bar='urn:bar'/>";
    KEYMemoryStream input(xml, sizeof(xml) - 1);

    KEYXMLReader reader(&input);
    KEYXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element></element>";
    KEYMemoryStream input(xml, sizeof(xml) - 1);

    KEYXMLReader reader(&input);
    KEYXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(!element.next());
  }
}

void KEYXMLReaderTest::testIterateElements()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub/></element>";
    KEYMemoryStream input(xml, sizeof(xml) - 1);

    KEYXMLReader reader(&input);
    KEYXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(element.isEmpty());
    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub1/><sub2/></element>";
    KEYMemoryStream input(xml, sizeof(xml) - 1);

    KEYXMLReader reader(&input);
    KEYXMLReader::ElementIterator element(reader);

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

void KEYXMLReaderTest::testIterateNestedElements()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub1><sub2/></sub1></element>";
    KEYMemoryStream input(xml, sizeof(xml) - 1);

    KEYXMLReader reader(&input);
    KEYXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub1"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(!element.isEmpty());

    {
      KEYXMLReader::ElementIterator sub1(reader);

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
    KEYMemoryStream input(xml, sizeof(xml) - 1);

    KEYXMLReader reader(&input);
    KEYXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub1"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(!element.isEmpty());

    {
      KEYXMLReader::ElementIterator sub1(reader);

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

void KEYXMLReaderTest::testIterateElementsWithAttributes()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element attr1='value1' attr2='value2'><sub attr3='value3'/></element>";
    KEYMemoryStream input(xml, sizeof(xml) - 1);

    KEYXMLReader reader(&input);
    KEYXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(element.isEmpty());
    CPPUNIT_ASSERT(!element.next());
  }
}

void KEYXMLReaderTest::testEmptyMixed()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element/>";
    KEYMemoryStream input(xml, sizeof(xml) - 1);

    KEYXMLReader reader(&input);
    KEYXMLReader::MixedIterator element(reader);

    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element xmlns='urn:foo' xmlns:bar='urn:bar'/>";
    KEYMemoryStream input(xml, sizeof(xml) - 1);

    KEYXMLReader reader(&input);
    KEYXMLReader::MixedIterator element(reader);

    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element></element>";
    KEYMemoryStream input(xml, sizeof(xml) - 1);

    KEYXMLReader reader(&input);
    KEYXMLReader::MixedIterator element(reader);

    CPPUNIT_ASSERT(!element.next());
  }
}

void KEYXMLReaderTest::testIterateMixed()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub/></element>";
    KEYMemoryStream input(xml, sizeof(xml) - 1);

    KEYXMLReader reader(&input);
    KEYXMLReader::MixedIterator element(reader);

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
    KEYMemoryStream input(xml, sizeof(xml) - 1);

    KEYXMLReader reader(&input);
    KEYXMLReader::MixedIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(!element.isElement());
    CPPUNIT_ASSERT(element.isText());
    CPPUNIT_ASSERT(0 == strcmp(element.getText(), "hello"));
    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub1/>hello</element>";
    KEYMemoryStream input(xml, sizeof(xml) - 1);

    KEYXMLReader reader(&input);
    KEYXMLReader::MixedIterator element(reader);

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
    KEYMemoryStream input(xml, sizeof(xml) - 1);

    KEYXMLReader reader(&input);
    KEYXMLReader::MixedIterator element(reader);

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

void KEYXMLReaderTest::testElementName()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element/>";
    KEYMemoryStream input(xml, sizeof(xml) - 1);

    KEYXMLReader reader(&input);

    CPPUNIT_ASSERT(0 == strcmp(reader.getName(), "element"));
    CPPUNIT_ASSERT(0 == reader.getNamespace());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub1><sub2/></sub1></element>";
    KEYMemoryStream input(xml, sizeof(xml) - 1);

    KEYXMLReader reader(&input);

    CPPUNIT_ASSERT(0 == strcmp(reader.getName(), "element"));
    CPPUNIT_ASSERT(0 == reader.getNamespace());

    KEYXMLReader::ElementIterator element(reader);
    CPPUNIT_ASSERT(element.next());

    {
      KEYXMLReader nested(element);

      CPPUNIT_ASSERT(0 == strcmp(nested.getName(), "sub1"));
      CPPUNIT_ASSERT(0 == nested.getNamespace());

      KEYXMLReader::ElementIterator sub1(nested);
      CPPUNIT_ASSERT(sub1.next());

      {
        KEYXMLReader nested2(element);

        CPPUNIT_ASSERT(0 == strcmp(nested2.getName(), "sub2"));
        CPPUNIT_ASSERT(0 == nested2.getNamespace());
      }
    }
  }
}

void KEYXMLReaderTest::testTokenizer()
{
  const unsigned char xml[] = "<?xml version='1.0'?><m:a n:b='c' xmlns:m='m' xmlns:n='n'><d/><u:d xmlns:u='u'/></m:a>";
  KEYMemoryStream input(xml, sizeof(xml) - 1);

  KEYXMLReader reader(&input, Tokenizer());

  CPPUNIT_ASSERT_EQUAL(1, getNameId(reader));
  CPPUNIT_ASSERT_EQUAL(8, getNamespaceId(reader));
  CPPUNIT_ASSERT_EQUAL(1 + 8, getId(reader));

  KEYXMLReader::AttributeIterator attrs(reader);
  CPPUNIT_ASSERT(attrs.next());

  CPPUNIT_ASSERT_EQUAL(2, getNameId(attrs));
  CPPUNIT_ASSERT_EQUAL(16, getNamespaceId(attrs));
  CPPUNIT_ASSERT_EQUAL(2 + 16, getId(attrs));
  CPPUNIT_ASSERT_EQUAL(3, getValueId(attrs));

  KEYXMLReader::ElementIterator elements(reader);
  CPPUNIT_ASSERT(elements.next());

  CPPUNIT_ASSERT_EQUAL(4, getNameId(elements));
  CPPUNIT_ASSERT_EQUAL(32, getNamespaceId(elements));
  CPPUNIT_ASSERT_EQUAL(4 + 32, getId(elements));

  KEYXMLReader nested(elements);
  CPPUNIT_ASSERT_EQUAL(4, getNameId(nested));
  CPPUNIT_ASSERT_EQUAL(32, getNamespaceId(nested));
  CPPUNIT_ASSERT_EQUAL(4 + 32, getId(nested));

  CPPUNIT_ASSERT(elements.next());
  CPPUNIT_ASSERT_EQUAL(4, getNameId(elements));
  CPPUNIT_ASSERT_EQUAL(0, getNamespaceId(elements));
  CPPUNIT_ASSERT_EQUAL(4, getId(elements));
}

CPPUNIT_TEST_SUITE_REGISTRATION(KEYXMLReaderTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
