BUILT_SOURCES = KN1Token.inc KN2Token.inc
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
	$(top_srcdir)/inc/libkeynote/KNDocument.h \
	$(top_srcdir)/inc/libkeynote/KNStringVector.h

AM_CXXFLAGS = -I$(top_srcdir)/inc $(WPD_CFLAGS) $(WPG_CFLAGS) $(XML_CFLAGS) $(ZLIB_CFLAGS) $(BOOST_CFLAGS) $(DEBUG_CXXFLAGS)

libkeynote_@KEYNOTE_MAJOR_VERSION@_@KEYNOTE_MINOR_VERSION@_la_LIBADD  = $(WPD_LIBS) $(WPG_LIBS) $(XML_LIBS) $(ZLIB_LIBS) @LIBKEYNOTE_WIN32_RESOURCE@
libkeynote_@KEYNOTE_MAJOR_VERSION@_@KEYNOTE_MINOR_VERSION@_la_DEPENDENCIES = @LIBKEYNOTE_WIN32_RESOURCE@
libkeynote_@KEYNOTE_MAJOR_VERSION@_@KEYNOTE_MINOR_VERSION@_la_LDFLAGS = $(version_info) -export-dynamic -no-undefined
libkeynote_@KEYNOTE_MAJOR_VERSION@_@KEYNOTE_MINOR_VERSION@_la_SOURCES = \
	KN1Defaults.cpp \
	KN1Defaults.h \
	KN1Parser.cpp \
	KN1Parser.h \
	KN1Token.cpp \
	KN1Token.h \
	KN2Defaults.cpp \
	KN2Defaults.h \
	KN2Parser.cpp \
	KN2Parser.h \
	KN2ParserUtils.cpp \
	KN2ParserUtils.h \
	KN2StyleParser.cpp \
	KN2StyleParser.h \
	KN2Token.cpp \
	KN2Token.h \
	KNCollector.cpp \
	KNCollector.h \
	KNCollectorBase.cpp \
	KNCollectorBase.h \
	KNContentCollector.cpp \
	KNContentCollector.h \
	KNDefaults.cpp \
	KNDefaults.h \
	KNDictionary.cpp \
	KNDictionary.h \
	KNDocument.cpp \
	KNMemoryStream.cpp \
	KNMemoryStream.h \
	KNObject.cpp \
	KNObject.h \
	KNOutput.cpp \
	KNOutput.h \
	KNParser.cpp \
	KNParser.h \
	KNPath.cpp \
	KNPath.h \
	KNPath_fwd.h \
	KNPropertyMap.cpp \
	KNPropertyMap.h \
	KNSVGGenerator.cpp \
	KNSVGGenerator.h \
	KNShape.cpp \
	KNShape.h \
	KNStringVector.cpp \
	KNStyle.cpp \
	KNStyle.h \
	KNStyleContext.cpp \
	KNStyleContext.h \
	KNStyles.cpp \
	KNStyles.h \
	KNStyles_fwd.h \
	KNStylesheet.cpp \
	KNStylesheet.h \
	KNText.cpp \
	KNText.h \
	KNText_fwd.h \
	KNThemeCollector.cpp \
	KNThemeCollector.h \
	KNTransformation.cpp \
	KNTransformation.h \
	KNTypes.cpp \
	KNTypes.h \
	KNTypes_fwd.h \
	KNXMLReader.cpp \
	KNXMLReader.h \
	KNZipStream.cpp \
	KNZipStream.h \
	KNZlibStream.cpp \
	KNZlibStream.h \
	libkeynote_utils.cpp \
	libkeynote_utils.h \
	libkeynote_xml.cpp \
	libkeynote_xml.h

if OS_WIN32

@LIBKEYNOTE_WIN32_RESOURCE@ : libkeynote.rc $(libkeynote_@KEYNOTE_MAJOR_VERSION@_@KEYNOTE_MINOR_VERSION@_la_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource libkeynote.rc @LIBKEYNOTE_WIN32_RESOURCE@

endif

distclean-local :
	rm -f $(BUILT_SOURCES)

EXTRA_DIST = \
	KN1Token.gperf \
	KN2Token.gperf \
	libkeynote.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	libkeynote.rc

KN1Token.cpp : KN1Token.inc
KN2Token.cpp : KN2Token.inc

%.inc : %.gperf
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
