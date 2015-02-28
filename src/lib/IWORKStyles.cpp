/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKStyles.h"

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

IWORKCellStyle::IWORKCellStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack)
  : m_style(style)
  , m_stack(stack)
{
}

IWORKCharacterStyle::IWORKCharacterStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack)
  : m_style(style)
  , m_stack(stack)
{
}

boost::optional<bool> IWORKCharacterStyle::getBold() const
{
  return extractOptional<bool>(m_style->lookup("bold", m_stack));
}

boost::optional<IWORKCapitalization> IWORKCharacterStyle::getCapitalization() const
{
  return extractOptional<IWORKCapitalization>(m_style->lookup("capitalization", m_stack));
}

boost::optional<bool> IWORKCharacterStyle::getItalic() const
{
  return extractOptional<bool>(m_style->lookup("italic", m_stack));
}

boost::optional<IWORKColor> IWORKCharacterStyle::getFontColor() const
{
  return extractOptional<IWORKColor>(m_style->lookup("fontColor", m_stack));
}

boost::optional<double> IWORKCharacterStyle::getFontSize() const
{
  return extractOptional<double>(m_style->lookup("fontSize", m_stack));
}

boost::optional<std::string> IWORKCharacterStyle::getFontName() const
{
  return extractOptional<std::string>(m_style->lookup("fontName", m_stack));
}

boost::optional<bool> IWORKCharacterStyle::getOutline() const
{
  return extractOptional<bool>(m_style->lookup("outline", m_stack));
}

boost::optional<bool> IWORKCharacterStyle::getStrikethru() const
{
  return extractOptional<bool>(m_style->lookup("strikethru", m_stack));
}

boost::optional<bool> IWORKCharacterStyle::getUnderline() const
{
  return extractOptional<bool>(m_style->lookup("underline", m_stack));
}

IWORKConnectionStyle::IWORKConnectionStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack)
  : m_style(style)
  , m_stack(stack)
{
}

IWORKGraphicStyle::IWORKGraphicStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack)
  : m_style(style)
  , m_stack(stack)
{
}

IWORKListStyle::IWORKListStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack)
  : m_style(style)
  , m_stack(stack)
{
}

IWORKParagraphStyle::IWORKParagraphStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack)
  : IWORKCharacterStyle(style, stack)
  , m_style(style)
  , m_stack(stack)
{
}

boost::optional<IWORKAlignment> IWORKParagraphStyle::getAlignment() const
{
  return extractOptional<IWORKAlignment>(m_style->lookup("alignment", m_stack));
}

boost::optional<IWORKTabStops_t> IWORKParagraphStyle::getTabs() const
{
  return extractOptional<IWORKTabStops_t>(m_style->lookup("tabs", m_stack));
}

IWORKTabularStyle::IWORKTabularStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack)
  : m_style(style)
  , m_stack(stack)
{
}

IWORKVectorStyle::IWORKVectorStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack)
  : m_style(style)
  , m_stack(stack)
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
