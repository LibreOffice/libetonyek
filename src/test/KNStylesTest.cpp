/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KNStyles.h"

#include "KNStylesTest.h"

namespace test
{

using boost::any;
using boost::any_cast;
using boost::get;
using boost::optional;
using boost::shared_ptr;
using boost::unordered_map;

using libkeynote::KNPropertyMap;
using libkeynote::KNStyleBase;
using libkeynote::KNStyleContext;
using libkeynote::KNStylePtr_t;
using libkeynote::KNStylesheet;
using libkeynote::KNStylesheetPtr_t;

using std::string;

namespace
{

class Style;

struct Stylesheet : public KNStylesheet
{
  unordered_map<string, shared_ptr<Style> > testStyles;
};

}

namespace
{

class Style : public KNStyleBase
{
public:
  explicit Style(const KNPropertyMap &props,
                 const optional<string> &ident = optional<string>(),
                 const optional<string> &parentIdent = optional<string>());

  optional<int> getAnswer(const KNStyleContext &context = KNStyleContext()) const;

  any get(const char *const prop, bool lookInParent) const;

private:
  virtual KNStylePtr_t find(const KNStylesheetPtr_t &stylesheet, const std::string &ident) const;
};

Style::Style(const KNPropertyMap &props, const optional<string> &ident, const optional<string> &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

optional<int> Style::getAnswer(const KNStyleContext &context) const
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

KNStylePtr_t Style::find(const KNStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  KNStylePtr_t style;

  const Stylesheet *const testStylesheet = static_cast<const Stylesheet *>(stylesheet.get());

  // reuse character styles for the test
  if (testStylesheet->testStyles.end() != testStylesheet->testStyles.find(ident))
    style = testStylesheet->testStyles.find(ident)->second;

  return style;
}

}

namespace
{

shared_ptr<Style> makeStyle(const KNPropertyMap &props,
                            const optional<string> &ident = optional<string>(),
                            const optional<string> &parentIdent = optional<string>())
{
  const shared_ptr<Style> style(new Style(props, ident, parentIdent));
  return style;
}

}

void KNStylesTest::setUp()
{
}

void KNStylesTest::tearDown()
{
}

void KNStylesTest::testLink()
{
  const shared_ptr<Stylesheet> stylesheet(new Stylesheet());
  KNStylePtr_t parent;

  {
    KNPropertyMap props;
    props.set("answer", 2);

    stylesheet->testStyles["wrong"] = makeStyle(props, string("wrong"));
  }

  {
    KNPropertyMap props;
    props.set("answer", 42);

    parent = stylesheet->testStyles["parent"] = makeStyle(props, string("parent"), string("grandparent"));
    CPPUNIT_ASSERT(bool(parent));
  }

  {
    KNPropertyMap props;
    props.set("antwort", 42);

    stylesheet->testStyles["grandparent"] = makeStyle(props, string("grandparent"));
  }

  // without parent
  {
    Style style = Style(KNPropertyMap());
    CPPUNIT_ASSERT(style.get("answer", false).empty());
    CPPUNIT_ASSERT(style.get("answer", true).empty());

    style.link(stylesheet);
    CPPUNIT_ASSERT(style.get("answer", false).empty());
    CPPUNIT_ASSERT(style.get("answer", true).empty());
  }

  // the style's props remain unchanged
  {
    KNPropertyMap props;
    props.set("answer", 8);

    Style style(props);
    CPPUNIT_ASSERT(!style.get("answer", false).empty());

    style.link(stylesheet);
    CPPUNIT_ASSERT(!style.get("answer", false).empty());
    CPPUNIT_ASSERT_EQUAL(8, any_cast<int>(style.get("answer", false)));
  }

  // with parent in the same stylesheet
  {
    Style style(KNPropertyMap(), string("test"), string("parent"));
    CPPUNIT_ASSERT(style.get("answer", false).empty());
    CPPUNIT_ASSERT(style.get("answer", true).empty());

    style.link(stylesheet);
    CPPUNIT_ASSERT(style.get("answer", false).empty());
    CPPUNIT_ASSERT(!style.get("answer", true).empty());
    CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(style.get("answer", true)));
  }

  // linking through more styles
  {
    Style style(KNPropertyMap(), string("test"), string("parent"));
    CPPUNIT_ASSERT(style.get("antwort", false).empty());
    CPPUNIT_ASSERT(style.get("antwort", true).empty());

    style.link(stylesheet);
    parent->link(stylesheet);
    CPPUNIT_ASSERT(style.get("antwort", false).empty());
    CPPUNIT_ASSERT(!style.get("antwort", true).empty());
    CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(style.get("antwort", true)));
  }
}

void KNStylesTest::testFlatten()
{
  // TODO: implement me
}

void KNStylesTest::testLookup()
{
  // without context
  {
    KNPropertyMap props;
    props.set("answer", 42);

    Style style(props);
    CPPUNIT_ASSERT(style.getAnswer());
    CPPUNIT_ASSERT_EQUAL(42, get(style.getAnswer()));
  }

  // with context
  {
    KNStyleContext context;

    KNPropertyMap ctxtProps;
    ctxtProps.set("answer", 2);

    context.push(makeStyle(ctxtProps));

    // lookup through context only
    {
      const Style style = Style(KNPropertyMap());
      CPPUNIT_ASSERT(style.getAnswer(context));
      CPPUNIT_ASSERT_EQUAL(2, get(style.getAnswer(context)));
    }

    KNPropertyMap props;
    props.set("answer", 42);

    // lookup in own prop. set
    {
      const Style style(props);
      CPPUNIT_ASSERT(style.getAnswer(context));
      CPPUNIT_ASSERT_EQUAL(42, get(style.getAnswer(context)));
    }
  }
}

CPPUNIT_TEST_SUITE_REGISTRATION(KNStylesTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
