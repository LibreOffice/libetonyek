/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string>
#include <sstream>

#include "KNPath.h"

#include "KNPathTest.h"

using libkeynote::KNPath;

using std::string;

namespace test
{

namespace
{

string toSVG(const KNPath &path)
{
  std::ostringstream output;

  const WPXPropertyListVector vec = path.toWPG();

  bool first = true;

  WPXPropertyListVector::Iter it(vec);
  while (!it.last())
  {
    const WPXPropertyList &element = it();

    CPPUNIT_ASSERT(0 != element["libwpg:path-action"]);

    if (first)
      first = false;
    else
      output << ' ';
    output << element["libwpg:path-action"]->getStr().cstr();
    if (element["svg:x1"])
      output << ' ' << element["svg:x1"]->getDouble();
    if (element["svg:y1"])
      output << ' ' << element["svg:y1"]->getDouble();
    if (element["svg:x2"])
      output << ' ' << element["svg:x2"]->getDouble();
    if (element["svg:y2"])
      output << ' ' << element["svg:y2"]->getDouble();
    if (element["svg:x"])
      output << ' ' << element["svg:x"]->getDouble();
    if (element["svg:y"])
      output << ' ' << element["svg:y"]->getDouble();

    it.next();
  }

  return output.str();
}

}

void KNPathTest::setUp()
{
}

void KNPathTest::tearDown()
{
}

void  KNPathTest::testConstruction()
{
  {
    const string src = "M 0.0 0.0";
    KNPath refPath;
    refPath.appendMoveTo(0, 0);

    KNPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }

  {
    const string src = "L 0.0 0.0";
    KNPath refPath;
    refPath.appendLineTo(0, 0);

    KNPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }

  {
    const string src = "C 0.5 0.5 0 0 1 1";
    KNPath refPath;
    refPath.appendCurveTo(0.5, 0.5, 0, 0, 1, 1);

    KNPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }

  {
    const string src = "Z";
    KNPath refPath;
    refPath.appendClose();

    KNPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }

  {
    const string src = "M 0 0 L 1 1";
    KNPath refPath;
    refPath.appendMoveTo(0, 0);
    refPath.appendLineTo(1, 1);

    KNPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }

  {
    const string src = "M 0 0 L 1 0 L 1 1 L 0 1 Z L 0 0";
    KNPath refPath;
    refPath.appendMoveTo(0, 0);
    refPath.appendLineTo(1, 0);
    refPath.appendLineTo(1, 1);
    refPath.appendLineTo(0, 1);
    refPath.appendClose();
    refPath.appendLineTo(0, 0);

    KNPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }

  {
    const string src = "M 0.0 0.0 L 0 1 C 1 1 0.5 0.5 0 0 Z";
    KNPath refPath;
    refPath.appendMoveTo(0, 0);
    refPath.appendLineTo(0, 1);
    refPath.appendCurveTo(1, 1, 0.5, 0.5, 0, 0);
    refPath.appendClose();

    KNPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }
}

void  KNPathTest::testConversion()
{
  {
    const string ref = "M 0 0";
    KNPath path;
    path.appendMoveTo(0, 0);

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }

  {
    const string ref = "L 0 0";
    KNPath path;
    path.appendLineTo(0, 0);

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }

  {
    const string ref = "C 1 1 0 0 0.5 0.5";
    KNPath path;
    path.appendCurveTo(1, 1, 0, 0, 0.5, 0.5);

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }

  {
    const string ref = "Z";
    KNPath path;
    path.appendClose();

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }

  {
    const string ref = "M 0 0 L 1 1";
    KNPath path;
    path.appendMoveTo(0, 0);
    path.appendLineTo(1, 1);

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }

  {
    const string ref = "M 0 0 L 1 0 L 1 1 L 0 1 Z L 0 0";
    KNPath path;
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
    KNPath path;
    path.appendMoveTo(0, 0);
    path.appendLineTo(0, 1);
    path.appendCurveTo(1, 1, 0.5, 0.5, 0, 0);
    path.appendClose();

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }
}

CPPUNIT_TEST_SUITE_REGISTRATION(KNPathTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
