target_test = test

check_PROGRAMS = $(target_test)

AM_CXXFLAGS = -I$(top_srcdir)/inc -I$(top_srcdir)/src/lib $(WPD_CFLAGS) $(CPPUNIT_CFLAGS) $(XML_CFLAGS) $(DEBUG_CXXFLAGS)

test_LDFLAGS = -L$(top_builddir)/src/lib
test_LDADD = \
	$(top_builddir)/src/lib/libkeynote-@KEYNOTE_MAJOR_VERSION@.@KEYNOTE_MINOR_VERSION@.la \
	$(WPD_LIBS) \
	$(CPPUNIT_LIBS) \
	$(XML_LIBS)

test_SOURCES = \
	KN2TokenTest.cpp \
	KN2TokenTest.h \
	KNPathTest.cpp \
	KNPathTest.h \
	KNPropertyMapTest.cpp \
	KNPropertyMapTest.h \
	KNShapeTest.cpp \
	KNShapeTest.h \
	KNStyleContextTest.cpp \
	KNStyleContextTest.h \
	KNTransformationTest.cpp \
	KNTransformationTest.h \
	KNXMLReaderTest.cpp \
	KNXMLReaderTest.h \
	test.cpp

TESTS = $(target_test)
