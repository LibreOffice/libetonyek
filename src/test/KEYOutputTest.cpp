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

using libetonyek::IWORKPropertyMap;
using libetonyek::IWORKTransformation;
using libetonyek::KEYOutput;
using libetonyek::KEYParagraphStyle;
using libetonyek::KEYParagraphStylePtr_t;
using libetonyek::KEYStyleContext;

using std::string;

namespace
{

KEYParagraphStylePtr_t makeStyle(const IWORKPropertyMap &props)
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

void KEYOutputTest::testStyle()
{
  KEYStyleContext context;
  const KEYOutput output(0, context);

  // empty context
  CPPUNIT_ASSERT(output.getStyleContext().find("answer", true).empty());

  // push a style
  {
    IWORKPropertyMap props;
    props.set("answer", 42);

    const KEYOutput output2(output, makeStyle(props));
    CPPUNIT_ASSERT(!output2.getStyleContext().find("answer", true).empty());
    CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(output2.getStyleContext().find("answer", true)));

    // nested styles are applied correctly
    {
      IWORKPropertyMap props2;
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
