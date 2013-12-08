bin_PROGRAMS = pages2html

AM_CXXFLAGS = -I$(top_srcdir)/src/conv/common -I$(top_srcdir)/inc $(REVENGE_CFLAGS) $(REVENGE_GENERATORS_CFLAGS) $(REVENGE_STREAM_CFLAGS) $(BOOST_CFLAGS) $(DEBUG_CXXFLAGS)

pages2html_DEPENDENCIES = @PAGES2HTML_WIN32_RESOURCE@

pages2html_LDADD = ../common/libcommon.a ../../lib/libetonyek-@ETONYEK_MAJOR_VERSION@.@ETONYEK_MINOR_VERSION@.la $(REVENGE_LIBS) $(REVENGE_GENERATORS_LIBS) $(REVENGE_STREAM_LIBS) $(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB) @PAGES2HTML_WIN32_RESOURCE@
pages2html_LDFLAGS = $(BOOST_LDFLAGS)

pages2html_SOURCES = \
	pages2html.cpp

if OS_WIN32

@PAGES2HTML_WIN32_RESOURCE@ : pages2html.rc $(pages2html_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource && \
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource pages2html.rc @PAGES2HTML_WIN32_RESOURCE@
endif

# Include the pages2html_SOURCES in case we build a tarball without stream
EXTRA_DIST = \
	$(pages2html_SOURCES)	\
	pages2html.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	pages2html.rc

