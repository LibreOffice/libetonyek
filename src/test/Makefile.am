tests = test detection

check_PROGRAMS = $(tests)
check_LIBRARIES = libtest_driver.a

libtest_driver_a_CPPFLAGS = $(CPPUNIT_CFLAGS) $(DEBUG_CXXFLAGS)
libtest_driver_a_SOURCES = \
	test.cpp

test_CPPFLAGS = \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/src/lib \
	$(REVENGE_CFLAGS) \
	$(CPPUNIT_CFLAGS) \
	$(XML_CFLAGS) \
	$(GLM_CFLAGS) \
	$(DEBUG_CXXFLAGS)

test_LDFLAGS = -L$(top_builddir)/src/lib
test_LDADD = \
	libtest_driver.a \
	$(top_builddir)/src/lib/libetonyek_internal.la \
	$(REVENGE_LIBS) \
	$(CPPUNIT_LIBS) \
	$(XML_LIBS)

test_SOURCES = \
	IWASnappyStreamTest.cpp \
	IWORKChainedTokenizerTest.cpp \
	IWORKFormulaTest.cpp \
	IWORKPathTest.cpp \
	IWORKPropertyMapTest.cpp \
	IWORKShapeTest.cpp \
	IWORKStyleTest.cpp \
	IWORKStyleStackTest.cpp \
	IWORKTokenizerBaseTest.cpp \
	IWORKTransformationTest.cpp \
	LibetonyekUtilsTest.cpp \
	TestProperties.cpp \
	TestProperties.h

detection_CPPFLAGS = \
	-DETONYEK_DETECTION_TEST_DIR=\"$(top_srcdir)/src/test/data\" \
	-I$(top_srcdir)/inc \
	$(REVENGE_STREAM_CFLAGS) \
	$(CPPUNIT_CFLAGS) \
	$(XML_CFLAGS) \
	$(DEBUG_CXXFLAGS)

detection_LDFLAGS = -L$(top_builddir)/src/lib
detection_LDADD = \
	libtest_driver.a \
	$(top_builddir)/src/lib/libetonyek-@ETONYEK_MAJOR_VERSION@.@ETONYEK_MINOR_VERSION@.la \
	$(REVENGE_STREAM_LIBS) \
	$(CPPUNIT_LIBS) \
	$(XML_LIBS)

detection_SOURCES = \
	EtonyekDocumentTest.cpp

TESTS = $(tests)

EXTRA_DIST = \
	data/keynote4.apxl \
	data/keynote4.apxl.gz \
	data/keynote4-package.key \
	data/keynote5-file.key \
	data/keynote6-file.key \
	data/keynote6-package.key \
	data/keynote6.zip \
	data/numbers2-file.numbers \
	data/numbers2-package.numbers \
	data/numbers2.xml \
	data/numbers2.xml.gz \
	data/numbers3-file.numbers \
	data/numbers3-package.numbers \
	data/numbers3.zip \
	data/pages4-file.pages \
	data/pages4-package.pages \
	data/pages4.xml \
	data/pages4.xml.gz \
	data/pages5-file.pages \
	data/pages5-package.pages \
	data/pages5.zip \
	data/unsupported \
	data/unsupported1.pages \
	data/unsupported2.package \
	data/unsupported.xml \
	data/unsupported.zip
