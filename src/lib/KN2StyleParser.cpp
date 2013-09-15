/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "libkeynote_xml.h"
#include "KN2StyleParser.h"
#include "KN2Token.h"
#include "KNCollector.h"
#include "KNXMLReader.h"

using boost::optional;

namespace libkeynote
{

KN2StyleParser::KN2StyleParser(const int nameId, const int nsId, KNCollector *const collector)
  : m_nameId(nameId)
  , m_nsId(nsId)
  , m_collector(collector)
  , m_style()
{
}

void KN2StyleParser::parse(const KNXMLReader &reader)
{
  optional<ID_t> id;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SFA == getNamespaceId(attr)) && (KN2Token::ID == getNameId(attr)))
    {
      KN_DEBUG_XML_TODO("attribute", attr.getName(), attr.getNamespace());
    }
    else if (KN2Token::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KN2Token::ID :
        id = attr.getValue();
        break;

      case KN2Token::ident :
      case KN2Token::parent_ident :
      case KN2Token::name :
      case KN2Token::cell_style_default_line_height :
      case KN2Token::locked :
      case KN2Token::seriesIndex :
      case KN2Token::sfclass :
      case KN2Token::tabular_style_name_internal :
        KN_DEBUG_XML_TODO("attribute", attr.getName(), attr.getNamespace());
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
    }
  }

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if ((KN2Token::NS_URI_SF == getKN2TokenID(ns)) && (KN2Token::property_map == getKN2TokenID(name)))
    {
      parsePropertyMap(reader);

      if (KN2Token::NS_URI_SF == m_nsId)
      {
        switch (m_nameId)
        {
        case KN2Token::characterstyle :
          m_collector->collectCharacterStyle(id, m_style);
          break;
        case KN2Token::graphic_style :
          m_collector->collectGraphicStyle(id, m_style);
          break;
        case KN2Token::headline_style :
          m_collector->collectHeadlineStyle(id, m_style);
          break;
        case KN2Token::layoutstyle :
          m_collector->collectLayoutStyle(id, m_style);
          break;
        case KN2Token::paragraphstyle :
          m_collector->collectParagraphStyle(id, m_style);
          break;
        default :
          KN_DEBUG_MSG(("unhandled style %d:%d\n", m_nsId, m_nameId));
          break;
        }
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }
}

