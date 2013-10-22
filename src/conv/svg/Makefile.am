bin_PROGRAMS = key2xhtml

AM_CXXFLAGS = -I$(top_srcdir)/src/conv/common -I$(top_srcdir)/inc -I$(top_builddir)/src/lib $(WPD_CFLAGS) $(BOOST_CFLAGS) $(DEBUG_CXXFLAGS)

key2xhtml_DEPENDENCIES = @KEY2XHTML_WIN32_RESOURCE@

key2xhtml_LDFLAGS = $(BOOST_LDFLAGS)
key2xhtml_LDADD = ../common/libcommon.a ../../lib/libetonyek-@ETONYEK_MAJOR_VERSION@.@ETONYEK_MINOR_VERSION@.la $(WPD_LIBS) $(ZLIB_LIBS) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB) @KEY2XHTML_WIN32_RESOURCE@

key2xhtml_SOURCES = \
	key2xhtml.cpp

if OS_WIN32

@KEY2XHTML_WIN32_RESOURCE@ : key2xhtml.rc $(key2xhtml_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource key2xhtml.rc @KEY2XHTML_WIN32_RESOURCE@
endif

EXTRA_DIST = \
	$(key2xhtml_SOURCES) \
	key2xhtml.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	key2xhtml.rc
