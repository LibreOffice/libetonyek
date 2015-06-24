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
  CPPUNIT_TEST(testAddressRanges);
  CPPUNIT_TEST(testOperators);
  CPPUNIT_TEST(testFunctions);
  CPPUNIT_TEST(testExpressions);
  CPPUNIT_TEST_SUITE_END();

private:
  void testNumbers();
  void testCellReferences();
  void testAddressRanges();
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
    CPPUNIT_ASSERT(formula.parse(testFormula));
    CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  }

}

void IWORKFormulaTest::testCellReferences()
{
  IWORKFormula formula;

  // both row and column absolute
  {
    const string testFormula = "=$B$2";
    CPPUNIT_ASSERT(formula.parse(testFormula));
    CPPUNIT_ASSERT_EQUAL(string("=[.$B$2]"), formula.toString());
  }

  // column absolute
  {
    const string testFormula = "=$AH91";
    CPPUNIT_ASSERT(formula.parse(testFormula));
    CPPUNIT_ASSERT_EQUAL(string("=[.$AH91]"), formula.toString());
  }

  // row absolute
  {
    const string testFormula = "=Z$4";
    CPPUNIT_ASSERT(formula.parse(testFormula));
    CPPUNIT_ASSERT_EQUAL(string("=[.Z$4]"), formula.toString());
  }

  // both row and column relative
  {
    const string testFormula = "=R34";
    CPPUNIT_ASSERT(formula.parse(testFormula));
    CPPUNIT_ASSERT_EQUAL(string("=[.R34]"), formula.toString());
  }

  // sheet with table and cell
  {
    const string testFormula = "=HOME.Table1.B5";
    CPPUNIT_ASSERT(formula.parse(testFormula));
    CPPUNIT_ASSERT_EQUAL(string("=[HOME.Table1.B5]"), formula.toString());
  }

  // table and cell
  {
    const string testFormula = "=Table1.$B3";
    CPPUNIT_ASSERT(formula.parse(testFormula));
    // CPPUNIT_ASSERT_EQUAL(string("=[Table1.$B3]"), formula.toString());
  }

}

void IWORKFormulaTest::testAddressRanges()
{
  // IWORKFormula formula;

  // range
  // {
  //   const string testFormula = "=$A4:$A81";
  //   CPPUNIT_ASSERT(formula.parse(testFormula));
  //   CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  // }

  // sheet with table and cell
  // {
  //   const string testFormula = "=HOME.Table1.B5:HOME.Table1.B20";
  //   CPPUNIT_ASSERT(formula.parse(testFormula));
  //   CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  // }

  // table and cell
  // {
  //   const string testFormula = "=Table1.$B3:Table1.$B20";
  //   CPPUNIT_ASSERT(formula.parse(testFormula));
  //   CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  // }

}


void IWORKFormulaTest::testOperators()
{
  IWORKFormula formula;

  //Unary
  {
    const string testFormula = "=-C10";
    CPPUNIT_ASSERT(formula.parse(testFormula));
    CPPUNIT_ASSERT_EQUAL(string("=-[.C10]"), formula.toString());
  }

  //Binary
  // {
  //   const string testFormula = "=B5+B6";
  //
  //   CPPUNIT_ASSERT(formula.parse(testFormula));
  //   CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  // }

}

void IWORKFormulaTest::testFunctions()
{
  IWORKFormula formula;

  // function with cell address
  {
    const string testFormula = "=ABS($B12)";
    CPPUNIT_ASSERT(formula.parse(testFormula));
    CPPUNIT_ASSERT_EQUAL(string("=ABS([.$B12])"), formula.toString());
  }

  // function with address range
  // {
  //   const string testFormula = "=SUM($B5:$B16)";
  //   CPPUNIT_ASSERT(formula.parse(testFormula));
  //   CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  // }

  // function with multiple arguments
  // {
  //   const string testFormula = "=COUNTIFS(A1:A38, ">100",A1:A38, "<=200")";
  //   CPPUNIT_ASSERT(formula.parse(testFormula));
  //   CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  // }

}

void IWORKFormulaTest::testExpressions()
{
  // IWORKFormula formula;

  // if-else
  // {
  //   const string testFormula = "IF((R1+R2)<45, R1+R2, 50)";
  //   CPPUNIT_ASSERT(formula.parse(testFormula));
  //   CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  // }

  // multiple sheet and table cell operation
  // {
  //   const string testFormula = "=HOME.Table1.B6+OFFICE.Table1.B6-WAREHOUSE.Table1.B6";
  //   CPPUNIT_ASSERT(formula.parse(testFormula));
  //   CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  // }

  // function with only column
  // {
  //   const string testFormula = "=SUM(B)";
  //   CPPUNIT_ASSERT(formula.parse(testFormula));
  //   CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());
  // }

}

CPPUNIT_TEST_SUITE_REGISTRATION(IWORKFormulaTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
