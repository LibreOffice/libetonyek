/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/lexical_cast.hpp>

#include "libkeynote_xml.h"
#include "KEY2StyleParser.h"
#include "KEY2Token.h"
#include "KEYCollector.h"
#include "KEYDefaults.h"
#include "KEYStyles.h"
#include "KEYTypes.h"
#include "KEYXMLReader.h"

using boost::any;
using boost::lexical_cast;
using boost::optional;

using std::string;

namespace libkeynote
{

KEY2StyleParser::KEY2StyleParser(const int nameId, const int nsId, KEYCollector *const collector, const KEYDefaults &defaults, const bool nested)
  : KEY2ParserUtils()
  , m_nameId(nameId)
  , m_nsId(nsId)
  , m_nested(nested)
  , m_collector(collector)
  , m_defaults(defaults)
  , m_props()
{
}

void KEY2StyleParser::parse(const KEYXMLReader &reader)
{
  optional<ID_t> id;
  optional<string> ident;
  optional<string> parentIdent;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KEY2Token::NS_URI_SFA == getNamespaceId(attr)) && (KEY2Token::ID == getNameId(attr)))
    {
      id = attr.getValue();
    }
    else if (KEY2Token::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KEY2Token::ident :
        ident = attr.getValue();
        break;
      case KEY2Token::parent_ident :
        parentIdent = attr.getValue();
        break;

      case KEY2Token::name :
      case KEY2Token::cell_style_default_line_height :
      case KEY2Token::locked :
      case KEY2Token::seriesIndex :
      case KEY2Token::sfclass :
      case KEY2Token::tabular_style_name_internal :
        KEY_DEBUG_XML_TODO("attribute", attr.getName(), attr.getNamespace());
        break;
      default :
        KEY_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
        break;
      }
    }
    else
    {
      KEY_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
    }
  }

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KEY2Token::NS_URI_SF == getNamespaceId(element)) && (KEY2Token::property_map == getNameId(element)))
    {
      parsePropertyMap(reader);

      if (KEY2Token::NS_URI_SF == m_nsId)
      {
        switch (m_nameId)
        {
        case KEY2Token::cell_style :
          m_collector->collectCellStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::characterstyle :
          m_collector->collectCharacterStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::connection_style :
          m_collector->collectConnectionStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::graphic_style :
          m_collector->collectGraphicStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::layoutstyle :
          m_collector->collectLayoutStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::liststyle :
          m_collector->collectListStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::paragraphstyle :
          m_collector->collectParagraphStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::placeholder_style :
          m_collector->collectPlaceholderStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::slide_style :
          m_collector->collectSlideStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::tabular_style :
          m_collector->collectTabularStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::vector_style :
          m_collector->collectVectorStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        default :
          KEY_DEBUG_MSG(("unhandled style %d:%d\n", m_nsId, m_nameId));
          break;
        }
      }
    }
    else
    {
      KEY_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }
}

void KEY2StyleParser::parseProperty(const KEYXMLReader &reader, const char *const key)
{
  checkNoAttributes(reader);

  // Parse a property's value, ignoring any extra tags around (there
  // should be none, but can we on that?) Note that the property can be
  // empty.
  KEYXMLReader::ElementIterator element(reader);
  bool done = false;
  while (element.next())
  {
    if (done)
      skipElement(element);
    else
      done = parsePropertyImpl(element, key);
  }
}

bool KEY2StyleParser::parsePropertyImpl(const KEYXMLReader &reader, const char *const key)
{
  bool parsed = true;
  any prop;

  const int nsToken = getNamespaceId(reader);

  if (KEY2Token::NS_URI_SF == nsToken)
  {
    const int nameToken = getNameId(reader);

    switch (nameToken)
    {
      // nested styles
    case KEY2Token::layoutstyle :
    case KEY2Token::liststyle :
    case KEY2Token::paragraphstyle :
    case KEY2Token::vector_style :
    {
      KEY2StyleParser parser(nameToken, nsToken, m_collector, m_defaults, true);
      parser.parse(reader);
      // TODO: need to get the style
      break;
    }
    case KEY2Token::layoutstyle_ref :
    case KEY2Token::liststyle_ref :
    case KEY2Token::paragraphstyle_ref :
    case KEY2Token::vector_style_ref :
    {
      const optional<string> dummyIdent;
      const optional<KEYPropertyMap> dummyProps;
      const optional<ID_t> id = readRef(reader);

      // TODO: need to get the style
      switch (nameToken)
      {
      case KEY2Token::layoutstyle_ref :
        m_collector->collectLayoutStyle(id, dummyProps, dummyIdent, dummyIdent, true, true);
        break;
      case KEY2Token::liststyle_ref :
        m_collector->collectListStyle(id, dummyProps, dummyIdent, dummyIdent, true, true);
        break;
      case KEY2Token::paragraphstyle_ref :
        m_collector->collectParagraphStyle(id, dummyProps, dummyIdent, dummyIdent, true, true);
        break;
      case KEY2Token::vector_style_ref :
        m_collector->collectVectorStyle(id, dummyProps, dummyIdent, dummyIdent, true, true);
        break;
      default :
        assert(0);
        break;
      }

      break;
    }

    // "normal" properties
    case KEY2Token::geometry :
    {
      const KEYGeometryPtr_t geometry = readGeometry(reader);
      if (geometry)
        prop = geometry;
      break;
    }

    default :
      parsed = false;
      skipElement(reader);
      break;
    }
  }
  else
  {
    parsed = false;
    skipElement(reader);
  }

  if (parsed && key && !prop.empty())
    m_props.set(key, prop);

  return parsed;
}

