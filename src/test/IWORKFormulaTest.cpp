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

#include "IWORKFormula.h"

using libetonyek::IWORKFormula;

using std::string;

namespace test
{

class IWORKFormulaTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(IWORKFormulaTest);
  CPPUNIT_TEST(testParse);
  CPPUNIT_TEST_SUITE_END();

private:
  void testParse();
};

void IWORKFormulaTest::setUp()
{
}

void IWORKFormulaTest::tearDown()
{
}

void IWORKFormulaTest::testParse()
{
  IWORKFormula formula;

  //Abs
  {
    const string testFormula = "=ABS($B2)";
    bool test = formula.parse(testFormula);
    CPPUNIT_ASSERT(test);
  }

  //Sum
  // {
  //   const string testFormula = "=SUM($B2:$B8)";
  //   bool test = formula.parse(testFormula);
  //   CPPUNIT_ASSERT(test);
  // }

}


CPPUNIT_TEST_SUITE_REGISTRATION(IWORKFormulaTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
