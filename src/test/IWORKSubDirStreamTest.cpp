/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <libetonyek/libetonyek.h>

#include <librevenge-stream/librevenge-stream.h>

#include "IWORKSubDirStream.h"
#include "libetonyek_utils.h"

#if !defined ETONYEK_STREAMS_TEST_DIR
#error ETONYEK_STREAMS_TEST_DIR not defined, cannot test
#endif

namespace test
{

using libetonyek::getLength;
using libetonyek::IWORKSubDirStream;
using libetonyek::RVNGInputStreamPtr_t;

class IWASubDirStreamTest : public CPPUNIT_NS::TestFixture
{
public:
  IWASubDirStreamTest();

  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(IWASubDirStreamTest);
  CPPUNIT_TEST(testSimple);
  CPPUNIT_TEST(testDelimited);
  CPPUNIT_TEST(testPassThrough);
  CPPUNIT_TEST_SUITE_END();

private:
  void testSimple();
  void testDelimited();
  void testPassThrough();

private:
  RVNGInputStreamPtr_t m_stream;
};

IWASubDirStreamTest::IWASubDirStreamTest()
  : m_stream()
{
}

void IWASubDirStreamTest::setUp()
{
  m_stream.reset(new librevenge::RVNGFileStream(ETONYEK_STREAMS_TEST_DIR  "/unsupported.zip"));
}

void IWASubDirStreamTest::tearDown()
{
  m_stream.reset();
}

void IWASubDirStreamTest::testSimple()
{
  RVNGInputStreamPtr_t stream{new IWORKSubDirStream(m_stream, "Index")};

  CPPUNIT_ASSERT(stream->isStructured());

  CPPUNIT_ASSERT(stream->existsSubStream("ViewState.iwa"));

  RVNGInputStreamPtr_t subStream(stream->getSubStreamByName("ViewState.iwa"));
  CPPUNIT_ASSERT(bool(subStream));
  RVNGInputStreamPtr_t origSubStream(m_stream->getSubStreamByName("Index/ViewState.iwa"));
  CPPUNIT_ASSERT(bool(origSubStream));
  CPPUNIT_ASSERT_EQUAL(getLength(origSubStream), getLength(subStream));
}

void IWASubDirStreamTest::testDelimited()
{
  RVNGInputStreamPtr_t stream{new IWORKSubDirStream(m_stream, "Index/")};

  CPPUNIT_ASSERT(stream->isStructured());

  CPPUNIT_ASSERT(stream->existsSubStream("ViewState.iwa"));

  RVNGInputStreamPtr_t subStream(stream->getSubStreamByName("ViewState.iwa"));
  CPPUNIT_ASSERT(bool(subStream));
  RVNGInputStreamPtr_t origSubStream(m_stream->getSubStreamByName("Index/ViewState.iwa"));
  CPPUNIT_ASSERT(bool(origSubStream));
  CPPUNIT_ASSERT_EQUAL(getLength(origSubStream), getLength(subStream));
}

void IWASubDirStreamTest::testPassThrough()
{
  RVNGInputStreamPtr_t stream{new IWORKSubDirStream(m_stream, "")};

  CPPUNIT_ASSERT(stream->isStructured());

  CPPUNIT_ASSERT(stream->existsSubStream("Index/ViewState.iwa"));

  RVNGInputStreamPtr_t subStream(stream->getSubStreamByName("Index/ViewState.iwa"));
  CPPUNIT_ASSERT(bool(subStream));
  RVNGInputStreamPtr_t origSubStream(m_stream->getSubStreamByName("Index/ViewState.iwa"));
  CPPUNIT_ASSERT(bool(origSubStream));
  CPPUNIT_ASSERT_EQUAL(getLength(origSubStream), getLength(subStream));
}

CPPUNIT_TEST_SUITE_REGISTRATION(IWASubDirStreamTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
