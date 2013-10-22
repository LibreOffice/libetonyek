BUILT_SOURCES = KEY1Token.inc KEY2Token.inc
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
	$(top_srcdir)/inc/libkeynote/KEYDocument.h \
	$(top_srcdir)/inc/libkeynote/KEYPresentationInterface.h \
	$(top_srcdir)/inc/libkeynote/KEYStringVector.h

AM_CXXFLAGS = -I$(top_srcdir)/inc $(WPD_CFLAGS) $(XML_CFLAGS) $(ZLIB_CFLAGS) $(BOOST_CFLAGS) $(DEBUG_CXXFLAGS)

libkeynote_@KEYNOTE_MAJOR_VERSION@_@KEYNOTE_MINOR_VERSION@_la_LIBADD  = $(WPD_LIBS) $(XML_LIBS) $(ZLIB_LIBS) @LIBKEYNOTE_WIN32_RESOURCE@
libkeynote_@KEYNOTE_MAJOR_VERSION@_@KEYNOTE_MINOR_VERSION@_la_DEPENDENCIES = @LIBKEYNOTE_WIN32_RESOURCE@
libkeynote_@KEYNOTE_MAJOR_VERSION@_@KEYNOTE_MINOR_VERSION@_la_LDFLAGS = $(version_info) -export-dynamic -no-undefined
libkeynote_@KEYNOTE_MAJOR_VERSION@_@KEYNOTE_MINOR_VERSION@_la_SOURCES = \
	KEY1Defaults.cpp \
	KEY1Defaults.h \
	KEY1Parser.cpp \
	KEY1Parser.h \
	KEY1Token.cpp \
	KEY1Token.h \
	KEY2Defaults.cpp \
	KEY2Defaults.h \
	KEY2Parser.cpp \
	KEY2Parser.h \
	KEY2ParserUtils.cpp \
	KEY2ParserUtils.h \
	KEY2StyleParser.cpp \
	KEY2StyleParser.h \
	KEY2Token.cpp \
	KEY2Token.h \
	KEYCollector.cpp \
	KEYCollector.h \
	KEYCollectorBase.cpp \
	KEYCollectorBase.h \
	KEYContentCollector.cpp \
	KEYContentCollector.h \
	KEYDefaults.cpp \
	KEYDefaults.h \
	KEYDictionary.cpp \
	KEYDictionary.h \
	KEYDocument.cpp \
	KEYMemoryStream.cpp \
	KEYMemoryStream.h \
	KEYObject.cpp \
	KEYObject.h \
	KEYOutput.cpp \
	KEYOutput.h \
	KEYParser.cpp \
	KEYParser.h \
	KEYPath.cpp \
	KEYPath.h \
	KEYPath_fwd.h \
	KEYPropertyMap.cpp \
	KEYPropertyMap.h \
	KEYSVGGenerator.cpp \
	KEYSVGGenerator.h \
	KEYShape.cpp \
	KEYShape.h \
	KEYStringVector.cpp \
	KEYStyle.cpp \
	KEYStyle.h \
	KEYStyleContext.cpp \
	KEYStyleContext.h \
	KEYStyles.cpp \
	KEYStyles.h \
	KEYStyles_fwd.h \
	KEYStylesheet.cpp \
	KEYStylesheet.h \
	KEYText.cpp \
	KEYText.h \
	KEYText_fwd.h \
	KEYThemeCollector.cpp \
	KEYThemeCollector.h \
	KEYTransformation.cpp \
	KEYTransformation.h \
	KEYTypes.cpp \
	KEYTypes.h \
	KEYTypes_fwd.h \
	KEYXMLReader.cpp \
	KEYXMLReader.h \
	KEYZipStream.cpp \
	KEYZipStream.h \
	KEYZlibStream.cpp \
	KEYZlibStream.h \
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
	KEY1Token.gperf \
	KEY2Token.gperf \
	libkeynote.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	libkeynote.rc

KEY1Token.cpp : KEY1Token.inc
KEY2Token.cpp : KEY2Token.inc

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
