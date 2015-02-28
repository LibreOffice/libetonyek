/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYStyles.h"

#include "IWORKStyle.h"
#include "IWORKStylesheet.h"

using boost::any;
using boost::any_cast;
using boost::optional;

namespace libetonyek
{

namespace
{

template<typename T>
T extract(const any &property)
{
  T value;
  if (!property.empty())
    value = any_cast<T>(property);
  return value;
}

template<typename T>
optional<T> extractOptional(const any &property)
{
  optional<T> value;
  if (!property.empty())
    value = any_cast<T>(property);
  return value;
}

}

KEYCellStyle::KEYCellStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : m_style(style)
  , m_context(context)
{
}

KEYCharacterStyle::KEYCharacterStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : m_style(style)
  , m_context(context)
{
}

boost::optional<bool> KEYCharacterStyle::getBold() const
{
  return extractOptional<bool>(m_style->lookup("bold", m_context));
}

boost::optional<IWORKCapitalization> KEYCharacterStyle::getCapitalization() const
{
  return extractOptional<IWORKCapitalization>(m_style->lookup("capitalization", m_context));
}

boost::optional<bool> KEYCharacterStyle::getItalic() const
{
  return extractOptional<bool>(m_style->lookup("italic", m_context));
}

boost::optional<IWORKColor> KEYCharacterStyle::getFontColor() const
{
  return extractOptional<IWORKColor>(m_style->lookup("fontColor", m_context));
}

boost::optional<double> KEYCharacterStyle::getFontSize() const
{
  return extractOptional<double>(m_style->lookup("fontSize", m_context));
}

boost::optional<std::string> KEYCharacterStyle::getFontName() const
{
  return extractOptional<std::string>(m_style->lookup("fontName", m_context));
}

boost::optional<bool> KEYCharacterStyle::getOutline() const
{
  return extractOptional<bool>(m_style->lookup("outline", m_context));
}

boost::optional<bool> KEYCharacterStyle::getStrikethru() const
{
  return extractOptional<bool>(m_style->lookup("strikethru", m_context));
}

boost::optional<bool> KEYCharacterStyle::getUnderline() const
{
  return extractOptional<bool>(m_style->lookup("underline", m_context));
}

KEYConnectionStyle::KEYConnectionStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : m_style(style)
  , m_context(context)
{
}

KEYGraphicStyle::KEYGraphicStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : m_style(style)
  , m_context(context)
{
}

KEYLayoutStyle::KEYLayoutStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : m_style(style)
  , m_context(context)
{
}

KEYListStyle::KEYListStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : m_style(style)
  , m_context(context)
{
}

KEYParagraphStyle::KEYParagraphStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : KEYCharacterStyle(style, context)
  , m_style(style)
  , m_context(context)
{
}

boost::optional<IWORKAlignment> KEYParagraphStyle::getAlignment() const
{
  return extractOptional<IWORKAlignment>(m_style->lookup("alignment", m_context));
}

boost::optional<IWORKTabStops_t> KEYParagraphStyle::getTabs() const
{
  return extractOptional<IWORKTabStops_t>(m_style->lookup("tabs", m_context));
}

KEYPlaceholderStyle::KEYPlaceholderStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : m_style(style)
  , m_context(context)
{
}

IWORKGeometryPtr_t KEYPlaceholderStyle::getGeometry() const
{
  return extract<IWORKGeometryPtr_t>(m_style->lookup("geometry", m_context));
}

KEYSlideStyle::KEYSlideStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : m_style(style)
  , m_context(context)
{
}

KEYTabularStyle::KEYTabularStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : m_style(style)
  , m_context(context)
{
}

KEYVectorStyle::KEYVectorStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : m_style(style)
  , m_context(context)
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
