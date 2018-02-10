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

using boost::none;

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
  CPPUNIT_TEST(testOperators);
  CPPUNIT_TEST(testFunctions);
  CPPUNIT_TEST(testExpressions);
  CPPUNIT_TEST(testInvalid);
  CPPUNIT_TEST_SUITE_END();

private:
  void testNumbers();
  void testStrings();
  void testCellReferences();
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
  IWORKFormula formula(none);

  // number
  {
    const string testFormula = "=4";
    CPPUNIT_ASSERT(formula.parse(testFormula));
    CPPUNIT_ASSERT_EQUAL(testFormula, formula.str(none));
  }

}

void IWORKFormulaTest::testStrings()
{
  IWORKFormula formula(none);

  // hello
  CPPUNIT_ASSERT(formula.parse("=\"hello\""));
  CPPUNIT_ASSERT_EQUAL(string("=hello"), formula.str(none));

  // function
  CPPUNIT_ASSERT(formula.parse("=NUMBER(\"42\")"));
  CPPUNIT_ASSERT_EQUAL(string("=NUMBER(42)"), formula.str(none));

}

void IWORKFormulaTest::testCellReferences()
{
  IWORKFormula formula(none);

  // both row and column absolute
  CPPUNIT_ASSERT(formula.parse("=$B$2"));
  CPPUNIT_ASSERT_EQUAL(string("=[.$B$2]"), formula.str(none));

  // column absolute
  CPPUNIT_ASSERT(formula.parse("=$AH91"));
  CPPUNIT_ASSERT_EQUAL(string("=[.$AH91]"), formula.str(none));

  // row absolute
  CPPUNIT_ASSERT(formula.parse("=Z$4"));
  CPPUNIT_ASSERT_EQUAL(string("=[.Z$4]"), formula.str(none));

  // both row and column relative
  CPPUNIT_ASSERT(formula.parse("=R34"));
  CPPUNIT_ASSERT_EQUAL(string("=[.R34]"), formula.str(none));

  // table and cell
  CPPUNIT_ASSERT(formula.parse("=Table1::$B3"));
  CPPUNIT_ASSERT_EQUAL(string("=[Table1.$B3]"), formula.str(none));

}

void IWORKFormulaTest::testOperators()
{
  IWORKFormula formula(none);

  // prefix
  CPPUNIT_ASSERT(formula.parse("=-C10"));
  CPPUNIT_ASSERT_EQUAL(string("=-[.C10]"), formula.str(none));

  // infix
  CPPUNIT_ASSERT(formula.parse("=B5+B6"));
  CPPUNIT_ASSERT_EQUAL(string("=[.B5]+[.B6]"), formula.str(none));

  //postfix
  CPPUNIT_ASSERT(formula.parse("=100%"));
  CPPUNIT_ASSERT_EQUAL(string("=100%"), formula.str(none));

  // equal
  CPPUNIT_ASSERT(formula.parse("=B5=B6"));
  CPPUNIT_ASSERT_EQUAL(string("=[.B5]=[.B6]"), formula.str(none));

  // not equal
  CPPUNIT_ASSERT(formula.parse("=B5<>B6"));
  CPPUNIT_ASSERT_EQUAL(string("=[.B5]<>[.B6]"), formula.str(none));

}

void IWORKFormulaTest::testFunctions()
{
  IWORKFormula formula(none);

  // function with cell address
  CPPUNIT_ASSERT(formula.parse("=ABS($B12)"));
  CPPUNIT_ASSERT_EQUAL(string("=ABS([.$B12])"), formula.str(none));

  // function with address range
  CPPUNIT_ASSERT(formula.parse("=SUM($B5:$B16)"));
  // CPPUNIT_ASSERT_EQUAL(string("=SUM([.$B5:.$B16])"), formula.str(none)); // FIXME

  // function with address range in paranthesis
  CPPUNIT_ASSERT(formula.parse("=SUM(($B5:$B16))"));
  // CPPUNIT_ASSERT_EQUAL(string("=SUM([.$B5:.$B16])"), formula.str(none)); // FIXME

  //function with address range table and cell
  CPPUNIT_ASSERT(formula.parse("=SUM(Table1::$B3:Table1::$B20)"));
  CPPUNIT_ASSERT_EQUAL(string("=SUM([Table1.$B3:Table1.$B20])"), formula.str(none));

  // function with multiple arguments
  CPPUNIT_ASSERT(formula.parse("=COUNTIFS(A1:A38,\">100\",A1:A38,\"<=200\")"));
  CPPUNIT_ASSERT_EQUAL(string("=COUNTIFS([.A1:.A38];>100;[.A1:.A38];<=200)"), formula.str(none));

  // function with number in name
  CPPUNIT_ASSERT(formula.parse("=HEX2OCT(18)"));
  CPPUNIT_ASSERT_EQUAL(string("=HEX2OCT(18)"), formula.str(none));

}

