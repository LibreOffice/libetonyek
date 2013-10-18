target_test = test

check_PROGRAMS = $(target_test)

AM_CXXFLAGS = -I$(top_srcdir)/inc -I$(top_srcdir)/src/lib $(WPD_CFLAGS) $(WPG_CFLAGS) $(CPPUNIT_CFLAGS) $(XML_CFLAGS) $(DEBUG_CXXFLAGS)

test_LDFLAGS = -L$(top_builddir)/src/lib
test_LDADD = \
	$(top_builddir)/src/lib/libkeynote-@KEYNOTE_MAJOR_VERSION@.@KEYNOTE_MINOR_VERSION@.la \
	$(WPD_LIBS) \
	$(WPG_LIBS) \
	$(CPPUNIT_LIBS) \
	$(XML_LIBS)

test_SOURCES = \
	KEY2TokenTest.cpp \
	KEY2TokenTest.h \
	KEYOutputTest.cpp \
	KEYOutputTest.h \
	KEYPathTest.cpp \
	KEYPathTest.h \
	KEYPropertyMapTest.cpp \
	KEYPropertyMapTest.h \
	KEYShapeTest.cpp \
	KEYShapeTest.h \
	KEYStyleContextTest.cpp \
	KEYStyleContextTest.h \
	KEYStylesTest.cpp \
	KEYStylesTest.h \
	KEYTransformationTest.cpp \
	KEYTransformationTest.h \
	KEYXMLReaderTest.cpp \
	KEYXMLReaderTest.h \
	test.cpp

TESTS = $(target_test)
