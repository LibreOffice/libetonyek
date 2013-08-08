target_test = test

check_PROGRAMS = $(target_test)

AM_CXXFLAGS = -I$(top_srcdir)/inc -I$(top_srcdir)/src/lib $(CPPUNIT_CFLAGS) $(XML_CFLAGS) $(DEBUG_CXXFLAGS)

test_LDFLAGS = -L$(top_srcdir)/src/lib
test_LDADD = \
	$(top_srcdir)/src/lib/libkeynote-@KEYNOTE_MAJOR_VERSION@.@KEYNOTE_MINOR_VERSION@.la \
	$(CPPUNIT_LIBS) \
	$(XML_LIBS)

test_SOURCES = \
	KNXMLAttributeIteratorTest.cpp \
	KNXMLAttributeIteratorTest.h \
	test.cpp

TESTS = $(target_test)
