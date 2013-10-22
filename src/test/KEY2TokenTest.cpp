/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY2Token.h"

#include "KEY2TokenTest.h"

using namespace libetonyek;

namespace test
{

void KEY2TokenTest::setUp()
{
}

void KEY2TokenTest::tearDown()
{
}

void KEY2TokenTest::testTokenizer()
{
  KEY2Tokenizer tok;

  // known tokens
  CPPUNIT_ASSERT(KEY2Token::presentation == tok("presentation"));
  CPPUNIT_ASSERT(KEY2Token::a == tok("a"));
  CPPUNIT_ASSERT(KEY2Token::_0 == tok("0"));

  // unknown tokens
  CPPUNIT_ASSERT(KEY2Token::INVALID_TOKEN == tok("bflmpsvz"));
  CPPUNIT_ASSERT(KEY2Token::INVALID_TOKEN == tok("abcdefghijklmnopqrstuvwxyz"));

  // empty token - maps to empty namespace
  CPPUNIT_ASSERT(KEY2Token::NS_NONE == tok(""));
}

CPPUNIT_TEST_SUITE_REGISTRATION(KEY2TokenTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
