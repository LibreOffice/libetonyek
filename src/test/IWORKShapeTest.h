/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKSHAPETEST_H_INCLUDED
#define IWORKSHAPETEST_H_INCLUDED

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

namespace test
{

class IWORKShapeTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(IWORKShapeTest);
  CPPUNIT_TEST(testMakePolygonPath);
  CPPUNIT_TEST(testMakeRoundedRectanglePath);
  CPPUNIT_TEST(testMakeArrowPath);
  CPPUNIT_TEST(testMakeDoubleArrowPath);
  CPPUNIT_TEST(testMakeStarPath);
  CPPUNIT_TEST(testMakeConnectionPath);
  CPPUNIT_TEST(testMakeCalloutPath);
  CPPUNIT_TEST(testMakeQuoteBubblePath);
  CPPUNIT_TEST_SUITE_END();

private:
  void testMakePolygonPath();
  void testMakeRoundedRectanglePath();
  void testMakeArrowPath();
  void testMakeDoubleArrowPath();
  void testMakeStarPath();
  void testMakeConnectionPath();
  void testMakeCalloutPath();
  void testMakeQuoteBubblePath();
};

}

#endif // IWORKSHAPETEST_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
