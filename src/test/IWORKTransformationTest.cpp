/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTransformation.h"
#include "IWORKTypes.h"

#include "IWORKTransformationTest.h"

#include "libetonyek_utils.h"

namespace test
{

using libetonyek::IWORKTransformation;
using libetonyek::etonyek_third_pi;
using libetonyek::etonyek_half_pi;
using libetonyek::etonyek_two_pi;
using libetonyek::etonyek_pi;

namespace
{

IWORKTransformation wrap(const double width, const double height, const IWORKTransformation &tr)
{
  using namespace libetonyek::transformations;
  return origin(width, height) * tr * center(width, height);
}

}

void IWORKTransformationTest::setUp()
{
}

void IWORKTransformationTest::tearDown()
{
}

void IWORKTransformationTest::testApplication()
{
  using namespace libetonyek::transformations;

  // identity - point
  {
    double x = 20;
    double y = 40;
    IWORKTransformation tr;
    tr(x, y);
    CPPUNIT_ASSERT_EQUAL(20.0, x);
    CPPUNIT_ASSERT_EQUAL(40.0, y);
  }

  // identity - distance
  {
    double x = 20;
    double y = 40;
    IWORKTransformation tr;
    tr(x, y, true);
    CPPUNIT_ASSERT_EQUAL(20.0, x);
    CPPUNIT_ASSERT_EQUAL(40.0, y);
  }

  // translation - point
  {
    double x = 20;
    double y = 40;
    IWORKTransformation tr = translate(10, 20);
    tr(x, y);
    CPPUNIT_ASSERT_EQUAL(30.0, x);
    CPPUNIT_ASSERT_EQUAL(60.0, y);
  }

  // translation - distance
  {
    double x = 20;
    double y = 40;
    IWORKTransformation tr = translate(10, 20);
    tr(x, y, true);
    CPPUNIT_ASSERT_EQUAL(20.0, x);
    CPPUNIT_ASSERT_EQUAL(40.0, y);
  }

  // non-translating transformation - point
  {
    double x = 20;
    double y = 40;
    IWORKTransformation tr = flip(true, false) * scale(0.25, 0.5);
    tr(x, y);
    CPPUNIT_ASSERT_EQUAL(-5.0, x);
    CPPUNIT_ASSERT_EQUAL(20.0, y);
  }

  // non-translating transformation - distance
  {
    double x = 20;
    double y = 40;
    IWORKTransformation tr = flip(true, false) * scale(0.25, 0.5);
    tr(x, y, true);
    CPPUNIT_ASSERT_EQUAL(-5.0, x);
    CPPUNIT_ASSERT_EQUAL(20.0, y);
  }
}

void IWORKTransformationTest::testConstruction()
{
  // identity
  CPPUNIT_ASSERT(IWORKTransformation() == IWORKTransformation(1, 0, 0, 1, 0, 0));

  using namespace libetonyek::transformations;

  // centering
  CPPUNIT_ASSERT(center(200, 100) == IWORKTransformation(1, 0, 0, 1, 100, 50));
  CPPUNIT_ASSERT(origin(200, 100) == IWORKTransformation(1, 0, 0, 1, -100, -50));

  // flipping
  CPPUNIT_ASSERT(flip(true, false) == IWORKTransformation(-1, 0, 0, 1, 0, 0));
  CPPUNIT_ASSERT(flip(false, true) == IWORKTransformation(1, 0, 0, -1, 0, 0));
  CPPUNIT_ASSERT(flip(true, true) == IWORKTransformation(-1, 0, 0, -1, 0, 0));

  // rotating
  CPPUNIT_ASSERT(rotate(etonyek_half_pi) == IWORKTransformation(0, 1, -1, 0, 0, 0));

  // scaling
  CPPUNIT_ASSERT(scale(2, 1) == IWORKTransformation(2, 0, 0, 1, 0, 0));
  CPPUNIT_ASSERT(scale(1, 2) == IWORKTransformation(1, 0, 0, 2, 0, 0));
  CPPUNIT_ASSERT(scale(3, 2) == IWORKTransformation(3, 0, 0, 2, 0, 0));

  // shearing
  // FIXME: find the problem and enable
  // CPPUNIT_ASSERT(shear(etonyek_pi / 4, 0) == IWORKTransformation(1, 2, 0, 1, 0, 0));
  // CPPUNIT_ASSERT(shear(0, etonyek_pi / 4) == IWORKTransformation(1, 0, 2, 1, 0, 0));
  // CPPUNIT_ASSERT(shear(etonyek_pi / 4, etonyek_pi / 4) == IWORKTransformation(1, 2, 2, 1, 0, 0));

  // translating
  CPPUNIT_ASSERT(translate(100, 0) == IWORKTransformation(1, 0, 0, 1, 100, 0));
  CPPUNIT_ASSERT(translate(0, 100) == IWORKTransformation(1, 0, 0, 1, 0, 100));
  CPPUNIT_ASSERT(translate(300, 100) == IWORKTransformation(1, 0, 0, 1, 300, 100));
}

void IWORKTransformationTest::testConstructionIdentity()
{
  using namespace libetonyek::transformations;

  CPPUNIT_ASSERT(center(0, 0) == IWORKTransformation());
  CPPUNIT_ASSERT(origin(0, 0) == IWORKTransformation());
  CPPUNIT_ASSERT(flip(false, false) == IWORKTransformation());
  CPPUNIT_ASSERT(rotate(0) == IWORKTransformation());
  CPPUNIT_ASSERT(rotate(etonyek_two_pi) == IWORKTransformation());
  CPPUNIT_ASSERT(scale(1, 1) == IWORKTransformation());
  CPPUNIT_ASSERT(shear(0, 0) == IWORKTransformation());
  CPPUNIT_ASSERT(translate(0, 0) == IWORKTransformation());
}

void IWORKTransformationTest::testConstructionFromGeometry()
{
  using namespace libetonyek::transformations;

  using libetonyek::IWORKGeometry;
  using libetonyek::IWORKPosition;
  using libetonyek::IWORKSize;

  {
    IWORKGeometry g;
    g.naturalSize = IWORKSize(100, 100);
    g.position = IWORKPosition(0, 0);

    const IWORKTransformation tr = makeTransformation(g);
    CPPUNIT_ASSERT(IWORKTransformation() == tr);
  }

  {
    IWORKGeometry g;
    g.naturalSize = IWORKSize(100, 100);
    g.position = IWORKPosition(200, 150);

    const IWORKTransformation tr = makeTransformation(g);
    CPPUNIT_ASSERT(translate(200, 150) == tr);
  }

  {
    IWORKGeometry g;
    g.naturalSize = IWORKSize(100, 100);
    g.position = IWORKPosition(0, 0);
    g.angle = etonyek_half_pi;

    const IWORKTransformation tr = makeTransformation(g);
    CPPUNIT_ASSERT(wrap(100, 100, rotate(etonyek_half_pi)) == tr);
  }

  {
    IWORKGeometry g;
    g.naturalSize = IWORKSize(100, 100);
    g.position = IWORKPosition(0, 0);
    g.horizontalFlip = true;

    const IWORKTransformation tr = makeTransformation(g);
    CPPUNIT_ASSERT(wrap(100, 100, flip(true, false)) == tr);
  }

  {
    IWORKGeometry g;
    g.naturalSize = IWORKSize(100, 100);
    g.position = IWORKPosition(0, 0);
    g.verticalFlip = true;

    const IWORKTransformation tr = makeTransformation(g);
    CPPUNIT_ASSERT(wrap(100, 100, flip(false, true)) == tr);
  }

  {
    IWORKGeometry g;
    g.naturalSize = IWORKSize(100, 100);
    g.position = IWORKPosition(200, 150);
    g.angle = etonyek_half_pi;

    const IWORKTransformation tr = makeTransformation(g);
    CPPUNIT_ASSERT(wrap(100, 100, rotate(etonyek_half_pi) * translate(200, 150)) == tr);
  }
}

void IWORKTransformationTest::testIdentities()
{
  using namespace libetonyek::transformations;

  CPPUNIT_ASSERT(center(100, 50) == translate(50, 25));
  CPPUNIT_ASSERT(origin(100, 50) == translate(-50, -25));
  CPPUNIT_ASSERT((flip(true, false) * flip(false, true)) == flip(true, true));
  CPPUNIT_ASSERT((flip(false, true) * flip(true, false)) == flip(true, true));
  CPPUNIT_ASSERT((rotate(etonyek_half_pi) * rotate(etonyek_third_pi)) == (rotate(etonyek_third_pi) * rotate(etonyek_half_pi)));
  CPPUNIT_ASSERT(scale(-1, -1) == flip(true, true));
  CPPUNIT_ASSERT((translate(10, 20) * translate(80, 40)) == (translate(80, 40) * translate(10, 20)));
  CPPUNIT_ASSERT((translate(1, 2) * scale(2, 2)) == (scale(2, 2) * translate(2, 4)));
}

void IWORKTransformationTest::testInverseOperations()
{
  using namespace libetonyek::transformations;

  CPPUNIT_ASSERT(center(10, 20) * origin(10, 20) == IWORKTransformation());
  CPPUNIT_ASSERT(origin(10, 20) * center(10, 20) == IWORKTransformation());

  CPPUNIT_ASSERT(flip(true, false) * flip(true, false) == IWORKTransformation());
  CPPUNIT_ASSERT(flip(false, true) * flip(false, true) == IWORKTransformation());
  CPPUNIT_ASSERT(flip(true, true) * flip(true, true) == IWORKTransformation());

  CPPUNIT_ASSERT(rotate(etonyek_pi) * rotate(-etonyek_pi) == IWORKTransformation());

  CPPUNIT_ASSERT(scale(2, 1) * scale(0.5, 1) == IWORKTransformation());
  CPPUNIT_ASSERT(scale(1, 2) * scale(1, 0.5) == IWORKTransformation());
  CPPUNIT_ASSERT(scale(3, 2) * scale(1.0 / 3, 0.5) == IWORKTransformation());

  // CPPUNIT_ASSERT(shear() == IWORKTransformation());

  CPPUNIT_ASSERT(translate(10, 20) * translate(-10, -20) == IWORKTransformation());
}

void IWORKTransformationTest::testMultiplication()
{
  using namespace libetonyek::transformations;

  CPPUNIT_ASSERT(IWORKTransformation() * IWORKTransformation() == IWORKTransformation());

  CPPUNIT_ASSERT(IWORKTransformation() * IWORKTransformation(1, 2, 3, 4, 5, 6) == IWORKTransformation(1, 2, 3, 4, 5, 6));
  CPPUNIT_ASSERT(IWORKTransformation(1, 2, 3, 4, 5, 6) * IWORKTransformation() == IWORKTransformation(1, 2, 3, 4, 5, 6));
  CPPUNIT_ASSERT(IWORKTransformation(1, 2, 3, 4, 5, 6) * IWORKTransformation(6, 5, 4, 3, 2, 1) == IWORKTransformation(14, 11, 34, 27, 56, 44));
}

CPPUNIT_TEST_SUITE_REGISTRATION(IWORKTransformationTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
