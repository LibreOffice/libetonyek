/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KN2TOKENTEST_H_INCLUDED
#define KN2TOKENTEST_H_INCLUDED

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

namespace test
{

class KN2TokenTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(KN2TokenTest);
  CPPUNIT_TEST(testTokenizer);
  CPPUNIT_TEST_SUITE_END();

private:
  void testTokenizer();
};

}

#endif // KN2TOKENTEST_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
