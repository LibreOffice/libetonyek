/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <iomanip>
#include <sstream>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "IWORKTransformation.h"
#include "IWORKTypes.h"

#include "libetonyek_utils.h"

namespace
{

void writeVec(std::ostream &os, double x, double y, double z, int w)
{
  os << '['
     << std::setw(w) << x << ',' << std::setw(w) << y << ',' << std::setw(w) << z
     << ']'
     ;
}

}

CPPUNIT_NS_BEGIN

template<>
struct assertion_traits<glm::dvec3>
{
  static bool equal(const glm::dvec3 &x, const glm::dvec3 &y)
  {
    return libetonyek::approxEqual(x, y);
  }

  static std::string toString(const glm::dvec3 &x)
  {
    std::ostringstream s;
    s << std::setfill(' ') << std::right << std::fixed << std::showpoint << std::setprecision(3);
    writeVec(s, x.x, x.y, x.z, 8);
    return s.str();
  }
};

template<>
struct assertion_traits<glm::dmat3>
{
  static bool equal(const glm::dmat3 &x, const glm::dmat3 &y)
  {
    return libetonyek::approxEqual(x, y);
  }

  static std::string toString(const glm::dmat3 &x)
  {
    std::ostringstream s;
    s << std::setfill(' ') << std::right << std::fixed << std::showpoint << std::setprecision(3);
    s << "\n[";
    writeVec(s, x[0][0], x[1][0], x[2][0], 8);
    s << "\n ";
    writeVec(s, x[0][1], x[1][1], x[2][1], 8);
    s << "\n ";
    writeVec(s, x[0][2], x[1][2], x[2][2], 8);
    s << ']';
    return s.str();
  }
};

CPPUNIT_NS_END

namespace test
{

using libetonyek::etonyek_half_pi;
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

  // centering
  CPPUNIT_ASSERT_EQUAL(glm::dmat3(1, 0, 0, 0, 1, 0, 100, 50, 1), center(200, 100));
  CPPUNIT_ASSERT_EQUAL(glm::dmat3(1, 0, 0, 0, 1, 0, -100, -50, 1), origin(200, 100));

  // flipping
  CPPUNIT_ASSERT_EQUAL(glm::dmat3(-1, 0, 0, 0, 1, 0, 0, 0, 1), flip(true, false));
  CPPUNIT_ASSERT_EQUAL(glm::dmat3(1, 0, 0, 0, -1, 0, 0, 0, 1), flip(false, true));
  CPPUNIT_ASSERT_EQUAL(glm::dmat3(-1, 0, 0, 0, -1, 0, 0, 0, 1), flip(true, true));

  // rotating
  CPPUNIT_ASSERT_EQUAL(glm::dmat3(0, 1, 0, -1, 0, 0, 0, 0, 1), rotate(etonyek_half_pi));

  // scaling
  CPPUNIT_ASSERT_EQUAL(glm::dmat3(2, 0, 0, 0, 1, 0, 0, 0, 1), scale(2, 1));
  CPPUNIT_ASSERT_EQUAL(glm::dmat3(1, 0, 0, 0, 2, 0, 0, 0, 1), scale(1, 2));
  CPPUNIT_ASSERT_EQUAL(glm::dmat3(3, 0, 0, 0, 2, 0, 0, 0, 1), scale(3, 2));

  // shearing
  // FIXME: find the problem and enable
  // CPPUNIT_ASSERT_EQUAL(glm::dmat3(1, 2, 0, 0, 1, 0, 0, 0, 1), shear(etonyek_pi / 4, 0));
  // CPPUNIT_ASSERT_EQUAL(glm::dmat3(1, 0, 0, 2, 1, 0, 0, 0, 1), shear(0, etonyek_pi / 4));
  // CPPUNIT_ASSERT_EQUAL(glm::dmat3(1, 2, 0, 2, 1, 0, 0, 0, 1), shear(etonyek_pi / 4, etonyek_pi / 4));

  // translating
  CPPUNIT_ASSERT_EQUAL(glm::dmat3(1, 0, 0, 0, 1, 0, 100, 0, 1), translate(100, 0));
  CPPUNIT_ASSERT_EQUAL(glm::dmat3(1, 0, 0, 0, 1, 0, 0, 100, 1), translate(0, 100));
  CPPUNIT_ASSERT_EQUAL(glm::dmat3(1, 0, 0, 0, 1, 0, 300, 100, 1), translate(300, 100));
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
    CPPUNIT_ASSERT_EQUAL(glm::dmat3(1, 0, 0, 0, 1, 0, 0, 0, 1), makeTransformation(g));
  }

  {
    IWORKGeometry g;
    g.m_naturalSize = IWORKSize(100, 100);
    g.m_position = IWORKPosition(200, 150);
    CPPUNIT_ASSERT_EQUAL(translate(200, 150), makeTransformation(g));
  }

  {
    IWORKGeometry g;
    g.m_naturalSize = IWORKSize(100, 100);
    g.m_position = IWORKPosition(0, 0);
    g.m_angle = etonyek_half_pi;
    CPPUNIT_ASSERT_EQUAL(wrap(100, 100, rotate(etonyek_half_pi)), makeTransformation(g));
  }

  {
    IWORKGeometry g;
    g.m_naturalSize = IWORKSize(100, 100);
    g.m_position = IWORKPosition(0, 0);
    g.m_horizontalFlip = true;
    CPPUNIT_ASSERT_EQUAL(wrap(100, 100, flip(true, false)), makeTransformation(g));
  }

  {
    IWORKGeometry g;
    g.m_naturalSize = IWORKSize(100, 100);
    g.m_position = IWORKPosition(0, 0);
    g.m_verticalFlip = true;
    CPPUNIT_ASSERT_EQUAL(wrap(100, 100, flip(false, true)), makeTransformation(g));
  }

  {
    IWORKGeometry g;
    g.m_naturalSize = IWORKSize(100, 100);
    g.m_position = IWORKPosition(200, 150);
    g.m_angle = etonyek_half_pi;
    CPPUNIT_ASSERT_EQUAL(wrap(100, 100, translate(200, 150) * rotate(etonyek_half_pi)), makeTransformation(g));
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