void KEY2StyleParser::parsePropertyMap(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::property_map, KEY2Token::NS_URI_SF));

  checkNoAttributes(reader);

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::SFTCellStylePropertyLayoutStyle :
      case KEY2Token::SFTCellStylePropertyParagraphStyle :
      case KEY2Token::SFTableStylePropertyBorderVectorStyle :
      case KEY2Token::SFTableStylePropertyCellLayoutStyle :
      case KEY2Token::SFTableStylePropertyCellParagraphStyle :
      case KEY2Token::SFTableStylePropertyCellStyle :
      case KEY2Token::SFTableStylePropertyHeaderBorderVectorStyle :
      case KEY2Token::SFTableStylePropertyHeaderColumnCellLayoutStyle :
      case KEY2Token::SFTableStylePropertyHeaderColumnCellParagraphStyle :
      case KEY2Token::SFTableStylePropertyHeaderColumnCellStyle :
      case KEY2Token::SFTableStylePropertyHeaderRowCellLayoutStyle :
      case KEY2Token::SFTableStylePropertyHeaderRowCellParagraphStyle :
      case KEY2Token::SFTableStylePropertyHeaderRowCellStyle :
      case KEY2Token::SFTableStylePropertyHeaderSeperatorVectorStyle :
      case KEY2Token::SFTableStylePropertyHeaderVectorStyle :
      case KEY2Token::SFTableStylePropertyVectorStyle :
      case KEY2Token::TableCellStylePropertyFormatNegativeStyle :
      case KEY2Token::bulletListStyle :
      case KEY2Token::followingLayoutStyle :
      case KEY2Token::followingParagraphStyle :
      case KEY2Token::headlineParagraphStyle :
      case KEY2Token::layoutParagraphStyle :
      case KEY2Token::layoutStyle :
      case KEY2Token::listStyle :
      case KEY2Token::tocStyle :
        parseProperty(element);
        break;
      case KEY2Token::geometry :
        parseProperty(element, "geometry");
        break;

      case KEY2Token::BGBuildDurationProperty :
      case KEY2Token::SFC2DAntialiasingModeProperty :
      case KEY2Token::SFC2DAreaDataPointFillProperty :
      case KEY2Token::SFC2DAreaDataPointStrokeProperty :
      case KEY2Token::SFC2DAreaDataPointSymbolProperty :
      case KEY2Token::SFC2DAreaFillProperty :
      case KEY2Token::SFC2DAreaShadowProperty :
      case KEY2Token::SFC2DAreaStrokeProperty :
      case KEY2Token::SFC2DAreaUseSeriesFillForDataPointFillProperty :
      case KEY2Token::SFC2DAreaUseStrokeColorForDataPointFillProperty :
      case KEY2Token::SFC2DBarFillProperty :
      case KEY2Token::SFC2DBarShadowProperty :
      case KEY2Token::SFC2DBarStrokeProperty :
      case KEY2Token::SFC2DBottomBorderOpacityProperty :
      case KEY2Token::SFC2DBottomBorderShadowProperty :
      case KEY2Token::SFC2DBottomBorderStrokeProperty :
      case KEY2Token::SFC2DCategoryDirectionGridLineOpacityProperty :
      case KEY2Token::SFC2DCategoryDirectionGridLineShadowProperty :
      case KEY2Token::SFC2DCategoryDirectionGridLineStrokeProperty :
      case KEY2Token::SFC2DCategoryDirectionMinorGridLineIntervalProperty :
      case KEY2Token::SFC2DCategoryDirectionMinorGridLineOpacityProperty :
      case KEY2Token::SFC2DCategoryDirectionMinorGridLineShadowProperty :
      case KEY2Token::SFC2DCategoryDirectionMinorGridLineStrokeProperty :
      case KEY2Token::SFC2DChartBackgroundFillProperty :
      case KEY2Token::SFC2DChartBackgroundOpacityProperty :
      case KEY2Token::SFC2DColumnFillProperty :
      case KEY2Token::SFC2DCombineLayersProperty :
      case KEY2Token::SFC2DLeftBorderOpacityProperty :
      case KEY2Token::SFC2DLeftBorderShadowProperty :
      case KEY2Token::SFC2DLeftBorderStrokeProperty :
      case KEY2Token::SFC2DLineConnectedPointsProperty :
      case KEY2Token::SFC2DLineDataPointFillProperty :
      case KEY2Token::SFC2DLineDataPointStrokeProperty :
      case KEY2Token::SFC2DLineDataPointSymbolProperty :
      case KEY2Token::SFC2DLineShadowProperty :
      case KEY2Token::SFC2DLineStrokeProperty :
      case KEY2Token::SFC2DLineUseSeriesFillForDataPointFillProperty :
      case KEY2Token::SFC2DLineUseStrokeColorForDataPointFillProperty :
      case KEY2Token::SFC2DMixedAreaDataPointFillProperty :
      case KEY2Token::SFC2DMixedAreaDataPointStrokeProperty :
      case KEY2Token::SFC2DMixedAreaDataPointSymbolProperty :
      case KEY2Token::SFC2DMixedAreaFillProperty :
      case KEY2Token::SFC2DMixedAreaShadowProperty :
      case KEY2Token::SFC2DMixedAreaStrokeProperty :
      case KEY2Token::SFC2DMixedAreaUseSeriesFillForDataPointFillProperty :
      case KEY2Token::SFC2DMixedAreaUseStrokeColorForDataPointFillProperty :
      case KEY2Token::SFC2DMixedColumnFillProperty :
      case KEY2Token::SFC2DMixedColumnShadowProperty :
      case KEY2Token::SFC2DMixedColumnStrokeProperty :
      case KEY2Token::SFC2DMixedConnectedPointsProperty :
      case KEY2Token::SFC2DMixedLineDataPointFillProperty :
      case KEY2Token::SFC2DMixedLineDataPointStrokeProperty :
      case KEY2Token::SFC2DMixedLineDataPointSymbolProperty :
      case KEY2Token::SFC2DMixedLineShadowProperty :
      case KEY2Token::SFC2DMixedLineStrokeProperty :
      case KEY2Token::SFC2DMixedLineUseStrokeColorForDataPointFillProperty :
      case KEY2Token::SFC2DOpacityProperty :
      case KEY2Token::SFC2DPieFillProperty :
      case KEY2Token::SFC2DPieShadowProperty :
      case KEY2Token::SFC2DPieStrokeProperty :
      case KEY2Token::SFC2DRightBorderOpacityProperty :
      case KEY2Token::SFC2DRightBorderShadowProperty :
      case KEY2Token::SFC2DRightBorderStrokeProperty :
      case KEY2Token::SFC2DScatterDataPointFillProperty :
      case KEY2Token::SFC2DScatterDataPointStrokeProperty :
      case KEY2Token::SFC2DScatterDataPointSymbolProperty :
      case KEY2Token::SFC2DScatterDataPointSymbolSizeProperty :
      case KEY2Token::SFC2DScatterShadowProperty :
      case KEY2Token::SFC2DScatterStrokeProperty :
      case KEY2Token::SFC2DShowBottomBorderProperty :
      case KEY2Token::SFC2DShowBottomTicksProperty :
      case KEY2Token::SFC2DShowCategoryDirectionGridLinesProperty :
      case KEY2Token::SFC2DShowCategoryDirectionMinorGridLinesProperty :
      case KEY2Token::SFC2DShowLeftBorderProperty :
      case KEY2Token::SFC2DShowLeftTicksProperty :
      case KEY2Token::SFC2DShowRightBorderProperty :
      case KEY2Token::SFC2DShowRightTicksProperty :
      case KEY2Token::SFC2DShowTopBorderProperty :
      case KEY2Token::SFC2DShowTopTicksProperty :
      case KEY2Token::SFC2DShowValueDirectionGridLinesProperty :
      case KEY2Token::SFC2DShowValueDirectionMinorGridLinesProperty :
      case KEY2Token::SFC2DTopBorderOpacityProperty :
      case KEY2Token::SFC2DTopBorderShadowProperty :
      case KEY2Token::SFC2DTopBorderStrokeProperty :
      case KEY2Token::SFC2DValueDirectionGridLineOpacityProperty :
      case KEY2Token::SFC2DValueDirectionGridLineShadowProperty :
      case KEY2Token::SFC2DValueDirectionGridLineStrokeProperty :
      case KEY2Token::SFC2DValueDirectionMinorGridLineIntervalProperty :
      case KEY2Token::SFC2DValueDirectionMinorGridLineOpacityProperty :
      case KEY2Token::SFC2DValueDirectionMinorGridLineShadowProperty :
      case KEY2Token::SFC2DValueDirectionMinorGridLineStrokeProperty :
      case KEY2Token::SFC3DAreaChartScaleProperty :
      case KEY2Token::SFC3DAreaFillProperty :
      case KEY2Token::SFC3DAreaLightingPackageProperty :
      case KEY2Token::SFC3DAreaShadowProperty :
      case KEY2Token::SFC3DBarChartScaleProperty :
      case KEY2Token::SFC3DBarFillProperty :
      case KEY2Token::SFC3DBarLightingPackageProperty :
      case KEY2Token::SFC3DBarShadowProperty :
      case KEY2Token::SFC3DBarShapeProperty :
      case KEY2Token::SFC3DBevelEdgesProperty :
      case KEY2Token::SFC3DCameraTypeProperty :
      case KEY2Token::SFC3DCategoryDirectionGridLineOpacityProperty :
      case KEY2Token::SFC3DCategoryDirectionGridLineStrokeProperty :
      case KEY2Token::SFC3DChartRotationProperty :
      case KEY2Token::SFC3DChartScaleProperty :
      case KEY2Token::SFC3DColumnBevelEdgesProperty :
      case KEY2Token::SFC3DColumnChartScaleProperty :
      case KEY2Token::SFC3DColumnFillProperty :
      case KEY2Token::SFC3DColumnLightingPackageProperty :
      case KEY2Token::SFC3DFloorDepthProperty :
      case KEY2Token::SFC3DFloorEmissiveColorProperty :
      case KEY2Token::SFC3DFloorFillProperty :
      case KEY2Token::SFC3DFloorSpecularColorProperty :
      case KEY2Token::SFC3DInterSetDepthGapProperty :
      case KEY2Token::SFC3DLabelThicknessProperty :
      case KEY2Token::SFC3DLightingPackageProperty :
      case KEY2Token::SFC3DLineChartScaleProperty :
      case KEY2Token::SFC3DLineFillProperty :
      case KEY2Token::SFC3DLineLightingPackageProperty :
      case KEY2Token::SFC3DLineShadowProperty :
      case KEY2Token::SFC3DMaterialPackageProperty :
      case KEY2Token::SFC3DPieBevelEdgesProperty :
      case KEY2Token::SFC3DPieChartScaleProperty :
      case KEY2Token::SFC3DPieFillProperty :
      case KEY2Token::SFC3DPieLightingPackageProperty :
      case KEY2Token::SFC3DPieShadowProperty :
      case KEY2Token::SFC3DReflectionsLevelProperty :
      case KEY2Token::SFC3DSceneBackgroundColorProperty :
      case KEY2Token::SFC3DSceneShowTextureProperty :
      case KEY2Token::SFC3DSceneTextureTilingProperty :
      case KEY2Token::SFC3DSeriesEmissiveColorProperty :
      case KEY2Token::SFC3DSeriesShininessProperty :
      case KEY2Token::SFC3DSeriesSpecularColorProperty :
      case KEY2Token::SFC3DShadowCameraXProperty :
      case KEY2Token::SFC3DShadowCameraYProperty :
      case KEY2Token::SFC3DShadowProperty :
      case KEY2Token::SFC3DShowBackWallProperty :
      case KEY2Token::SFC3DShowFloorProperty :
      case KEY2Token::SFC3DShowReflectionsProperty :
      case KEY2Token::SFC3DTextureProperty :
      case KEY2Token::SFC3DTextureTilingProperty :
      case KEY2Token::SFC3DValueDirectionGridLineOpacityProperty :
      case KEY2Token::SFC3DValueDirectionGridLineStrokeProperty :
      case KEY2Token::SFC3DWallEmissiveColorProperty :
      case KEY2Token::SFC3DWallFillProperty :
      case KEY2Token::SFC3DWallSpecularColorProperty :
      case KEY2Token::SFC3DWallsShininessProperty :
      case KEY2Token::SFCAreaSeriesValueParagraphStyleProperty :
      case KEY2Token::SFCAreaShowValueLabelProperty :
      case KEY2Token::SFCAreaValueLabelPositionProperty :
      case KEY2Token::SFCAxisLabelsOrientationProperty :
      case KEY2Token::SFCBarSeriesValueParagraphStyleProperty :
      case KEY2Token::SFCBarShowValueLabelProperty :
      case KEY2Token::SFCBarValueLabelPositionProperty :
      case KEY2Token::SFCCategoryAxisLabelsDepthProperty :
      case KEY2Token::SFCCategoryAxisLabelsOpacityProperty :
      case KEY2Token::SFCCategoryAxisLabelsOrientationProperty :
      case KEY2Token::SFCCategoryAxisLabelsPositionProperty :
      case KEY2Token::SFCCategoryAxisMajorTickLengthProperty :
      case KEY2Token::SFCCategoryAxisMajorTickStrokeProperty :
      case KEY2Token::SFCCategoryAxisMinorTickLengthProperty :
      case KEY2Token::SFCCategoryAxisMinorTickStrokeProperty :
      case KEY2Token::SFCCategoryAxisParagraphStyleProperty :
      case KEY2Token::SFCCategoryAxisShowMajorTickMarksProperty :
      case KEY2Token::SFCCategoryAxisShowMinorTickMarksProperty :
      case KEY2Token::SFCCategoryAxisTickMarkLocationProperty :
      case KEY2Token::SFCCategoryAxisTitleParagraphStyleProperty :
      case KEY2Token::SFCCategoryAxisTitlePositionProperty :
      case KEY2Token::SFCCategoryHorizontalSpacingProperty :
      case KEY2Token::SFCCategoryVerticalSpacingProperty :
      case KEY2Token::SFCChartTitleParagraphStyleProperty :
      case KEY2Token::SFCChartTitlePositionProperty :
      case KEY2Token::SFCDefaultChartAngleProperty :
      case KEY2Token::SFCDefaultChartBoundsProperty :
      case KEY2Token::SFCDefaultLegendAngleProperty :
      case KEY2Token::SFCDefaultLegendBoundsProperty :
      case KEY2Token::SFCDepthProperty :
      case KEY2Token::SFCEmissiveColorProperty :
      case KEY2Token::SFCErrorBarXAxisStrokeProperty :
      case KEY2Token::SFCErrorBarXLineEndProperty :
      case KEY2Token::SFCErrorBarYAxisShadowProperty :
      case KEY2Token::SFCErrorBarYAxisStrokeProperty :
      case KEY2Token::SFCErrorBarYLineEndProperty :
      case KEY2Token::SFCHorizontalAxisLogarithmicProperty :
      case KEY2Token::SFCHorizontalAxisNumberFormatSeparatorProperty :
      case KEY2Token::SFCHorizontalAxisNumberOfDecadesProperty :
      case KEY2Token::SFCHorizontalAxisNumberOfStepsProperty :
      case KEY2Token::SFCHorizontalAxisShowMinimumValueProperty :
      case KEY2Token::SFCInitialNumberOfSeriesProperty :
      case KEY2Token::SFCInterBarGapProperty :
      case KEY2Token::SFCInterSetGapProperty :
      case KEY2Token::SFCLabelOpacityProperty :
      case KEY2Token::SFCLabelShadowProperty :
      case KEY2Token::SFCLegendFillProperty :
      case KEY2Token::SFCLegendOpacityProperty :
      case KEY2Token::SFCLegendParagraphStyleProperty :
      case KEY2Token::SFCLegendPositionProperty :
      case KEY2Token::SFCLegendShadowProperty :
      case KEY2Token::SFCLegendStrokeProperty :
      case KEY2Token::SFCLineSeriesValueParagraphStyleProperty :
      case KEY2Token::SFCLineShowValueLabelProperty :
      case KEY2Token::SFCLineValueLabelPositionProperty :
      case KEY2Token::SFCMixedAreaValueLabelPositionProperty :
      case KEY2Token::SFCMixedColumnValueLabelPositionProperty :
      case KEY2Token::SFCMixedLineValueLabelPositionProperty :
      case KEY2Token::SFCMixedSeriesValueParagraphStyleProperty :
      case KEY2Token::SFCMixedShowValueLabelProperty :
      case KEY2Token::SFCNumberOfDefinedSeriesStylesProperty :
      case KEY2Token::SFCPieSeriesStartAngleProperty :
      case KEY2Token::SFCPieSeriesValueParagraphStyleProperty :
      case KEY2Token::SFCPieShowSeriesNameLabelProperty :
      case KEY2Token::SFCPieShowValueLabelProperty :
      case KEY2Token::SFCPieValueLabelPosition2Property :
      case KEY2Token::SFCPieValueLabelPositionProperty :
      case KEY2Token::SFCPieWedgeExplosionProperty :
      case KEY2Token::SFCPieWedgeGroupingsProperty :
      case KEY2Token::SFCSeriesValueLabelDateTimeFormatProperty :
      case KEY2Token::SFCSeriesValueLabelDurationFormatProperty :
      case KEY2Token::SFCSeriesValueLabelFormatTypeProperty :
      case KEY2Token::SFCSeriesValueLabelNumberFormatProperty :
      case KEY2Token::SFCSeriesValueLabelPercentageAxisNumberFormatProperty :
      case KEY2Token::SFCSeriesValueLabelPieFormatTypeProperty :
      case KEY2Token::SFCSeriesValueLabelPieNumberFormatProperty :
      case KEY2Token::SFCShininessProperty :
      case KEY2Token::SFCShowCategoryAxisLabelsProperty :
      case KEY2Token::SFCShowCategoryAxisSeriesLabelsProperty :
      case KEY2Token::SFCShowCategoryAxisTitleProperty :
      case KEY2Token::SFCShowChartTitleProperty :
      case KEY2Token::SFCShowLastCategoryAxisLabelsProperty :
      case KEY2Token::SFCShowPieLabelAsPercentageProperty :
      case KEY2Token::SFCShowValueAxisTitleProperty :
      case KEY2Token::SFCShowValueY2AxisTitleProperty :
      case KEY2Token::SFCSpecularColorProperty :
      case KEY2Token::SFCStackedAreaValueLabelPositionProperty :
      case KEY2Token::SFCStackedBarValueLabelPositionProperty :
      case KEY2Token::SFCTrendLineShadowProperty :
      case KEY2Token::SFCTrendLineStrokeProperty :
      case KEY2Token::SFCTrendlineEquationOpacityProperty :
      case KEY2Token::SFCTrendlineEquationParagraphStyleProperty :
      case KEY2Token::SFCTrendlineRSquaredOpacityProperty :
      case KEY2Token::SFCTrendlineRSquaredParagraphStyleProperty :
      case KEY2Token::SFCValueAxisDateTimeFormatProperty :
      case KEY2Token::SFCValueAxisDurationFormatProperty :
      case KEY2Token::SFCValueAxisFormatTypeProperty :
      case KEY2Token::SFCValueAxisHorizontalDateTimeFormatProperty :
      case KEY2Token::SFCValueAxisHorizontalDurationFormatProperty :
      case KEY2Token::SFCValueAxisHorizontalFormatTypeProperty :
      case KEY2Token::SFCValueAxisHorizontalNumberFormatProperty :
      case KEY2Token::SFCValueAxisLabelsDepthProperty :
      case KEY2Token::SFCValueAxisLabelsOpacityProperty :
      case KEY2Token::SFCValueAxisLabelsOrientationProperty :
      case KEY2Token::SFCValueAxisLabelsPositionProperty :
      case KEY2Token::SFCValueAxisLogarithmicProperty :
      case KEY2Token::SFCValueAxisMajorTickLengthProperty :
      case KEY2Token::SFCValueAxisMajorTickStrokeProperty :
      case KEY2Token::SFCValueAxisMaximumValueProperty :
      case KEY2Token::SFCValueAxisMinimumValueProperty :
      case KEY2Token::SFCValueAxisMinorTickLengthProperty :
      case KEY2Token::SFCValueAxisMinorTickStrokeProperty :
      case KEY2Token::SFCValueAxisNumberFormatProperty :
      case KEY2Token::SFCValueAxisNumberOfDecadesProperty :
      case KEY2Token::SFCValueAxisNumberOfStepsProperty :
      case KEY2Token::SFCValueAxisParagraphStyleProperty :
      case KEY2Token::SFCValueAxisPercentageProperty :
      case KEY2Token::SFCValueAxisShowMajorTickMarksProperty :
      case KEY2Token::SFCValueAxisShowMinimumValueProperty :
      case KEY2Token::SFCValueAxisShowMinorTickMarksProperty :
      case KEY2Token::SFCValueAxisTickMarkLocationProperty :
      case KEY2Token::SFCValueAxisTitleParagraphStyleProperty :
      case KEY2Token::SFCValueAxisTitlePositionPropertya :
      case KEY2Token::SFCValueAxisY2DateTimeFormatProperty :
      case KEY2Token::SFCValueAxisY2DurationFormatProperty :
      case KEY2Token::SFCValueAxisY2FormatTypeProperty :
      case KEY2Token::SFCValueAxisY2NumberFormatProperty :
      case KEY2Token::SFCValueAxisY2ParagraphStyleProperty :
      case KEY2Token::SFCValueAxisY2TitleParagraphStyleProperty :
      case KEY2Token::SFCValueNumberFormatDecimalPlacesProperty :
      case KEY2Token::SFCValueNumberFormatPrefixProperty :
      case KEY2Token::SFCValueNumberFormatSeparatorProperty :
      case KEY2Token::SFCValueNumberFormatSuffixProperty :
      case KEY2Token::SFCValueY2AxisLabelsDepthProperty :
      case KEY2Token::SFCValueY2AxisLabelsOpacityProperty :
      case KEY2Token::SFCValueY2AxisLabelsOrientationProperty :
      case KEY2Token::SFCValueY2AxisLabelsPositionProperty :
      case KEY2Token::SFCValueY2AxisLogarithmicProperty :
      case KEY2Token::SFCValueY2AxisMajorTickLengthProperty :
      case KEY2Token::SFCValueY2AxisMajorTickStrokeProperty :
      case KEY2Token::SFCValueY2AxisMinorTickLengthProperty :
      case KEY2Token::SFCValueY2AxisMinorTickStrokeProperty :
      case KEY2Token::SFCValueY2AxisNumberOfDecadesProperty :
      case KEY2Token::SFCValueY2AxisNumberOfStepsProperty :
      case KEY2Token::SFCValueY2AxisPercentageProperty :
      case KEY2Token::SFCValueY2AxisShowMajorTickMarksProperty :
      case KEY2Token::SFCValueY2AxisShowMinimumValueProperty :
      case KEY2Token::SFCValueY2AxisShowMinorTickMarksProperty :
      case KEY2Token::SFCValueY2AxisTickMarkLocationProperty :
      case KEY2Token::SFTAutoResizeProperty :
      case KEY2Token::SFTBackgroundProperty :
      case KEY2Token::SFTCellStylePropertyDateTimeFormat :
      case KEY2Token::SFTCellStylePropertyDurationFormat :
      case KEY2Token::SFTCellStylePropertyFormatType :
      case KEY2Token::SFTCellStylePropertyImplicitFormatType :
      case KEY2Token::SFTCellStylePropertyNumberFormat :
      case KEY2Token::SFTCellTextWrapProperty :
      case KEY2Token::SFTDefaultBodyCellStyleProperty :
      case KEY2Token::SFTDefaultBodyVectorStyleProperty :
      case KEY2Token::SFTDefaultBorderVectorStyleProperty :
      case KEY2Token::SFTDefaultColumnCountProperty :
      case KEY2Token::SFTDefaultFooterBodyVectorStyleProperty :
      case KEY2Token::SFTDefaultFooterBorderVectorStyleProperty :
      case KEY2Token::SFTDefaultFooterRowCellStyleProperty :
      case KEY2Token::SFTDefaultFooterRowCountProperty :
      case KEY2Token::SFTDefaultFooterSeparatorVectorStyleProperty :
      case KEY2Token::SFTDefaultGeometryProperty :
      case KEY2Token::SFTDefaultGroupingLevel0VectorStyleProperty :
      case KEY2Token::SFTDefaultGroupingLevel1VectorStyleProperty :
      case KEY2Token::SFTDefaultGroupingLevel2VectorStyleProperty :
      case KEY2Token::SFTDefaultGroupingLevel3VectorStyleProperty :
      case KEY2Token::SFTDefaultGroupingLevel4VectorStyleProperty :
      case KEY2Token::SFTDefaultGroupingRow0CellStyleProperty :
      case KEY2Token::SFTDefaultGroupingRow1CellStyleProperty :
      case KEY2Token::SFTDefaultGroupingRow2CellStyleProperty :
      case KEY2Token::SFTDefaultGroupingRow3CellStyleProperty :
      case KEY2Token::SFTDefaultGroupingRow4CellStyleProperty :
      case KEY2Token::SFTDefaultHeaderBodyVectorStyleProperty :
      case KEY2Token::SFTDefaultHeaderBorderVectorStyleProperty :
      case KEY2Token::SFTDefaultHeaderColumnCellStyleProperty :
      case KEY2Token::SFTDefaultHeaderColumnCountProperty :
      case KEY2Token::SFTDefaultHeaderRowCellStyleProperty :
      case KEY2Token::SFTDefaultHeaderRowCountProperty :
      case KEY2Token::SFTDefaultHeaderSeparatorVectorStyleProperty :
      case KEY2Token::SFTDefaultRowCountProperty :
      case KEY2Token::SFTDefaultTableNameIsDisplayedProperty :
      case KEY2Token::SFTGroupingRowFillProperty :
      case KEY2Token::SFTHeaderColumnRepeatsProperty :
      case KEY2Token::SFTHeaderRowRepeatsProperty :
      case KEY2Token::SFTStrokeProperty :
      case KEY2Token::SFTTableBandedCellFillProperty :
      case KEY2Token::SFTTableBandedRowsProperty :
      case KEY2Token::SFTTableBehaviorProperty :
      case KEY2Token::SFTableCellStylePropertyFill :
      case KEY2Token::SFTableCellStylePropertyType :
      case KEY2Token::SFTableStylePropertyBackgroundFill :
      case KEY2Token::SFTableStylePropertyPrototypeColumnCount :
      case KEY2Token::SFTableStylePropertyPrototypeGeometry :
      case KEY2Token::SFTableStylePropertyPrototypeIsHeaderColumn :
      case KEY2Token::SFTableStylePropertyPrototypeIsHeaderRow :
      case KEY2Token::SFTableStylePropertyPrototypeIsResize :
      case KEY2Token::SFTableStylePropertyPrototypeRowCount :
      case KEY2Token::Series_0 :
      case KEY2Token::Series_1 :
      case KEY2Token::Series_2 :
      case KEY2Token::Series_3 :
      case KEY2Token::Series_4 :
      case KEY2Token::Series_5 :
      case KEY2Token::TableCellStylePropertyFormatDecimals :
      case KEY2Token::TableCellStylePropertyFormatEnabled :
      case KEY2Token::TableCellStylePropertyFormatPrefix :
      case KEY2Token::TableCellStylePropertyFormatSuffix :
      case KEY2Token::TableCellStylePropertyFormatThousandsSeparator :
      case KEY2Token::TableVectorStyleStrokeProperty :
      case KEY2Token::TableVectorStyleTypeProperty :
      case KEY2Token::alignment :
      case KEY2Token::animationAutoPlay :
      case KEY2Token::animationDelay :
      case KEY2Token::animationDuration :
      case KEY2Token::atsuFontFeatures :
      case KEY2Token::baselineShift :
      case KEY2Token::blendMode :
      case KEY2Token::bodyPlaceholderVisibility :
      case KEY2Token::bold :
      case KEY2Token::capitalization :
      case KEY2Token::columns :
      case KEY2Token::decimalTab :
      case KEY2Token::defaultTabStops :
      case KEY2Token::dropCap :
      case KEY2Token::effect :
      case KEY2Token::effectColor :
      case KEY2Token::externalTextWrap :
      case KEY2Token::fill :
      case KEY2Token::filters :
      case KEY2Token::firstLineIndent :
      case KEY2Token::firstTopicNumber :
      case KEY2Token::fontColor :
      case KEY2Token::fontName :
      case KEY2Token::fontSize :
      case KEY2Token::headLineEnd :
      case KEY2Token::headOffset :
      case KEY2Token::headlineIndent :
      case KEY2Token::hidden :
      case KEY2Token::hyphenate :
      case KEY2Token::italic :
      case KEY2Token::keepLinesTogether :
      case KEY2Token::keepWithNext :
      case KEY2Token::kerning :
      case KEY2Token::labelCharacterStyle1 :
      case KEY2Token::labelCharacterStyle2 :
      case KEY2Token::labelCharacterStyle3 :
      case KEY2Token::labelCharacterStyle4 :
      case KEY2Token::labelCharacterStyle5 :
      case KEY2Token::labelCharacterStyle6 :
      case KEY2Token::labelCharacterStyle7 :
      case KEY2Token::labelCharacterStyle8 :
      case KEY2Token::labelCharacterStyle9 :
      case KEY2Token::language :
      case KEY2Token::layoutContinuous :
      case KEY2Token::layoutMargins :
      case KEY2Token::leftIndent :
      case KEY2Token::ligatures :
      case KEY2Token::lineSpacing :
      case KEY2Token::listLabelGeometries:
      case KEY2Token::listLabelIndents :
      case KEY2Token::listLabelTypes :
      case KEY2Token::listLevels :
      case KEY2Token::listTextIndents :
      case KEY2Token::minimumHorizontalInset :
      case KEY2Token::objectPlaceholderVisibility :
      case KEY2Token::opacity :
      case KEY2Token::outline :
      case KEY2Token::outlineColor :
      case KEY2Token::outlineLevel :
      case KEY2Token::outlineStyleType :
      case KEY2Token::padding :
      case KEY2Token::pageBreakBefore :
      case KEY2Token::paragraphBorderType :
      case KEY2Token::paragraphBorders :
      case KEY2Token::paragraphFill :
      case KEY2Token::paragraphRuleOffset :
      case KEY2Token::paragraphStroke :
      case KEY2Token::reflection :
      case KEY2Token::rightIndent :
      case KEY2Token::shadow :
      case KEY2Token::showInTOC :
      case KEY2Token::shrinkTextToFit :
      case KEY2Token::slideNumberPlaceholderVisibility :
      case KEY2Token::spaceAfter :
      case KEY2Token::spaceBefore :
      case KEY2Token::strikethru :
      case KEY2Token::strikethruColor :
      case KEY2Token::strikethruWidth :
      case KEY2Token::stroke :
      case KEY2Token::superscript :
      case KEY2Token::tabs :
      case KEY2Token::tailLineEnd :
      case KEY2Token::tailOffset :
      case KEY2Token::textBackground :
      case KEY2Token::textBorders :
      case KEY2Token::textShadow :
      case KEY2Token::titlePlaceholderVisibility :
      case KEY2Token::tracking :
      case KEY2Token::transition :
      case KEY2Token::underline :
      case KEY2Token::underlineColor :
      case KEY2Token::underlineWidth :
      case KEY2Token::verticalAlignment :
      case KEY2Token::verticalText :
      case KEY2Token::widowControl :
      case KEY2Token::word_strikethrough :
      case KEY2Token::word_underline :
        KEY_DEBUG_XML_TODO_ELEMENT(element);
        skipElement(element);
        break;

      default :
        KEY_DEBUG_XML_UNKNOWN_ELEMENT(element);
        skipElement(element);
        break;
      }
    }
    else
    {
      KEY_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }
}

