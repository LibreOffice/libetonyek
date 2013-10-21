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

#include "KEYPath.h"

#include "KEYPathTest.h"

using libkeynote::KEYPath;

using std::string;

namespace test
{

namespace
{

double in2pt(const double in)
{
  return 72 * in;
}

string toSVG(const KEYPath &path)
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

void KEYPathTest::setUp()
{
}

void KEYPathTest::tearDown()
{
}

void  KEYPathTest::testConstruction()
{
  {
    const string src = "M 0.0 0.0";
    KEYPath refPath;
    refPath.appendMoveTo(0, 0);

    KEYPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }

  {
    const string src = "L 0.0 0.0";
    KEYPath refPath;
    refPath.appendLineTo(0, 0);

    KEYPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }

  {
    const string src = "C 0.5 0.5 0 0 1 1";
    KEYPath refPath;
    refPath.appendCurveTo(0.5, 0.5, 0, 0, 1, 1);

    KEYPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }

  {
    const string src = "Z";
    KEYPath refPath;
    refPath.appendClose();

    KEYPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }

  {
    const string src = "M 0 0 L 1 1";
    KEYPath refPath;
    refPath.appendMoveTo(0, 0);
    refPath.appendLineTo(1, 1);

    KEYPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }

  {
    const string src = "M 0 0 L 1 0 L 1 1 L 0 1 Z L 0 0";
    KEYPath refPath;
    refPath.appendMoveTo(0, 0);
    refPath.appendLineTo(1, 0);
    refPath.appendLineTo(1, 1);
    refPath.appendLineTo(0, 1);
    refPath.appendClose();
    refPath.appendLineTo(0, 0);

    KEYPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }

  {
    const string src = "M 0.0 0.0 L 0 1 C 1 1 0.5 0.5 0 0 Z";
    KEYPath refPath;
    refPath.appendMoveTo(0, 0);
    refPath.appendLineTo(0, 1);
    refPath.appendCurveTo(1, 1, 0.5, 0.5, 0, 0);
    refPath.appendClose();

    KEYPath testPath(src);

    CPPUNIT_ASSERT(refPath == testPath);
  }
}

void  KEYPathTest::testConversion()
{
  {
    const string ref = "M 0 0";
    KEYPath path;
    path.appendMoveTo(0, 0);

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }

  {
    const string ref = "L 0 0";
    KEYPath path;
    path.appendLineTo(0, 0);

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }

  {
    const string ref = "C 1 1 0 0 0.5 0.5";
    KEYPath path;
    path.appendCurveTo(1, 1, 0, 0, 0.5, 0.5);

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }

  {
    const string ref = "Z";
    KEYPath path;
    path.appendClose();

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }

  {
    const string ref = "M 0 0 L 1 1";
    KEYPath path;
    path.appendMoveTo(0, 0);
    path.appendLineTo(1, 1);

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }

  {
    const string ref = "M 0 0 L 1 0 L 1 1 L 0 1 Z L 0 0";
    KEYPath path;
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
    KEYPath path;
    path.appendMoveTo(0, 0);
    path.appendLineTo(0, 1);
    path.appendCurveTo(1, 1, 0.5, 0.5, 0, 0);
    path.appendClose();

    CPPUNIT_ASSERT_EQUAL(ref, toSVG(path));
  }
}

CPPUNIT_TEST_SUITE_REGISTRATION(KEYPathTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
