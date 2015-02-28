/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYStyles.h"
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

bool KEYStyleBase::link(const IWORKStylesheetPtr_t &stylesheet)
{
  if (m_parent || !m_parentIdent)
    return true;

  IWORKStylesheetPtr_t currentStylesheet = stylesheet;

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

KEYStyleBase::KEYStyleBase(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : m_props(props)
  , m_ident(ident)
  , m_parentIdent(parentIdent)
  , m_parent()
{
}

KEYStyleBase::~KEYStyleBase()
{
}

const IWORKPropertyMap &KEYStyleBase::getPropertyMap() const
{
  return m_props;
}

boost::any KEYStyleBase::lookup(const char *property, const IWORKStyleContext &context) const
{
  any value = getPropertyMap().get(property, true);
  if (value.empty())
    value = context.find(property, true);

  return value;
}

KEYCellStyle::KEYCellStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

IWORKStylePtr_t KEYCellStyle::find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->cellStyles[ident];
}

KEYCharacterStyle::KEYCharacterStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

boost::optional<bool> KEYCharacterStyle::getBold(const IWORKStyleContext &context) const
{
  return extractOptional<bool>(lookup("bold", context));
}

boost::optional<IWORKCapitalization> KEYCharacterStyle::getCapitalization(const IWORKStyleContext &context) const
{
  return extractOptional<IWORKCapitalization>(lookup("capitalization", context));
}

boost::optional<bool> KEYCharacterStyle::getItalic(const IWORKStyleContext &context) const
{
  return extractOptional<bool>(lookup("italic", context));
}

boost::optional<IWORKColor> KEYCharacterStyle::getFontColor(const IWORKStyleContext &context) const
{
  return extractOptional<IWORKColor>(lookup("fontColor", context));
}

boost::optional<double> KEYCharacterStyle::getFontSize(const IWORKStyleContext &context) const
{
  return extractOptional<double>(lookup("fontSize", context));
}

boost::optional<std::string> KEYCharacterStyle::getFontName(const IWORKStyleContext &context) const
{
  return extractOptional<std::string>(lookup("fontName", context));
}

boost::optional<bool> KEYCharacterStyle::getOutline(const IWORKStyleContext &context) const
{
  return extractOptional<bool>(lookup("outline", context));
}

boost::optional<bool> KEYCharacterStyle::getStrikethru(const IWORKStyleContext &context) const
{
  return extractOptional<bool>(lookup("strikethru", context));
}

boost::optional<bool> KEYCharacterStyle::getUnderline(const IWORKStyleContext &context) const
{
  return extractOptional<bool>(lookup("underline", context));
}

IWORKStylePtr_t KEYCharacterStyle::find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->characterStyles[ident];
}

KEYConnectionStyle::KEYConnectionStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

IWORKStylePtr_t KEYConnectionStyle::find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->connectionStyles[ident];
}

KEYGraphicStyle::KEYGraphicStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

IWORKStylePtr_t KEYGraphicStyle::find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->graphicStyles[ident];
}

KEYLayoutStyle::KEYLayoutStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

IWORKStylePtr_t KEYLayoutStyle::find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->layoutStyles[ident];
}

KEYListStyle::KEYListStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

IWORKStylePtr_t KEYListStyle::find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->listStyles[ident];
}

KEYParagraphStyle::KEYParagraphStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYCharacterStyle(props, ident, parentIdent)
{
}

boost::optional<IWORKAlignment> KEYParagraphStyle::getAlignment(const IWORKStyleContext &context) const
{
  return extractOptional<IWORKAlignment>(lookup("alignment", context));
}

boost::optional<IWORKTabStops_t> KEYParagraphStyle::getTabs(const IWORKStyleContext &context) const
{
  return extractOptional<IWORKTabStops_t>(lookup("tabs", context));
}

IWORKStylePtr_t KEYParagraphStyle::find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->paragraphStyles[ident];
}

KEYPlaceholderStyle::KEYPlaceholderStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

IWORKGeometryPtr_t KEYPlaceholderStyle::getGeometry(const IWORKStyleContext &context) const
{
  return extract<IWORKGeometryPtr_t>(lookup("geometry", context));
}

IWORKStylePtr_t KEYPlaceholderStyle::find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->placeholderStyles[ident];
}

KEYSlideStyle::KEYSlideStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

IWORKStylePtr_t KEYSlideStyle::find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->slideStyles[ident];
}

KEYTabularStyle::KEYTabularStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

IWORKStylePtr_t KEYTabularStyle::find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->tabularStyles[ident];
}

KEYVectorStyle::KEYVectorStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KEYStyleBase(props, ident, parentIdent)
{
}

IWORKStylePtr_t KEYVectorStyle::find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->vectorStyles[ident];
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
