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
  CPPUNIT_TEST(testStrings);
  CPPUNIT_TEST(testCellReferences);
  CPPUNIT_TEST(testAddressRanges);
  CPPUNIT_TEST(testOperators);
  CPPUNIT_TEST(testFunctions);
  CPPUNIT_TEST(testExpressions);
  CPPUNIT_TEST(testInvalid);
  CPPUNIT_TEST_SUITE_END();

private:
  void testNumbers();
  void testStrings();
  void testCellReferences();
  void testAddressRanges();
  void testOperators();
  void testFunctions();
  void testExpressions();
  void testInvalid();
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

void IWORKFormulaTest::testStrings()
{
  IWORKFormula formula;

  // hello
  CPPUNIT_ASSERT(formula.parse("=\"hello\""));
  CPPUNIT_ASSERT_EQUAL(string("=hello"), formula.toString());

  // function
  CPPUNIT_ASSERT(formula.parse("=NUMBER(\"42\")"));
  CPPUNIT_ASSERT_EQUAL(string("=NUMBER(42)"), formula.toString());

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

  // table and cell
  CPPUNIT_ASSERT(formula.parse("=Table1::$B3"));
  CPPUNIT_ASSERT_EQUAL(string("=[Table1.$B3]"), formula.toString());

}

void IWORKFormulaTest::testAddressRanges()
{
  IWORKFormula formula;

  // range
  CPPUNIT_ASSERT(formula.parse("=$A4:$A81"));
  CPPUNIT_ASSERT_EQUAL(string("=[.$A4:.$A81]"), formula.toString());

  // table and cell
  CPPUNIT_ASSERT(formula.parse("=Table1::$B3:Table1::$B20"));
  CPPUNIT_ASSERT_EQUAL(string("=[Table1.$B3:Table1.$B20]"), formula.toString());

}


void IWORKFormulaTest::testOperators()
{
  IWORKFormula formula;

  // prefix
  CPPUNIT_ASSERT(formula.parse("=-C10"));
  CPPUNIT_ASSERT_EQUAL(string("=-[.C10]"), formula.toString());

  // infix
  CPPUNIT_ASSERT(formula.parse("=B5+B6"));
  CPPUNIT_ASSERT_EQUAL(string("=[.B5]+[.B6]"), formula.toString());

  //postfix
  CPPUNIT_ASSERT(formula.parse("=100%"));
  CPPUNIT_ASSERT_EQUAL(string("=100%"), formula.toString());

  // equal
  CPPUNIT_ASSERT(formula.parse("=B5=B6"));
  CPPUNIT_ASSERT_EQUAL(string("=[.B5]=[.B6]"), formula.toString());

  // not equal
  CPPUNIT_ASSERT(formula.parse("=B5<>B6"));
  CPPUNIT_ASSERT_EQUAL(string("=[.B5]<>[.B6]"), formula.toString());

}

void IWORKFormulaTest::testFunctions()
{
  IWORKFormula formula;

  // function with cell address
  CPPUNIT_ASSERT(formula.parse("=ABS($B12)"));
  CPPUNIT_ASSERT_EQUAL(string("=ABS([.$B12])"), formula.toString());

  // function with address range
  CPPUNIT_ASSERT(formula.parse("=SUM($B5:$B16)"));
  CPPUNIT_ASSERT_EQUAL(string("=SUM([.$B5:.$B16])"), formula.toString());

  // function with multiple arguments
  CPPUNIT_ASSERT(formula.parse("=COUNTIFS(A1:A38,\">100\",A1:A38,\"<=200\")"));
  CPPUNIT_ASSERT_EQUAL(string("=COUNTIFS([.A1:.A38]>100[.A1:.A38]<=200)"), formula.toString());

  // function with number in name
  CPPUNIT_ASSERT(formula.parse("=HEX2OCT(18)"));
  CPPUNIT_ASSERT_EQUAL(string("=HEX2OCT(18)"), formula.toString());

}

void IWORKFormulaTest::testExpressions()
{
  IWORKFormula formula;

  // if-else
  // CPPUNIT_ASSERT(formula.parse("=IF((R1+R2)<45,R1+R2,50)"));
  // CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());

  // multiple sheet and table cell operation
  CPPUNIT_ASSERT(formula.parse("=Table1::B6+Table2::B6-Table3::B6"));
  CPPUNIT_ASSERT_EQUAL(string("=[Table1.B6]+[Table2.B6]-[Table3.B6]"), formula.toString());

  // basic paranthesized
  // CPPUNIT_ASSERT(formula.parse("=(23)+$B6"));
  // CPPUNIT_ASSERT_EQUAL(string("=(23)+[.$B6]"), formula.toString());

  // paranthesized with function
  // CPPUNIT_ASSERT(formula.parse("=(23)+$B6+(ABS(($Z7)))"));
  // CPPUNIT_ASSERT_EQUAL(string("=(23)+[.$B6]+(ABS(([.$Z7])))"), formula.toString());

  // paranthesize with operators
  // CPPUNIT_ASSERT(formula.parse("=((-23)+4)*(B7-2)"));
  // CPPUNIT_ASSERT_EQUAL(string("=((-23)+4)*([.B7]-2)"), formula.toString());

  // function with only column
  //   CPPUNIT_ASSERT(formula.parse("=SUM(B)"));
  //   CPPUNIT_ASSERT_EQUAL(testFormula, formula.toString());

}

void IWORKFormulaTest::testInvalid()
{
  IWORKFormula formula;

  // doesn't start with equal sign
  CPPUNIT_ASSERT(!formula.parse("4"));

  // invalid cell reference
  CPPUNIT_ASSERT(!formula.parse("=$B23B"));

  // invalid cell range
  CPPUNIT_ASSERT(!formula.parse("=SUM($B2:$B23X)"));

  // missing unary operand
  CPPUNIT_ASSERT(!formula.parse("=-"));

  // missing binary operand
  CPPUNIT_ASSERT(!formula.parse("=4="));

  // missing paranthesis
  CPPUNIT_ASSERT(!formula.parse("=(23*B3)+(7-2"));

  // invalid table cell seperator
  CPPUNIT_ASSERT(!formula.parse("=SHEET1;B5"));

  // invalid function argument seperator
  CPPUNIT_ASSERT(!formula.parse("=IF((R1+R2)<45;R1+R2,50)"));
}

CPPUNIT_TEST_SUITE_REGISTRATION(IWORKFormulaTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
