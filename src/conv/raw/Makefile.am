bin_PROGRAMS = key2raw

AM_CXXFLAGS = -I$(top_srcdir)/src/conv/common -I$(top_srcdir)/inc -I$(top_builddir)/src/lib $(WPD_CFLAGS) $(BOOST_CFLAGS) $(DEBUG_CXXFLAGS)

key2raw_DEPENDENCIES = @KEY2RAW_WIN32_RESOURCE@
key2raw_LDFLAGS = $(BOOST_LDFLAGS)
key2raw_LDADD = ../common/libcommon.a ../../lib/libetonyek-@ETONYEK_MAJOR_VERSION@.@ETONYEK_MINOR_VERSION@.la $(WPD_LIBS) $(ZLIB_LIBS) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB) @KEY2RAW_WIN32_RESOURCE@

key2raw_SOURCES = \
	key2raw.cpp

if OS_WIN32

@KEY2RAW_WIN32_RESOURCE@ : key2raw.rc $(key2raw_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource key2raw.rc @KEY2RAW_WIN32_RESOURCE@
endif

EXTRA_DIST = \
	$(key2raw_SOURCES) \
	key2raw.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	key2raw.rc
