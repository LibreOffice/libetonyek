BUILT_SOURCES = KEY1Token.inc KEY2Token.inc NUMToken.inc PAGToken.inc
GPERFFLAGS = --compare-strncmp --enum --null-strings --readonly-tables

if PLATFORM_WIN32
version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -avoid-version
else
version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
endif

noinst_LTLIBRARIES = libetonyek_internal.la
lib_LTLIBRARIES = libetonyek-@ETONYEK_MAJOR_VERSION@.@ETONYEK_MINOR_VERSION@.la
libetonyek_@ETONYEK_MAJOR_VERSION@_@ETONYEK_MINOR_VERSION@_includedir = $(includedir)/libetonyek-@ETONYEK_MAJOR_VERSION@.@ETONYEK_MINOR_VERSION@/libetonyek
libetonyek_@ETONYEK_MAJOR_VERSION@_@ETONYEK_MINOR_VERSION@_include_HEADERS = \
	$(top_srcdir)/inc/libetonyek/libetonyek.h \
	$(top_srcdir)/inc/libetonyek/EtonyekDocument.h

AM_CXXFLAGS = -I$(top_srcdir)/inc $(REVENGE_CFLAGS) $(XML_CFLAGS) $(ZLIB_CFLAGS) $(BOOST_CFLAGS) $(DEBUG_CXXFLAGS) -DLIBETONYEK_BUILD

libetonyek_@ETONYEK_MAJOR_VERSION@_@ETONYEK_MINOR_VERSION@_la_LIBADD  = libetonyek_internal.la $(REVENGE_LIBS) $(XML_LIBS) $(ZLIB_LIBS) @LIBETONYEK_WIN32_RESOURCE@
libetonyek_@ETONYEK_MAJOR_VERSION@_@ETONYEK_MINOR_VERSION@_la_DEPENDENCIES = libetonyek_internal.la @LIBETONYEK_WIN32_RESOURCE@
libetonyek_@ETONYEK_MAJOR_VERSION@_@ETONYEK_MINOR_VERSION@_la_LDFLAGS = $(version_info) -export-dynamic -no-undefined
libetonyek_@ETONYEK_MAJOR_VERSION@_@ETONYEK_MINOR_VERSION@_la_SOURCES = \
	EtonyekDocument.cpp

libetonyek_internal_la_SOURCES = \
	IWORKEnum.h \
	IWORKMemoryStream.cpp \
	IWORKMemoryStream.h \
	IWORKPath.cpp \
	IWORKPath.h \
	IWORKPath_fwd.h \
	IWORKPropertyMap.cpp \
	IWORKPropertyMap.h \
	IWORKTransformation.cpp \
	IWORKTransformation.h \
	IWORKTypes.cpp \
	IWORKTypes.h \
	IWORKTypes_fwd.h \
	IWORKXMLReader.cpp \
	IWORKXMLReader.h \
	IWORKZlibStream.cpp \
	IWORKZlibStream.h \
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
	KEY2TableParser.cpp \
	KEY2TableParser.h \
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
	KEYObject.cpp \
	KEYObject.h \
	KEYOutput.cpp \
	KEYOutput.h \
	KEYParser.cpp \
	KEYParser.h \
	KEYShape.cpp \
	KEYShape.h \
	KEYStyle.cpp \
	KEYStyle.h \
	KEYStyleContext.cpp \
	KEYStyleContext.h \
	KEYStyles.cpp \
	KEYStyles.h \
	KEYStyles_fwd.h \
	KEYStylesheet.cpp \
	KEYStylesheet.h \
	KEYTable.cpp \
	KEYTable.h \
	KEYText.cpp \
	KEYText.h \
	KEYText_fwd.h \
	KEYThemeCollector.cpp \
	KEYThemeCollector.h \
	KEYTypes.cpp \
	KEYTypes.h \
	KEYTypes_fwd.h \
	NUMCollector.cpp \
	NUMCollector.h \
	NUMParser.cpp \
	NUMParser.h \
	NUMToken.cpp \
	NUMToken.h \
	PAGCollector.cpp \
	PAGCollector.h \
	PAGParser.cpp \
	PAGParser.h \
	PAGToken.cpp \
	PAGToken.h \
	libetonyek_utils.cpp \
	libetonyek_utils.h \
	libetonyek_xml.cpp \
	libetonyek_xml.h

if OS_WIN32

@LIBETONYEK_WIN32_RESOURCE@ : libetonyek.rc $(libetonyek_@ETONYEK_MAJOR_VERSION@_@ETONYEK_MINOR_VERSION@_la_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource libetonyek.rc @LIBETONYEK_WIN32_RESOURCE@

endif

distclean-local :
	rm -f $(BUILT_SOURCES)

EXTRA_DIST = \
	KEY1Token.gperf \
	KEY2Token.gperf \
	NUMToken.gperf \
	PAGToken.gperf \
	libetonyek.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	libetonyek.rc

KEY1Token.cpp : KEY1Token.inc
KEY2Token.cpp : KEY2Token.inc
NUMToken.cpp : NUMToken.inc
PAGToken.cpp : PAGToken.inc

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
