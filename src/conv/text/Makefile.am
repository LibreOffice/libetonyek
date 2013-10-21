bin_PROGRAMS = key2text

AM_CXXFLAGS = -I$(top_srcdir)/src/conv/common -I$(top_srcdir)/inc -I$(top_builddir)/src/lib $(WPD_CFLAGS) $(BOOST_CFLAGS) $(DEBUG_CXXFLAGS)

key2text_DEPENDENCIES = @KEY2RAW_WIN32_RESOURCE@
key2text_LDFLAGS = $(BOOST_LDFLAGS)
key2text_LDADD = ../common/libcommon.a ../../lib/libkeynote-@KEYNOTE_MAJOR_VERSION@.@KEYNOTE_MINOR_VERSION@.la $(WPD_LIBS) $(ZLIB_LIBS) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB) @KEY2RAW_WIN32_RESOURCE@

key2text_SOURCES = \
	key2text.cpp

if OS_WIN32

@KEY2RAW_WIN32_RESOURCE@ : key2text.rc $(key2text_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource key2text.rc @KEY2RAW_WIN32_RESOURCE@
endif

EXTRA_DIST = \
	$(key2text_SOURCES) \
	key2text.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	key2text.rc
