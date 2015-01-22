/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKPath.h"
#include "IWORKTypes.h"
#include "KEYObject.h"
#include "KEYShape.h"
#include "KEYTypes.h"

#include "KEYShapeTest.h"

namespace test
{

using libetonyek::IWORKPath;
using libetonyek::IWORKPathPtr_t;
using libetonyek::IWORKSize;
using libetonyek::etonyek_root_three;
using libetonyek::etonyek_root_two;

void KEYShapeTest::setUp()
{
}

void KEYShapeTest::tearDown()
{
}

void KEYShapeTest::testMakePolygonPath()
{
  using libetonyek::makePolygonPath;

  const IWORKSize size(100, 100);

  // triangle
  {
    const double d = 25 * (2 - etonyek_root_three);

    // FIXME: the shape is not scaled to whole width...
    IWORKPath ref;
    ref.appendMoveTo(50, 0);
    ref.appendLineTo(100 - d, 75);
    ref.appendLineTo(d, 75);
    ref.appendClose();

    const IWORKPathPtr_t test = makePolygonPath(size, 3);

    CPPUNIT_ASSERT(bool(test));
    CPPUNIT_ASSERT(*test == ref);
  }

  // diamond
  {
    IWORKPath ref;
    ref.appendMoveTo(50, 0);
    ref.appendLineTo(100, 50);
    ref.appendLineTo(50, 100);
    ref.appendLineTo(0, 50);
    ref.appendClose();

    const IWORKPathPtr_t test = makePolygonPath(size, 4);

    CPPUNIT_ASSERT(bool(test));
    CPPUNIT_ASSERT(*test == ref);
  }

  // octagon
  {
    const double d = 25 * (2 - etonyek_root_two);

    IWORKPath ref;
    ref.appendMoveTo(50, 0);
    ref.appendLineTo(100 - d, d);
    ref.appendLineTo(100, 50);
    ref.appendLineTo(100 - d, 100 -d);
    ref.appendLineTo(50, 100);
    ref.appendLineTo(d, 100 - d);
    ref.appendLineTo(0, 50);
    ref.appendLineTo(d, d);
    ref.appendClose();

    const IWORKPathPtr_t test = makePolygonPath(size, 8);

    CPPUNIT_ASSERT(bool(test));
    CPPUNIT_ASSERT(*test == ref);
  }
}

void KEYShapeTest::testMakeRoundedRectanglePath()
{
  // TODO: implement me
}

void KEYShapeTest::testMakeArrowPath()
{
  // TODO: implement me
}

void KEYShapeTest::testMakeDoubleArrowPath()
{
  // TODO: implement me
}

void KEYShapeTest::testMakeStarPath()
{
  // TODO: implement me
}

void KEYShapeTest::testMakeConnectionPath()
{
  // TODO: implement me
}

void KEYShapeTest::testMakeCalloutPath()
{
  // TODO: implement me
}

void KEYShapeTest::testMakeQuoteBubblePath()
{
  // TODO: implement me
}

CPPUNIT_TEST_SUITE_REGISTRATION(KEYShapeTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
