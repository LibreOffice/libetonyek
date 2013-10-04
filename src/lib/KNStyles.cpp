/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KNStyles.h"
#include "KNStylesheet.h"

using boost::any;
using boost::any_cast;

namespace libkeynote
{

bool KNStyleBase::link(const KNStylesheetPtr_t &stylesheet)
{
  if (m_parent || !m_parentIdent)
    return true;

  KNStylesheetPtr_t currentStylesheet = stylesheet;

  if (currentStylesheet && (m_ident == m_parentIdent))
    currentStylesheet = currentStylesheet->parent;

  if (!currentStylesheet)
    return false;

  m_parent = find(currentStylesheet, get(m_parentIdent));
  if (m_parent)
    m_props.setParent(&m_parent->getPropertyMap());

  return bool(m_parent);
}

void KNStyleBase::flatten()
{
  // TODO: implement me
}

KNStyleBase::KNStyleBase(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : m_props(props)
  , m_ident(ident)
  , m_parentIdent(parentIdent)
  , m_parent()
{
}

KNStyleBase::~KNStyleBase()
{
}

const KNPropertyMap &KNStyleBase::getPropertyMap() const
{
  return m_props;
}

KNCellStyle::KNCellStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNStylePtr_t KNCellStyle::find(const KNStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->cellStyles[ident];
}

KNCharacterStyle::KNCharacterStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNStylePtr_t KNCharacterStyle::find(const KNStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->characterStyles[ident];
}

KNConnectionStyle::KNConnectionStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNStylePtr_t KNConnectionStyle::find(const KNStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->connectionStyles[ident];
}

KNGraphicStyle::KNGraphicStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNStylePtr_t KNGraphicStyle::find(const KNStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->graphicStyles[ident];
}

KNLayoutStyle::KNLayoutStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNStylePtr_t KNLayoutStyle::find(const KNStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->layoutStyles[ident];
}

KNListStyle::KNListStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNStylePtr_t KNListStyle::find(const KNStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->listStyles[ident];
}

KNParagraphStyle::KNParagraphStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNStylePtr_t KNParagraphStyle::find(const KNStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->paragraphStyles[ident];
}

KNPlaceholderStyle::KNPlaceholderStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNGeometryPtr_t KNPlaceholderStyle::getGeometry(const KNStyleContext &context) const
{
  const char *const key = "geometry";
  any prop = getPropertyMap().get(key, true);
  if (prop.empty())
    prop = context.find(key, true);

  KNGeometryPtr_t value;
  if (!prop.empty())
    value = any_cast<KNGeometryPtr_t>(prop);

  return value;
}

KNStylePtr_t KNPlaceholderStyle::find(const KNStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->placeholderStyles[ident];
}

KNSlideStyle::KNSlideStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNStylePtr_t KNSlideStyle::find(const KNStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->slideStyles[ident];
}

KNTabularStyle::KNTabularStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNStylePtr_t KNTabularStyle::find(const KNStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->tabularStyles[ident];
}

KNVectorStyle::KNVectorStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNStylePtr_t KNVectorStyle::find(const KNStylesheetPtr_t &stylesheet, const std::string &ident) const
{
  return stylesheet->vectorStyles[ident];
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
