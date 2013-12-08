bin_PROGRAMS = numbers2csv

AM_CXXFLAGS = -I$(top_srcdir)/src/conv/common -I$(top_srcdir)/inc -I$(top_builddir)/src/lib $(REVENGE_CFLAGS) $(REVENGE_GENERATORS_CFLAGS) $(REVENGE_STREAM_CFLAGS) $(BOOST_CFLAGS) $(DEBUG_CXXFLAGS)

numbers2csv_DEPENDENCIES = @NUMBERS2CSV_WIN32_RESOURCE@

numbers2csv_LDFLAGS = $(BOOST_LDFLAGS)
numbers2csv_LDADD = ../common/libcommon.a ../../lib/libetonyek-@ETONYEK_MAJOR_VERSION@.@ETONYEK_MINOR_VERSION@.la $(REVENGE_LIBS) $(REVENGE_GENERATORS_LIBS) $(REVENGE_STREAM_LIBS) $(ZLIB_LIBS) $(BOOST_FILESYSTEM_LIB) @NUMBERS2CSV_WIN32_RESOURCE@

numbers2csv_SOURCES = \
	numbers2csv.cpp

if OS_WIN32

@NUMBERS2CSV_WIN32_RESOURCE@ : numbers2csv.rc $(numbers2csv_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource numbers2csv.rc @NUMBERS2CSV_WIN32_RESOURCE@
endif

EXTRA_DIST = \
	$(numbers2csv_SOURCES) \
	numbers2csv.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	numbers2csv.rc
