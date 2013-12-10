bin_PROGRAMS = key2text numbers2text pages2text

AM_CXXFLAGS = -I$(top_srcdir)/inc -I$(top_builddir)/src/lib $(REVENGE_CFLAGS) $(REVENGE_GENERATORS_CFLAGS) $(REVENGE_STREAM_CFLAGS) $(BOOST_CFLAGS) $(DEBUG_CXXFLAGS)

key2text_DEPENDENCIES = @KEY2RAW_WIN32_RESOURCE@
key2text_LDFLAGS = $(BOOST_LDFLAGS)
key2text_LDADD = ../../lib/libetonyek-@ETONYEK_MAJOR_VERSION@.@ETONYEK_MINOR_VERSION@.la $(REVENGE_LIBS) $(REVENGE_GENERATORS_LIBS) $(REVENGE_STREAM_LIBS) $(ZLIB_LIBS) $(BOOST_FILESYSTEM_LIB) @KEY2RAW_WIN32_RESOURCE@

key2text_SOURCES = \
	key2text.cpp

numbers2text_DEPENDENCIES = @NUMBERS2RAW_WIN32_RESOURCE@
numbers2text_LDFLAGS = $(BOOST_LDFLAGS)
numbers2text_LDADD = ../../lib/libetonyek-@ETONYEK_MAJOR_VERSION@.@ETONYEK_MINOR_VERSION@.la $(REVENGE_LIBS) $(REVENGE_GENERATORS_LIBS) $(REVENGE_STREAM_LIBS) $(ZLIB_LIBS) $(BOOST_FILESYSTEM_LIB) @NUMBERS2RAW_WIN32_RESOURCE@

numbers2text_SOURCES = \
	numbers2text.cpp

pages2text_DEPENDENCIES = @PAGES2RAW_WIN32_RESOURCE@
pages2text_LDFLAGS = $(BOOST_LDFLAGS)
pages2text_LDADD = ../../lib/libetonyek-@ETONYEK_MAJOR_VERSION@.@ETONYEK_MINOR_VERSION@.la $(REVENGE_LIBS) $(REVENGE_GENERATORS_LIBS) $(REVENGE_STREAM_LIBS) $(ZLIB_LIBS) $(BOOST_FILESYSTEM_LIB) @PAGES2RAW_WIN32_RESOURCE@

pages2text_SOURCES = \
	pages2text.cpp

if OS_WIN32

@KEY2RAW_WIN32_RESOURCE@ : key2text.rc $(key2text_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource key2text.rc @KEY2RAW_WIN32_RESOURCE@

@NUMBERS2RAW_WIN32_RESOURCE@ : numbers2text.rc $(numbers2text_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource numbers2text.rc @NUMBERS2RAW_WIN32_RESOURCE@

@PAGES2RAW_WIN32_RESOURCE@ : pages2text.rc $(pages2text_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource pages2text.rc @PAGES2RAW_WIN32_RESOURCE@

endif

EXTRA_DIST = \
	$(key2text_SOURCES) \
	$(numbers2text_SOURCES) \
	$(pages2text_SOURCES) \
	key2text.rc.in \
	numbers2text.rc.in \
	pages2text.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	key2text.rc \
	numbers2text.rc \
	pages2text.rc
