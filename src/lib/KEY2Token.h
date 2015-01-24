/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY2TOKEN_H_INCLUDED
#define KEY2TOKEN_H_INCLUDED

namespace libetonyek
{

namespace detail
{

template<unsigned N, unsigned P>
struct log_impl
{
  static const unsigned value = log_impl<(N >> 1), P + 1>::value;
};

template<unsigned P>
struct log_impl<1, P>
{
  static const unsigned value = P;
};

template<unsigned P>
struct log_impl<0, P>
{
};

template<unsigned N>
struct log
{
  static const unsigned value = log_impl<N, 0>::value;
};

}

namespace KEY2Token
{

enum
{
  INVALID_TOKEN = 0,

  // elements
  SFTCellStylePropertyLayoutStyle,
  SFTCellStylePropertyParagraphStyle,
  SFTableStylePropertyBorderVectorStyle,
  SFTableStylePropertyCellLayoutStyle,
  SFTableStylePropertyCellParagraphStyle,
  SFTableStylePropertyCellStyle,
  SFTableStylePropertyHeaderBorderVectorStyle,
  SFTableStylePropertyHeaderColumnCellLayoutStyle,
  SFTableStylePropertyHeaderColumnCellParagraphStyle,
  SFTableStylePropertyHeaderColumnCellStyle,
  SFTableStylePropertyHeaderRowCellLayoutStyle,
  SFTableStylePropertyHeaderRowCellParagraphStyle,
  SFTableStylePropertyHeaderRowCellStyle,
  SFTableStylePropertyHeaderSeperatorVectorStyle,
  SFTableStylePropertyHeaderVectorStyle,
  SFTableStylePropertyVectorStyle,
  TableCellStylePropertyFormatNegativeStyle,
  alignment,
  anon_styles,
  baselineShift,
  bezier,
  bezier_path,
  bezier_ref,
  body_placeholder,
  body_placeholder_ref,
  bold,
  br,
  bulletListStyle,
  c,
  calc_engine,
  callout2_path,
  capitalization,
  cell_style,
  cell_style_ref,
  characterstyle,
  characterstyle_ref,
  color,
  columns,
  connection_line,
  connection_path,
  connection_style,
  content,
  crbr,
  ct,
  d,
  data,
  data_ref,
  datasource,
  drawables,
  du,
  editable_bezier_path,
  f,
  filtered,
  filtered_image,
  followingLayoutStyle,
  followingParagraphStyle,
  fontColor,
  fontName,
  fontSize,
  g,
  geometry,
  graphic_style,
  grid,
  grid_column,
  grid_row,
  group,
  head,
  headlineParagraphStyle,
  headline_style,
  image,
  image_media,
  intratopicbr,
  italic,
  layer,
  layer_ref,
  layers,
  layout,
  layoutParagraphStyle,
  layoutStyle,
  layoutstyle,
  layoutstyle_ref,
  leveled,
  line,
  link,
  listStyle,
  liststyle,
  liststyle_ref,
  lnbr,
  master_slide,
  master_slides,
  media,
  metadata,
  movie_media,
  n,
  naturalSize,
  notes,
  number,
  other_datas,
  outline,
  p,
  page,
  paragraphstyle,
  paragraphstyle_ref,
  parent_ref,
  path,
  placeholder_style,
  placeholder_style_ref,
  point,
  point_path,
  position,
  presentation,
  property_map,
  proxy_master_layer,
  r,
  rows,
  s,
  scalar_path,
  self_contained_movie,
  shape,
  size,
  slide,
  slide_list,
  slide_style,
  so,
  span,
  sticky_note,
  sticky_notes,
  strikethru,
  string,
  style,
  styles,
  stylesheet,
  superscript,
  t,
  tab,
  tabs,
  tabstop,
  tabular_info,
  tabular_model,
  tabular_style,
  tail,
  text,
  text_body,
  text_storage,
  theme,
  theme_list,
  title_placeholder,
  title_placeholder_ref,
  tocStyle,
  type,
  underline,
  unfiltered,
  unfiltered_ref,
  vector_style,
  vector_style_ref,

  // attributes
  ID,
  IDREF,
  a,
  align,
  angle,
  aspectRatioLocked,
  b,
  col_span,
  cornerRadius,
  displayname,
  h,
  height,
  hfs_type,
  horizontalFlip,
  ident,
  locked,
  parent_ident,
  pos,
  right,
  row_span,
  scalar,
  shearXAngle,
  shearYAngle,
  sizesLocked,
  tailAtCenter,
  tailPositionX,
  tailPositionY,
  tailSize,
  v,
  version,
  verticalFlip,
  w,
  width,
  x,
  y,

  // attribute values
  COMPATIBLE_VERSION_STR_3,
  VERSION_STR_2,
  VERSION_STR_3,
  VERSION_STR_4,
  VERSION_STR_5,
  _0,
  _1,
  double_,
  false_,
  i,
  star,
  true_,

  LAST_TOKEN = true_
};

// namespaces
enum
{
  TOKEN_RANGE = 2 << (detail::log<LAST_TOKEN + 1>::value + 1),
  NS_NONE = TOKEN_RANGE,
  NS_URI_KEY = NS_NONE + TOKEN_RANGE,
  NS_URI_SF = NS_URI_KEY + TOKEN_RANGE,
  NS_URI_SFA = NS_URI_SF + TOKEN_RANGE,
  NS_URI_XSI = NS_URI_SFA + TOKEN_RANGE
};

}

struct KEY2Tokenizer
{
  int operator()(const char *str) const;
};

}

#endif // KEY2TOKEN_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
