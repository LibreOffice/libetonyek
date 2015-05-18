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

#include "IWORKChainedTokenizer.h"
#include "IWORKTokenizerBase.h"

using namespace libetonyek;

namespace test
{

namespace
{

const int TOKEN_A = 1;
const int TOKEN_B = 2;
const int TOKEN_CA = 3;
const int TOKEN_CB = 4;
const int TOKEN_NS_A = 1 << 8;
const int TOKEN_NS_B = 2 << 8;

class TokenizerA : public IWORKTokenizerBase
{
public:
  TokenizerA();

private:
  virtual int queryId(const char *name) const;
};

TokenizerA::TokenizerA()
{
}

int TokenizerA::queryId(const char *const name) const
{
  assert(name);

  const std::string nameStr(name);

  assert(!nameStr.empty());

  if ("a" == nameStr)
    return TOKEN_A;
  else if ("c" == nameStr)
    return TOKEN_CA;
  else if ("nsa" == nameStr)
    return TOKEN_NS_A;

  return 0;
}

class TokenizerB : public IWORKTokenizerBase
{
public:
  TokenizerB();

private:
  virtual int queryId(const char *name) const;
};

TokenizerB::TokenizerB()
{
}

int TokenizerB::queryId(const char *const name) const
{
  assert(name);

  const std::string nameStr(name);

  assert(!nameStr.empty());

  if ("b" == nameStr)
    return TOKEN_B;
  else if ("c" == nameStr)
    return TOKEN_CB;
  else if ("nsb" == nameStr)
    return TOKEN_NS_B;

  return 0;
}

}

class IWORKChainedTokenizerTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(IWORKChainedTokenizerTest);
  CPPUNIT_TEST(testNormal);
  CPPUNIT_TEST(testQualified);
  CPPUNIT_TEST_SUITE_END();

private:
  void testNormal();
  void testQualified();
};

void IWORKChainedTokenizerTest::setUp()
{
}

void IWORKChainedTokenizerTest::tearDown()
{
}

void IWORKChainedTokenizerTest::testNormal()
{
  const TokenizerA tokenizerA;
  const TokenizerB tokenizerB;
  const IWORKChainedTokenizer tok(tokenizerA, tokenizerB);

  // known tokens
  CPPUNIT_ASSERT_EQUAL(TOKEN_A, tok.getId("a"));
  CPPUNIT_ASSERT_EQUAL(TOKEN_B, tok.getId("b"));
  CPPUNIT_ASSERT_EQUAL(TOKEN_CA, tok.getId("c"));
  CPPUNIT_ASSERT_EQUAL(TOKEN_NS_A, tok.getId("nsa"));
  CPPUNIT_ASSERT_EQUAL(TOKEN_NS_B, tok.getId("nsb"));

  // unknown token
  CPPUNIT_ASSERT_EQUAL(0, tok.getId("foo"));

  // empty tokens are handled gracefully
  CPPUNIT_ASSERT_EQUAL(0, tok.getId(""));
  CPPUNIT_ASSERT_EQUAL(0, tok.getId(0));
}

void IWORKChainedTokenizerTest::testQualified()
{
  const TokenizerA tokenizerA;
  const TokenizerB tokenizerB;
  const IWORKChainedTokenizer tok(tokenizerA, tokenizerB);

  // known name + namespace combinations
  CPPUNIT_ASSERT_EQUAL(TOKEN_A | TOKEN_NS_A, tok.getQualifiedId("a", "nsa"));
  CPPUNIT_ASSERT_EQUAL(TOKEN_B | TOKEN_NS_B, tok.getQualifiedId("b", "nsb"));
  CPPUNIT_ASSERT_EQUAL(TOKEN_CA | TOKEN_NS_A, tok.getQualifiedId("c", "nsa"));
  CPPUNIT_ASSERT_EQUAL(TOKEN_CB | TOKEN_NS_B, tok.getQualifiedId("c", "nsb"));

  // unknown name + namespace combinations
  CPPUNIT_ASSERT_EQUAL(0, tok.getQualifiedId("a", "zzz"));
  CPPUNIT_ASSERT_EQUAL(0, tok.getQualifiedId("x", "nsa"));
  CPPUNIT_ASSERT_EQUAL(0, tok.getQualifiedId("x", "zzz"));

  // mismatched name + namespace
  CPPUNIT_ASSERT_EQUAL(0, tok.getQualifiedId("a", "nsb"));
  CPPUNIT_ASSERT_EQUAL(0, tok.getQualifiedId("b", "nsa"));

  // empty name and/or namespace
  CPPUNIT_ASSERT_EQUAL(TOKEN_A, tok.getQualifiedId("a", ""));
  CPPUNIT_ASSERT_EQUAL(TOKEN_A, tok.getQualifiedId("a", 0));
  CPPUNIT_ASSERT_EQUAL(TOKEN_B, tok.getQualifiedId("b", ""));
  CPPUNIT_ASSERT_EQUAL(TOKEN_B, tok.getQualifiedId("b", 0));
  CPPUNIT_ASSERT_EQUAL(TOKEN_CA, tok.getQualifiedId("c", ""));
  CPPUNIT_ASSERT_EQUAL(TOKEN_CA, tok.getQualifiedId("c", 0));
  CPPUNIT_ASSERT_EQUAL(0, tok.getQualifiedId("", "nsa"));
  CPPUNIT_ASSERT_EQUAL(0, tok.getQualifiedId(0, "nsa"));
  CPPUNIT_ASSERT_EQUAL(0, tok.getQualifiedId("", ""));
  CPPUNIT_ASSERT_EQUAL(0, tok.getQualifiedId(0, ""));
  CPPUNIT_ASSERT_EQUAL(0, tok.getQualifiedId("", 0));
  CPPUNIT_ASSERT_EQUAL(0, tok.getQualifiedId(0, 0));
}

CPPUNIT_TEST_SUITE_REGISTRATION(IWORKChainedTokenizerTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
