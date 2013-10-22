/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYOutput.h"
#include "KEYStyleContext.h"
#include "KEYStyles.h"

#include "KEYOutputTest.h"

namespace test
{

using boost::any_cast;
using boost::optional;

using libetonyek::KEYOutput;
using libetonyek::KEYParagraphStyle;
using libetonyek::KEYParagraphStylePtr_t;
using libetonyek::KEYPropertyMap;
using libetonyek::KEYStyleContext;
using libetonyek::KEYTransformation;

using std::string;

namespace
{

KEYParagraphStylePtr_t makeStyle(const KEYPropertyMap &props)
{
  const optional<string> dummyIdent;
  const KEYParagraphStylePtr_t style(new KEYParagraphStyle(props, dummyIdent, dummyIdent));

  return style;
}

}

void KEYOutputTest::setUp()
{
}

void KEYOutputTest::tearDown()
{
}

void KEYOutputTest::testTransformation()
{
  KEYStyleContext context;
  const KEYOutput output(0, context);

  // default transformation
  CPPUNIT_ASSERT(KEYTransformation() == output.getTransformation());

  using namespace libetonyek::transformations;

  // translation
  {
    const KEYTransformation test = translate(3, 4);
    const KEYOutput output2(output, test);
    CPPUNIT_ASSERT(output.getTransformation() != output2.getTransformation());
    CPPUNIT_ASSERT(test == output2.getTransformation());

    // nested transformations are applied in the right order: the
    // innermost one first
    {
      const KEYTransformation test2 = scale(2, 1);
      KEYOutput output3(output2, test2);
      CPPUNIT_ASSERT(test2 * test == output3.getTransformation());
    }

    CPPUNIT_ASSERT(test == output2.getTransformation());
  }

  CPPUNIT_ASSERT(KEYTransformation() == output.getTransformation());
}

void KEYOutputTest::testStyle()
{
  KEYStyleContext context;
  const KEYOutput output(0, context);

  // empty context
  CPPUNIT_ASSERT(output.getStyleContext().find("answer", true).empty());

  // push a style
  {
    KEYPropertyMap props;
    props.set("answer", 42);

    const KEYOutput output2(output, makeStyle(props));
    CPPUNIT_ASSERT(!output2.getStyleContext().find("answer", true).empty());
    CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(output2.getStyleContext().find("answer", true)));

    // nested styles are applied correctly
    {
      KEYPropertyMap props2;
      props2.set("answer", 2);
      const KEYOutput output3(output2, makeStyle(props2));
      CPPUNIT_ASSERT(!output3.getStyleContext().find("answer", true).empty());
      CPPUNIT_ASSERT_EQUAL(2, any_cast<int>(output3.getStyleContext().find("answer", true)));
    }

    CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(output2.getStyleContext().find("answer", true)));
  }

  CPPUNIT_ASSERT(output.getStyleContext().find("answer", true).empty());
}

CPPUNIT_TEST_SUITE_REGISTRATION(KEYOutputTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
