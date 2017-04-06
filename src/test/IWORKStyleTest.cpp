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
#include "IWORKStyle.h"

#include "TestProperties.h"

namespace test
{

using boost::optional;

using libetonyek::property::Answer;
using libetonyek::property::Antwort;
using libetonyek::ID_t;
using libetonyek::IWORKPropertyInfo;
using libetonyek::IWORKPropertyMap;
using libetonyek::IWORKStyle;
using libetonyek::IWORKStylePtr_t;
using libetonyek::IWORKStylesheet;
using libetonyek::IWORKStylesheetPtr_t;

using std::shared_ptr;
using std::string;
using std::unordered_map;

namespace
{

IWORKStylePtr_t makeStyle(const IWORKPropertyMap &props,
                          const optional<string> &ident = optional<string>(),
                          const optional<string> &parentIdent = optional<string>())
{
  const IWORKStylePtr_t style(new IWORKStyle(props, ident, parentIdent));
  return style;
}

template<class Property>
bool hasProperty(const IWORKStyle &style, const bool lookInParent)
{
  return style.getPropertyMap().has<Property>(lookInParent);
}

template<class Property>
const typename IWORKPropertyInfo<Property>::ValueType &getProperty(const IWORKStyle &style, const bool lookInParent)
{
  return style.getPropertyMap().get<Property>(lookInParent);
}

}

class IWORKStyleTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(IWORKStyleTest);
  CPPUNIT_TEST(testLink);
  CPPUNIT_TEST(testFlatten);
  CPPUNIT_TEST(testLookup);
  CPPUNIT_TEST_SUITE_END();

private:
  void testLink();
  void testFlatten();
  void testLookup();
};

void IWORKStyleTest::setUp()
{
}

void IWORKStyleTest::tearDown()
{
}

void IWORKStyleTest::testLink()
{
  const IWORKStylesheetPtr_t stylesheet(new IWORKStylesheet());
  IWORKStylePtr_t parent;

  {
    IWORKPropertyMap props;
    props.put<Answer>(2);

    stylesheet->m_styles["wrong"] = makeStyle(props, string("wrong"));
  }

  {
    IWORKPropertyMap props;
    props.put<Answer>(42);

    parent = stylesheet->m_styles["parent"] = makeStyle(props, string("parent"), string("grandparent"));
    CPPUNIT_ASSERT(bool(parent));
  }

  {
    IWORKPropertyMap props;
    props.put<Antwort>(42);

    stylesheet->m_styles["grandparent"] = makeStyle(props, string("grandparent"));
  }

  optional<ID_t> dummyIdent;

  // without parent
  {
    IWORKStyle style(IWORKPropertyMap(), dummyIdent, dummyIdent);
    CPPUNIT_ASSERT(!hasProperty<Answer>(style, false));
    CPPUNIT_ASSERT(!hasProperty<Answer>(style, true));

    style.link(stylesheet);
    CPPUNIT_ASSERT(!hasProperty<Answer>(style, false));
    CPPUNIT_ASSERT(!hasProperty<Answer>(style, true));
  }

  // the style's props remain unchanged
  {
    IWORKPropertyMap props;
    props.put<Answer>(8);

    IWORKStyle style(props, dummyIdent, dummyIdent);
    CPPUNIT_ASSERT(hasProperty<Answer>(style, false));

    style.link(stylesheet);
    CPPUNIT_ASSERT(hasProperty<Answer>(style, false));
    CPPUNIT_ASSERT_EQUAL(8, getProperty<Answer>(style, false));
  }

  // with parent in the same stylesheet
  {
    IWORKStyle style(IWORKPropertyMap(), string("test"), string("parent"));
    CPPUNIT_ASSERT(!hasProperty<Answer>(style, false));
    CPPUNIT_ASSERT(!hasProperty<Answer>(style, true));

    style.link(stylesheet);
    CPPUNIT_ASSERT(!hasProperty<Answer>(style, false));
    CPPUNIT_ASSERT(hasProperty<Answer>(style, true));
    CPPUNIT_ASSERT_EQUAL(42, getProperty<Answer>(style, true));
  }

  // linking through more styles
  {
    IWORKStyle style(IWORKPropertyMap(), string("test"), string("parent"));
    CPPUNIT_ASSERT(!hasProperty<Antwort>(style, false));
    CPPUNIT_ASSERT(!hasProperty<Antwort>(style, true));

    style.link(stylesheet);
    parent->link(stylesheet);
    CPPUNIT_ASSERT(!hasProperty<Antwort>(style, false));
    CPPUNIT_ASSERT(hasProperty<Antwort>(style, true));
    CPPUNIT_ASSERT_EQUAL(42, getProperty<Antwort>(style, true));
  }
}

void IWORKStyleTest::testFlatten()
{
  // TODO: implement me
}

void IWORKStyleTest::testLookup()
{
  optional<ID_t> dummyIdent;

  IWORKPropertyMap props;
  props.put<Answer>(42);

  IWORKStyle style(props, dummyIdent, dummyIdent);
  CPPUNIT_ASSERT(style.has<Answer>());
  CPPUNIT_ASSERT_EQUAL(42, style.get<Answer>());
}

CPPUNIT_TEST_SUITE_REGISTRATION(IWORKStyleTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
