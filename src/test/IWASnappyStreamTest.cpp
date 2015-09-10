/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>
#include <string>

#include <boost/make_shared.hpp>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "IWASnappyStream.h"
#include "IWORKMemoryStream.h"

using namespace libetonyek;

using std::string;

namespace test
{

namespace
{

void assertCompressed(const string &message, const unsigned char *const expected, const size_t expectedSize, const unsigned char *const compressed, const size_t compressedSize)
{
  const RVNGInputStreamPtr_t stream(new IWORKMemoryStream(compressed, compressedSize));
  IWASnappyStream uncompressedStream(stream);
  unsigned long uncompressedSize = 0;
  const unsigned char *const uncompressed = uncompressedStream.read(expectedSize, uncompressedSize);
  CPPUNIT_ASSERT_EQUAL_MESSAGE(message + ": size", expectedSize, uncompressedSize);
  CPPUNIT_ASSERT_MESSAGE(message + ": input exhausted", uncompressedStream.isEnd());
  CPPUNIT_ASSERT_MESSAGE(message + ": content", std::equal(expected, expected + expectedSize, uncompressed));
}

void assertAnyException(const string &message, const unsigned char *const compressed, const size_t compressedSize)
{
  const RVNGInputStreamPtr_t stream(new IWORKMemoryStream(compressed, compressedSize));
  bool exception = false;
  try
  {
    IWASnappyStream uncompressedStream(stream);
  }
  catch (...)
  {
    exception = true;
  }
  CPPUNIT_ASSERT_MESSAGE(message, exception);
}

}

class IWASnappyStreamTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(IWASnappyStreamTest);
  CPPUNIT_TEST(testSimple);
  CPPUNIT_TEST(testInvalid);
  CPPUNIT_TEST_SUITE_END();

private:
  void testSimple();
  void testInvalid();
};

void IWASnappyStreamTest::setUp()
{
}

void IWASnappyStreamTest::tearDown()
{
}

#define BYTES(b) (reinterpret_cast<const unsigned char *>(b)), (sizeof(b) - 1)

void IWASnappyStreamTest::testSimple()
{
  // assertCompressed("empty", BYTES(""), BYTES("\x1\x0"));
  assertCompressed("literal run", BYTES("a"), BYTES("\x3\x1\x0\x61"));
  assertCompressed("literal run", BYTES("ab"), BYTES("\x4\x2\x4\x61\x62"));
  assertCompressed("long literal run", BYTES("ab"), BYTES("\x5\x2\xf0\x1\x61\x62"));
  assertCompressed("very long literal run", BYTES("ab"), BYTES("\x5\x2\xf4\x1\x0\x61\x62"));
  assertCompressed("extra long literal run", BYTES("ab"), BYTES("\x5\x2\xf8\x1\x0\x0\x61\x62"));
  assertCompressed("near reference",
                   BYTES("abcdabcd"), BYTES("\x8\x8\xc\x61\x62\x63\x64\x1\x4"));
  assertCompressed("near reference of length 5",
                   BYTES("abcdeabcde"), BYTES("\x9\xa\x10\x61\x62\x63\x64\x65\x5\x5"));
  assertCompressed("near reference to the middle of a run",
                   BYTES("abcdefbcde"), BYTES("\xa\xa\x14\x61\x62\x63\x64\x65\x66\x1\x5"));
  assertCompressed("repeated near reference",
                   BYTES("abcbcbcb"), BYTES("\x7\x8\x8\x61\x62\x63\x5\x2"));
  assertCompressed("far reference", BYTES("aa"), BYTES("\x6\x2\x0\x61\x2\x1\x0"));
  assertCompressed("far reference of length 2",
                   BYTES("abab"), BYTES("\x7\x4\x4\x61\x62\x6\x2\x0"));
}

void IWASnappyStreamTest::testInvalid()
{
  assertAnyException("Too short literal run", BYTES("\x3\x4\x10\x61"));
  assertAnyException("Near reference without any data", BYTES("\x3\x1\x5\x0"));
  assertAnyException("Far reference without any data", BYTES("\x4\x1\x2\x1\x0"));
}

#undef BYTES

CPPUNIT_TEST_SUITE_REGISTRATION(IWASnappyStreamTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
