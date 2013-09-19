/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KN2Token.h"

#include "KN2TokenTest.h"

using namespace libkeynote;

namespace test
{

void KN2TokenTest::setUp()
{
}

void KN2TokenTest::tearDown()
{
}

void KN2TokenTest::testTokenizer()
{
  KN2Tokenizer tok;

  // known tokens
  CPPUNIT_ASSERT(KN2Token::presentation == tok("presentation"));
  CPPUNIT_ASSERT(KN2Token::a == tok("a"));
  CPPUNIT_ASSERT(KN2Token::_0 == tok("0"));

  // unknown tokens
  CPPUNIT_ASSERT(KN2Token::INVALID_TOKEN == tok("bflmpsvz"));
  CPPUNIT_ASSERT(KN2Token::INVALID_TOKEN == tok("abcdefghijklmnopqrstuvwxyz"));

  // empty token - maps to empty namespace
  CPPUNIT_ASSERT(KN2Token::NS_NONE == tok(""));
}

CPPUNIT_TEST_SUITE_REGISTRATION(KN2TokenTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
