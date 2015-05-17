/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTOKEN_H_INCLUDED
#define IWORKTOKEN_H_INCLUDED

#include <boost/static_assert.hpp>

#include "IWORKTokenInfo.h"

namespace libetonyek
{

class IWORKTokenizer;

namespace IWORKToken
{

enum
{
  INVALID_TOKEN = 0,
  FIRST_TOKEN = IWORKTokenInfo<IWORKParser>::first,

  // namespace prefixes
  sf,
  sfa,

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
  attachment,
  attachment_ref,
  attachments,
  baselineShift,
  bezier,
  bezier_path,
  bezier_ref,
  body_placeholder_ref,
  bold,
  bulletListStyle,
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
  container_hint,
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
  firstLineIndent,
  followingLayoutStyle,
  followingParagraphStyle,
  fontColor,
  fontName,
  fontSize,
  footers,
  g,
  geometry,
  graphic_style,
  grid,
  grid_column,
  grid_row,
  group,
  head,
  headers,
  headline_style,
  href,
  image,
  image_media,
  intratopicbr,
  italic,
  keepLinesTogether,
  keepWithNext,
  language,
  layer,
  layer_ref,
  layers,
  layout,
  layoutParagraphStyle,
  layoutStyle,
  layoutstyle,
  layoutstyle_ref,
  leftIndent,
  leveled,
  line,
  lineSpacing,
  linespacing,
  link,
  listStyle,
  liststyle,
  liststyle_ref,
  lnbr,
  media,
  metadata,
  movie_media,
  n,
  naturalSize,
  number,
  other_datas,
  outline,
  p,
  paragraphBorderType,
  paragraphFill,
  paragraphstyle,
  paragraphstyle_ref,
  parent_ref,
  path,
  placeholder_style,
  placeholder_style_ref,
  point,
  point_path,
  position,
  property_map,
  proxy_master_layer,
  r,
  rightIndent,
  rows,
  s,
  scalar_path,
  section,
  self_contained_movie,
  shape,
  size,
  slide_style,
  so,
  spaceAfter,
  spaceBefore,
  span,
  sticky_note,
  strikethru,
  string,
  style,
  styles,
  superscript,
  t,
  tab,
  tabs,
  tabs_ref,
  tabstop,
  tabular_info,
  tabular_model,
  tabular_style,
  tail,
  text,
  textBackground,
  text_body,
  text_storage,
  title_placeholder_ref,
  tocStyle,
  type,
  underline,
  unfiltered,
  unfiltered_ref,
  vector_style,
  vector_style_ref,
  widowControl,

  // attributes
  ID,
  IDREF,
  a,
  align,
  amt,
  angle,
  aspectRatioLocked,
  b,
  col_span,
  cornerRadius,
  displayname,
  frame_h,
  frame_w,
  frame_x,
  frame_y,
  h,
  height,
  hfs_type,
  horizontalFlip,
  ident,
  kind,
  locked,
  mode,
  parent_ident,
  pos,
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
  verticalFlip,
  w,
  width,
  x,
  y,

  // attribute values
  __multilingual,
  _0,
  _1,
  false_,
  relative,
  true_,

  LAST_TOKEN
};

BOOST_STATIC_ASSERT(IWORKTokenInfo<IWORKParser>::last >= LAST_TOKEN);

enum Namespace
{
  NS_URI_SF = sf << 16,
  NS_URI_SFA = sfa << 16
};

const IWORKTokenizer &getTokenizer();

}

}

#endif // IWORKTOKEN_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
