/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstring>

#include "KNMemoryStream.h"
#include "KNXMLReader.h"

#include "KNXMLReaderTest.h"

using libkeynote::KNMemoryStream;
using libkeynote::KNXMLReader;

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

void KNXMLReaderTest::setUp()
{
}

void KNXMLReaderTest::tearDown()
{
}

void KNXMLReaderTest::testEmptyAttributeList()
{
  const unsigned char xml[] = "<?xml version='1.0'?><element/>";
  KNMemoryStream input(xml, sizeof(xml) - 1);

  KNXMLReader reader(&input);
  KNXMLReader::AttributeIterator attr(reader);

  CPPUNIT_ASSERT(!attr.next());
}

void KNXMLReaderTest::testOnlyXmlnsAttributes()
{
  const unsigned char xml[] = "<?xml version='1.0'?><element xmlns='urn:foo' xmlns:bar='urn:bar'/>";
  KNMemoryStream input(xml, sizeof(xml) - 1);

  KNXMLReader reader(&input);
  KNXMLReader::AttributeIterator attr(reader);

  CPPUNIT_ASSERT(!attr.next());
}

void KNXMLReaderTest::testIterateAttributes()
{
  const unsigned char xml[] = "<?xml version='1.0'?><element a='1' xmlns:foo='urn:foo' b='2' foo:c='3'/>";
  KNMemoryStream input(xml, sizeof(xml) - 1);

  KNXMLReader reader(&input);
  KNXMLReader::AttributeIterator attr(reader);

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

void KNXMLReaderTest::testEmptyElement()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element/>";
    KNMemoryStream input(xml, sizeof(xml) - 1);

    KNXMLReader reader(&input);
    KNXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element xmlns='urn:foo' xmlns:bar='urn:bar'/>";
    KNMemoryStream input(xml, sizeof(xml) - 1);

    KNXMLReader reader(&input);
    KNXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element></element>";
    KNMemoryStream input(xml, sizeof(xml) - 1);

    KNXMLReader reader(&input);
    KNXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(!element.next());
  }
}

void KNXMLReaderTest::testIterateElements()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub/></element>";
    KNMemoryStream input(xml, sizeof(xml) - 1);

    KNXMLReader reader(&input);
    KNXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(element.isEmpty());
    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub1/><sub2/></element>";
    KNMemoryStream input(xml, sizeof(xml) - 1);

    KNXMLReader reader(&input);
    KNXMLReader::ElementIterator element(reader);

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

void KNXMLReaderTest::testIterateNestedElements()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub1><sub2/></sub1></element>";
    KNMemoryStream input(xml, sizeof(xml) - 1);

    KNXMLReader reader(&input);
    KNXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub1"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(!element.isEmpty());

    {
      KNXMLReader::ElementIterator sub1(reader);

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
    KNMemoryStream input(xml, sizeof(xml) - 1);

    KNXMLReader reader(&input);
    KNXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub1"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(!element.isEmpty());

    {
      KNXMLReader::ElementIterator sub1(reader);

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

void KNXMLReaderTest::testIterateElementsWithAttributes()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element attr1='value1' attr2='value2'><sub attr3='value3'/></element>";
    KNMemoryStream input(xml, sizeof(xml) - 1);

    KNXMLReader reader(&input);
    KNXMLReader::ElementIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(0 == strcmp(element.getName(), "sub"));
    CPPUNIT_ASSERT(0 == element.getNamespace());
    CPPUNIT_ASSERT(element.isEmpty());
    CPPUNIT_ASSERT(!element.next());
  }
}

void KNXMLReaderTest::testEmptyMixed()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element/>";
    KNMemoryStream input(xml, sizeof(xml) - 1);

    KNXMLReader reader(&input);
    KNXMLReader::MixedIterator element(reader);

    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element xmlns='urn:foo' xmlns:bar='urn:bar'/>";
    KNMemoryStream input(xml, sizeof(xml) - 1);

    KNXMLReader reader(&input);
    KNXMLReader::MixedIterator element(reader);

    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element></element>";
    KNMemoryStream input(xml, sizeof(xml) - 1);

    KNXMLReader reader(&input);
    KNXMLReader::MixedIterator element(reader);

    CPPUNIT_ASSERT(!element.next());
  }
}

