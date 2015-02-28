/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "IWORKStyle.h"
#include "IWORKStyleStack.h"

namespace test
{

using boost::any_cast;
using boost::optional;

using libetonyek::IWORKStyle;
using libetonyek::IWORKStylePtr_t;
using libetonyek::IWORKStyleStack;
using libetonyek::IWORKPropertyMap;

using std::string;

namespace
{

IWORKStylePtr_t makeStyle(const IWORKPropertyMap &props)
{
  const optional<string> dummyIdent;
  const IWORKStylePtr_t style(new IWORKStyle(props, dummyIdent, dummyIdent));

  return style;
}

}

class IWORKStyleStackTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(IWORKStyleStackTest);
  CPPUNIT_TEST(testLookup);
  CPPUNIT_TEST_SUITE_END();

private:
  void testLookup();
};

void IWORKStyleStackTest::setUp()
{
}

void IWORKStyleStackTest::tearDown()
{
}

void IWORKStyleStackTest::testLookup()
{
  IWORKStyleStack context;

  // lookup in empty context finds nothing
  CPPUNIT_ASSERT(context.find("answer").empty());

  IWORKPropertyMap props;
  props.set("answer", 42);

  // lookup works after we push a style
  context.push();
  context.set(makeStyle(props));
  CPPUNIT_ASSERT(!context.find("answer").empty());
  CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(context.find("answer")));

  // and stops working after we pop it again
  context.pop();
  CPPUNIT_ASSERT(context.find("answer").empty());

  // the styles are arranged in a stack -- a property is found in
  // the last pushed one
  IWORKPropertyMap props2;
  props2.set("answer", 1);
  context.push();
  context.set(makeStyle(props2));
  context.push();
  context.set(makeStyle(props));
  CPPUNIT_ASSERT(!context.find("answer").empty());
  CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(context.find("answer")));

  context.pop();
  context.pop();
  CPPUNIT_ASSERT(context.find("answer").empty());

  // lookup works recursively through more levels
  context.push();
  context.set(makeStyle(props));
  context.push();
  context.set(makeStyle(IWORKPropertyMap()));
  CPPUNIT_ASSERT(!context.find("answer").empty());
  CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(context.find("answer")));

  // yet another one...
  context.push();
  context.set(makeStyle(IWORKPropertyMap()));
  CPPUNIT_ASSERT(!context.find("answer").empty());
  CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(context.find("answer")));
}

CPPUNIT_TEST_SUITE_REGISTRATION(IWORKStyleStackTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
