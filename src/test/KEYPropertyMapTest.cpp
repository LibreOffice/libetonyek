/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYPropertyMap.h"

#include "KEYPropertyMapTest.h"

namespace test
{

using boost::any;
using boost::any_cast;

using libetonyek::KEYPropertyMap;

void KEYPropertyMapTest::setUp()
{
}

void KEYPropertyMapTest::tearDown()
{
}

void KEYPropertyMapTest::testLookup()
{
  KEYPropertyMap props;

  // empty map
  CPPUNIT_ASSERT(props.get("answer").empty());

  // known key
  props.set("answer", 42);
  CPPUNIT_ASSERT(!props.get("answer").empty());
  CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(props.get("answer")));

  // unknown key
  CPPUNIT_ASSERT(props.get("antwort").empty());
}

void KEYPropertyMapTest::testLookupWithParent()
{
  // simple recursive lookup test
  {
    KEYPropertyMap parent;
    parent.set("answer", 42);

    KEYPropertyMap props;
    props.setParent(&parent);

    CPPUNIT_ASSERT(!props.get("answer", true).empty());
    CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(props.get("answer", true)));

    // rewrite key in props
    props.set("answer", 3);
    CPPUNIT_ASSERT(!props.get("answer", true).empty());
    CPPUNIT_ASSERT_EQUAL(3, any_cast<int>(props.get("answer", true)));

    // unknown key
    CPPUNIT_ASSERT(props.get("antwort", true).empty());

    KEYPropertyMap grandparent;
    grandparent.set("antwort", 17);

    // recursive lookup through more parents
    parent.setParent(&grandparent);
    CPPUNIT_ASSERT(!props.get("antwort", true).empty());
    CPPUNIT_ASSERT_EQUAL(17, any_cast<int>(props.get("antwort", true)));
  }

  // switching of parents
  {
    KEYPropertyMap parent1;
    parent1.set("answer", 42);

    KEYPropertyMap parent2;
    parent2.set("antwort", 13);

    KEYPropertyMap props;

    props.setParent(&parent1);
    CPPUNIT_ASSERT(!props.get("answer", true).empty());
    CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(props.get("answer", true)));

    // switch parent
    props.setParent(&parent2);
    CPPUNIT_ASSERT(props.get("answer", true).empty());
    CPPUNIT_ASSERT(!props.get("antwort", true).empty());
    CPPUNIT_ASSERT_EQUAL(13, any_cast<int>(props.get("antwort", true)));

    // disable parent
    props.setParent(0);
    CPPUNIT_ASSERT(props.get("answer", true).empty());
    CPPUNIT_ASSERT(props.get("antwort", true).empty());
  }
}

CPPUNIT_TEST_SUITE_REGISTRATION(KEYPropertyMapTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