KEYGeometryPtr_t KEY2StyleParser::readGeometry(const KEYXMLReader &reader)
{
  optional<KEYSize> naturalSize;
  optional<KEYPosition> pos;
  optional<double> angle;
  optional<double> shearXAngle;
  optional<double> shearYAngle;
  optional<bool> aspectRatioLocked;
  optional<bool> sizesLocked;
  optional<bool> horizontalFlip;
  optional<bool> verticalFlip;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KEY2Token::angle :
        angle = lexical_cast<double>(attr.getValue());
        break;
      case KEY2Token::aspectRatioLocked :
        aspectRatioLocked = bool_cast(attr.getValue());
        break;
      case KEY2Token::horizontalFlip :
        horizontalFlip = bool_cast(attr.getValue());
        break;
      case KEY2Token::shearXAngle :
        shearXAngle = lexical_cast<double>(attr.getValue());
        break;
      case KEY2Token::shearYAngle :
        shearYAngle = lexical_cast<double>(attr.getValue());
        break;
      case KEY2Token::sizesLocked :
        sizesLocked = bool_cast(attr.getValue());
        break;
      case KEY2Token::verticalFlip :
        verticalFlip = bool_cast(attr.getValue());
        break;
      default :
        KEY_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
        break;
      }
    }
    else if (KEY2Token::NS_URI_SFA == getNamespaceId(attr) && (KEY2Token::ID == getNameId(attr)))
    {
      // ignore
    }
    else
    {
      KEY_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
    }
  }

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::naturalSize :
        naturalSize = readSize(reader);
        break;
      case KEY2Token::position :
        pos = readPosition(reader);
        break;
      case KEY2Token::size :
        // ignore
        skipElement(element);
        break;
      default :
        KEY_DEBUG_XML_UNKNOWN_ELEMENT(element);
        skipElement(element);
        break;
      }
    }
    else
    {
      KEY_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }

  m_defaults.applyGeometry(naturalSize, pos);
  assert(naturalSize && pos);

  const KEYGeometryPtr_t geometry(new KEYGeometry());
  geometry->naturalSize = get(naturalSize);
  geometry->position = get(pos);
  geometry->angle = angle;
  geometry->shearXAngle = shearXAngle;
  geometry->shearYAngle = shearYAngle;
  geometry->horizontalFlip = horizontalFlip;
  geometry->verticalFlip = verticalFlip;
  geometry->aspectRatioLocked = aspectRatioLocked;
  geometry->sizesLocked = sizesLocked;

  return geometry;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
