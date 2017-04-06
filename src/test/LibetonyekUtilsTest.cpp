/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <limits>
#include <memory>
#include <stdexcept>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "IWORKMemoryStream.h"
#include "libetonyek_utils.h"

using libetonyek::EndOfStreamException;
using libetonyek::IWORKMemoryStream;
using libetonyek::RVNGInputStreamPtr_t;
using libetonyek::readSVar;
using libetonyek::readUVar;

using std::numeric_limits;

namespace test
{

namespace
{

RVNGInputStreamPtr_t makeStream(const char *const bytes, const size_t len)
{
  return std::make_shared<IWORKMemoryStream>(reinterpret_cast<const unsigned char *>(bytes), len);
}

RVNGInputStreamPtr_t makeEmptyStream()
{
  const RVNGInputStreamPtr_t stream = makeStream("\0", 1);
  stream->seek(1, librevenge::RVNG_SEEK_SET);
  return stream;
}

}

class LibetonyekUtilsTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(LibetonyekUtilsTest);
  CPPUNIT_TEST(testReadSVar);
  CPPUNIT_TEST(testReadUVar);
  CPPUNIT_TEST_SUITE_END();

private:
  void testReadSVar();
  void testReadUVar();
};

void LibetonyekUtilsTest::setUp()
{
}

void LibetonyekUtilsTest::tearDown()
{
}

void LibetonyekUtilsTest::testReadSVar()
{
  CPPUNIT_ASSERT_EQUAL(int64_t(0), readSVar(makeStream("\x0", 1)));
  CPPUNIT_ASSERT_EQUAL(int64_t(-1), readSVar(makeStream("\x1", 1)));
  CPPUNIT_ASSERT_EQUAL(int64_t(1), readSVar(makeStream("\x2", 1)));
  CPPUNIT_ASSERT_EQUAL(int64_t(-2), readSVar(makeStream("\x3", 1)));
  CPPUNIT_ASSERT_EQUAL(int64_t(0x7fffffffL), readSVar(makeStream("\xfe\xff\xff\xff\xf", 5)));
  CPPUNIT_ASSERT_EQUAL(int64_t(numeric_limits<int32_t>::min()), readSVar(makeStream("\xff\xff\xff\xff\xf", 5)));
  CPPUNIT_ASSERT_EQUAL(numeric_limits<int64_t>::max(), readSVar(makeStream("\xfe\xff\xff\xff\xff\xff\xff\xff\xff\x1", 10)));
  CPPUNIT_ASSERT_EQUAL(numeric_limits<int64_t>::min(), readSVar(makeStream("\xff\xff\xff\xff\xff\xff\xff\xff\xff\x1", 10)));
  CPPUNIT_ASSERT_THROW(readSVar(makeStream("\x80\x80\x80\x80\x80\x80\x80\x80\x80\x2", 10)), std::range_error);
  CPPUNIT_ASSERT_THROW(readSVar(makeStream("\x81\x80\x80\x80\x80\x80\x80\x80\x80\x2", 10)), std::range_error);
  CPPUNIT_ASSERT_THROW(readSVar(makeEmptyStream()), EndOfStreamException);
  CPPUNIT_ASSERT_THROW(readSVar(makeStream("\x80", 1)), EndOfStreamException);
  CPPUNIT_ASSERT_THROW(readSVar(makeStream("\xff\xff", 2)), EndOfStreamException);
}

void LibetonyekUtilsTest::testReadUVar()
{
  CPPUNIT_ASSERT_EQUAL(uint64_t(0), readUVar(makeStream("\x0", 1)));
  CPPUNIT_ASSERT_EQUAL(uint64_t(1), readUVar(makeStream("\x1", 1)));
  CPPUNIT_ASSERT_EQUAL(uint64_t(0x7f), readUVar(makeStream("\x7f", 1)));
  CPPUNIT_ASSERT_EQUAL(uint64_t(0x80), readUVar(makeStream("\x80\x1", 2)));
  CPPUNIT_ASSERT_EQUAL(uint64_t(0x81), readUVar(makeStream("\x81\x1", 2)));
  CPPUNIT_ASSERT_EQUAL(uint64_t(0x12345678UL), readUVar(makeStream("\xf8\xac\xd1\x91\x01", 5)));
  CPPUNIT_ASSERT_EQUAL(numeric_limits<uint64_t>::max(), readUVar(makeStream("\xff\xff\xff\xff\xff\xff\xff\xff\xff\x1", 10)));
  CPPUNIT_ASSERT_THROW(readUVar(makeStream("\x80\x80\x80\x80\x80\x80\x80\x80\x80\x2", 10)), std::range_error);
  CPPUNIT_ASSERT_THROW(readUVar(makeEmptyStream()), EndOfStreamException);
  CPPUNIT_ASSERT_THROW(readUVar(makeStream("\x80", 1)), EndOfStreamException);
  CPPUNIT_ASSERT_THROW(readUVar(makeStream("\xff\xff", 2)), EndOfStreamException);
}

CPPUNIT_TEST_SUITE_REGISTRATION(LibetonyekUtilsTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
