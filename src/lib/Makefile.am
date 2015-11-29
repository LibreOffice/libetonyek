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

AM_CXXFLAGS = \
	-DLIBETONYEK_BUILD \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/src/lib/contexts \
	$(BOOST_CFLAGS) \
	$(GLM_CFLAGS) \
	$(MDDS_CFLAGS) \
	$(REVENGE_CFLAGS) \
	$(XML_CFLAGS) \
	$(ZLIB_CFLAGS) \
	$(DEBUG_CXXFLAGS)

libetonyek_@ETONYEK_MAJOR_VERSION@_@ETONYEK_MINOR_VERSION@_la_LIBADD  = libetonyek_internal.la $(REVENGE_LIBS) $(XML_LIBS) $(ZLIB_LIBS) @LIBETONYEK_WIN32_RESOURCE@
libetonyek_@ETONYEK_MAJOR_VERSION@_@ETONYEK_MINOR_VERSION@_la_DEPENDENCIES = libetonyek_internal.la @LIBETONYEK_WIN32_RESOURCE@
libetonyek_@ETONYEK_MAJOR_VERSION@_@ETONYEK_MINOR_VERSION@_la_LDFLAGS = $(version_info) -export-dynamic -no-undefined
libetonyek_@ETONYEK_MAJOR_VERSION@_@ETONYEK_MINOR_VERSION@_la_SOURCES = \
	EtonyekDocument.cpp

