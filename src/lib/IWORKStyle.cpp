/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKStyle.h"

#include "libetonyek_utils.h"
#include "IWORKStyleStack.h"

namespace libetonyek
{

IWORKStyle::IWORKStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent)
  : m_props(props)
  , m_ident(ident)
  , m_parentIdent(parentIdent)
  , m_parent()
{
}

IWORKStyle::IWORKStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const IWORKStylePtr_t &parent)
  : m_props(props)
  , m_ident(ident)
  , m_parentIdent()
  , m_parent(parent)
{
  if (m_parent)
    m_props.setParent(&m_parent->getPropertyMap());
}

bool IWORKStyle::link(const IWORKStylesheetPtr_t &stylesheet)
{
  if (m_parent || !m_parentIdent)
    return true;

  IWORKStylesheetPtr_t currentStylesheet = stylesheet;

  if (currentStylesheet && (m_ident == m_parentIdent))
  {
    assert(currentStylesheet->parent != currentStylesheet);
    currentStylesheet = currentStylesheet->parent;
  }

  if (!currentStylesheet)
    return false;
  m_parent=currentStylesheet->find(m_parentIdent.get());
  if (m_parent)
    m_props.setParent(&m_parent->getPropertyMap());

  return bool(m_parent);
}

void IWORKStyle::flatten()
{
  // TODO: implement me
}

const IWORKPropertyMap &IWORKStyle::getPropertyMap() const
{
  return m_props;
}

const boost::optional<std::string> &IWORKStyle::getIdent() const
{
  return m_ident;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
