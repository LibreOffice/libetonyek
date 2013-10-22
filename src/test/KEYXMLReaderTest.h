/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYXMLREADERTEST_H_INCLUDED
#define KEYXMLREADERTEST_H_INCLUDED

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

namespace test
{

class KEYXMLReaderTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(KEYXMLReaderTest);

  CPPUNIT_TEST(testEmptyAttributeList);
  CPPUNIT_TEST(testOnlyXmlnsAttributes);
  CPPUNIT_TEST(testIterateAttributes);

  CPPUNIT_TEST(testEmptyElement);
  CPPUNIT_TEST(testIterateElements);
  CPPUNIT_TEST(testIterateNestedElements);
  CPPUNIT_TEST(testIterateElementsWithAttributes);

  CPPUNIT_TEST(testEmptyMixed);
  CPPUNIT_TEST(testIterateMixed);

  CPPUNIT_TEST(testElementName);

  CPPUNIT_TEST(testTokenizer);

  CPPUNIT_TEST_SUITE_END();

private:
  void testEmptyAttributeList();
  void testOnlyXmlnsAttributes();
  void testIterateAttributes();

  void testEmptyElement();
  void testIterateElements();
  void testIterateNestedElements();
  void testIterateElementsWithAttributes();

  void testEmptyMixed();
  void testIterateMixed();

  void testElementName();

  void testTokenizer();
};

}

#endif // KEYXMLREADERTEST_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
