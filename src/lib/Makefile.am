BUILT_SOURCES = KNToken.inc
GPERFFLAGS = --compare-strncmp --enum --null-strings --readonly-tables

if PLATFORM_WIN32
version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -avoid-version
else
version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
endif

lib_LTLIBRARIES = libkeynote-@KEYNOTE_MAJOR_VERSION@.@KEYNOTE_MINOR_VERSION@.la
libkeynote_@KEYNOTE_MAJOR_VERSION@_@KEYNOTE_MINOR_VERSION@_includedir = $(includedir)/libkeynote-@KEYNOTE_MAJOR_VERSION@.@KEYNOTE_MINOR_VERSION@/libkeynote
libkeynote_@KEYNOTE_MAJOR_VERSION@_@KEYNOTE_MINOR_VERSION@_include_HEADERS = \
	$(top_srcdir)/inc/libkeynote/libkeynote.h \
	$(top_srcdir)/inc/libkeynote/KeyNoteDocument.h \
	$(top_srcdir)/inc/libkeynote/KNStringVector.h

AM_CXXFLAGS = -I$(top_srcdir)/inc $(WPD_CFLAGS) $(WPG_CFLAGS) $(XML_CFLAGS) $(ZLIB_CFLAGS) $(BOOST_CFLAGS) $(DEBUG_CXXFLAGS)

libkeynote_@KEYNOTE_MAJOR_VERSION@_@KEYNOTE_MINOR_VERSION@_la_LIBADD  = $(WPD_LIBS) $(WPG_LIBS) $(XML_LIBS) $(ZLIB_LIBS) @LIBKEYNOTE_WIN32_RESOURCE@
libkeynote_@KEYNOTE_MAJOR_VERSION@_@KEYNOTE_MINOR_VERSION@_la_DEPENDENCIES = @LIBKEYNOTE_WIN32_RESOURCE@
libkeynote_@KEYNOTE_MAJOR_VERSION@_@KEYNOTE_MINOR_VERSION@_la_LDFLAGS = $(version_info) -export-dynamic -no-undefined
libkeynote_@KEYNOTE_MAJOR_VERSION@_@KEYNOTE_MINOR_VERSION@_la_SOURCES = \
	KNCollector.cpp \
	KNCollector.h \
	KNMemoryStream.cpp \
	KNMemoryStream.h \
	KNParser.cpp \
	KNParser.h \
	KN1Parser.cpp \
	KN1Parser.h \
	KN2Parser.cpp \
	KN2Parser.h \
	KNStringVector.cpp \
	KNSVGGenerator.cpp \
	KNSVGGenerator.h \
	KNToken.cpp \
	KNToken.h \
	KNZipStream.cpp \
	KNZipStream.h \
	KNZlibStream.cpp \
	KNZlibStream.h \
	KeyNoteDocument.cpp \
	libkeynote_utils.cpp \
	libkeynote_utils.h

if OS_WIN32

@LIBKEYNOTE_WIN32_RESOURCE@ : libkeynote.rc $(libkeynote_@KEYNOTE_MAJOR_VERSION@_@KEYNOTE_MINOR_VERSION@_la_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource libkeynote.rc @LIBKEYNOTE_WIN32_RESOURCE@

endif

distclean-local :
	rm -f $(BUILT_SOURCES)

EXTRA_DIST = \
	libkeynote.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	libkeynote.rc

KNToken.cpp : KNToken.inc

KNToken.inc : KNToken.gperf
	if $(GPERF) $(GPERFFLAGS) --language C++ $< > $@.tmp; then \
		mv $@.tmp $@; \
	else \
		rm -f $@.tmp; \
		if $(GPERF) --version >/dev/null 2>&1; then \
			exit 1; \
		else \
			touch $@; \
		fi \
	fi
