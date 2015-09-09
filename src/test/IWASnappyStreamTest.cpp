/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/make_shared.hpp>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "IWASnappyStream.h"
#include "IWORKMemoryStream.h"

using namespace libetonyek;

namespace test
{

namespace
{

#if 0
RVNGInputStreamPtr_t makeStream(const char *const bytes, const size_t len)
{
  return boost::make_shared<IWORKMemoryStream>(reinterpret_cast<const unsigned char *>(bytes), len);
}
#endif

}

class IWASnappyStreamTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(IWASnappyStreamTest);
  CPPUNIT_TEST_SUITE_END();

private:
};

void IWASnappyStreamTest::setUp()
{
}

void IWASnappyStreamTest::tearDown()
{
}

CPPUNIT_TEST_SUITE_REGISTRATION(IWASnappyStreamTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
