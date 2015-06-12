/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string>
#include <sstream>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "IWORKPath.h"

using libetonyek::IWORKPath;

using std::string;

namespace test
{

namespace
{

double in2pt(const double in)
{
  return 72 * in;
}

string toSVG(const IWORKPath &path)
{
  std::ostringstream output;

  const librevenge::RVNGPropertyListVector vec = path.toWPG();

  bool first = true;

  librevenge::RVNGPropertyListVector::Iter it(vec);
  while (!it.last())
  {
    const librevenge::RVNGPropertyList &element = it();

    CPPUNIT_ASSERT(0 != element["librevenge:path-action"]);

    if (first)
      first = false;
    else
      output << ' ';
    output << element["librevenge:path-action"]->getStr().cstr();
    if (element["svg:x1"])
      output << ' ' << in2pt(element["svg:x1"]->getDouble());
    if (element["svg:y1"])
      output << ' ' << in2pt(element["svg:y1"]->getDouble());
    if (element["svg:x2"])
      output << ' ' << in2pt(element["svg:x2"]->getDouble());
    if (element["svg:y2"])
      output << ' ' << in2pt(element["svg:y2"]->getDouble());
    if (element["svg:x"])
      output << ' ' << in2pt(element["svg:x"]->getDouble());
    if (element["svg:y"])
      output << ' ' << in2pt(element["svg:y"]->getDouble());

    it.next();
  }

  return output.str();
}

}

class IWORKPathTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(IWORKPathTest);
  CPPUNIT_TEST(testConstruction);
  CPPUNIT_TEST(testConversion);
  CPPUNIT_TEST_SUITE_END();

private:
  void testConstruction();
  void testConversion();
};

void IWORKPathTest::setUp()
{
}

void IWORKPathTest::tearDown()
{
}

void  IWORKPathTest::testConstruction()
{
  {
    const string src = "M 0.0 0.0";
    IWORKPath refPath;
    refPath.appendMoveTo(0, 0);

    IWORKPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }

  {
    const string src = "L 0.0 0.0";
    IWORKPath refPath;
    refPath.appendLineTo(0, 0);

    IWORKPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }

  {
    const string src = "C 0.5 0.5 0 0 1 1";
    IWORKPath refPath;
    refPath.appendCurveTo(0.5, 0.5, 0, 0, 1, 1);

    IWORKPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }

  {
    const string src = "Z";
    IWORKPath refPath;
    refPath.appendClose();

    IWORKPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }

  {
    const string src = "M 0 0 L 1 1";
    IWORKPath refPath;
    refPath.appendMoveTo(0, 0);
    refPath.appendLineTo(1, 1);

    IWORKPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }

  {
    const string src = "M 0 0 L 1 0 L 1 1 L 0 1 Z L 0 0";
    IWORKPath refPath;
    refPath.appendMoveTo(0, 0);
    refPath.appendLineTo(1, 0);
    refPath.appendLineTo(1, 1);
    refPath.appendLineTo(0, 1);
    refPath.appendClose();
    refPath.appendLineTo(0, 0);

    IWORKPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }

  {
    const string src = "M 0.0 0.0 L 0 1 C 1 1 0.5 0.5 0 0 Z";
    IWORKPath refPath;
    refPath.appendMoveTo(0, 0);
    refPath.appendLineTo(0, 1);
    refPath.appendCurveTo(1, 1, 0.5, 0.5, 0, 0);
    refPath.appendClose();

    IWORKPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }
}

void  IWORKPathTest::testConversion()
{
  {
    const string ref = "M 0 0";
    IWORKPath path;
    path.appendMoveTo(0, 0);

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }

  {
    const string ref = "L 0 0";
    IWORKPath path;
    path.appendLineTo(0, 0);

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }

  {
    const string ref = "C 1 1 0 0 0.5 0.5";
    IWORKPath path;
    path.appendCurveTo(1, 1, 0, 0, 0.5, 0.5);

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }

  {
    const string ref = "Z";
    IWORKPath path;
    path.appendClose();

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }

  {
    const string ref = "M 0 0 L 1 1";
    IWORKPath path;
    path.appendMoveTo(0, 0);
    path.appendLineTo(1, 1);

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }

  {
    const string ref = "M 0 0 L 1 0 L 1 1 L 0 1 L 0 0 Z";
    IWORKPath path;
    path.appendMoveTo(0, 0);
    path.appendLineTo(1, 0);
    path.appendLineTo(1, 1);
    path.appendLineTo(0, 1);
    path.appendClose();
    path.appendLineTo(0, 0);

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }

  {
    const string ref = "M 0 0 L 0 1 C 1 1 0.5 0.5 0 0 Z";
    IWORKPath path;
    path.appendMoveTo(0, 0);
    path.appendLineTo(0, 1);
    path.appendCurveTo(1, 1, 0.5, 0.5, 0, 0);
    path.appendClose();

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }
}

CPPUNIT_TEST_SUITE_REGISTRATION(IWORKPathTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
