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

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/io.hpp>

#include "IWORKTransformation.h"
#include "IWORKTypes.h"

#include "libetonyek_utils.h"

namespace test
{

using libetonyek::approxEqual;
using libetonyek::etonyek_half_pi;

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
  CPPUNIT_TEST(testConstructionFromGeometry);
  CPPUNIT_TEST_SUITE_END();

private:
  void testConstruction();
  void testConstructionFromGeometry();
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
  CPPUNIT_ASSERT_EQUAL(IWORKTransformation(1, 0, 0, 1, 100, 50), center(200, 100));
  CPPUNIT_ASSERT_EQUAL(IWORKTransformation(1, 0, 0, 1, -100, -50), origin(200, 100));

  // flipping
  CPPUNIT_ASSERT_EQUAL(IWORKTransformation(-1, 0, 0, 1, 0, 0), flip(true, false));
  CPPUNIT_ASSERT_EQUAL(IWORKTransformation(1, 0, 0, -1, 0, 0), flip(false, true));
  CPPUNIT_ASSERT_EQUAL(IWORKTransformation(-1, 0, 0, -1, 0, 0), flip(true, true));

  // rotating
  CPPUNIT_ASSERT_EQUAL(IWORKTransformation(0, 1, -1, 0, 0, 0), rotate(etonyek_half_pi));

  // scaling
  CPPUNIT_ASSERT_EQUAL(IWORKTransformation(2, 0, 0, 1, 0, 0), scale(2, 1));
  CPPUNIT_ASSERT_EQUAL(IWORKTransformation(1, 0, 0, 2, 0, 0), scale(1, 2));
  CPPUNIT_ASSERT_EQUAL(IWORKTransformation(3, 0, 0, 2, 0, 0), scale(3, 2));

  // shearing
  // FIXME: find the problem and enable
  // CPPUNIT_ASSERT_EQUAL(IWORKTransformation(1, 2, 0, 1, 0, 0), shear(etonyek_pi / 4, 0));
  // CPPUNIT_ASSERT_EQUAL(IWORKTransformation(1, 0, 2, 1, 0, 0), shear(0, etonyek_pi / 4));
  // CPPUNIT_ASSERT_EQUAL(IWORKTransformation(1, 2, 2, 1, 0, 0), shear(etonyek_pi / 4, etonyek_pi / 4));

  // translating
  CPPUNIT_ASSERT_EQUAL(IWORKTransformation(1, 0, 0, 1, 100, 0), translate(100, 0));
  CPPUNIT_ASSERT_EQUAL(IWORKTransformation(1, 0, 0, 1, 0, 100), translate(0, 100));
  CPPUNIT_ASSERT_EQUAL(IWORKTransformation(1, 0, 0, 1, 300, 100), translate(300, 100));
#endif
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
    CPPUNIT_ASSERT_EQUAL(tr, glm::dmat3(1, 0, 0, 0, 1, 0, 0, 0, 1));
  }

  {
    IWORKGeometry g;
    g.m_naturalSize = IWORKSize(100, 100);
    g.m_position = IWORKPosition(200, 150);

    const glm::dmat3 tr = makeTransformation(g);
    CPPUNIT_ASSERT_EQUAL(tr, translate(200, 150));
  }

  {
    IWORKGeometry g;
    g.m_naturalSize = IWORKSize(100, 100);
    g.m_position = IWORKPosition(0, 0);
    g.m_angle = etonyek_half_pi;

    const glm::dmat3 tr = makeTransformation(g);
    CPPUNIT_ASSERT_EQUAL(tr, wrap(100, 100, rotate(etonyek_half_pi)));
  }

  {
    IWORKGeometry g;
    g.m_naturalSize = IWORKSize(100, 100);
    g.m_position = IWORKPosition(0, 0);
    g.m_horizontalFlip = true;

    const glm::dmat3 tr = makeTransformation(g);
    CPPUNIT_ASSERT_EQUAL(tr, wrap(100, 100, flip(true, false)));
  }

  {
    IWORKGeometry g;
    g.m_naturalSize = IWORKSize(100, 100);
    g.m_position = IWORKPosition(0, 0);
    g.m_verticalFlip = true;

    const glm::dmat3 tr = makeTransformation(g);
    CPPUNIT_ASSERT_EQUAL(tr, wrap(100, 100, flip(false, true)));
  }

  {
    IWORKGeometry g;
    g.m_naturalSize = IWORKSize(100, 100);
    g.m_position = IWORKPosition(200, 150);
    g.m_angle = etonyek_half_pi;

    const glm::dmat3 tr = makeTransformation(g);
    CPPUNIT_ASSERT_EQUAL(tr, wrap(100, 100, translate(200, 150) * rotate(etonyek_half_pi)));
  }

  {
    IWORKGeometry g1;
    g1.m_naturalSize = IWORKSize(200, 200);
    g1.m_position = IWORKPosition(200, 150);
    g1.m_verticalFlip = true;

    IWORKGeometry g2;
    g2.m_naturalSize = IWORKSize(100, 100);
    g2.m_position = IWORKPosition(0, 0);

    const glm::dmat3 tr1 = makeTransformation(g1);
    const glm::dmat3 tr2 = makeTransformation(g2);
    const glm::dmat3 tr = tr1 * tr2;
    CPPUNIT_ASSERT_EQUAL(glm::dvec3(200, 250, 1), (tr * glm::dvec3(0, 100, 1)));
    CPPUNIT_ASSERT_EQUAL(glm::dvec3(200, 350, 1), (tr * glm::dvec3(0, 0, 1)));
  }
}

CPPUNIT_TEST_SUITE_REGISTRATION(IWORKTransformationTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