libetonyek_internal_la_SOURCES = \
	IWAField.cpp \
	IWAField.h \
	IWAMessage.cpp \
	IWAMessage.h \
	IWAObjectType.h \
	IWAParser.cpp \
	IWAParser.h \
	IWAReader.cpp \
	IWAReader.h \
	IWASnappyStream.cpp \
	IWASnappyStream.h \
	IWORKChainedTokenizer.cpp \
	IWORKChainedTokenizer.h \
	IWORKChart.cpp \
	IWORKChart.h \
	IWORKCollector.cpp \
	IWORKCollector.h \
	IWORKDictionary.cpp \
	IWORKDictionary.h \
	IWORKDiscardContext.cpp \
	IWORKDiscardContext.h \
	IWORKDocumentInterface.cpp \
	IWORKDocumentInterface.h \
	IWORKEnum.h \
	IWORKFormula.cpp \
	IWORKFormula.h \
	IWORKMemoryStream.cpp \
	IWORKMemoryStream.h \
	IWORKOutputElements.cpp \
	IWORKOutputElements.h \
	IWORKOutputManager.cpp \
	IWORKOutputManager.h \
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
	IWORKStyleStack.cpp \
	IWORKStyleStack.h \
	IWORKStyle_fwd.h \
	IWORKStylesheet.cpp \
	IWORKStylesheet.h \
	IWORKTable.cpp \
	IWORKTable.h \
	IWORKText.cpp \
	IWORKText.h \
	IWORKTextRedirector.cpp \
	IWORKTextRedirector.h \
	IWORKText_fwd.h \
	IWORKToken.cpp \
	IWORKToken.h \
	IWORKTokenInfo.h \
	IWORKTokenizer.cpp \
	IWORKTokenizer.h \
	IWORKTokenizerBase.cpp \
	IWORKTokenizerBase.h \
	IWORKTransformation.cpp \
	IWORKTransformation.h \
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
	KEY1Parser.cpp \
	KEY1Parser.h \
	KEY1Token.cpp \
	KEY1Token.h \
	KEY2Dictionary.cpp \
	KEY2Dictionary.h \
	KEY2Parser.cpp \
	KEY2Parser.h \
	KEY2ParserState.cpp \
	KEY2ParserState.h \
	KEY2Token.cpp \
	KEY2Token.h \
	KEY2XMLContextBase.h \
	KEY6ObjectType.h \
	KEY6Parser.cpp \
	KEY6Parser.h \
	KEYCollector.cpp \
	KEYCollector.h \
	KEYTypes.cpp \
	KEYTypes.h \
	KEYTypes_fwd.h \
	NUM1Dictionary.cpp \
	NUM1Dictionary.h \
	NUM1Parser.cpp \
	NUM1Parser.h \
	NUM1ParserState.cpp \
	NUM1ParserState.h \
	NUM1Token.cpp \
	NUM1Token.h \
	NUM1XMLContextBase.h \
	NUMCollector.cpp \
	NUMCollector.h \
	PAG1Dictionary.cpp \
	PAG1Dictionary.h \
	PAG1Parser.cpp \
	PAG1Parser.h \
	PAG1ParserState.cpp \
	PAG1ParserState.h \
	PAG1Token.cpp \
	PAG1Token.h \
	PAG1XMLContextBase.h \
	PAGCollector.cpp \
	PAGCollector.h \
	PAGEnum.h \
	PAGProperties.cpp \
	PAGProperties.h \
	PAGTypes.cpp \
	PAGTypes.h \
	contexts/IWORKBezierElement.cpp \
	contexts/IWORKBezierElement.h \
	contexts/IWORKBrContext.cpp \
	contexts/IWORKBrContext.h \
	contexts/IWORKColorElement.cpp \
	contexts/IWORKColorElement.h \
	contexts/IWORKContainerContext.h \
	contexts/IWORKChartInfoElement.cpp \
	contexts/IWORKChartInfoElement.h \
	contexts/IWORKDataElement.cpp \
	contexts/IWORKDataElement.h \
	contexts/IWORKFilteredImageElement.cpp \
	contexts/IWORKFilteredImageElement.h \
	contexts/IWORKFoElement.cpp \
	contexts/IWORKFoElement.h \
	contexts/IWORKGeometryElement.cpp \
	contexts/IWORKGeometryElement.h \
	contexts/IWORKGroupElement.cpp \
	contexts/IWORKGroupElement.h \
	contexts/IWORKHeaderFooterContext.cpp \
	contexts/IWORKHeaderFooterContext.h \
	contexts/IWORKImageContext.cpp \
	contexts/IWORKImageContext.h \
	contexts/IWORKImageElement.cpp \
	contexts/IWORKImageElement.h \
	contexts/IWORKLayoutElement.cpp \
	contexts/IWORKLayoutElement.h \
	contexts/IWORKLineElement.cpp \
	contexts/IWORKLineElement.h \
	contexts/IWORKLinkElement.cpp \
	contexts/IWORKLinkElement.h \
	contexts/IWORKMediaElement.cpp \
	contexts/IWORKMediaElement.h \
	contexts/IWORKMetadataElement.cpp \
	contexts/IWORKMetadataElement.h \
	contexts/IWORKNumberConverter.cpp \
	contexts/IWORKNumberConverter.h \
	contexts/IWORKNumberElement.h \
	contexts/IWORKNumericPropertyContext.h \
	contexts/IWORKPElement.cpp \
	contexts/IWORKPElement.h \
	contexts/IWORKPathElement.cpp \
	contexts/IWORKPathElement.h \
	contexts/IWORKPositionElement.cpp \
	contexts/IWORKPositionElement.h \
	contexts/IWORKPropertyContext.h \
	contexts/IWORKPropertyContextBase.cpp \
	contexts/IWORKPropertyContextBase.h \
	contexts/IWORKPropertyMapElement.cpp \
	contexts/IWORKPropertyMapElement.h \
	contexts/IWORKPtrPropertyContext.h \
	contexts/IWORKRefContext.cpp \
	contexts/IWORKRefContext.h \
	contexts/IWORKShapeContext.cpp \
	contexts/IWORKShapeContext.h \
	contexts/IWORKSizeElement.cpp \
	contexts/IWORKSizeElement.h \
	contexts/IWORKSpanElement.cpp \
	contexts/IWORKSpanElement.h \
	contexts/IWORKStringElement.cpp \
	contexts/IWORKStringElement.h \
	contexts/IWORKStyleContext.cpp \
	contexts/IWORKStyleContext.h \
	contexts/IWORKStyleRefContext.cpp \
	contexts/IWORKStyleRefContext.h \
	contexts/IWORKStylesContext.cpp \
	contexts/IWORKStylesContext.h \
	contexts/IWORKStylesheetBase.cpp \
	contexts/IWORKStylesheetBase.h \
	contexts/IWORKTabElement.cpp \
	contexts/IWORKTabElement.h \
	contexts/IWORKTabsElement.cpp \
	contexts/IWORKTabsElement.h \
	contexts/IWORKTabularInfoElement.cpp \
	contexts/IWORKTabularInfoElement.h \
	contexts/IWORKTextElement.cpp \
	contexts/IWORKTextElement.h \
	contexts/IWORKTextBodyElement.cpp \
	contexts/IWORKTextBodyElement.h \
	contexts/IWORKTextStorageElement.cpp \
	contexts/IWORKTextStorageElement.h \
	contexts/IWORKUnfilteredElement.cpp \
	contexts/IWORKUnfilteredElement.h \
	contexts/IWORKValueContext.h \
	contexts/KEY2StyleContext.cpp \
	contexts/KEY2StyleContext.h \
	contexts/PAG1FootnotesElement.cpp \
	contexts/PAG1FootnotesElement.h \
	contexts/PAG1StyleContext.cpp \
	contexts/PAG1StyleContext.h \
	contexts/PAG1TextStorageElement.cpp \
	contexts/PAG1TextStorageElement.h \
	libetonyek_utils.cpp \
	libetonyek_utils.h \
	libetonyek_xml.cpp \
	libetonyek_xml.h

if OS_WIN32

@LIBETONYEK_WIN32_RESOURCE@ : libetonyek.rc $(libetonyek_@ETONYEK_MAJOR_VERSION@_@ETONYEK_MINOR_VERSION@_la_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource libetonyek.rc @LIBETONYEK_WIN32_RESOURCE@

endif

EXTRA_DIST = \
	IWORKToken.gperf \
	KEY1Token.gperf \
	KEY2Token.gperf \
	NUM1Token.gperf \
	PAG1Token.gperf \
	libetonyek.rc.in \
	libetonyek.rc \
	$(BUILT_SOURCES)

MOSTLYCLEANFILES = $(BUILT_SOURCES)
DISTCLEANFILES = libetonyek.rc

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