void KNXMLReaderTest::testIterateMixed()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub/></element>";
    KNMemoryStream input(xml, sizeof(xml) - 1);

    KNXMLReader reader(&input);
    KNXMLReader::MixedIterator element(reader);

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
    KNMemoryStream input(xml, sizeof(xml) - 1);

    KNXMLReader reader(&input);
    KNXMLReader::MixedIterator element(reader);

    CPPUNIT_ASSERT(element.next());
    CPPUNIT_ASSERT(!element.isElement());
    CPPUNIT_ASSERT(element.isText());
    CPPUNIT_ASSERT(0 == strcmp(element.getText(), "hello"));
    CPPUNIT_ASSERT(!element.next());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub1/>hello</element>";
    KNMemoryStream input(xml, sizeof(xml) - 1);

    KNXMLReader reader(&input);
    KNXMLReader::MixedIterator element(reader);

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
    KNMemoryStream input(xml, sizeof(xml) - 1);

    KNXMLReader reader(&input);
    KNXMLReader::MixedIterator element(reader);

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

void KNXMLReaderTest::testElementName()
{
  {
    const unsigned char xml[] = "<?xml version='1.0'?><element/>";
    KNMemoryStream input(xml, sizeof(xml) - 1);

    KNXMLReader reader(&input);

    CPPUNIT_ASSERT(0 == strcmp(reader.getName(), "element"));
    CPPUNIT_ASSERT(0 == reader.getNamespace());
  }

  {
    const unsigned char xml[] = "<?xml version='1.0'?><element><sub1><sub2/></sub1></element>";
    KNMemoryStream input(xml, sizeof(xml) - 1);

    KNXMLReader reader(&input);

    CPPUNIT_ASSERT(0 == strcmp(reader.getName(), "element"));
    CPPUNIT_ASSERT(0 == reader.getNamespace());

    KNXMLReader::ElementIterator element(reader);
    CPPUNIT_ASSERT(element.next());

    {
      KNXMLReader nested(element);

      CPPUNIT_ASSERT(0 == strcmp(nested.getName(), "sub1"));
      CPPUNIT_ASSERT(0 == nested.getNamespace());

      KNXMLReader::ElementIterator sub1(nested);
      CPPUNIT_ASSERT(sub1.next());

      {
        KNXMLReader nested2(element);

        CPPUNIT_ASSERT(0 == strcmp(nested2.getName(), "sub2"));
        CPPUNIT_ASSERT(0 == nested2.getNamespace());
      }
    }
  }
}

void KNXMLReaderTest::testTokenizer()
{
  const unsigned char xml[] = "<?xml version='1.0'?><m:a n:b='c' xmlns:m='m' xmlns:n='n'><d/><u:d xmlns:u='u'/></m:a>";
  KNMemoryStream input(xml, sizeof(xml) - 1);

  KNXMLReader reader(&input, Tokenizer());

  CPPUNIT_ASSERT_EQUAL(1, getNameId(reader));
  CPPUNIT_ASSERT_EQUAL(8, getNamespaceId(reader));
  CPPUNIT_ASSERT_EQUAL(1 + 8, getId(reader));

  KNXMLReader::AttributeIterator attrs(reader);
  attrs.next();

  CPPUNIT_ASSERT_EQUAL(2, getNameId(attrs));
  CPPUNIT_ASSERT_EQUAL(16, getNamespaceId(attrs));
  CPPUNIT_ASSERT_EQUAL(2 + 16, getId(attrs));
  CPPUNIT_ASSERT_EQUAL(3, getValueId(attrs));

  KNXMLReader::ElementIterator elements(reader);
  elements.next();

  CPPUNIT_ASSERT_EQUAL(4, getNameId(elements));
  CPPUNIT_ASSERT_EQUAL(32, getNamespaceId(elements));
  CPPUNIT_ASSERT_EQUAL(4 + 32, getId(elements));

  KNXMLReader nested(elements);
  CPPUNIT_ASSERT_EQUAL(4, getNameId(nested));
  CPPUNIT_ASSERT_EQUAL(32, getNamespaceId(nested));
  CPPUNIT_ASSERT_EQUAL(4 + 32, getId(nested));

  elements.next();
  CPPUNIT_ASSERT_EQUAL(4, getNameId(elements));
  CPPUNIT_ASSERT_EQUAL(0, getNamespaceId(elements));
  CPPUNIT_ASSERT_EQUAL(4, getId(elements));
}

CPPUNIT_TEST_SUITE_REGISTRATION(KNXMLReaderTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
