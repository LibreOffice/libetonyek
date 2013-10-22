/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY2TOKENTEST_H_INCLUDED
#define KEY2TOKENTEST_H_INCLUDED

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

namespace test
{

class KEY2TokenTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(KEY2TokenTest);
  CPPUNIT_TEST(testTokenizer);
  CPPUNIT_TEST_SUITE_END();

private:
  void testTokenizer();
};

}

#endif // KEY2TOKENTEST_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
