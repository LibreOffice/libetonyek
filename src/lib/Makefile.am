BUILT_SOURCES = IWORKToken.inc KEY1Token.inc KEY2Token.inc NUM1Token.inc PAG1Token.inc
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

AM_CXXFLAGS = -I$(top_srcdir)/inc -I$(top_srcdir)/src/lib/contexts $(REVENGE_CFLAGS) $(XML_CFLAGS) $(ZLIB_CFLAGS) $(BOOST_CFLAGS) $(GLM_CFLAGS) $(DEBUG_CXXFLAGS) -DLIBETONYEK_BUILD

libetonyek_@ETONYEK_MAJOR_VERSION@_@ETONYEK_MINOR_VERSION@_la_LIBADD  = libetonyek_internal.la $(REVENGE_LIBS) $(XML_LIBS) $(ZLIB_LIBS) @LIBETONYEK_WIN32_RESOURCE@
libetonyek_@ETONYEK_MAJOR_VERSION@_@ETONYEK_MINOR_VERSION@_la_DEPENDENCIES = libetonyek_internal.la @LIBETONYEK_WIN32_RESOURCE@
libetonyek_@ETONYEK_MAJOR_VERSION@_@ETONYEK_MINOR_VERSION@_la_LDFLAGS = $(version_info) -export-dynamic -no-undefined
libetonyek_@ETONYEK_MAJOR_VERSION@_@ETONYEK_MINOR_VERSION@_la_SOURCES = \
	EtonyekDocument.cpp

libetonyek_internal_la_SOURCES = \
	IWORKChainedTokenizer.cpp \
	IWORKChainedTokenizer.h \
	IWORKCollector.cpp \
	IWORKCollector.h \
	IWORKDictionary.cpp \
	IWORKDictionary.h \
	IWORKDocumentInterface.cpp \
	IWORKDocumentInterface.h \
	IWORKEnum.h \
	IWORKMemoryStream.cpp \
	IWORKMemoryStream.h \
	IWORKOutputElements.cpp \
	IWORKOutputElements.h \
	IWORKParser.cpp \
	IWORKParser.h \
	IWORKPath.cpp \
	IWORKPath.h \
	IWORKPath_fwd.h \
	IWORKPresentationRedirector.cpp \
	IWORKPresentationRedirector.h \
	IWORKProperties.cpp \
	IWORKProperties.h \
	IWORKPropertyInfo.h \
	IWORKPropertyMap.cpp \
	IWORKPropertyMap.h \
	IWORKShape.cpp \
	IWORKShape.h \
	IWORKSpreadsheetRedirector.cpp \
	IWORKSpreadsheetRedirector.h \
	IWORKStyle.cpp \
	IWORKStyle.h \
	IWORKStyle_fwd.h \
	IWORKStylesheet.cpp \
	IWORKStylesheet.h \
	IWORKStyleStack.cpp \
	IWORKStyleStack.h \
	IWORKTable.cpp \
	IWORKTable.h \
	IWORKText.cpp \
	IWORKText.h \
	IWORKText_fwd.h \
	IWORKTextRedirector.cpp \
	IWORKTextRedirector.h \
	IWORKTransformation.cpp \
	IWORKTransformation.h \
	IWORKToken.cpp \
	IWORKToken.h \
	IWORKTokenInfo.h \
	IWORKTokenizer.cpp \
	IWORKTokenizer.h \
	IWORKTokenizerBase.cpp \
	IWORKTokenizerBase.h \
	IWORKTypes.cpp \
	IWORKTypes.h \
	IWORKTypes_fwd.h \
	IWORKXMLContext.cpp \
	IWORKXMLContext.h \
	IWORKXMLContextBase.cpp \
	IWORKXMLContextBase.h \
	IWORKXMLParserState.cpp \
	IWORKXMLParserState.h \
	IWORKZlibStream.cpp \
	IWORKZlibStream.h \
	IWORKOutputManager.cpp \
	IWORKOutputManager.h \
	KEY1Parser.cpp \
	KEY1Parser.h \
	KEY1Token.cpp \
	KEY1Token.h \
	KEY2Parser.cpp \
	KEY2Parser.h \
	KEY2ParserState.cpp \
	KEY2ParserState.h \
	KEY2Token.cpp \
	KEY2Token.h \
	KEY2XMLContextBase.h \
	KEYCollector.cpp \
	KEYCollector.h \
	KEYDictionary.cpp \
	KEYDictionary.h \
	KEYTypes.cpp \
	KEYTypes.h \
	KEYTypes_fwd.h \
	NUMCollector.cpp \
	NUMCollector.h \
	NUMDictionary.cpp \
	NUMDictionary.h \
	NUM1Parser.cpp \
	NUM1Parser.h \
	NUM1ParserState.cpp \
	NUM1ParserState.h \
	NUM1Token.cpp \
	NUM1Token.h \
	NUM1XMLContextBase.h \
	PAGCollector.cpp \
	PAGCollector.h \
	PAGDictionary.cpp \
	PAGDictionary.h \
	PAGTypes.cpp \
	PAGTypes.h \
	PAG1Parser.cpp \
	PAG1Parser.h \
	PAG1ParserState.cpp \
	PAG1ParserState.h \
	PAG1Token.cpp \
	PAG1Token.h \
	PAG1XMLContextBase.h \
	contexts/IWORKColorElement.cpp \
	contexts/IWORKColorElement.h \
	contexts/IWORKGeometryElement.cpp \
	contexts/IWORKGeometryElement.h \
	contexts/IWORKLayoutElement.cpp \
	contexts/IWORKLayoutElement.h \
	contexts/IWORKMediaElement.cpp \
	contexts/IWORKMediaElement.h \
	contexts/IWORKPElement.cpp \
	contexts/IWORKPElement.h \
	contexts/IWORKPositionElement.cpp \
	contexts/IWORKPositionElement.h \
	contexts/IWORKPropertyMapElement.cpp \
	contexts/IWORKPropertyMapElement.h \
	contexts/IWORKRefContext.cpp \
	contexts/IWORKRefContext.h \
	contexts/IWORKSizeElement.cpp \
	contexts/IWORKSizeElement.h \
	contexts/IWORKStringElement.cpp \
	contexts/IWORKStringElement.h \
	contexts/IWORKStylesContext.cpp \
	contexts/IWORKStylesContext.h \
	contexts/IWORKStyleContext.cpp \
	contexts/IWORKStyleContext.h \
	contexts/IWORKStyleRefContext.cpp \
	contexts/IWORKStyleRefContext.h \
	contexts/IWORKTabularInfoElement.cpp \
	contexts/IWORKTabularInfoElement.h \
	contexts/IWORKTextBodyElement.cpp \
	contexts/IWORKTextBodyElement.h \
	contexts/IWORKTextStorageElement.cpp \
	contexts/IWORKTextStorageElement.h \
	contexts/KEY2StyleContext.cpp \
	contexts/KEY2StyleContext.h \
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
	IWORKToken.gperf \
	KEY1Token.gperf \
	KEY2Token.gperf \
	NUM1Token.gperf \
	PAG1Token.gperf \
	libetonyek.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	libetonyek.rc

IWORKToken.cpp : IWORKToken.inc
KEY1Token.cpp : KEY1Token.inc
KEY2Token.cpp : KEY2Token.inc
NUM1Token.cpp : NUM1Token.inc
PAG1Token.cpp : PAG1Token.inc

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
