/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYStyleContext.h"
#include "KEYStyles.h"

#include "KEYStyleContextTest.h"

namespace test
{

using boost::any_cast;
using boost::optional;

using libkeynote::KEYStyleContext;
using libkeynote::KEYParagraphStyle;
using libkeynote::KEYParagraphStylePtr_t;
using libkeynote::KEYPropertyMap;

using std::string;

namespace
{

KEYParagraphStylePtr_t makeStyle(const KEYPropertyMap &props)
{
  const optional<string> dummyIdent;
  const KEYParagraphStylePtr_t style(new KEYParagraphStyle(props, dummyIdent, dummyIdent));

  return style;
}

}

void KEYStyleContextTest::setUp()
{
}

void KEYStyleContextTest::tearDown()
{
}

void KEYStyleContextTest::testLookup()
{
  KEYStyleContext context;

  // lookup in empty context finds nothing
  CPPUNIT_ASSERT(context.find("answer").empty());

  KEYPropertyMap props;
  props.set("answer", 42);

  // lookup works after we push a style
  context.push(makeStyle(props));
  CPPUNIT_ASSERT(!context.find("answer").empty());
  CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(context.find("answer")));

  // and stops working after we pop it again
  context.pop();
  CPPUNIT_ASSERT(context.find("answer").empty());

  // the styles are arranged in a stack -- a property is found in
  // the last pushed one
  KEYPropertyMap props2;
  props2.set("answer", 1);
  context.push(makeStyle(props2));
  context.push(makeStyle(props));
  CPPUNIT_ASSERT(!context.find("answer").empty());
  CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(context.find("answer")));

  context.pop();
  context.pop();
  CPPUNIT_ASSERT(context.find("answer").empty());

  // lookup works recursively through more levels
  context.push(makeStyle(props));
  context.push(makeStyle(KEYPropertyMap()));
  CPPUNIT_ASSERT(!context.find("answer").empty());
  CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(context.find("answer")));

  // yet another one...
  context.push(makeStyle(KEYPropertyMap()));
  CPPUNIT_ASSERT(!context.find("answer").empty());
  CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(context.find("answer")));
}

CPPUNIT_TEST_SUITE_REGISTRATION(KEYStyleContextTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
