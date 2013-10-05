/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/math/constants/constants.hpp>

#include "KNObject.h"
#include "KNPath.h"
#include "KNShape.h"
#include "KNTypes.h"

#include "KNShapeTest.h"

namespace test
{

namespace m = boost::math::double_constants;

using libkeynote::KNPath;
using libkeynote::KNPathPtr_t;
using libkeynote::KNSize;

void KNShapeTest::setUp()
{
}

void KNShapeTest::tearDown()
{
}

void KNShapeTest::testMakePolygonPath()
{
  using libkeynote::makePolygonPath;

  const KNSize size(100, 100);

  // triangle
  {
    const double d = 25 * (2 - m::root_three);

    // FIXME: the shape is not scaled to whole width...
    KNPath ref;
    ref.appendMoveTo(50, 0);
    ref.appendLineTo(100 - d, 75);
    ref.appendLineTo(d, 75);
    ref.appendClose();

    const KNPathPtr_t test = makePolygonPath(size, 3);

    CPPUNIT_ASSERT(bool(test));
    CPPUNIT_ASSERT(*test == ref);
  }

  // diamond
  {
    KNPath ref;
    ref.appendMoveTo(50, 0);
    ref.appendLineTo(100, 50);
    ref.appendLineTo(50, 100);
    ref.appendLineTo(0, 50);
    ref.appendClose();

    const KNPathPtr_t test = makePolygonPath(size, 4);

    CPPUNIT_ASSERT(bool(test));
    CPPUNIT_ASSERT(*test == ref);
  }

  // octagon
  {
    const double d = 25 * (2 - m::root_two);

    KNPath ref;
    ref.appendMoveTo(50, 0);
    ref.appendLineTo(100 - d, d);
    ref.appendLineTo(100, 50);
    ref.appendLineTo(100 - d, 100 -d);
    ref.appendLineTo(50, 100);
    ref.appendLineTo(d, 100 - d);
    ref.appendLineTo(0, 50);
    ref.appendLineTo(d, d);
    ref.appendClose();

    const KNPathPtr_t test = makePolygonPath(size, 8);

    CPPUNIT_ASSERT(bool(test));
    CPPUNIT_ASSERT(*test == ref);
  }
}

void KNShapeTest::testMakeRoundedRectanglePath()
{
  // TODO: implement me
}

void KNShapeTest::testMakeArrowPath()
{
  // TODO: implement me
}

void KNShapeTest::testMakeDoubleArrowPath()
{
  // TODO: implement me
}

void KNShapeTest::testMakeStarPath()
{
  // TODO: implement me
}

void KNShapeTest::testMakeConnectionPath()
{
  // TODO: implement me
}

void KNShapeTest::testMakeCalloutPath()
{
  // TODO: implement me
}

void KNShapeTest::testMakeQuoteBubblePath()
{
  // TODO: implement me
}

CPPUNIT_TEST_SUITE_REGISTRATION(KNShapeTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
