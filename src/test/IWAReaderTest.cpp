/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <memory>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "IWAReader.h"
#include "IWORKMemoryStream.h"
#include "libetonyek_utils.h"

using namespace libetonyek;

using std::string;

namespace test
{

namespace
{

RVNGInputStreamPtr_t makeStream(const unsigned char *const bytes, const unsigned long length)
{
  return std::make_shared<IWORKMemoryStream>(bytes, length);
}

}

class IWAReaderTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(IWAReaderTest);
  CPPUNIT_TEST(testString);
  CPPUNIT_TEST(testBytes);
  CPPUNIT_TEST_SUITE_END();

private:
  void testString();
  void testBytes();
};

void IWAReaderTest::setUp()
{
}

void IWAReaderTest::tearDown()
{
}

#define BYTES(b) (reinterpret_cast<const unsigned char *>(b)), (sizeof(b) - 1)

void IWAReaderTest::testString()
{
  using namespace IWAReader;

  CPPUNIT_ASSERT_EQUAL(string("hello"), String::read(makeStream(BYTES("hello")), 5));
  CPPUNIT_ASSERT_EQUAL(string("hello"), String::read(makeStream(BYTES("hello world")), 5));
}

void IWAReaderTest::testBytes()
{
  const RVNGInputStreamPtr_t input(IWAReader::Bytes::read(makeStream(BYTES("\x78\x56\x34\x12")), 4));
  CPPUNIT_ASSERT(!input->isEnd());
  CPPUNIT_ASSERT_EQUAL(4ul, getLength(input));
  CPPUNIT_ASSERT_EQUAL(0x12345678u, readU32(input));
}

#undef BYTES

CPPUNIT_TEST_SUITE_REGISTRATION(IWAReaderTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
