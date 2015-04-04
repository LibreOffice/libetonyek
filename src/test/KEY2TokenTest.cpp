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

#include "libetonyek_xml.h"
#include "IWORKChainedTokenizer.h"
#include "IWORKToken.h"
#include "IWORKTokenizer.h"
#include "KEY2Token.h"

using namespace libetonyek;

namespace test
{

namespace
{

const char *const KEY_NS = "http://developer.apple.com/namespaces/keynote2";
const char *const SF_NS = "http://developer.apple.com/namespaces/sf";

void testTokenizer(const IWORKTokenizer &tok)
{
  // known tokens
  CPPUNIT_ASSERT(KEY2Token::presentation == tok.getId("presentation"));
  CPPUNIT_ASSERT(KEY2Token::version == tok.getId("version"));
  CPPUNIT_ASSERT(KEY2Token::double_ == tok.getId("double"));

  // unknown tokens
  CPPUNIT_ASSERT(KEY2Token::INVALID_TOKEN == tok.getId("bflmpsvz"));
  CPPUNIT_ASSERT(KEY2Token::INVALID_TOKEN == tok.getId("abcdefghijklmnopqrstuvwxyz"));

  // empty token - maps to empty namespace
  CPPUNIT_ASSERT(0 == tok.getId(""));

  // known name + namespace combinations
  CPPUNIT_ASSERT((KEY2Token::NS_URI_KEY | KEY2Token::presentation) == tok.getQualifiedId("presentation", KEY_NS));
  CPPUNIT_ASSERT((KEY2Token::NS_URI_KEY | KEY2Token::version) == tok.getQualifiedId("version", KEY_NS));

  // unknown name + namespace combinations
  CPPUNIT_ASSERT(KEY2Token::INVALID_TOKEN == tok.getQualifiedId("presentation", SF_NS));
  CPPUNIT_ASSERT(KEY2Token::INVALID_TOKEN == tok.getQualifiedId("double", SF_NS));
}

}

class KEY2TokenTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(KEY2TokenTest);
  CPPUNIT_TEST(testSimpleTokenizer);
  CPPUNIT_TEST(testChainedTokenizer);
  CPPUNIT_TEST_SUITE_END();

private:
  void testSimpleTokenizer();
  void testChainedTokenizer();
};

void KEY2TokenTest::setUp()
{
}

void KEY2TokenTest::tearDown()
{
}

void KEY2TokenTest::testSimpleTokenizer()
{
  const IWORKTokenizer &tok(KEY2Token::getTokenizer());

  testTokenizer(tok);

  CPPUNIT_ASSERT(KEY2Token::INVALID_TOKEN == tok.getId("style"));
}

void KEY2TokenTest::testChainedTokenizer()
{
  const IWORKChainedTokenizer tok(KEY2Token::getTokenizer(), IWORKToken::getTokenizer());

  testTokenizer(tok);

  CPPUNIT_ASSERT(IWORKToken::style == tok.getId("style"));
}

CPPUNIT_TEST_SUITE_REGISTRATION(KEY2TokenTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
