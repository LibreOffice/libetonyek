/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY1TOKEN_H_INCLUDED
#define KEY1TOKEN_H_INCLUDED

#include <boost/static_assert.hpp>

#include "IWORKTokenInfo.h"

namespace libetonyek
{

class IWORKTokenizer;

namespace KEY1Token
{

enum
{
  INVALID_TOKEN = 0,
  FIRST_TOKEN = IWORKTokenInfo<KEY1Parser>::first,

  // namespace prefixes

  // elements
  application_name,
  application_version,
  array,
  axes,
  axis,
  background_fill_style,
  body,
  bullet,
  bullet_characters,
  bullets,
  character_bullet_style,
  chart_prototype,
  color,
  content,
  dash_style,
  data,
  dict,
  drawables,
  fill_style,
  font,
  gradient,
  guide,
  guides,
  image,
  legend,
  line,
  line_head_style,
  line_tail_style,
  master_slide,
  master_slides,
  metadata,
  null,
  number,
  page_number,
  presentation,
  prototype_bullets,
  prototype_data,
  prototype_drawables,
  prototype_plugin,
  prototype_plugins,
  rect,
  reference,
  serie,
  series,
  shadow_style,
  shape,
  size,
  slide,
  slide_list,
  string,
  stroke_style,
  styles,
  symbol,
  text_attributes,
  textbox,
  theme,
  thumbnail,
  thumbnails,
  time_stamp,
  title,
  transition_style,
  ui_state,

  // attributes
  altLineOpacity,
  altLineVisible,
  angle,
  buildChunkingStyle,
  bullet_indentation,
  byte_size,
  chartFrame,
  dataFormatterHasThousandsSeparators,
  dataFormatterNumberOfDecimals,
  dataFormatterPrefix,
  dataFormatterSuffix,
  DefaultLegendRelativePosition,
  DefaultLegendSize,
  display_name,
  duration,
  end_color,
  file,
  fill_color,
  fill_type,
  floating_content,
  font_name,
  font_size,
  gradient_angle,
  gridOpacity,
  id,
  id_ref,
  image_data,
  image_scale,
  interBarGap,
  interSeriesGap,
  is_filled,
  key,
  labelOpacity,
  labelPosition,
  labelVisible,
  layerElementsForShadowing,
  level,
  lineOpacity,
  lineVisible,
  location,
  lock_aspect_ratio,
  locked,
  majorTickPositions,
  marker_type,
  master_slide_id,
  minorTickPositions,
  name,
  natural_size,
  numberOfPoints,
  offset,
  opacity,
  orientation,
  paragraph_alignment,
  paragraph_first_line_indent,
  paragraph_head_indent,
  pattern,
  pieSliceOffset,
  pieSlicePercentVisible,
  point_at_top,
  radius,
  seriesDirection,
  showGrid,
  showZero,
  size_technique,
  slide_size,
  spacing,
  start_color,
  steps,
  stroke_color,
  stroke_width,
  symbolFillMode,
  tab_stops,
  tickLabelsAngle,
  tickLabelsOpacity,
  tickLabelsVisible,
  titleVisible,
  transformation,
  type,
  userMaximum,
  userMinimum,
  useUserMaximum,
  useUserMinimum,
  version,
  vertical_alignment,
  visibility,
  visible,
  width,

  // attribute values

  LAST_TOKEN
};

BOOST_STATIC_ASSERT(IWORKTokenInfo<KEY1Parser>::last >= LAST_TOKEN);

const IWORKTokenizer &getTokenizer();

}

}

#endif // KEY1TOKEN_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
