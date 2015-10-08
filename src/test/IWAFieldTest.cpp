/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>
#include <stdexcept>

#include <boost/make_shared.hpp>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "IWAField.h"
#include "IWORKMemoryStream.h"

using namespace libetonyek;

namespace test
{

namespace
{

RVNGInputStreamPtr_t makeStream(const unsigned char *const bytes, const unsigned long length)
{
  return boost::make_shared<IWORKMemoryStream>(bytes, length);
}

}

class IWAFieldTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(IWAFieldTest);
  CPPUNIT_TEST(testEmpty);
  CPPUNIT_TEST(testParse);
  CPPUNIT_TEST(testParsePacked);
  CPPUNIT_TEST(testOptional);
  CPPUNIT_TEST(testRepeated);
  CPPUNIT_TEST_SUITE_END();

private:
  void testEmpty();
  void testParse();
  void testParsePacked();
  void testOptional();
  void testRepeated();
};

void IWAFieldTest::setUp()
{
}

void IWAFieldTest::tearDown()
{
}

void IWAFieldTest::testEmpty()
{
  IWAUInt64Field field;

  CPPUNIT_ASSERT_EQUAL(IWAField::TAG_UINT64, field.tag());

  // repeated
  CPPUNIT_ASSERT(field.empty());
  CPPUNIT_ASSERT_EQUAL(size_t(0), field.size());
  // CPPUNIT_ASSERT_THROW(field[0], std::out_of_range);

  // optional
  CPPUNIT_ASSERT(!field.is());
  CPPUNIT_ASSERT(!bool(field));
  // CPPUNIT_ASSERT_THROW(field.get(), std::logic_error);
}

#define BYTES(b) (reinterpret_cast<const unsigned char *>(b)), (sizeof(b) - 1)

void IWAFieldTest::testParse()
{
  IWAUInt64Field field;
  CPPUNIT_ASSERT_NO_THROW(field.parse(makeStream(BYTES("\x1")), 1));

  // repeated
  CPPUNIT_ASSERT(!field.empty());
  CPPUNIT_ASSERT_EQUAL(size_t(1), field.size());
  CPPUNIT_ASSERT_EQUAL(uint64_t(1), field[0]);

  // optional
  CPPUNIT_ASSERT(field.is());
  CPPUNIT_ASSERT(bool(field));
  CPPUNIT_ASSERT_EQUAL(uint64_t(1), field.get());

  // parse another value
  CPPUNIT_ASSERT_NO_THROW(field.parse(makeStream(BYTES("\xac\x2")), 1));

  CPPUNIT_ASSERT_EQUAL(size_t(2), field.size());
  CPPUNIT_ASSERT_EQUAL(uint64_t(1), field[0]);
  CPPUNIT_ASSERT_EQUAL(uint64_t(300), field[1]);

  CPPUNIT_ASSERT_EQUAL(uint64_t(1), field.get());
}

void IWAFieldTest::testParsePacked()
{
  IWAUInt64Field field;
  CPPUNIT_ASSERT_NO_THROW(field.parse(makeStream(BYTES("\x1\x4\x8\x10")), 3));

  // repeated
  CPPUNIT_ASSERT_EQUAL(size_t(3), field.size());
  CPPUNIT_ASSERT_EQUAL(uint64_t(1), field[0]);
  CPPUNIT_ASSERT_EQUAL(uint64_t(4), field[1]);
  CPPUNIT_ASSERT_EQUAL(uint64_t(8), field[2]);

  // optional
  CPPUNIT_ASSERT(field.is());
  CPPUNIT_ASSERT_EQUAL(uint64_t(1), field.get());
}

void IWAFieldTest::testOptional()
{
  {
    IWAUInt64Field field;
    CPPUNIT_ASSERT_NO_THROW(field.parse(makeStream(BYTES("\x1\x4")), 2));
    const boost::optional<uint64_t> &value = field;
    CPPUNIT_ASSERT(value);
    CPPUNIT_ASSERT_EQUAL(uint64_t(1), get(value));
  }

  {
    IWASInt32Field field;
    CPPUNIT_ASSERT_NO_THROW(field.parse(makeStream(BYTES("\x3")), 1));
    CPPUNIT_ASSERT(field);
    CPPUNIT_ASSERT_EQUAL(int32_t(-2), field.get());
    const boost::optional<int64_t> value = field;
    CPPUNIT_ASSERT(value);
    CPPUNIT_ASSERT_EQUAL(int64_t(-2), get(value));
  }
}

void IWAFieldTest::testRepeated()
{
  IWAUInt64Field field;
  CPPUNIT_ASSERT_NO_THROW(field.parse(makeStream(BYTES("\x1\x4\x8")), 3));
  const uint64_t expected[] = {1, 4, 8};
  const std::deque<uint64_t> &values = field;
  CPPUNIT_ASSERT_EQUAL(ETONYEK_NUM_ELEMENTS(expected), values.size());
  CPPUNIT_ASSERT(std::equal(values.begin(), values.end(), expected));
  CPPUNIT_ASSERT(std::equal(field.begin(), field.end(), expected));
}

#undef BYTES

CPPUNIT_TEST_SUITE_REGISTRATION(IWAFieldTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
