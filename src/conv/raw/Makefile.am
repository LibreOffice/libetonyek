bin_PROGRAMS = key2raw numbers2raw pages2raw

AM_CXXFLAGS = -I$(top_srcdir)/inc -I$(top_builddir)/src/lib $(REVENGE_CFLAGS) $(REVENGE_GENERATORS_CFLAGS) $(REVENGE_STREAM_CFLAGS) $(BOOST_CFLAGS) $(DEBUG_CXXFLAGS)

key2raw_DEPENDENCIES = @KEY2RAW_WIN32_RESOURCE@
key2raw_LDFLAGS = $(BOOST_LDFLAGS)
key2raw_LDADD = ../../lib/libetonyek-@ETONYEK_MAJOR_VERSION@.@ETONYEK_MINOR_VERSION@.la $(REVENGE_LIBS) $(REVENGE_GENERATORS_LIBS) $(REVENGE_STREAM_LIBS) $(ZLIB_LIBS) $(BOOST_FILESYSTEM_LIB) @KEY2RAW_WIN32_RESOURCE@

key2raw_SOURCES = \
	key2raw.cpp

numbers2raw_DEPENDENCIES = @NUMBERS2RAW_WIN32_RESOURCE@
numbers2raw_LDFLAGS = $(BOOST_LDFLAGS)
numbers2raw_LDADD = ../../lib/libetonyek-@ETONYEK_MAJOR_VERSION@.@ETONYEK_MINOR_VERSION@.la $(REVENGE_LIBS) $(REVENGE_GENERATORS_LIBS) $(REVENGE_STREAM_LIBS) $(ZLIB_LIBS) $(BOOST_FILESYSTEM_LIB) @NUMBERS2RAW_WIN32_RESOURCE@

numbers2raw_SOURCES = \
	numbers2raw.cpp

pages2raw_DEPENDENCIES = @PAGES2RAW_WIN32_RESOURCE@
pages2raw_LDFLAGS = $(BOOST_LDFLAGS)
pages2raw_LDADD = ../../lib/libetonyek-@ETONYEK_MAJOR_VERSION@.@ETONYEK_MINOR_VERSION@.la $(REVENGE_LIBS) $(REVENGE_GENERATORS_LIBS) $(REVENGE_STREAM_LIBS) $(ZLIB_LIBS) $(BOOST_FILESYSTEM_LIB) @PAGES2RAW_WIN32_RESOURCE@

pages2raw_SOURCES = \
	pages2raw.cpp

if OS_WIN32

@KEY2RAW_WIN32_RESOURCE@ : key2raw.rc $(key2raw_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource key2raw.rc @KEY2RAW_WIN32_RESOURCE@

@NUMBERS2RAW_WIN32_RESOURCE@ : numbers2raw.rc $(numbers2raw_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource numbers2raw.rc @NUMBERS2RAW_WIN32_RESOURCE@

@PAGES2RAW_WIN32_RESOURCE@ : pages2raw.rc $(pages2raw_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource pages2raw.rc @PAGES2RAW_WIN32_RESOURCE@

endif

EXTRA_DIST = \
	$(key2raw_SOURCES) \
	$(numbers2raw_SOURCES) \
	$(pages2raw_SOURCES) \
	key2raw.rc.in \
	numbers2raw.rc.in \
	pages2raw.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	key2raw.rc \
	numbers2raw.rc \
	pages2raw.rc
