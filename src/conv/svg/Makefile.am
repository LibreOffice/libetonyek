bin_PROGRAMS = keynote2xhtml

AM_CXXFLAGS = -I../common -I$(top_srcdir)/inc -I$(top_builddir)/src/lib $(WPD_CFLAGS) $(WPG_CFLAGS) $(BOOST_CFLAGS) $(DEBUG_CXXFLAGS)

keynote2xhtml_DEPENDENCIES = @KN2XHTML_WIN32_RESOURCE@

keynote2xhtml_LDFLAGS = $(BOOST_LDFLAGS)
keynote2xhtml_LDADD = ../common/libcommon.a ../../lib/libkeynote-@KEYNOTE_MAJOR_VERSION@.@KEYNOTE_MINOR_VERSION@.la $(WPD_LIBS) $(ZLIB_LIBS) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB) @KN2XHTML_WIN32_RESOURCE@

keynote2xhtml_SOURCES = \
	keynote2xhtml.cpp

if OS_WIN32

@KN2XHTML_WIN32_RESOURCE@ : keynote2xhtml.rc $(keynote2xhtml_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource keynote2xhtml.rc @KN2XHTML_WIN32_RESOURCE@
endif

EXTRA_DIST = \
	$(keynote2xhtml_SOURCES) \
	keynote2xhtml.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	keynote2xhtml.rc
