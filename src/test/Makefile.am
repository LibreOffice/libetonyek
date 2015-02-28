target_test = test

check_PROGRAMS = $(target_test)

AM_CXXFLAGS = -I$(top_srcdir)/inc -I$(top_srcdir)/src/lib $(REVENGE_CFLAGS) $(CPPUNIT_CFLAGS) $(XML_CFLAGS) $(DEBUG_CXXFLAGS)

test_LDFLAGS = -L$(top_builddir)/src/lib
test_LDADD = \
	$(top_builddir)/src/lib/libetonyek_internal.la \
	$(REVENGE_LIBS) \
	$(CPPUNIT_LIBS) \
	$(XML_LIBS)

test_SOURCES = \
	IWORKPathTest.cpp \
	IWORKPathTest.h \
	IWORKPropertyMapTest.cpp \
	IWORKPropertyMapTest.h \
	IWORKToken.h \
	IWORKTokenInfo.h \
	IWORKTransformationTest.cpp \
	IWORKTransformationTest.h \
	IWORKXMLReaderTest.cpp \
	IWORKXMLReaderTest.h \
	KEY2TokenTest.cpp \
	KEY2TokenTest.h \
	KEYShapeTest.cpp \
	KEYShapeTest.h \
	KEYStyleContextTest.cpp \
	KEYStyleContextTest.h \
	KEYStylesTest.cpp \
	KEYStylesTest.h \
	test.cpp

TESTS = $(target_test)
