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

#if !defined ETONYEK_DETECTION_TEST_DIR
#error ETONYEK_DETECTION_TEST_DIR not defined, cannot test
#endif

namespace test
{

using libetonyek::EtonyekDocument;

using std::string;

namespace
{

template<class Stream>
void assertDetection(const string &name, const EtonyekDocument::Confidence expectedConfidence, const EtonyekDocument::Type *const expectedType = 0)
{
  Stream input((string(ETONYEK_DETECTION_TEST_DIR) + "/" + name).c_str());
  EtonyekDocument::Type type = EtonyekDocument::TYPE_UNKNOWN;
  const EtonyekDocument::Confidence confidence = EtonyekDocument::isSupported(&input, &type);
  CPPUNIT_ASSERT_EQUAL_MESSAGE(name + ": confidence", expectedConfidence, confidence);
  if (expectedType)
    CPPUNIT_ASSERT_EQUAL_MESSAGE(name + ": type", *expectedType, type);
}

void assertSupportedFile(const string &name, const EtonyekDocument::Confidence confidence, const EtonyekDocument::Type type)
{
  assertDetection<librevenge::RVNGFileStream>(name, confidence, &type);
}

void assertSupportedPackage(const string &name, const EtonyekDocument::Confidence confidence, const EtonyekDocument::Type type)
{
  assertDetection<librevenge::RVNGDirectoryStream>(name, confidence, &type);
}

void assertUnsupportedFile(const string &name)
{
  assertDetection<librevenge::RVNGFileStream>(name, EtonyekDocument::CONFIDENCE_NONE);
}

void assertUnsupportedPackage(const string &name)
{
  assertDetection<librevenge::RVNGDirectoryStream>(name, EtonyekDocument::CONFIDENCE_NONE);
}

static const EtonyekDocument::Confidence EXCELLENT = EtonyekDocument::CONFIDENCE_EXCELLENT;
static const EtonyekDocument::Confidence SUPPORTED_PART = EtonyekDocument::CONFIDENCE_SUPPORTED_PART;

}

class EtonyekDocumentTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(EtonyekDocumentTest);
  CPPUNIT_TEST(testDetectKeynote);
  CPPUNIT_TEST(testDetectNumbers);
  CPPUNIT_TEST(testDetectPages);
  CPPUNIT_TEST(testUnsupported);
  CPPUNIT_TEST_SUITE_END();

private:
  void testDetectKeynote();
  void testDetectNumbers();
  void testDetectPages();
  void testUnsupported();
};

void EtonyekDocumentTest::setUp()
{
}

void EtonyekDocumentTest::tearDown()
{
}

void EtonyekDocumentTest::testDetectKeynote()
{
  const EtonyekDocument::Type type(EtonyekDocument::TYPE_KEYNOTE);

  // version 2-5
  assertSupportedPackage("keynote4-package.key", EXCELLENT, type);
  assertSupportedFile("keynote4.apxl.gz", SUPPORTED_PART, type);
  assertSupportedFile("keynote4.apxl", SUPPORTED_PART, type);
  assertSupportedFile("keynote5-file.key", EXCELLENT, type);

  // version 6
  assertSupportedPackage("keynote6-package.key", EXCELLENT, type);
  assertSupportedFile("keynote6.zip", SUPPORTED_PART, type);
  assertSupportedFile("keynote6-file.key", EXCELLENT, type);
}

void EtonyekDocumentTest::testDetectNumbers()
{
  const EtonyekDocument::Type type(EtonyekDocument::TYPE_NUMBERS);

  // version 1-2
  assertSupportedPackage("numbers2-package.numbers", EXCELLENT, type);
  assertSupportedFile("numbers2.xml", SUPPORTED_PART, type);
  assertSupportedFile("numbers2.xml.gz", SUPPORTED_PART, type);
  assertSupportedFile("numbers2-file.numbers", EXCELLENT, type);

  // version 3
  assertSupportedPackage("numbers3-package.numbers", EXCELLENT, type);
  assertSupportedFile("numbers3.zip", SUPPORTED_PART, type);
  assertSupportedFile("numbers3-file.numbers", EXCELLENT, type);
}

void EtonyekDocumentTest::testDetectPages()
{
  const EtonyekDocument::Type type(EtonyekDocument::TYPE_PAGES);

  // version 1-4
  assertSupportedPackage("pages4-package.pages", EXCELLENT, type);
  assertSupportedFile("pages4.xml", SUPPORTED_PART, type);
  assertSupportedFile("pages4.xml.gz", SUPPORTED_PART, type);
  assertSupportedFile("pages4-file.pages", EXCELLENT, type);

  // version 5
  assertSupportedPackage("pages5-package.pages", EXCELLENT, type);
  assertSupportedFile("pages5.zip", SUPPORTED_PART, type);
  assertSupportedFile("pages5-file.pages", EXCELLENT, type);
  assertSupportedFile("pages5-extra-dir.pages", EXCELLENT, type);
}

void EtonyekDocumentTest::testUnsupported()
{
  // ensure the detection is not too broad
  assertUnsupportedPackage("unsupported1.package");
  assertUnsupportedPackage("unsupported2.package");
  assertUnsupportedFile("unsupported.xml");
  assertUnsupportedFile("unsupported.zip");
}

CPPUNIT_TEST_SUITE_REGISTRATION(EtonyekDocumentTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
