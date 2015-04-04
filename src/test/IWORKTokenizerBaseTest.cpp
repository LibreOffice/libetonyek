/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "IWORKTokenizerBase.h"

using namespace libetonyek;

namespace test
{

namespace
{

const int TOKEN_A = 1;
const int TOKEN_B = 2;
const int TOKEN_NS = 1 << 8;

class TestTokenizer : public IWORKTokenizerBase
{
private:
  virtual int queryId(const char *name) const;
};

int TestTokenizer::queryId(const char *const name) const
{
  assert(name);

  const std::string nameStr(name);

  assert(!nameStr.empty());

  if ("a" == nameStr)
    return TOKEN_A;
  else if ("b" == nameStr)
    return TOKEN_B;
  else if ("ns" == nameStr)
    return TOKEN_NS;

  return 0;
}

}

class IWORKTokenizerBaseTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(IWORKTokenizerBaseTest);
  CPPUNIT_TEST(testNormal);
  CPPUNIT_TEST(testQualified);
  CPPUNIT_TEST_SUITE_END();

private:
  void testNormal();
  void testQualified();
};

void IWORKTokenizerBaseTest::setUp()
{
}

void IWORKTokenizerBaseTest::tearDown()
{
}

void IWORKTokenizerBaseTest::testNormal()
{
  const TestTokenizer tok;

  // known tokens
  CPPUNIT_ASSERT_EQUAL(TOKEN_A, tok.getId("a"));
  CPPUNIT_ASSERT_EQUAL(TOKEN_B, tok.getId("b"));
  CPPUNIT_ASSERT_EQUAL(TOKEN_NS, tok.getId("ns"));

  // unknown token
  CPPUNIT_ASSERT_EQUAL(0, tok.getId("foo"));

  // empty tokens are handled gracefully
  CPPUNIT_ASSERT_EQUAL(0, tok.getId(""));
  CPPUNIT_ASSERT_EQUAL(0, tok.getId(0));
}

void IWORKTokenizerBaseTest::testQualified()
{
  const TestTokenizer tok;

  // known name + namespace combinations
  CPPUNIT_ASSERT_EQUAL(TOKEN_A | TOKEN_NS, tok.getQualifiedId("a", "ns"));

  // unknown name + namespace combinations
  CPPUNIT_ASSERT_EQUAL(0, tok.getQualifiedId("a", "zzz"));
  CPPUNIT_ASSERT_EQUAL(0, tok.getQualifiedId("x", "ns"));
  CPPUNIT_ASSERT_EQUAL(0, tok.getQualifiedId("x", "zzz"));

  // empty namespace
  CPPUNIT_ASSERT_EQUAL(TOKEN_A, tok.getQualifiedId("a", ""));
  CPPUNIT_ASSERT_EQUAL(TOKEN_A, tok.getQualifiedId("a", 0));
}

CPPUNIT_TEST_SUITE_REGISTRATION(IWORKTokenizerBaseTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
