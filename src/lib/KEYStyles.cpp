/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYStyles.h"
#include "KEYStylesheet.h"

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

bool KEYStyleBase::link(const KEYStylesheetPtr_t &stylesheet)
{
  if (m_parent || !m_parentIdent)
    return true;

  KEYStylesheetPtr_t currentStylesheet = stylesheet;

  if (currentStylesheet && (m_ident == m_parentIdent))
    currentStylesheet = currentStylesheet->parent;

  if (!currentStylesheet)
    return false;

  m_parent = find(currentStylesheet, get(m_parentIdent));
  if (m_parent)
    m_props.setParent(&m_parent->getPropertyMap());

  return bool(m_parent);
}

void KEYStyleBase::flatten()
{
  // TODO: implement me
}

KEYStyleBase::KEYStyleBase(const KEYPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : m_props(props)
  , m_ident(ident)
  , m_parentIdent(parentIdent)
  , m_parent()
{
}

KEYStyleBase::~KEYStyleBase()
{
}

const KEYPropertyMap &KEYStyleBase::getPropertyMap() const
{
  return m_props;
}

boost::any KEYStyleBase::lookup(const char *property, const KEYStyleContext &context) const
{
  any value = getPropertyMap().get(property, true);
  if (value.empty())
    value = context.find(property, true);

  return value;
}

KEYCellStyle::KEYCellStyle(const KEYPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

KEYStylePtr_t KEYCellStyle::find(const KEYStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->cellStyles[ident];
}

KEYCharacterStyle::KEYCharacterStyle(const KEYPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

boost::optional<bool> KEYCharacterStyle::getBold(const KEYStyleContext &context) const
{
  return extractOptional<bool>(lookup("bold", context));
}

boost::optional<KEYCapitalization> KEYCharacterStyle::getCapitalization(const KEYStyleContext &context) const
{
  return extractOptional<KEYCapitalization>(lookup("capitalization", context));
}

boost::optional<bool> KEYCharacterStyle::getItalic(const KEYStyleContext &context) const
{
  return extractOptional<bool>(lookup("italic", context));
}

boost::optional<KEYColor> KEYCharacterStyle::getFontColor(const KEYStyleContext &context) const
{
  return extractOptional<KEYColor>(lookup("fontColor", context));
}

boost::optional<double> KEYCharacterStyle::getFontSize(const KEYStyleContext &context) const
{
  return extractOptional<double>(lookup("fontSize", context));
}

boost::optional<std::string> KEYCharacterStyle::getFontName(const KEYStyleContext &context) const
{
  return extractOptional<std::string>(lookup("fontName", context));
}

boost::optional<bool> KEYCharacterStyle::getOutline(const KEYStyleContext &context) const
{
  return extractOptional<bool>(lookup("outline", context));
}

boost::optional<bool> KEYCharacterStyle::getStrikethru(const KEYStyleContext &context) const
{
  return extractOptional<bool>(lookup("strikethru", context));
}

boost::optional<bool> KEYCharacterStyle::getUnderline(const KEYStyleContext &context) const
{
  return extractOptional<bool>(lookup("underline", context));
}

KEYStylePtr_t KEYCharacterStyle::find(const KEYStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->characterStyles[ident];
}

KEYConnectionStyle::KEYConnectionStyle(const KEYPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

KEYStylePtr_t KEYConnectionStyle::find(const KEYStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->connectionStyles[ident];
}

KEYGraphicStyle::KEYGraphicStyle(const KEYPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

KEYStylePtr_t KEYGraphicStyle::find(const KEYStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->graphicStyles[ident];
}

KEYLayoutStyle::KEYLayoutStyle(const KEYPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

KEYStylePtr_t KEYLayoutStyle::find(const KEYStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->layoutStyles[ident];
}

KEYListStyle::KEYListStyle(const KEYPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

KEYStylePtr_t KEYListStyle::find(const KEYStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->listStyles[ident];
}

KEYParagraphStyle::KEYParagraphStyle(const KEYPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYCharacterStyle(props, ident, parentIdent)
{
}

KEYStylePtr_t KEYParagraphStyle::find(const KEYStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->paragraphStyles[ident];
}

KEYPlaceholderStyle::KEYPlaceholderStyle(const KEYPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

KEYGeometryPtr_t KEYPlaceholderStyle::getGeometry(const KEYStyleContext &context) const
{
  return extract<KEYGeometryPtr_t>(lookup("geometry", context));
}

KEYStylePtr_t KEYPlaceholderStyle::find(const KEYStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->placeholderStyles[ident];
}

KEYSlideStyle::KEYSlideStyle(const KEYPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

KEYStylePtr_t KEYSlideStyle::find(const KEYStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->slideStyles[ident];
}

KEYTabularStyle::KEYTabularStyle(const KEYPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

KEYStylePtr_t KEYTabularStyle::find(const KEYStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->tabularStyles[ident];
}

KEYVectorStyle::KEYVectorStyle(const KEYPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

KEYStylePtr_t KEYVectorStyle::find(const KEYStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->vectorStyles[ident];
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
