/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/math/constants/constants.hpp>

#include "KEYTransformation.h"
#include "KEYTypes.h"

#include "KEYTransformationTest.h"

namespace test
{

namespace m = boost::math::constants;

using libetonyek::KEYTransformation;

namespace
{

KEYTransformation wrap(const double width, const double height, const KEYTransformation &tr)
{
  using namespace libetonyek::transformations;
  return origin(width, height) * tr * center(width, height);
}

}

void KEYTransformationTest::setUp()
{
}

void KEYTransformationTest::tearDown()
{
}

void KEYTransformationTest::testApplication()
{
  using namespace libetonyek::transformations;

  // identity - point
  {
    double x = 20;
    double y = 40;
    KEYTransformation tr;
    tr(x, y);
    CPPUNIT_ASSERT_EQUAL(20.0, x);
    CPPUNIT_ASSERT_EQUAL(40.0, y);
  }

  // identity - distance
  {
    double x = 20;
    double y = 40;
    KEYTransformation tr;
    tr(x, y, true);
    CPPUNIT_ASSERT_EQUAL(20.0, x);
    CPPUNIT_ASSERT_EQUAL(40.0, y);
  }

  // translation - point
  {
    double x = 20;
    double y = 40;
    KEYTransformation tr = translate(10, 20);
    tr(x, y);
    CPPUNIT_ASSERT_EQUAL(30.0, x);
    CPPUNIT_ASSERT_EQUAL(60.0, y);
  }

  // translation - distance
  {
    double x = 20;
    double y = 40;
    KEYTransformation tr = translate(10, 20);
    tr(x, y, true);
    CPPUNIT_ASSERT_EQUAL(20.0, x);
    CPPUNIT_ASSERT_EQUAL(40.0, y);
  }

  // non-translating transformation - point
  {
    double x = 20;
    double y = 40;
    KEYTransformation tr = flip(true, false) * scale(0.25, 0.5);
    tr(x, y);
    CPPUNIT_ASSERT_EQUAL(-5.0, x);
    CPPUNIT_ASSERT_EQUAL(20.0, y);
  }

  // non-translating transformation - distance
  {
    double x = 20;
    double y = 40;
    KEYTransformation tr = flip(true, false) * scale(0.25, 0.5);
    tr(x, y, true);
    CPPUNIT_ASSERT_EQUAL(-5.0, x);
    CPPUNIT_ASSERT_EQUAL(20.0, y);
  }
}

void KEYTransformationTest::testConstruction()
{
  // identity
  CPPUNIT_ASSERT(KEYTransformation() == KEYTransformation(1, 0, 0, 1, 0, 0));

  using namespace libetonyek::transformations;

  // centering
  CPPUNIT_ASSERT(center(200, 100) == KEYTransformation(1, 0, 0, 1, 100, 50));
  CPPUNIT_ASSERT(origin(200, 100) == KEYTransformation(1, 0, 0, 1, -100, -50));

  // flipping
  CPPUNIT_ASSERT(flip(true, false) == KEYTransformation(-1, 0, 0, 1, 0, 0));
  CPPUNIT_ASSERT(flip(false, true) == KEYTransformation(1, 0, 0, -1, 0, 0));
  CPPUNIT_ASSERT(flip(true, true) == KEYTransformation(-1, 0, 0, -1, 0, 0));

  // rotating
  CPPUNIT_ASSERT(rotate(m::half_pi<double>()) == KEYTransformation(0, 1, -1, 0, 0, 0));

  // scaling
  CPPUNIT_ASSERT(scale(2, 1) == KEYTransformation(2, 0, 0, 1, 0, 0));
  CPPUNIT_ASSERT(scale(1, 2) == KEYTransformation(1, 0, 0, 2, 0, 0));
  CPPUNIT_ASSERT(scale(3, 2) == KEYTransformation(3, 0, 0, 2, 0, 0));

  // shearing
  // FIXME: find the problem and enable
  // CPPUNIT_ASSERT(shear(m::pi<double>() / 4, 0) == KEYTransformation(1, 2, 0, 1, 0, 0));
  // CPPUNIT_ASSERT(shear(0, m::pi<double>() / 4) == KEYTransformation(1, 0, 2, 1, 0, 0));
  // CPPUNIT_ASSERT(shear(m::pi<double>() / 4, m::pi<double>() / 4) == KEYTransformation(1, 2, 2, 1, 0, 0));

  // translating
  CPPUNIT_ASSERT(translate(100, 0) == KEYTransformation(1, 0, 0, 1, 100, 0));
  CPPUNIT_ASSERT(translate(0, 100) == KEYTransformation(1, 0, 0, 1, 0, 100));
  CPPUNIT_ASSERT(translate(300, 100) == KEYTransformation(1, 0, 0, 1, 300, 100));
}

void KEYTransformationTest::testConstructionIdentity()
{
  using namespace libetonyek::transformations;

  CPPUNIT_ASSERT(center(0, 0) == KEYTransformation());
  CPPUNIT_ASSERT(origin(0, 0) == KEYTransformation());
  CPPUNIT_ASSERT(flip(false, false) == KEYTransformation());
  CPPUNIT_ASSERT(rotate(0) == KEYTransformation());
  CPPUNIT_ASSERT(rotate(m::two_pi<double>()) == KEYTransformation());
  CPPUNIT_ASSERT(scale(1, 1) == KEYTransformation());
  CPPUNIT_ASSERT(shear(0, 0) == KEYTransformation());
  CPPUNIT_ASSERT(translate(0, 0) == KEYTransformation());
}

void KEYTransformationTest::testConstructionFromGeometry()
{
  using namespace libetonyek::transformations;

  using libetonyek::KEYGeometry;
  using libetonyek::KEYPosition;
  using libetonyek::KEYSize;

  {
    KEYGeometry g;
    g.naturalSize = KEYSize(100, 100);
    g.position = KEYPosition(0, 0);

    const KEYTransformation tr = makeTransformation(g);
    CPPUNIT_ASSERT(KEYTransformation() == tr);
  }

  {
    KEYGeometry g;
    g.naturalSize = KEYSize(100, 100);
    g.position = KEYPosition(200, 150);

    const KEYTransformation tr = makeTransformation(g);
    CPPUNIT_ASSERT(translate(200, 150) == tr);
  }

  {
    KEYGeometry g;
    g.naturalSize = KEYSize(100, 100);
    g.position = KEYPosition(0, 0);
    g.angle = m::half_pi<double>();

    const KEYTransformation tr = makeTransformation(g);
    CPPUNIT_ASSERT(wrap(100, 100, rotate(m::half_pi<double>())) == tr);
  }

  {
    KEYGeometry g;
    g.naturalSize = KEYSize(100, 100);
    g.position = KEYPosition(0, 0);
    g.horizontalFlip = true;

    const KEYTransformation tr = makeTransformation(g);
    CPPUNIT_ASSERT(wrap(100, 100, flip(true, false)) == tr);
  }

  {
    KEYGeometry g;
    g.naturalSize = KEYSize(100, 100);
    g.position = KEYPosition(0, 0);
    g.verticalFlip = true;

    const KEYTransformation tr = makeTransformation(g);
    CPPUNIT_ASSERT(wrap(100, 100, flip(false, true)) == tr);
  }

  {
    KEYGeometry g;
    g.naturalSize = KEYSize(100, 100);
    g.position = KEYPosition(200, 150);
    g.angle = m::half_pi<double>();

    const KEYTransformation tr = makeTransformation(g);
    CPPUNIT_ASSERT(wrap(100, 100, rotate(m::half_pi<double>()) * translate(200, 150)) == tr);
  }
}

void KEYTransformationTest::testIdentities()
{
  using namespace libetonyek::transformations;

  CPPUNIT_ASSERT(center(100, 50) == translate(50, 25));
  CPPUNIT_ASSERT(origin(100, 50) == translate(-50, -25));
  CPPUNIT_ASSERT((flip(true, false) * flip(false, true)) == flip(true, true));
  CPPUNIT_ASSERT((flip(false, true) * flip(true, false)) == flip(true, true));
  CPPUNIT_ASSERT((rotate(m::half_pi<double>()) * rotate(m::third_pi<double>())) == (rotate(m::third_pi<double>()) * rotate(m::half_pi<double>())));
  CPPUNIT_ASSERT(scale(-1, -1) == flip(true, true));
  CPPUNIT_ASSERT((translate(10, 20) * translate(80, 40)) == (translate(80, 40) * translate(10, 20)));
  CPPUNIT_ASSERT((translate(1, 2) * scale(2, 2)) == (scale(2, 2) * translate(2, 4)));
}

void KEYTransformationTest::testInverseOperations()
{
  using namespace libetonyek::transformations;

  CPPUNIT_ASSERT(center(10, 20) * origin(10, 20) == KEYTransformation());
  CPPUNIT_ASSERT(origin(10, 20) * center(10, 20) == KEYTransformation());

  CPPUNIT_ASSERT(flip(true, false) * flip(true, false) == KEYTransformation());
  CPPUNIT_ASSERT(flip(false, true) * flip(false, true) == KEYTransformation());
  CPPUNIT_ASSERT(flip(true, true) * flip(true, true) == KEYTransformation());

  CPPUNIT_ASSERT(rotate(m::pi<double>()) * rotate(-m::pi<double>()) == KEYTransformation());

  CPPUNIT_ASSERT(scale(2, 1) * scale(0.5, 1) == KEYTransformation());
  CPPUNIT_ASSERT(scale(1, 2) * scale(1, 0.5) == KEYTransformation());
  CPPUNIT_ASSERT(scale(3, 2) * scale(1.0 / 3, 0.5) == KEYTransformation());

  // CPPUNIT_ASSERT(shear() == KEYTransformation());

  CPPUNIT_ASSERT(translate(10, 20) * translate(-10, -20) == KEYTransformation());
}

void KEYTransformationTest::testMultiplication()
{
  using namespace libetonyek::transformations;

  CPPUNIT_ASSERT(KEYTransformation() * KEYTransformation() == KEYTransformation());

  CPPUNIT_ASSERT(KEYTransformation() * KEYTransformation(1, 2, 3, 4, 5, 6) == KEYTransformation(1, 2, 3, 4, 5, 6));
  CPPUNIT_ASSERT(KEYTransformation(1, 2, 3, 4, 5, 6) * KEYTransformation() == KEYTransformation(1, 2, 3, 4, 5, 6));
  CPPUNIT_ASSERT(KEYTransformation(1, 2, 3, 4, 5, 6) * KEYTransformation(6, 5, 4, 3, 2, 1) == KEYTransformation(14, 11, 34, 27, 56, 44));
}

CPPUNIT_TEST_SUITE_REGISTRATION(KEYTransformationTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
