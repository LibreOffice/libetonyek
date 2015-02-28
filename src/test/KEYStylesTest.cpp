/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYStyles.h"

#include "KEYStylesTest.h"

namespace test
{

using boost::any;
using boost::any_cast;
using boost::get;
using boost::optional;
using boost::shared_ptr;
using boost::unordered_map;

using libetonyek::IWORKPropertyMap;
using libetonyek::IWORKStylePtr_t;
using libetonyek::IWORKStylesheet;
using libetonyek::IWORKStylesheetPtr_t;
using libetonyek::KEYStyleBase;
using libetonyek::IWORKStyleContext;

using std::string;

namespace
{

class Style;

struct Stylesheet : public IWORKStylesheet
{
  unordered_map<string, shared_ptr<Style> > testStyles;
};

}

namespace
{

class Style : public KEYStyleBase
{
public:
  explicit Style(const IWORKPropertyMap &props,
                 const optional<string> &ident = optional<string>(),
                 const optional<string> &parentIdent = optional<string>());

  optional<int> getAnswer(const IWORKStyleContext &context = IWORKStyleContext()) const;

  any get(const char *const prop, bool lookInParent) const;

private:
  virtual IWORKStylePtr_t find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const;
};

Style::Style(const IWORKPropertyMap &props, const optional<string> &ident, const optional<string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

optional<int> Style::getAnswer(const IWORKStyleContext &context) const
{
  optional<int> value;

  any prop = lookup("answer", context);
  if (!prop.empty())
    value = boost::any_cast<int>(prop);

  return value;
}

any Style::get(const char *const prop, const bool lookInParent) const
{
  return getPropertyMap().get(prop, lookInParent);
}

IWORKStylePtr_t Style::find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  IWORKStylePtr_t style;

  const Stylesheet *const testStylesheet = static_cast<const Stylesheet *>(stylesheet.get());

  // reuse character styles for the test
  if (testStylesheet->testStyles.end() != testStylesheet->testStyles.find(ident))
    style = testStylesheet->testStyles.find(ident)->second;

  return style;
}

}

namespace
{

shared_ptr<Style> makeStyle(const IWORKPropertyMap &props,
                            const optional<string> &ident = optional<string>(),
                            const optional<string> &parentIdent = optional<string>())
{
  const shared_ptr<Style> style(new Style(props, ident, parentIdent));
  return style;
}

}

void KEYStylesTest::setUp()
{
}

void KEYStylesTest::tearDown()
{
}

void KEYStylesTest::testLink()
{
  const shared_ptr<Stylesheet> stylesheet(new Stylesheet());
  IWORKStylePtr_t parent;

  {
    IWORKPropertyMap props;
    props.set("answer", 2);

    stylesheet->testStyles["wrong"] = makeStyle(props, string("wrong"));
  }

  {
    IWORKPropertyMap props;
    props.set("answer", 42);

    parent = stylesheet->testStyles["parent"] = makeStyle(props, string("parent"), string("grandparent"));
    CPPUNIT_ASSERT(bool(parent));
  }

  {
    IWORKPropertyMap props;
    props.set("antwort", 42);

    stylesheet->testStyles["grandparent"] = makeStyle(props, string("grandparent"));
  }

  // without parent
  {
    Style style = Style(IWORKPropertyMap());
    CPPUNIT_ASSERT(style.get("answer", false).empty());
    CPPUNIT_ASSERT(style.get("answer", true).empty());

    style.link(stylesheet);
    CPPUNIT_ASSERT(style.get("answer", false).empty());
    CPPUNIT_ASSERT(style.get("answer", true).empty());
  }

  // the style's props remain unchanged
  {
    IWORKPropertyMap props;
    props.set("answer", 8);

    Style style(props);
    CPPUNIT_ASSERT(!style.get("answer", false).empty());

    style.link(stylesheet);
    CPPUNIT_ASSERT(!style.get("answer", false).empty());
    CPPUNIT_ASSERT_EQUAL(8, any_cast<int>(style.get("answer", false)));
  }

  // with parent in the same stylesheet
  {
    Style style(IWORKPropertyMap(), string("test"), string("parent"));
    CPPUNIT_ASSERT(style.get("answer", false).empty());
    CPPUNIT_ASSERT(style.get("answer", true).empty());

    style.link(stylesheet);
    CPPUNIT_ASSERT(style.get("answer", false).empty());
    CPPUNIT_ASSERT(!style.get("answer", true).empty());
    CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(style.get("answer", true)));
  }

  // linking through more styles
  {
    Style style(IWORKPropertyMap(), string("test"), string("parent"));
    CPPUNIT_ASSERT(style.get("antwort", false).empty());
    CPPUNIT_ASSERT(style.get("antwort", true).empty());

    style.link(stylesheet);
    parent->link(stylesheet);
    CPPUNIT_ASSERT(style.get("antwort", false).empty());
    CPPUNIT_ASSERT(!style.get("antwort", true).empty());
    CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(style.get("antwort", true)));
  }
}

void KEYStylesTest::testFlatten()
{
  // TODO: implement me
}

void KEYStylesTest::testLookup()
{
  // without context
  {
    IWORKPropertyMap props;
    props.set("answer", 42);

    Style style(props);
    CPPUNIT_ASSERT(style.getAnswer());
    CPPUNIT_ASSERT_EQUAL(42, get(style.getAnswer()));
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
      const Style style = Style(IWORKPropertyMap());
      CPPUNIT_ASSERT(style.getAnswer(context));
      CPPUNIT_ASSERT_EQUAL(2, get(style.getAnswer(context)));
    }

    IWORKPropertyMap props;
    props.set("answer", 42);

    // lookup in own prop. set
    {
      const Style style(props);
      CPPUNIT_ASSERT(style.getAnswer(context));
      CPPUNIT_ASSERT_EQUAL(42, get(style.getAnswer(context)));
    }
  }
}

CPPUNIT_TEST_SUITE_REGISTRATION(KEYStylesTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
