/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cmath>

#include "KNTransformation.h"

#include "KNTransformationTest.h"

namespace test
{

using libkeynote::KNTransformation;

void KNTransformationTest::setUp()
{
}

void KNTransformationTest::tearDown()
{
}

void KNTransformationTest::testConstruction()
{
  // identity
  CPPUNIT_ASSERT(KNTransformation() == KNTransformation(1, 0, 0, 1, 0, 0));

  using namespace libkeynote::transformations;

  // centering
  CPPUNIT_ASSERT(center(200, 100) == KNTransformation(1, 0, 0, 1, 100, 50));
  CPPUNIT_ASSERT(decenter(200, 100) == KNTransformation(1, 0, 0, 1, -100, -50));

  // flipping
  CPPUNIT_ASSERT(flip(true, false) == KNTransformation(-1, 0, 0, 1, 0, 0));
  CPPUNIT_ASSERT(flip(false, true) == KNTransformation(1, 0, 0, -1, 0, 0));
  CPPUNIT_ASSERT(flip(true, true) == KNTransformation(-1, 0, 0, -1, 0, 0));

  // rotating
  // CPPUNIT_ASSERT(rotate(M_PI / 2) == KNTransformation(0, -1, 1, 0, 0, 0));

  // scaling
  CPPUNIT_ASSERT(scale(2, 1) == KNTransformation(2, 0, 0, 1, 0, 0));
  CPPUNIT_ASSERT(scale(1, 2) == KNTransformation(1, 0, 0, 2, 0, 0));
  CPPUNIT_ASSERT(scale(3, 2) == KNTransformation(3, 0, 0, 2, 0, 0));

  // shearing
  // CPPUNIT_ASSERT(shear(M_PI / 4, 0) == KNTransformation(1, 2, 0, 1, 0, 0));
  // CPPUNIT_ASSERT(shear(0, M_PI / 4) == KNTransformation(1, 0, 2, 1, 0, 0));
  // CPPUNIT_ASSERT(shear(M_PI / 4, M_PI / 4) == KNTransformation(1, 2, 2, 1, 0, 0));

  // translating
  CPPUNIT_ASSERT(translate(100, 0) == KNTransformation(1, 0, 0, 1, 100, 0));
  CPPUNIT_ASSERT(translate(0, 100) == KNTransformation(1, 0, 0, 1, 0, 100));
  CPPUNIT_ASSERT(translate(300, 100) == KNTransformation(1, 0, 0, 1, 300, 100));
}

void KNTransformationTest::testConstructionIdentity()
{
  using namespace libkeynote::transformations;

  CPPUNIT_ASSERT(center(0, 0) == KNTransformation());
  CPPUNIT_ASSERT(decenter(0, 0) == KNTransformation());
  CPPUNIT_ASSERT(flip(false, false) == KNTransformation());
  CPPUNIT_ASSERT(rotate(0) == KNTransformation());
  // CPPUNIT_ASSERT(rotate(2 * M_PI) == KNTransformation());
  CPPUNIT_ASSERT(scale(1, 1) == KNTransformation());
  CPPUNIT_ASSERT(shear(0, 0) == KNTransformation());
  CPPUNIT_ASSERT(translate(0, 0) == KNTransformation());
}

void KNTransformationTest::testConstructionFromGeometry()
{
  // TODO: implement me
}

void KNTransformationTest::testIdentities()
{
  // TODO: implement me
}

void KNTransformationTest::testInverseOperations()
{
  // TODO: implement me
}

void KNTransformationTest::testMultiplication()
{
  // TODO: implement me
}

CPPUNIT_TEST_SUITE_REGISTRATION(KNTransformationTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
