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

IWORKCellStyle::IWORKCellStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : m_style(style)
  , m_context(context)
{
}

IWORKCharacterStyle::IWORKCharacterStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : m_style(style)
  , m_context(context)
{
}

boost::optional<bool> IWORKCharacterStyle::getBold() const
{
  return extractOptional<bool>(m_style->lookup("bold", m_context));
}

boost::optional<IWORKCapitalization> IWORKCharacterStyle::getCapitalization() const
{
  return extractOptional<IWORKCapitalization>(m_style->lookup("capitalization", m_context));
}

boost::optional<bool> IWORKCharacterStyle::getItalic() const
{
  return extractOptional<bool>(m_style->lookup("italic", m_context));
}

boost::optional<IWORKColor> IWORKCharacterStyle::getFontColor() const
{
  return extractOptional<IWORKColor>(m_style->lookup("fontColor", m_context));
}

boost::optional<double> IWORKCharacterStyle::getFontSize() const
{
  return extractOptional<double>(m_style->lookup("fontSize", m_context));
}

boost::optional<std::string> IWORKCharacterStyle::getFontName() const
{
  return extractOptional<std::string>(m_style->lookup("fontName", m_context));
}

boost::optional<bool> IWORKCharacterStyle::getOutline() const
{
  return extractOptional<bool>(m_style->lookup("outline", m_context));
}

boost::optional<bool> IWORKCharacterStyle::getStrikethru() const
{
  return extractOptional<bool>(m_style->lookup("strikethru", m_context));
}

boost::optional<bool> IWORKCharacterStyle::getUnderline() const
{
  return extractOptional<bool>(m_style->lookup("underline", m_context));
}

IWORKConnectionStyle::IWORKConnectionStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : m_style(style)
  , m_context(context)
{
}

IWORKGraphicStyle::IWORKGraphicStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : m_style(style)
  , m_context(context)
{
}

IWORKListStyle::IWORKListStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : m_style(style)
  , m_context(context)
{
}

IWORKParagraphStyle::IWORKParagraphStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : IWORKCharacterStyle(style, context)
  , m_style(style)
  , m_context(context)
{
}

boost::optional<IWORKAlignment> IWORKParagraphStyle::getAlignment() const
{
  return extractOptional<IWORKAlignment>(m_style->lookup("alignment", m_context));
}

boost::optional<IWORKTabStops_t> IWORKParagraphStyle::getTabs() const
{
  return extractOptional<IWORKTabStops_t>(m_style->lookup("tabs", m_context));
}

IWORKTabularStyle::IWORKTabularStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : m_style(style)
  , m_context(context)
{
}

IWORKVectorStyle::IWORKVectorStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context)
  : m_style(style)
  , m_context(context)
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
