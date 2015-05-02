/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKPROPERTIES_H_INCLUDED
#define IWORKPROPERTIES_H_INCLUDED

#include "IWORKEnum.h"
#include "IWORKPropertyInfo.h"
#include "IWORKTypes.h"

namespace libetonyek
{

namespace property
{

struct Alignment {};
struct Baseline {};
struct BaselineShift {};
struct Bold {};
struct Capitalization {};
struct FontColor {};
struct FontName {};
struct FontSize {};
struct Geometry {};
struct Italic {};
struct LineSpacing {};
struct Outline {};
struct ParagraphFill {};
struct SpaceAfter {};
struct SpaceBefore {};
struct Strikethru {};
struct Tabs {};
struct TextBackground {};
struct Underline {};

}

template<>
struct IWORKPropertyInfo<property::Alignment>
{
  typedef IWORKAlignment ValueType;
  static const IWORKPropertyID_t id;
};

template<>
struct IWORKPropertyInfo<property::Baseline>
{
  typedef IWORKBaseline ValueType;
  static const IWORKPropertyID_t id;
};

template<>
struct IWORKPropertyInfo<property::BaselineShift>
{
  typedef double ValueType;
  static const IWORKPropertyID_t id;
};

template<>
struct IWORKPropertyInfo<property::Bold>
{
  typedef bool ValueType;
  static const IWORKPropertyID_t id;
};

template<>
struct IWORKPropertyInfo<property::Capitalization>
{
  typedef IWORKCapitalization ValueType;
  static const IWORKPropertyID_t id;
};

template<>
struct IWORKPropertyInfo<property::FontColor>
{
  typedef IWORKColor ValueType;
  static const IWORKPropertyID_t id;
};

template<>
struct IWORKPropertyInfo<property::FontName>
{
  typedef std::string ValueType;
  static const IWORKPropertyID_t id;
};

template<>
struct IWORKPropertyInfo<property::FontSize>
{
  typedef double ValueType;
  static const IWORKPropertyID_t id;
};

template<>
struct IWORKPropertyInfo<property::Geometry>
{
  typedef IWORKGeometryPtr_t ValueType;
  static const IWORKPropertyID_t id;
};

template<>
struct IWORKPropertyInfo<property::Italic>
{
  typedef bool ValueType;
  static const IWORKPropertyID_t id;
};

template<>
struct IWORKPropertyInfo<property::LineSpacing>
{
  typedef IWORKLineSpacing ValueType;
  static const IWORKPropertyID_t id;
};

template<>
struct IWORKPropertyInfo<property::Outline>
{
  typedef bool ValueType;
  static const IWORKPropertyID_t id;
};

template<>
struct IWORKPropertyInfo<property::ParagraphFill>
{
  typedef IWORKColor ValueType;
  static const IWORKPropertyID_t id;
};

template<>
struct IWORKPropertyInfo<property::SpaceAfter>
{
  typedef double ValueType;
  static const IWORKPropertyID_t id;
};

template<>
struct IWORKPropertyInfo<property::SpaceBefore>
{
  typedef double ValueType;
  static const IWORKPropertyID_t id;
};

template<>
struct IWORKPropertyInfo<property::Strikethru>
{
  typedef bool ValueType;
  static const IWORKPropertyID_t id;
};

template<>
struct IWORKPropertyInfo<property::Tabs>
{
  typedef IWORKTabStops_t ValueType;
  static const IWORKPropertyID_t id;
};

template<>
struct IWORKPropertyInfo<property::TextBackground>
{
  typedef IWORKColor ValueType;
  static const IWORKPropertyID_t id;
};

template<>
struct IWORKPropertyInfo<property::Underline>
{
  typedef bool ValueType;
  static const IWORKPropertyID_t id;
};

}

#endif // IWORKPROPERTIES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