void IWORKFormulaTest::testExpressions()
{
  IWORKFormula formula(none);

  // if-else
  CPPUNIT_ASSERT(formula.parse("=IF((R1+R2)<45,R1+R2,50)"));
  CPPUNIT_ASSERT_EQUAL(string("=IF(([.R1]+[.R2])<45;[.R1]+[.R2];50)"), formula.str(none));

  // multiple sheet and table cell operation
  CPPUNIT_ASSERT(formula.parse("=Table1::B6+Table2::B6-Table3::B6"));
  CPPUNIT_ASSERT_EQUAL(string("=[Table1.B6]+[Table2.B6]-[Table3.B6]"), formula.str(none));

  // basic paranthesized
  CPPUNIT_ASSERT(formula.parse("=(23)+$B6"));
  CPPUNIT_ASSERT_EQUAL(string("=(23)+[.$B6]"), formula.str(none));

  // paranthesized with function
  CPPUNIT_ASSERT(formula.parse("=(23)+$B6+(ABS(($Z7)))"));
  CPPUNIT_ASSERT_EQUAL(string("=(23)+[.$B6]+(ABS(([.$Z7])))"), formula.str(none));

  // paranthesize with operators
  CPPUNIT_ASSERT(formula.parse("=((-23)+4)*(B7-2)"));
  CPPUNIT_ASSERT_EQUAL(string("=((-23)+4)*([.B7]-2)"), formula.str(none));

  // function with only column
  CPPUNIT_ASSERT(formula.parse("=SUM(B)"));
  // CPPUNIT_ASSERT_EQUAL(testFormula, formula.str(none));

  // function with row range
  CPPUNIT_ASSERT(formula.parse("=SUM(7:$9)"));
  // CPPUNIT_ASSERT_EQUAL(testFormula, formula.str(none));

  // function with column range
  CPPUNIT_ASSERT(formula.parse("=SUM($B:C)"));
  // CPPUNIT_ASSERT_EQUAL(testFormula, formula.str(none));

}

void IWORKFormulaTest::testInvalid()
{
  IWORKFormula formula(none);

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

  // address range not allowed in arbitrary context
  // CPPUNIT_ASSERT(!formula.parse("=$A4:$A81")); // FIXME
  // CPPUNIT_ASSERT(!formula.parse("=($A4:$A81)")); // FIXME
  // CPPUNIT_ASSERT(!formula.parse("=-$A4:$A81")); // FIXME
  // CPPUNIT_ASSERT(!formula.parse("=$A4:$A81%")); // FIXME
  // CPPUNIT_ASSERT(!formula.parse("=SUM($A4:$A81%)")); // FIXME
  // CPPUNIT_ASSERT(!formula.parse("=SUM(-($A4:$A81))")); // FIXME
  // CPPUNIT_ASSERT(!formula.parse("=4+$A4:$A81")); // FIXME

  // invalid special address
  CPPUNIT_ASSERT(!formula.parse("=SUM(B:C2)"));
  CPPUNIT_ASSERT(!formula.parse("=SUM(9:C2)"));
  CPPUNIT_ASSERT(!formula.parse("=SUM(C2:C)"));
  CPPUNIT_ASSERT(!formula.parse("=SUM(C2:9)"));
  CPPUNIT_ASSERT(!formula.parse("=SUM(B:7)"));
  CPPUNIT_ASSERT(!formula.parse("=SUM(9:B)"));
}

CPPUNIT_TEST_SUITE_REGISTRATION(IWORKFormulaTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
