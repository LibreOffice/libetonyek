target_test = test

check_PROGRAMS = $(target_test)

AM_CXXFLAGS = \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/src/lib \
	$(REVENGE_CFLAGS) \
	$(CPPUNIT_CFLAGS) \
	$(XML_CFLAGS) \
	$(GLM_CFLAGS) \
	$(DEBUG_CXXFLAGS)

test_LDFLAGS = -L$(top_builddir)/src/lib
test_LDADD = \
	$(top_builddir)/src/lib/libetonyek_internal.la \
	$(REVENGE_LIBS) \
	$(CPPUNIT_LIBS) \
	$(XML_LIBS)

test_SOURCES = \
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
	TestProperties.h \
	test.cpp

TESTS = $(target_test)
