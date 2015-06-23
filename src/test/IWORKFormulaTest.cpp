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
  CPPUNIT_TEST(testNumbers);
  CPPUNIT_TEST(testCellReferences);
  CPPUNIT_TEST(testOperators);
  CPPUNIT_TEST(testFunctions);
  CPPUNIT_TEST(testExpressions);
  CPPUNIT_TEST_SUITE_END();

private:
  void testNumbers();
  void testCellReferences();
  void testOperators();
  void testFunctions();
  void testExpressions();
};

void IWORKFormulaTest::setUp()
{
}

void IWORKFormulaTest::tearDown()
{
}

void IWORKFormulaTest::testNumbers()
{
  IWORKFormula formula;

  // number
  {
    const string testFormula = "=4";
    bool test = formula.parse(testFormula);
    CPPUNIT_ASSERT(test);
    CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  }

}

void IWORKFormulaTest::testCellReferences()
{
  IWORKFormula formula;

  // both row and column absolute
  {
    const string testFormula = "=$B$2";
    bool test = formula.parse(testFormula);
    CPPUNIT_ASSERT(test);
    // CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  }

  // column absolute
  {
    const string testFormula = "=$AH91";
    bool test = formula.parse(testFormula);
    CPPUNIT_ASSERT(test);
    // CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  }

  // row absolute
  {
    const string testFormula = "=Z$4";
    bool test = formula.parse(testFormula);
    CPPUNIT_ASSERT(test);
    // CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  }

  // both row and column relative
  {
    const string testFormula = "=R34";
    bool test = formula.parse(testFormula);
    CPPUNIT_ASSERT(test);
    // CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  }

  // address range
  // {
  //   const string testFormula = "=FUNC($A4:$A81)";
  //   bool test = formula.parse(testFormula);
  //   CPPUNIT_ASSERT(test);
  //   CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  // }

  // sheet with table and cell
  {
    const string testFormula = "=HOME.Table1.B5";
    bool test = formula.parse(testFormula);
    CPPUNIT_ASSERT(test);
    // CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  }

  // table and cell
  {
    const string testFormula = "=Table1.$B3";
    bool test = formula.parse(testFormula);
    CPPUNIT_ASSERT(test);
    // CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  }

}

void IWORKFormulaTest::testOperators()
{
  IWORKFormula formula;

  //Unary
  {
    const string testFormula = "=-C10";
    bool test = formula.parse(testFormula);
    CPPUNIT_ASSERT(test);
    // CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  }

  //Binary
  // {
  //   const string testFormula = "=B5+B6";
  //   bool test = formula.parse(testFormula);
  //   CPPUNIT_ASSERT(test);
  //   CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  // }

}

void IWORKFormulaTest::testFunctions()
{
  IWORKFormula formula;

  // function1 with cell address
  {
    const string testFormula = "=ABS($B12)";
    bool test = formula.parse(testFormula);
    CPPUNIT_ASSERT(test);
    // CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  }

  // function2 with cell address
  {
    const string testFormula = "=ISODD($R3)";
    bool test = formula.parse(testFormula);
    CPPUNIT_ASSERT(test);
    // CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  }

  // function with address range
  // {
  //   const string testFormula = "=SUM($B5:$B16)";
  //   bool test = formula.parse(testFormula);
  //   CPPUNIT_ASSERT(test);
  //   CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  // }

}

void IWORKFormulaTest::testExpressions()
{
  // IWORKFormula formula;

  // if-else
  // {
  //   const string testFormula = "IF((R1+R2)<45, R1+R2, 50)";
  //   bool test = formula.parse(testFormula);
  //   CPPUNIT_ASSERT(test);
  //   CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  // }

  // multiple sheet and table cell operation
  // {
  //   const string testFormula = "=HOME.Table1.B6+OFFICE.Table1.B6-WAREHOUSE.Table1.B6";
  //   bool test = formula.parse(testFormula);
  //   CPPUNIT_ASSERT(test);
  //   CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  // }

}

CPPUNIT_TEST_SUITE_REGISTRATION(IWORKFormulaTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
