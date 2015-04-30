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

#include "IWORKTransformation.h"
#include "IWORKTypes.h"

#include "libetonyek_utils.h"

namespace test
{

using libetonyek::etonyek_third_pi;
using libetonyek::etonyek_half_pi;
using libetonyek::etonyek_two_pi;
using libetonyek::etonyek_pi;

namespace
{

glm::dmat3 wrap(const double width, const double height, const glm::dmat3 &tr)
{
  using namespace libetonyek::transformations;
  return center(width, height) * tr * origin(width, height);
}

}

class IWORKTransformationTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(IWORKTransformationTest);
  CPPUNIT_TEST(testConstruction);
  CPPUNIT_TEST(testConstructionIdentity);
  CPPUNIT_TEST(testConstructionFromGeometry);
  CPPUNIT_TEST(testIdentities);
  CPPUNIT_TEST(testInverseOperations);
  CPPUNIT_TEST_SUITE_END();

private:
  void testConstruction();
  void testConstructionIdentity();
  void testConstructionFromGeometry();
  void testIdentities();
  void testInverseOperations();
};

void IWORKTransformationTest::setUp()
{
}

void IWORKTransformationTest::tearDown()
{
}

void IWORKTransformationTest::testConstruction()
{
  using namespace libetonyek::transformations;

#if 0
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
#endif
}

void IWORKTransformationTest::testConstructionIdentity()
{
  using namespace libetonyek::transformations;

  glm::dmat3 eye;

  CPPUNIT_ASSERT(center(0, 0) == eye);
  CPPUNIT_ASSERT(origin(0, 0) == eye);
  CPPUNIT_ASSERT(flip(false, false) == eye);
  CPPUNIT_ASSERT(rotate(0) == eye);
  CPPUNIT_ASSERT(rotate(etonyek_two_pi) == eye);
  CPPUNIT_ASSERT(scale(1, 1) == eye);
  CPPUNIT_ASSERT(shear(0, 0) == eye);
  CPPUNIT_ASSERT(translate(0, 0) == eye);
}

void IWORKTransformationTest::testConstructionFromGeometry()
{
  using namespace libetonyek::transformations;

  using libetonyek::IWORKGeometry;
  using libetonyek::IWORKPosition;
  using libetonyek::IWORKSize;

  {
    IWORKGeometry g;
    g.m_naturalSize = IWORKSize(100, 100);
    g.m_position = IWORKPosition(0, 0);

    const glm::dmat3 tr = makeTransformation(g);
    CPPUNIT_ASSERT(glm::dmat3() == tr);
  }

  {
    IWORKGeometry g;
    g.m_naturalSize = IWORKSize(100, 100);
    g.m_position = IWORKPosition(200, 150);

    const glm::dmat3 tr = makeTransformation(g);
    CPPUNIT_ASSERT(translate(200, 150) == tr);
  }

  {
    IWORKGeometry g;
    g.m_naturalSize = IWORKSize(100, 100);
    g.m_position = IWORKPosition(0, 0);
    g.m_angle = etonyek_half_pi;

    const glm::dmat3 tr = makeTransformation(g);
    CPPUNIT_ASSERT(wrap(100, 100, rotate(etonyek_half_pi)) == tr);
  }

  {
    IWORKGeometry g;
    g.m_naturalSize = IWORKSize(100, 100);
    g.m_position = IWORKPosition(0, 0);
    g.m_horizontalFlip = true;

    const glm::dmat3 tr = makeTransformation(g);
    CPPUNIT_ASSERT(wrap(100, 100, flip(true, false)) == tr);
  }

  {
    IWORKGeometry g;
    g.m_naturalSize = IWORKSize(100, 100);
    g.m_position = IWORKPosition(0, 0);
    g.m_verticalFlip = true;

    const glm::dmat3 tr = makeTransformation(g);
    CPPUNIT_ASSERT(wrap(100, 100, flip(false, true)) == tr);
  }

  {
    IWORKGeometry g;
    g.m_naturalSize = IWORKSize(100, 100);
    g.m_position = IWORKPosition(200, 150);
    g.m_angle = etonyek_half_pi;

    const glm::dmat3 tr = makeTransformation(g);
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

  glm::dmat3 eye;

  CPPUNIT_ASSERT(center(10, 20) * origin(10, 20) == eye);
  CPPUNIT_ASSERT(origin(10, 20) * center(10, 20) == eye);

  CPPUNIT_ASSERT(flip(true, false) * flip(true, false) == eye);
  CPPUNIT_ASSERT(flip(false, true) * flip(false, true) == eye);
  CPPUNIT_ASSERT(flip(true, true) * flip(true, true) == eye);

  CPPUNIT_ASSERT(rotate(etonyek_pi) * rotate(-etonyek_pi) == eye);

  CPPUNIT_ASSERT(scale(2, 1) * scale(0.5, 1) == eye);
  CPPUNIT_ASSERT(scale(1, 2) * scale(1, 0.5) == eye);
  CPPUNIT_ASSERT(scale(3, 2) * scale(1.0 / 3, 0.5) == eye);

  // CPPUNIT_ASSERT(shear() == eye);

  CPPUNIT_ASSERT(translate(10, 20) * translate(-10, -20) == eye);
}

CPPUNIT_TEST_SUITE_REGISTRATION(IWORKTransformationTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
