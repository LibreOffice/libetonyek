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

#include "IWORKStyle.h"
#include "IWORKStyleContext.h"

namespace test
{

using boost::any;
using boost::any_cast;
using boost::get;
using boost::optional;
using boost::shared_ptr;
using boost::unordered_map;

using libetonyek::ID_t;
using libetonyek::IWORKPropertyMap;
using libetonyek::IWORKStyle;
using libetonyek::IWORKStylePtr_t;
using libetonyek::IWORKStylesheet;
using libetonyek::IWORKStylesheetPtr_t;
using libetonyek::IWORKStyleContext;

using std::string;

namespace
{

IWORKStylePtr_t makeStyle(const IWORKPropertyMap &props,
                          const optional<string> &ident = optional<string>(),
                          const optional<string> &parentIdent = optional<string>())
{
  const IWORKStylePtr_t style(new IWORKStyle(props, ident, parentIdent));
  return style;
}

optional<int> getAnswer(const IWORKStyle &style, const IWORKStyleContext &context = IWORKStyleContext())
{
  optional<int> value;

  any prop = style.lookup("answer", context);
  if (!prop.empty())
    value = any_cast<int>(prop);

  return value;
}

any get(const IWORKStyle &style, const char *const prop, const bool lookInParent)
{
  return style.getPropertyMap().get(prop, lookInParent);
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
    props.set("answer", 2);

    stylesheet->m_styles["wrong"] = makeStyle(props, string("wrong"));
  }

  {
    IWORKPropertyMap props;
    props.set("answer", 42);

    parent = stylesheet->m_styles["parent"] = makeStyle(props, string("parent"), string("grandparent"));
    CPPUNIT_ASSERT(bool(parent));
  }

  {
    IWORKPropertyMap props;
    props.set("antwort", 42);

    stylesheet->m_styles["grandparent"] = makeStyle(props, string("grandparent"));
  }

  optional<ID_t> dummyIdent;

  // without parent
  {
    IWORKStyle style(IWORKPropertyMap(), dummyIdent, dummyIdent);
    CPPUNIT_ASSERT(get(style, "answer", false).empty());
    CPPUNIT_ASSERT(get(style, "answer", true).empty());

    style.link(stylesheet);
    CPPUNIT_ASSERT(get(style, "answer", false).empty());
    CPPUNIT_ASSERT(get(style, "answer", true).empty());
  }

  // the style's props remain unchanged
  {
    IWORKPropertyMap props;
    props.set("answer", 8);

    IWORKStyle style(props, dummyIdent, dummyIdent);
    CPPUNIT_ASSERT(!get(style, "answer", false).empty());

    style.link(stylesheet);
    CPPUNIT_ASSERT(!get(style, "answer", false).empty());
    CPPUNIT_ASSERT_EQUAL(8, any_cast<int>(get(style, "answer", false)));
  }

  // with parent in the same stylesheet
  {
    IWORKStyle style(IWORKPropertyMap(), string("test"), string("parent"));
    CPPUNIT_ASSERT(get(style, "answer", false).empty());
    CPPUNIT_ASSERT(get(style, "answer", true).empty());

    style.link(stylesheet);
    CPPUNIT_ASSERT(get(style, "answer", false).empty());
    CPPUNIT_ASSERT(!get(style, "answer", true).empty());
    CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(get(style, "answer", true)));
  }

  // linking through more styles
  {
    IWORKStyle style(IWORKPropertyMap(), string("test"), string("parent"));
    CPPUNIT_ASSERT(get(style, "antwort", false).empty());
    CPPUNIT_ASSERT(get(style, "antwort", true).empty());

    style.link(stylesheet);
    parent->link(stylesheet);
    CPPUNIT_ASSERT(get(style, "antwort", false).empty());
    CPPUNIT_ASSERT(!get(style, "antwort", true).empty());
    CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(get(style, "antwort", true)));
  }
}

void IWORKStyleTest::testFlatten()
{
  // TODO: implement me
}

void IWORKStyleTest::testLookup()
{
  optional<ID_t> dummyIdent;

  // without context
  {
    IWORKPropertyMap props;
    props.set("answer", 42);

    IWORKStyle style(props, dummyIdent, dummyIdent);
    CPPUNIT_ASSERT(getAnswer(style));
    CPPUNIT_ASSERT_EQUAL(42, get(getAnswer(style)));
  }

  // with context
  {
    IWORKStyleContext context;

    IWORKPropertyMap ctxtProps;
    ctxtProps.set("answer", 2);

    context.push();
    context.set(makeStyle(ctxtProps));

    // lookup through context only
    {
      const IWORKStyle style(IWORKPropertyMap(), dummyIdent, dummyIdent);
      CPPUNIT_ASSERT(getAnswer(style, context));
      CPPUNIT_ASSERT_EQUAL(2, get(getAnswer(style, context)));
    }

    IWORKPropertyMap props;
    props.set("answer", 42);

    // lookup in own prop. set
    {
      const IWORKStyle style(props, dummyIdent, dummyIdent);
      CPPUNIT_ASSERT(getAnswer(style, context));
      CPPUNIT_ASSERT_EQUAL(42, get(getAnswer(style, context)));
    }
  }
}

CPPUNIT_TEST_SUITE_REGISTRATION(IWORKStyleTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
