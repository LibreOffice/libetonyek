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
  key1,

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
  description,
  dict,
  div,
  drawables,
  element,
  fill_style,
  font,
  g,
  gradient,
  guide,
  guides,
  image,
  image_bullet_style,
  legend,
  line,
  line_head_style,
  line_tail_style,
  master_slide,
  master_slides,
  metadata,
  node,
  notes,
  null,
  number,
  page_number,
  plugin,
  plugin_data,
  presentation,
  prototype_bullets,
  prototype_data,
  prototype_drawables,
  prototype_plugin,
  prototype_plugins,
  rect,
  reference,
  segment,
  sequence_bullet_style,
  serie,
  series,
  shadow_style,
  shape,
  size,
  slide,
  slide_list,
  span,
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
  DefaultLegendRelativePosition,
  DefaultLegendSize,
  altLineOpacity,
  altLineVisible,
  angle,
  bl,
  br,
  buildChunkingStyle,
  bullet_indentation,
  byte_size,
  chartFrame,
  dataFormatterHasThousandsSeparators,
  dataFormatterNumberOfDecimals,
  dataFormatterPrefix,
  dataFormatterSuffix,
  direction,
  display_name,
  duration,
  end,
  end_color,
  file,
  fill_color,
  fill_type,
  floating_content,
  font_color,
  font_kerning,
  font_ligatures,
  font_name,
  font_size,
  font_superscript,
  font_underline,
  gradient_angle,
  gridOpacity,
  grow_horizontally,
  head,
  id,
  ident,
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
  match_point,
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
  paragraph_tail_indent,
  path,
  pattern,
  pieSliceOffset,
  pieSlicePercentVisible,
  point_at_top,
  pos,
  radius,
  seriesDirection,
  showGrid,
  showZero,
  size_technique,
  slide_size,
  spacing,
  start,
  start_color,
  steps,
  stroke_color,
  stroke_width,
  symbolFillMode,
  table,
  tab_stops,
  tail,
  tickLabelsAngle,
  tickLabelsOpacity,
  tickLabelsVisible,
  titleVisible,
  tl,
  tr,
  transformation,
  type,
  useUserMaximum,
  useUserMinimum,
  userMaximum,
  userMinimum,
  version,
  vertical_alignment,
  visibility,
  visible,
  width,

  // attribute values
  bottom,
  center,
  character,
  horizontal,
  hidden,
  inherited,
  justified,
  left,
  middle,
  none,
  relative,
  right,
  scale_to_fit,
  sequence,
  solid,
  text,
  tile,
  top,
  tracks_master,
  vertical,

  LAST_TOKEN
};

BOOST_STATIC_ASSERT(IWORKTokenInfo<KEY1Parser>::last >= LAST_TOKEN);

enum Namespace
{
  NS_URI_KEY = key1 << 16
};

const IWORKTokenizer &getTokenizer();

}

}

#endif // KEY1TOKEN_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
