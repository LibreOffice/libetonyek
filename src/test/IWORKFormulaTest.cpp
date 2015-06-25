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
  CPPUNIT_ASSERT(formula.parse("=$B$2"));
  CPPUNIT_ASSERT_EQUAL(string("=[.$B$2]"), formula.toString());

  // column absolute
  CPPUNIT_ASSERT(formula.parse("=$AH91"));
  CPPUNIT_ASSERT_EQUAL(string("=[.$AH91]"), formula.toString());

  // row absolute
  CPPUNIT_ASSERT(formula.parse("=Z$4"));
  CPPUNIT_ASSERT_EQUAL(string("=[.Z$4]"), formula.toString());

  // both row and column relative
  CPPUNIT_ASSERT(formula.parse("=R34"));
  CPPUNIT_ASSERT_EQUAL(string("=[.R34]"), formula.toString());

  // sheet with table and cell
  CPPUNIT_ASSERT(formula.parse("=HOME.Table1.B5"));
  CPPUNIT_ASSERT_EQUAL(string("=[HOME.Table1.B5]"), formula.toString());

  // table and cell
  CPPUNIT_ASSERT(formula.parse("=Table1.$B3"));
  // CPPUNIT_ASSERT_EQUAL(string("=[Table1.$B3]"), formula.toString());

}

void IWORKFormulaTest::testAddressRanges()
{
  IWORKFormula formula;

  // range
  CPPUNIT_ASSERT(formula.parse("=$A4:$A81"));
  CPPUNIT_ASSERT_EQUAL(string("=[.$A4]:[.$A81]"), formula.toString());

  // sheet with table and cell
  CPPUNIT_ASSERT(formula.parse("=HOME.Table1.B5:HOME.Table1.B20"));
  CPPUNIT_ASSERT_EQUAL(string("=[HOME.Table1.B5]:[HOME.Table1.B20]"), formula.toString());

  // table and cell
  CPPUNIT_ASSERT(formula.parse("=Table1.$B3:Table1.$B20"));
  CPPUNIT_ASSERT_EQUAL(string("=[Table1.$B3:Table1.$B20]"), formula.toString());

}


void IWORKFormulaTest::testOperators()
{
  IWORKFormula formula;

  //Unary
  CPPUNIT_ASSERT(formula.parse("=-C10"));
  CPPUNIT_ASSERT_EQUAL(string("=-[.C10]"), formula.toString());

  // binary
  CPPUNIT_ASSERT(formula.parse("=B5+B6"));
  CPPUNIT_ASSERT_EQUAL(string("=[.B5]+[.B6]"), formula.toString());

}

void IWORKFormulaTest::testFunctions()
{
  IWORKFormula formula;

  // function with cell address
  CPPUNIT_ASSERT(formula.parse("=ABS($B12)"));
  CPPUNIT_ASSERT_EQUAL(string("=ABS([.$B12])"), formula.toString());

  // function with address range
  CPPUNIT_ASSERT(formula.parse("=SUM($B5:$B16)"));
  CPPUNIT_ASSERT_EQUAL(string("=SUM([.$B5]:[.$B16])"), formula.toString());

  // function with multiple arguments
  CPPUNIT_ASSERT(formula.parse("=COUNTIFS(A1:A38,'>100',A1:A38,'<=200')"));
  // CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());

}

void IWORKFormulaTest::testExpressions()
{
  IWORKFormula formula;

  // if-else
  // CPPUNIT_ASSERT(formula.parse("=IF((R1+R2)<45,R1+R2,50)"));
  // CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());

  // multiple sheet and table cell operation
  CPPUNIT_ASSERT(formula.parse("=HOME.Table1.B6+OFFICE.Table1.B6-WAREHOUSE.Table1.B6"));
  CPPUNIT_ASSERT_EQUAL(string("=[HOME.Table1.B6]+[OFFICE.Table1.B6]-[WAREHOUSE.Table1.B6]"), formula.toString());

  // function with only column
  //   CPPUNIT_ASSERT(formula.parse("=SUM(B)"));
  //   CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());

}

CPPUNIT_TEST_SUITE_REGISTRATION(IWORKFormulaTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