void KN2StyleParser::parsePropertyMap(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::property_map, KN2Token::NS_URI_SF));

  checkNoAttributes(reader);

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::BGBuildDurationProperty :
      case KN2Token::SFC2DAntialiasingModeProperty :
      case KN2Token::SFC2DAreaDataPointFillProperty :
      case KN2Token::SFC2DAreaDataPointStrokeProperty :
      case KN2Token::SFC2DAreaDataPointSymbolProperty :
      case KN2Token::SFC2DAreaFillProperty :
      case KN2Token::SFC2DAreaShadowProperty :
      case KN2Token::SFC2DAreaStrokeProperty :
      case KN2Token::SFC2DAreaUseSeriesFillForDataPointFillProperty :
      case KN2Token::SFC2DAreaUseStrokeColorForDataPointFillProperty :
      case KN2Token::SFC2DBarFillProperty :
      case KN2Token::SFC2DBarShadowProperty :
      case KN2Token::SFC2DBarStrokeProperty :
      case KN2Token::SFC2DBottomBorderOpacityProperty :
      case KN2Token::SFC2DBottomBorderShadowProperty :
      case KN2Token::SFC2DBottomBorderStrokeProperty :
      case KN2Token::SFC2DCategoryDirectionGridLineOpacityProperty :
      case KN2Token::SFC2DCategoryDirectionGridLineShadowProperty :
      case KN2Token::SFC2DCategoryDirectionGridLineStrokeProperty :
      case KN2Token::SFC2DCategoryDirectionMinorGridLineIntervalProperty :
      case KN2Token::SFC2DCategoryDirectionMinorGridLineOpacityProperty :
      case KN2Token::SFC2DCategoryDirectionMinorGridLineShadowProperty :
      case KN2Token::SFC2DCategoryDirectionMinorGridLineStrokeProperty :
      case KN2Token::SFC2DChartBackgroundFillProperty :
      case KN2Token::SFC2DChartBackgroundOpacityProperty :
      case KN2Token::SFC2DColumnFillProperty :
      case KN2Token::SFC2DCombineLayersProperty :
      case KN2Token::SFC2DLeftBorderOpacityProperty :
      case KN2Token::SFC2DLeftBorderShadowProperty :
      case KN2Token::SFC2DLeftBorderStrokeProperty :
      case KN2Token::SFC2DLineConnectedPointsProperty :
      case KN2Token::SFC2DLineDataPointFillProperty :
      case KN2Token::SFC2DLineDataPointStrokeProperty :
      case KN2Token::SFC2DLineDataPointSymbolProperty :
      case KN2Token::SFC2DLineShadowProperty :
      case KN2Token::SFC2DLineStrokeProperty :
      case KN2Token::SFC2DLineUseSeriesFillForDataPointFillProperty :
      case KN2Token::SFC2DLineUseStrokeColorForDataPointFillProperty :
      case KN2Token::SFC2DMixedAreaDataPointFillProperty :
      case KN2Token::SFC2DMixedAreaDataPointStrokeProperty :
      case KN2Token::SFC2DMixedAreaDataPointSymbolProperty :
      case KN2Token::SFC2DMixedAreaFillProperty :
      case KN2Token::SFC2DMixedAreaShadowProperty :
      case KN2Token::SFC2DMixedAreaStrokeProperty :
      case KN2Token::SFC2DMixedAreaUseSeriesFillForDataPointFillProperty :
      case KN2Token::SFC2DMixedAreaUseStrokeColorForDataPointFillProperty :
      case KN2Token::SFC2DMixedColumnFillProperty :
      case KN2Token::SFC2DMixedColumnShadowProperty :
      case KN2Token::SFC2DMixedColumnStrokeProperty :
      case KN2Token::SFC2DMixedConnectedPointsProperty :
      case KN2Token::SFC2DMixedLineDataPointFillProperty :
      case KN2Token::SFC2DMixedLineDataPointStrokeProperty :
      case KN2Token::SFC2DMixedLineDataPointSymbolProperty :
      case KN2Token::SFC2DMixedLineShadowProperty :
      case KN2Token::SFC2DMixedLineStrokeProperty :
      case KN2Token::SFC2DMixedLineUseStrokeColorForDataPointFillProperty :
      case KN2Token::SFC2DOpacityProperty :
      case KN2Token::SFC2DPieFillProperty :
      case KN2Token::SFC2DPieShadowProperty :
      case KN2Token::SFC2DPieStrokeProperty :
      case KN2Token::SFC2DRightBorderOpacityProperty :
      case KN2Token::SFC2DRightBorderShadowProperty :
      case KN2Token::SFC2DRightBorderStrokeProperty :
      case KN2Token::SFC2DScatterDataPointFillProperty :
      case KN2Token::SFC2DScatterDataPointStrokeProperty :
      case KN2Token::SFC2DScatterDataPointSymbolProperty :
      case KN2Token::SFC2DScatterDataPointSymbolSizeProperty :
      case KN2Token::SFC2DScatterShadowProperty :
      case KN2Token::SFC2DScatterStrokeProperty :
      case KN2Token::SFC2DShowBottomBorderProperty :
      case KN2Token::SFC2DShowBottomTicksProperty :
      case KN2Token::SFC2DShowCategoryDirectionGridLinesProperty :
      case KN2Token::SFC2DShowCategoryDirectionMinorGridLinesProperty :
      case KN2Token::SFC2DShowLeftBorderProperty :
      case KN2Token::SFC2DShowLeftTicksProperty :
      case KN2Token::SFC2DShowRightBorderProperty :
      case KN2Token::SFC2DShowRightTicksProperty :
      case KN2Token::SFC2DShowTopBorderProperty :
      case KN2Token::SFC2DShowTopTicksProperty :
      case KN2Token::SFC2DShowValueDirectionGridLinesProperty :
      case KN2Token::SFC2DShowValueDirectionMinorGridLinesProperty :
      case KN2Token::SFC2DTopBorderOpacityProperty :
      case KN2Token::SFC2DTopBorderShadowProperty :
      case KN2Token::SFC2DTopBorderStrokeProperty :
      case KN2Token::SFC2DValueDirectionGridLineOpacityProperty :
      case KN2Token::SFC2DValueDirectionGridLineShadowProperty :
      case KN2Token::SFC2DValueDirectionGridLineStrokeProperty :
      case KN2Token::SFC2DValueDirectionMinorGridLineIntervalProperty :
      case KN2Token::SFC2DValueDirectionMinorGridLineOpacityProperty :
      case KN2Token::SFC2DValueDirectionMinorGridLineShadowProperty :
      case KN2Token::SFC2DValueDirectionMinorGridLineStrokeProperty :
      case KN2Token::SFC3DAreaChartScaleProperty :
      case KN2Token::SFC3DAreaFillProperty :
      case KN2Token::SFC3DAreaLightingPackageProperty :
      case KN2Token::SFC3DAreaShadowProperty :
      case KN2Token::SFC3DBarChartScaleProperty :
      case KN2Token::SFC3DBarFillProperty :
      case KN2Token::SFC3DBarLightingPackageProperty :
      case KN2Token::SFC3DBarShadowProperty :
      case KN2Token::SFC3DBarShapeProperty :
      case KN2Token::SFC3DBevelEdgesProperty :
      case KN2Token::SFC3DCameraTypeProperty :
      case KN2Token::SFC3DCategoryDirectionGridLineOpacityProperty :
      case KN2Token::SFC3DCategoryDirectionGridLineStrokeProperty :
      case KN2Token::SFC3DChartRotationProperty :
      case KN2Token::SFC3DChartScaleProperty :
      case KN2Token::SFC3DColumnBevelEdgesProperty :
      case KN2Token::SFC3DColumnChartScaleProperty :
      case KN2Token::SFC3DColumnFillProperty :
      case KN2Token::SFC3DColumnLightingPackageProperty :
      case KN2Token::SFC3DFloorDepthProperty :
      case KN2Token::SFC3DFloorEmissiveColorProperty :
      case KN2Token::SFC3DFloorFillProperty :
      case KN2Token::SFC3DFloorSpecularColorProperty :
      case KN2Token::SFC3DInterSetDepthGapProperty :
      case KN2Token::SFC3DLabelThicknessProperty :
      case KN2Token::SFC3DLightingPackageProperty :
      case KN2Token::SFC3DLineChartScaleProperty :
      case KN2Token::SFC3DLineFillProperty :
      case KN2Token::SFC3DLineLightingPackageProperty :
      case KN2Token::SFC3DLineShadowProperty :
      case KN2Token::SFC3DMaterialPackageProperty :
      case KN2Token::SFC3DPieBevelEdgesProperty :
      case KN2Token::SFC3DPieChartScaleProperty :
      case KN2Token::SFC3DPieFillProperty :
      case KN2Token::SFC3DPieLightingPackageProperty :
      case KN2Token::SFC3DPieShadowProperty :
      case KN2Token::SFC3DReflectionsLevelProperty :
      case KN2Token::SFC3DSceneBackgroundColorProperty :
      case KN2Token::SFC3DSceneShowTextureProperty :
      case KN2Token::SFC3DSceneTextureTilingProperty :
      case KN2Token::SFC3DSeriesEmissiveColorProperty :
      case KN2Token::SFC3DSeriesShininessProperty :
      case KN2Token::SFC3DSeriesSpecularColorProperty :
      case KN2Token::SFC3DShadowCameraXProperty :
      case KN2Token::SFC3DShadowCameraYProperty :
      case KN2Token::SFC3DShadowProperty :
      case KN2Token::SFC3DShowBackWallProperty :
      case KN2Token::SFC3DShowFloorProperty :
      case KN2Token::SFC3DShowReflectionsProperty :
      case KN2Token::SFC3DTextureProperty :
      case KN2Token::SFC3DTextureTilingProperty :
      case KN2Token::SFC3DValueDirectionGridLineOpacityProperty :
      case KN2Token::SFC3DValueDirectionGridLineStrokeProperty :
      case KN2Token::SFC3DWallEmissiveColorProperty :
      case KN2Token::SFC3DWallFillProperty :
      case KN2Token::SFC3DWallSpecularColorProperty :
      case KN2Token::SFC3DWallsShininessProperty :
      case KN2Token::SFCAreaSeriesValueParagraphStyleProperty :
      case KN2Token::SFCAreaShowValueLabelProperty :
      case KN2Token::SFCAreaValueLabelPositionProperty :
      case KN2Token::SFCAxisLabelsOrientationProperty :
      case KN2Token::SFCBarSeriesValueParagraphStyleProperty :
      case KN2Token::SFCBarShowValueLabelProperty :
      case KN2Token::SFCBarValueLabelPositionProperty :
      case KN2Token::SFCCategoryAxisLabelsDepthProperty :
      case KN2Token::SFCCategoryAxisLabelsOpacityProperty :
      case KN2Token::SFCCategoryAxisLabelsOrientationProperty :
      case KN2Token::SFCCategoryAxisLabelsPositionProperty :
      case KN2Token::SFCCategoryAxisMajorTickLengthProperty :
      case KN2Token::SFCCategoryAxisMajorTickStrokeProperty :
      case KN2Token::SFCCategoryAxisMinorTickLengthProperty :
      case KN2Token::SFCCategoryAxisMinorTickStrokeProperty :
      case KN2Token::SFCCategoryAxisParagraphStyleProperty :
      case KN2Token::SFCCategoryAxisShowMajorTickMarksProperty :
      case KN2Token::SFCCategoryAxisShowMinorTickMarksProperty :
      case KN2Token::SFCCategoryAxisTickMarkLocationProperty :
      case KN2Token::SFCCategoryAxisTitleParagraphStyleProperty :
      case KN2Token::SFCCategoryAxisTitlePositionProperty :
      case KN2Token::SFCCategoryHorizontalSpacingProperty :
      case KN2Token::SFCCategoryVerticalSpacingProperty :
      case KN2Token::SFCChartTitleParagraphStyleProperty :
      case KN2Token::SFCChartTitlePositionProperty :
      case KN2Token::SFCDefaultChartAngleProperty :
      case KN2Token::SFCDefaultChartBoundsProperty :
      case KN2Token::SFCDefaultLegendAngleProperty :
      case KN2Token::SFCDefaultLegendBoundsProperty :
      case KN2Token::SFCDepthProperty :
      case KN2Token::SFCEmissiveColorProperty :
      case KN2Token::SFCErrorBarXAxisStrokeProperty :
      case KN2Token::SFCErrorBarXLineEndProperty :
      case KN2Token::SFCErrorBarYAxisShadowProperty :
      case KN2Token::SFCErrorBarYAxisStrokeProperty :
      case KN2Token::SFCErrorBarYLineEndProperty :
      case KN2Token::SFCHorizontalAxisLogarithmicProperty :
      case KN2Token::SFCHorizontalAxisNumberFormatSeparatorProperty :
      case KN2Token::SFCHorizontalAxisNumberOfDecadesProperty :
      case KN2Token::SFCHorizontalAxisNumberOfStepsProperty :
      case KN2Token::SFCHorizontalAxisShowMinimumValueProperty :
      case KN2Token::SFCInitialNumberOfSeriesProperty :
      case KN2Token::SFCInterBarGapProperty :
      case KN2Token::SFCInterSetGapProperty :
      case KN2Token::SFCLabelOpacityProperty :
      case KN2Token::SFCLabelShadowProperty :
      case KN2Token::SFCLegendFillProperty :
      case KN2Token::SFCLegendOpacityProperty :
      case KN2Token::SFCLegendParagraphStyleProperty :
      case KN2Token::SFCLegendPositionProperty :
      case KN2Token::SFCLegendShadowProperty :
      case KN2Token::SFCLegendStrokeProperty :
      case KN2Token::SFCLineSeriesValueParagraphStyleProperty :
      case KN2Token::SFCLineShowValueLabelProperty :
      case KN2Token::SFCLineValueLabelPositionProperty :
      case KN2Token::SFCMixedAreaValueLabelPositionProperty :
      case KN2Token::SFCMixedColumnValueLabelPositionProperty :
      case KN2Token::SFCMixedLineValueLabelPositionProperty :
      case KN2Token::SFCMixedSeriesValueParagraphStyleProperty :
      case KN2Token::SFCMixedShowValueLabelProperty :
      case KN2Token::SFCNumberOfDefinedSeriesStylesProperty :
      case KN2Token::SFCPieSeriesStartAngleProperty :
      case KN2Token::SFCPieSeriesValueParagraphStyleProperty :
      case KN2Token::SFCPieShowSeriesNameLabelProperty :
      case KN2Token::SFCPieShowValueLabelProperty :
      case KN2Token::SFCPieValueLabelPosition2Property :
      case KN2Token::SFCPieValueLabelPositionProperty :
      case KN2Token::SFCPieWedgeExplosionProperty :
      case KN2Token::SFCPieWedgeGroupingsProperty :
      case KN2Token::SFCSeriesValueLabelDateTimeFormatProperty :
      case KN2Token::SFCSeriesValueLabelDurationFormatProperty :
      case KN2Token::SFCSeriesValueLabelFormatTypeProperty :
      case KN2Token::SFCSeriesValueLabelNumberFormatProperty :
      case KN2Token::SFCSeriesValueLabelPercentageAxisNumberFormatProperty :
      case KN2Token::SFCSeriesValueLabelPieFormatTypeProperty :
      case KN2Token::SFCSeriesValueLabelPieNumberFormatProperty :
      case KN2Token::SFCShininessProperty :
      case KN2Token::SFCShowCategoryAxisLabelsProperty :
      case KN2Token::SFCShowCategoryAxisSeriesLabelsProperty :
      case KN2Token::SFCShowCategoryAxisTitleProperty :
      case KN2Token::SFCShowChartTitleProperty :
      case KN2Token::SFCShowLastCategoryAxisLabelsProperty :
      case KN2Token::SFCShowPieLabelAsPercentageProperty :
      case KN2Token::SFCShowValueAxisTitleProperty :
      case KN2Token::SFCShowValueY2AxisTitleProperty :
      case KN2Token::SFCSpecularColorProperty :
      case KN2Token::SFCStackedAreaValueLabelPositionProperty :
      case KN2Token::SFCStackedBarValueLabelPositionProperty :
      case KN2Token::SFCTrendLineShadowProperty :
      case KN2Token::SFCTrendLineStrokeProperty :
      case KN2Token::SFCTrendlineEquationOpacityProperty :
      case KN2Token::SFCTrendlineEquationParagraphStyleProperty :
      case KN2Token::SFCTrendlineRSquaredOpacityProperty :
      case KN2Token::SFCTrendlineRSquaredParagraphStyleProperty :
      case KN2Token::SFCValueAxisDateTimeFormatProperty :
      case KN2Token::SFCValueAxisDurationFormatProperty :
      case KN2Token::SFCValueAxisFormatTypeProperty :
      case KN2Token::SFCValueAxisHorizontalDateTimeFormatProperty :
      case KN2Token::SFCValueAxisHorizontalDurationFormatProperty :
      case KN2Token::SFCValueAxisHorizontalFormatTypeProperty :
      case KN2Token::SFCValueAxisHorizontalNumberFormatProperty :
      case KN2Token::SFCValueAxisLabelsDepthProperty :
      case KN2Token::SFCValueAxisLabelsOpacityProperty :
      case KN2Token::SFCValueAxisLabelsOrientationProperty :
      case KN2Token::SFCValueAxisLabelsPositionProperty :
      case KN2Token::SFCValueAxisLogarithmicProperty :
      case KN2Token::SFCValueAxisMajorTickLengthProperty :
      case KN2Token::SFCValueAxisMajorTickStrokeProperty :
      case KN2Token::SFCValueAxisMaximumValueProperty :
      case KN2Token::SFCValueAxisMinimumValueProperty :
      case KN2Token::SFCValueAxisMinorTickLengthProperty :
      case KN2Token::SFCValueAxisMinorTickStrokeProperty :
      case KN2Token::SFCValueAxisNumberFormatProperty :
      case KN2Token::SFCValueAxisNumberOfDecadesProperty :
      case KN2Token::SFCValueAxisNumberOfStepsProperty :
      case KN2Token::SFCValueAxisParagraphStyleProperty :
      case KN2Token::SFCValueAxisPercentageProperty :
      case KN2Token::SFCValueAxisShowMajorTickMarksProperty :
      case KN2Token::SFCValueAxisShowMinimumValueProperty :
      case KN2Token::SFCValueAxisShowMinorTickMarksProperty :
      case KN2Token::SFCValueAxisTickMarkLocationProperty :
      case KN2Token::SFCValueAxisTitleParagraphStyleProperty :
      case KN2Token::SFCValueAxisTitlePositionPropertya :
      case KN2Token::SFCValueAxisY2DateTimeFormatProperty :
      case KN2Token::SFCValueAxisY2DurationFormatProperty :
      case KN2Token::SFCValueAxisY2FormatTypeProperty :
      case KN2Token::SFCValueAxisY2NumberFormatProperty :
      case KN2Token::SFCValueAxisY2ParagraphStyleProperty :
      case KN2Token::SFCValueAxisY2TitleParagraphStyleProperty :
      case KN2Token::SFCValueNumberFormatDecimalPlacesProperty :
      case KN2Token::SFCValueNumberFormatPrefixProperty :
      case KN2Token::SFCValueNumberFormatSeparatorProperty :
      case KN2Token::SFCValueNumberFormatSuffixProperty :
      case KN2Token::SFCValueY2AxisLabelsDepthProperty :
      case KN2Token::SFCValueY2AxisLabelsOpacityProperty :
      case KN2Token::SFCValueY2AxisLabelsOrientationProperty :
      case KN2Token::SFCValueY2AxisLabelsPositionProperty :
      case KN2Token::SFCValueY2AxisLogarithmicProperty :
      case KN2Token::SFCValueY2AxisMajorTickLengthProperty :
      case KN2Token::SFCValueY2AxisMajorTickStrokeProperty :
      case KN2Token::SFCValueY2AxisMinorTickLengthProperty :
      case KN2Token::SFCValueY2AxisMinorTickStrokeProperty :
      case KN2Token::SFCValueY2AxisNumberOfDecadesProperty :
      case KN2Token::SFCValueY2AxisNumberOfStepsProperty :
      case KN2Token::SFCValueY2AxisPercentageProperty :
      case KN2Token::SFCValueY2AxisShowMajorTickMarksProperty :
      case KN2Token::SFCValueY2AxisShowMinimumValueProperty :
      case KN2Token::SFCValueY2AxisShowMinorTickMarksProperty :
      case KN2Token::SFCValueY2AxisTickMarkLocationProperty :
      case KN2Token::SFTAutoResizeProperty :
      case KN2Token::SFTBackgroundProperty :
      case KN2Token::SFTCellStylePropertyDateTimeFormat :
      case KN2Token::SFTCellStylePropertyDurationFormat :
      case KN2Token::SFTCellStylePropertyFormatType :
      case KN2Token::SFTCellStylePropertyImplicitFormatType :
      case KN2Token::SFTCellStylePropertyLayoutStyle :
      case KN2Token::SFTCellStylePropertyNumberFormat :
      case KN2Token::SFTCellStylePropertyParagraphStyle :
      case KN2Token::SFTCellTextWrapProperty :
      case KN2Token::SFTDefaultBodyCellStyleProperty :
      case KN2Token::SFTDefaultBodyVectorStyleProperty :
      case KN2Token::SFTDefaultBorderVectorStyleProperty :
      case KN2Token::SFTDefaultColumnCountProperty :
      case KN2Token::SFTDefaultFooterBodyVectorStyleProperty :
      case KN2Token::SFTDefaultFooterBorderVectorStyleProperty :
      case KN2Token::SFTDefaultFooterRowCellStyleProperty :
      case KN2Token::SFTDefaultFooterRowCountProperty :
      case KN2Token::SFTDefaultFooterSeparatorVectorStyleProperty :
      case KN2Token::SFTDefaultGeometryProperty :
      case KN2Token::SFTDefaultGroupingLevel0VectorStyleProperty :
      case KN2Token::SFTDefaultGroupingLevel1VectorStyleProperty :
      case KN2Token::SFTDefaultGroupingLevel2VectorStyleProperty :
      case KN2Token::SFTDefaultGroupingLevel3VectorStyleProperty :
      case KN2Token::SFTDefaultGroupingLevel4VectorStyleProperty :
      case KN2Token::SFTDefaultGroupingRow0CellStyleProperty :
      case KN2Token::SFTDefaultGroupingRow1CellStyleProperty :
      case KN2Token::SFTDefaultGroupingRow2CellStyleProperty :
      case KN2Token::SFTDefaultGroupingRow3CellStyleProperty :
      case KN2Token::SFTDefaultGroupingRow4CellStyleProperty :
      case KN2Token::SFTDefaultHeaderBodyVectorStyleProperty :
      case KN2Token::SFTDefaultHeaderBorderVectorStyleProperty :
      case KN2Token::SFTDefaultHeaderColumnCellStyleProperty :
      case KN2Token::SFTDefaultHeaderColumnCountProperty :
      case KN2Token::SFTDefaultHeaderRowCellStyleProperty :
      case KN2Token::SFTDefaultHeaderRowCountProperty :
      case KN2Token::SFTDefaultHeaderSeparatorVectorStyleProperty :
      case KN2Token::SFTDefaultRowCountProperty :
      case KN2Token::SFTDefaultTableNameIsDisplayedProperty :
      case KN2Token::SFTGroupingRowFillProperty :
      case KN2Token::SFTHeaderColumnRepeatsProperty :
      case KN2Token::SFTHeaderRowRepeatsProperty :
      case KN2Token::SFTStrokeProperty :
      case KN2Token::SFTTableBandedCellFillProperty :
      case KN2Token::SFTTableBandedRowsProperty :
      case KN2Token::SFTTableBehaviorProperty :
      case KN2Token::SFTableCellStylePropertyFill :
      case KN2Token::SFTableCellStylePropertyType :
      case KN2Token::SFTableStylePropertyBackgroundFill :
      case KN2Token::SFTableStylePropertyBorderVectorStyle :
      case KN2Token::SFTableStylePropertyCellLayoutStyle :
      case KN2Token::SFTableStylePropertyCellParagraphStyle :
      case KN2Token::SFTableStylePropertyCellStyle :
      case KN2Token::SFTableStylePropertyHeaderBorderVectorStyle :
      case KN2Token::SFTableStylePropertyHeaderColumnCellLayoutStyle :
      case KN2Token::SFTableStylePropertyHeaderColumnCellParagraphStyle :
      case KN2Token::SFTableStylePropertyHeaderColumnCellStyle :
      case KN2Token::SFTableStylePropertyHeaderRowCellLayoutStyle :
      case KN2Token::SFTableStylePropertyHeaderRowCellParagraphStyle :
      case KN2Token::SFTableStylePropertyHeaderRowCellStyle :
      case KN2Token::SFTableStylePropertyHeaderSeperatorVectorStyle :
      case KN2Token::SFTableStylePropertyHeaderVectorStyle :
      case KN2Token::SFTableStylePropertyPrototypeColumnCount :
      case KN2Token::SFTableStylePropertyPrototypeGeometry :
      case KN2Token::SFTableStylePropertyPrototypeIsHeaderColumn :
      case KN2Token::SFTableStylePropertyPrototypeIsHeaderRow :
      case KN2Token::SFTableStylePropertyPrototypeIsResize :
      case KN2Token::SFTableStylePropertyPrototypeRowCount :
      case KN2Token::SFTableStylePropertyVectorStyle :
      case KN2Token::Series_0 :
      case KN2Token::Series_1 :
      case KN2Token::Series_2 :
      case KN2Token::Series_3 :
      case KN2Token::Series_4 :
      case KN2Token::Series_5 :
      case KN2Token::TableCellStylePropertyFormatDecimals :
      case KN2Token::TableCellStylePropertyFormatEnabled :
      case KN2Token::TableCellStylePropertyFormatNegativeStyle :
      case KN2Token::TableCellStylePropertyFormatPrefix :
      case KN2Token::TableCellStylePropertyFormatSuffix :
      case KN2Token::TableCellStylePropertyFormatThousandsSeparator :
      case KN2Token::TableVectorStyleStrokeProperty :
      case KN2Token::TableVectorStyleTypeProperty :
      case KN2Token::alignment :
      case KN2Token::animationAutoPlay :
      case KN2Token::animationDelay :
      case KN2Token::animationDuration :
      case KN2Token::atsuFontFeatures :
      case KN2Token::baselineShift :
      case KN2Token::blendMode :
      case KN2Token::bodyPlaceholderVisibility :
      case KN2Token::bold :
      case KN2Token::bulletListStyle :
      case KN2Token::capitalization :
      case KN2Token::columns :
      case KN2Token::decimalTab :
      case KN2Token::defaultTabStops :
      case KN2Token::dropCap :
      case KN2Token::effect :
      case KN2Token::effectColor :
      case KN2Token::externalTextWrap :
      case KN2Token::fill :
      case KN2Token::filters :
      case KN2Token::firstLineIndent :
      case KN2Token::firstTopicNumber :
      case KN2Token::followingLayoutStyle :
      case KN2Token::followingParagraphStyle :
      case KN2Token::fontColor :
      case KN2Token::fontName :
      case KN2Token::fontSize :
      case KN2Token::geometry :
      case KN2Token::headLineEnd :
      case KN2Token::headOffset :
      case KN2Token::headlineIndent :
      case KN2Token::headlineParagraphStyle :
      case KN2Token::hidden :
      case KN2Token::hyphenate :
      case KN2Token::italic :
      case KN2Token::keepLinesTogether :
      case KN2Token::keepWithNext :
      case KN2Token::kerning :
      case KN2Token::labelCharacterStyle1 :
      case KN2Token::labelCharacterStyle2 :
      case KN2Token::labelCharacterStyle3 :
      case KN2Token::labelCharacterStyle4 :
      case KN2Token::labelCharacterStyle5 :
      case KN2Token::labelCharacterStyle6 :
      case KN2Token::labelCharacterStyle7 :
      case KN2Token::labelCharacterStyle8 :
      case KN2Token::labelCharacterStyle9 :
      case KN2Token::language :
      case KN2Token::layoutContinuous :
      case KN2Token::layoutMargins :
      case KN2Token::layoutParagraphStyle :
      case KN2Token::layoutStyle :
      case KN2Token::leftIndent :
      case KN2Token::ligatures :
      case KN2Token::lineSpacing :
      case KN2Token::listLabelGeometries:
      case KN2Token::listLabelIndents :
      case KN2Token::listLabelTypes :
      case KN2Token::listLevels :
      case KN2Token::listStyle :
      case KN2Token::listTextIndents :
      case KN2Token::minimumHorizontalInset :
      case KN2Token::objectPlaceholderVisibility :
      case KN2Token::opacity :
      case KN2Token::outline :
      case KN2Token::outlineColor :
      case KN2Token::outlineLevel :
      case KN2Token::outlineStyleType :
      case KN2Token::padding :
      case KN2Token::pageBreakBefore :
      case KN2Token::paragraphBorderType :
      case KN2Token::paragraphBorders :
      case KN2Token::paragraphFill :
      case KN2Token::paragraphRuleOffset :
      case KN2Token::paragraphStroke :
      case KN2Token::reflection :
      case KN2Token::rightIndent :
      case KN2Token::shadow :
      case KN2Token::showInTOC :
      case KN2Token::shrinkTextToFit :
      case KN2Token::slideNumberPlaceholderVisibility :
      case KN2Token::spaceAfter :
      case KN2Token::spaceBefore :
      case KN2Token::strikethru :
      case KN2Token::strikethruColor :
      case KN2Token::strikethruWidth :
      case KN2Token::stroke :
      case KN2Token::superscript :
      case KN2Token::tabs :
      case KN2Token::tailLineEnd :
      case KN2Token::tailOffset :
      case KN2Token::textBackground :
      case KN2Token::textBorders :
      case KN2Token::textShadow :
      case KN2Token::titlePlaceholderVisibility :
      case KN2Token::tocStyle :
      case KN2Token::tracking :
      case KN2Token::transition :
      case KN2Token::underline :
      case KN2Token::underlineColor :
      case KN2Token::underlineWidth :
      case KN2Token::verticalAlignment :
      case KN2Token::verticalText :
      case KN2Token::widowControl :
      case KN2Token::word_strikethrough :
      case KN2Token::word_underline :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(element);
        break;

      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
