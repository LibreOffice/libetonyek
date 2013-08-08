bin_PROGRAMS = keynote2raw

AM_CXXFLAGS = -I$(top_srcdir)/inc -I$(top_builddir)/src/lib $(WPD_CFLAGS) $(WPG_CFLAGS) $(DEBUG_CXXFLAGS)

keynote2raw_DEPENDENCIES = @KN2RAW_WIN32_RESOURCE@
keynote2raw_LDADD = ../../lib/libkeynote-@KEYNOTE_MAJOR_VERSION@.@KEYNOTE_MINOR_VERSION@.la $(WPD_LIBS) $(ZLIB_LIBS) @KN2RAW_WIN32_RESOURCE@

keynote2raw_SOURCES = \
	keynote2raw.cpp

if OS_WIN32

@KN2RAW_WIN32_RESOURCE@ : keynote2raw.rc $(keynote2raw_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource keynote2raw.rc @KN2RAW_WIN32_RESOURCE@
endif

EXTRA_DIST = \
	$(keynote2raw_SOURCES) \
	keynote2raw.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	keynote2raw.rc	 
