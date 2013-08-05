if PLATFORM_WIN32
version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -avoid-version
else
version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
endif

lib_LTLIBRARIES = libkeynote-@KN_MAJOR_VERSION@.@KN_MINOR_VERSION@.la
libkeynote_@KN_MAJOR_VERSION@_@KN_MINOR_VERSION@_includedir = $(includedir)/libkeynote-@KN_MAJOR_VERSION@.@KN_MINOR_VERSION@/libkeynote
libkeynote_@KN_MAJOR_VERSION@_@KN_MINOR_VERSION@_include_HEADERS = \
	$(top_srcdir)/inc/libkeynote/libkeynote.h \
	$(top_srcdir)/inc/libkeynote/KeyNoteDocument.h \
	$(top_srcdir)/inc/libkeynote/KNStringVector.h

AM_CXXFLAGS = -I$(top_srcdir)/inc $(WPD_CFLAGS) $(WPG_CFLAGS) $(ZLIB_CFLAGS) $(DEBUG_CXXFLAGS)

libkeynote_@KN_MAJOR_VERSION@_@KN_MINOR_VERSION@_la_LIBADD  = $(WPD_LIBS) $(ZLIB_LIBS) @LIBKEYNOTE_WIN32_RESOURCE@
libkeynote_@KN_MAJOR_VERSION@_@KN_MINOR_VERSION@_la_DEPENDENCIES = @LIBKEYNOTE_WIN32_RESOURCE@
libkeynote_@KN_MAJOR_VERSION@_@KN_MINOR_VERSION@_la_LDFLAGS = $(version_info) -export-dynamic -no-undefined
libkeynote_@KN_MAJOR_VERSION@_@KN_MINOR_VERSION@_la_SOURCES = \
	KNCollector.cpp \
	KNCollector.h \
	KNMemoryStream.cpp \
	KNMemoryStream.h \
	KNParser.cpp \
	KNParser.h \
	KNStringVector.cpp \
	KNSVGGenerator.cpp \
	KNSVGGenerator.h \
	KNZipStream.cpp \
	KNZipStream.h \
	KeyNoteDocument.cpp \
	libkeynote_utils.cpp \
	libkeynote_utils.h

if OS_WIN32

@LIBKEYNOTE_WIN32_RESOURCE@ : libkeynote.rc $(libkeynote_@KN_MAJOR_VERSION@_@KN_MINOR_VERSION@_la_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource libkeynote.rc @LIBKEYNOTE_WIN32_RESOURCE@

endif

distclean-local :
	@echo

EXTRA_DIST = \
	libkeynote.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	libkeynote.rc
