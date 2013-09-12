/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string>

#include "KNPath.h"

#include "KNPathTest.h"

using libkeynote::KNPath;

using std::string;

namespace test
{

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

void  KNPathTest::testSVG()
{
  {
    const string ref = "M 0 0";
    KNPath path;
    path.appendMoveTo(0, 0);

    CPPUNIT_ASSERT_EQUAL(ref, path.toSvg());
  }

  {
    const string ref = "L 0 0";
    KNPath path;
    path.appendLineTo(0, 0);

    CPPUNIT_ASSERT_EQUAL(ref, path.toSvg());
  }

  {
    const string ref = "C 1 1 0 0 0.5 0.5";
    KNPath path;
    path.appendCurveTo(1, 1, 0, 0, 0.5, 0.5);

    CPPUNIT_ASSERT_EQUAL(ref, path.toSvg());
  }

  {
    const string ref = "Z";
    KNPath path;
    path.appendClose();

    CPPUNIT_ASSERT_EQUAL(ref, path.toSvg());
  }

  {
    const string ref = "M 0 0 L 1 1";
    KNPath path;
    path.appendMoveTo(0, 0);
    path.appendLineTo(1, 1);

    CPPUNIT_ASSERT_EQUAL(ref, path.toSvg());
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

    CPPUNIT_ASSERT_EQUAL(ref, path.toSvg());
  }

  {
    const string ref = "M 0.0 0.0 L 0 1 C 1 1 0.5 0.5 0 0 Z";
    KNPath path;
    path.appendMoveTo(0, 0);
    path.appendLineTo(0, 1);
    path.appendCurveTo(1, 1, 0.5, 0.5, 0, 0);
    path.appendClose();

    CPPUNIT_ASSERT_EQUAL(ref, path.toSvg());
  }
}

CPPUNIT_TEST_SUITE_REGISTRATION(KNPathTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
