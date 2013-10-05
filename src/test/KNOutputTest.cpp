/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KNOutput.h"
#include "KNStyleContext.h"
#include "KNStyles.h"

#include "KNOutputTest.h"

namespace test
{

using boost::any_cast;
using boost::optional;

using libkeynote::KNOutput;
using libkeynote::KNParagraphStyle;
using libkeynote::KNParagraphStylePtr_t;
using libkeynote::KNPropertyMap;
using libkeynote::KNStyleContext;
using libkeynote::KNTransformation;

using std::string;

namespace
{

KNParagraphStylePtr_t makeStyle(const KNPropertyMap &props)
{
  const optional<string> dummyIdent;
  const KNParagraphStylePtr_t style(new KNParagraphStyle(props, dummyIdent, dummyIdent));

  return style;
}

}

void KNOutputTest::setUp()
{
}

void KNOutputTest::tearDown()
{
}

void KNOutputTest::testTransformation()
{
  KNStyleContext context;
  const KNOutput output(0, context);

  // default transformation
  CPPUNIT_ASSERT(KNTransformation() == output.getTransformation());

  using namespace libkeynote::transformations;

  // translation
  {
    const KNTransformation test = translate(3, 4);
    const KNOutput output2(output, test);
    CPPUNIT_ASSERT(output.getTransformation() != output2.getTransformation());
    CPPUNIT_ASSERT(test == output2.getTransformation());

    // nested transformations are applied in the right order: the
    // innermost one first
    {
      const KNTransformation test2 = scale(2, 1);
      KNOutput output3(output2, test2);
      CPPUNIT_ASSERT(test2 * test == output3.getTransformation());
    }

    CPPUNIT_ASSERT(test == output2.getTransformation());
  }

  CPPUNIT_ASSERT(KNTransformation() == output.getTransformation());
}

void KNOutputTest::testStyle()
{
  KNStyleContext context;
  const KNOutput output(0, context);

  // empty context
  CPPUNIT_ASSERT(output.getStyleContext().find("answer", true).empty());

  // push a style
  {
    KNPropertyMap props;
    props.set("answer", 42);

    const KNOutput output2(output, makeStyle(props));
    CPPUNIT_ASSERT(!output2.getStyleContext().find("answer", true).empty());
    CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(output2.getStyleContext().find("answer", true)));

    // nested styles are applied correctly
    {
      KNPropertyMap props2;
      props2.set("answer", 2);
      const KNOutput output3(output2, makeStyle(props2));
      CPPUNIT_ASSERT(!output3.getStyleContext().find("answer", true).empty());
      CPPUNIT_ASSERT_EQUAL(2, any_cast<int>(output3.getStyleContext().find("answer", true)));
    }

    CPPUNIT_ASSERT_EQUAL(42, any_cast<int>(output2.getStyleContext().find("answer", true)));
  }

  CPPUNIT_ASSERT(output.getStyleContext().find("answer", true).empty());
}

CPPUNIT_TEST_SUITE_REGISTRATION(KNOutputTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
