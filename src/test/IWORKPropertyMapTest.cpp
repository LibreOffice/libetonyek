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

#include "IWORKPropertyInfo.h"
#include "IWORKPropertyMap.h"

#include "TestProperties.h"

namespace test
{

using libetonyek::property::Answer;
using libetonyek::property::Antwort;
using libetonyek::IWORKPropertyInfo;
using libetonyek::IWORKPropertyMap;

class IWORKPropertyMapTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(IWORKPropertyMapTest);
  CPPUNIT_TEST(testLookup);
  CPPUNIT_TEST(testLookupWithParent);
  CPPUNIT_TEST_SUITE_END();

private:
  void testLookup();
  void testLookupWithParent();
};

void IWORKPropertyMapTest::setUp()
{
}

void IWORKPropertyMapTest::tearDown()
{
}

void IWORKPropertyMapTest::testLookup()
{
  IWORKPropertyMap props;

  // empty map
  CPPUNIT_ASSERT(!props.has<Answer>());
  CPPUNIT_ASSERT_THROW(props.get<Answer>(), IWORKPropertyMap::NotFoundException);

  // existing value
  props.put<Answer>(42);
  CPPUNIT_ASSERT(props.has<Answer>());
  CPPUNIT_ASSERT_NO_THROW(props.get<Answer>());
  CPPUNIT_ASSERT_EQUAL(42, props.get<Answer>());

  // non-existing value
  CPPUNIT_ASSERT(!props.has<Antwort>());
  CPPUNIT_ASSERT_THROW(props.get<Antwort>(), IWORKPropertyMap::NotFoundException);

  // cleared value
  props.clear<Answer>();
  CPPUNIT_ASSERT(!props.has<Answer>());
  CPPUNIT_ASSERT_THROW(props.get<Answer>(), IWORKPropertyMap::NotFoundException);
}

void IWORKPropertyMapTest::testLookupWithParent()
{
  // simple recursive lookup test
  {
    IWORKPropertyMap parent;
    parent.put<Answer>(42);

    IWORKPropertyMap props;
    props.setParent(&parent);

    CPPUNIT_ASSERT(!props.has<Answer>());
    CPPUNIT_ASSERT(props.has<Answer>(true));
    CPPUNIT_ASSERT_EQUAL(42, props.get<Answer>(true));

    // rewrite key in props
    props.put<Answer>(3);
    CPPUNIT_ASSERT(props.has<Answer>(true));
    CPPUNIT_ASSERT_EQUAL(3, props.get<Answer>(true));

    // nonexisting value
    CPPUNIT_ASSERT(!props.has<Antwort>(true));

    IWORKPropertyMap grandparent;
    grandparent.put<Antwort>(17);

    // recursive lookup through more parents
    parent.setParent(&grandparent);
    CPPUNIT_ASSERT(props.has<Antwort>(true));
    CPPUNIT_ASSERT_EQUAL(17, props.get<Antwort>(true));

    // cleared value
    props.clear<Answer>();
    CPPUNIT_ASSERT(!props.has<Answer>());
    CPPUNIT_ASSERT(!props.has<Answer>(true));
  }

  // switching of parents
  {
    IWORKPropertyMap parent1;
    parent1.put<Answer>(42);

    IWORKPropertyMap parent2;
    parent2.put<Antwort>(13);

    IWORKPropertyMap props;

    props.setParent(&parent1);
    CPPUNIT_ASSERT(props.has<Answer>(true));
    CPPUNIT_ASSERT_EQUAL(42, props.get<Answer>(true));

    // switch parent
    props.setParent(&parent2);
    CPPUNIT_ASSERT(!props.has<Answer>(true));
    CPPUNIT_ASSERT(props.has<Antwort>(true));
    CPPUNIT_ASSERT_EQUAL(13, props.get<Antwort>(true));

    // disable parent
    props.setParent(0);
    CPPUNIT_ASSERT(!props.has<Answer>(true));
    CPPUNIT_ASSERT(!props.has<Antwort>(true));
  }
}

CPPUNIT_TEST_SUITE_REGISTRATION(IWORKPropertyMapTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
