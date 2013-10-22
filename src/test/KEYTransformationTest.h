/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYTRANSFORMATIONTEST_H_INCLUDED
#define KEYTRANSFORMATIONTEST_H_INCLUDED

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

namespace test
{

class KEYTransformationTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(KEYTransformationTest);
  CPPUNIT_TEST(testApplication);
  CPPUNIT_TEST(testConstruction);
  CPPUNIT_TEST(testConstructionIdentity);
  CPPUNIT_TEST(testConstructionFromGeometry);
  CPPUNIT_TEST(testIdentities);
  CPPUNIT_TEST(testInverseOperations);
  CPPUNIT_TEST(testMultiplication);
  CPPUNIT_TEST_SUITE_END();

private:
  void testApplication();
  void testConstruction();
  void testConstructionIdentity();
  void testConstructionFromGeometry();
  void testIdentities();
  void testInverseOperations();
  void testMultiplication();
};

}

#endif //  KEYTRANSFORMATIONTEST_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
