/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KNPropertyMap.h"

#include "KNPropertyMapTest.h"

namespace test
{

using boost::any;
using boost::any_cast;

using libkeynote::KNPropertyMap;

void KNPropertyMapTest::setUp()
{
}

void KNPropertyMapTest::tearDown()
{
}

void KNPropertyMapTest::testLookup()
{
  KNPropertyMap props;

  // empty map
  CPPUNIT_ASSERT(props.get("answer").empty());

  // known key
  props.set("answer", 42);
  CPPUNIT_ASSERT(!props.get("answer").empty());
  CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(props.get("answer")));

  // unknown key
  CPPUNIT_ASSERT(props.get("antwort").empty());
}

void KNPropertyMapTest::testLookupWithParent()
{
  // simple recursive lookup test
  {
    KNPropertyMap parent;
    parent.set("answer", 42);

    KNPropertyMap props;
    props.setParent(&parent);

    CPPUNIT_ASSERT(!props.get("answer", true).empty());
    CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(props.get("answer", true)));

    // rewrite key in props
    props.set("answer", 3);
    CPPUNIT_ASSERT(!props.get("answer", true).empty());
    CPPUNIT_ASSERT_EQUAL(3, any_cast<int>(props.get("answer", true)));

    // unknown key
    CPPUNIT_ASSERT(props.get("antwort", true).empty());

    KNPropertyMap grandparent;
    grandparent.set("antwort", 17);

    // recursive lookup through more parents
    parent.setParent(&grandparent);
    CPPUNIT_ASSERT(!props.get("antwort", true).empty());
    CPPUNIT_ASSERT_EQUAL(17, any_cast<int>(props.get("antwort", true)));
  }

  // switching of parents
  {
    KNPropertyMap parent1;
    parent1.set("answer", 42);

    KNPropertyMap parent2;
    parent2.set("antwort", 13);

    KNPropertyMap props;

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

CPPUNIT_TEST_SUITE_REGISTRATION(KNPropertyMapTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
