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

#include "IWORKProperties.h"

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
  {
    ETONYEK_DEBUG_MSG(("IWORKStyle::find: can not find parent %s\n", m_parentIdent.get().c_str()));
    return false;
  }
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

const boost::optional<std::string> &IWORKStyle::getParentIdent() const
{
  return m_parentIdent;
}

const IWORKStylePtr_t IWORKStyle::getParent() const
{
  return m_parent;
}

void IWORKStyle::createListLevelStyles()
{
  using namespace property;
  if (m_props.has<ListLevelStyles>(false))
    return;
  std::deque<IWORKListLabelGeometry> const emptyLabelGeometries;
  std::deque<IWORKListLabelTypeInfo_t> const emptyTypeInfos;
  std::deque<double> const emptyDoubleQue;
  std::deque<IWORKListLabelGeometry> const &labelGeometries=
    has<ListLabelGeometries>() ? get<ListLabelGeometries>() : emptyLabelGeometries;
  std::deque<double> const &labelIndents=
    has<ListLabelIndents>() ? get<ListLabelIndents>() : emptyDoubleQue;
  std::deque<double> const &textIndents=
    has<ListTextIndents>() ? get<ListTextIndents>() : emptyDoubleQue;
  std::deque<IWORKListLabelTypeInfo_t> const &typeInfos=
    has<ListLabelTypes>() ? get<ListLabelTypes>() : emptyTypeInfos;
  using std::max;

  const std::size_t levels = (max)((max)(labelGeometries.size(), typeInfos.size()),
                                   (max)(labelIndents.size(), textIndents.size()));
  std::deque<IWORKPropertyMap> levelProps(levels);
  for (std::size_t i = 0; i != levels; ++i)
  {
    if (i < labelGeometries.size())
      levelProps[i].put<ListLabelGeometry>(labelGeometries[i]);
    if (i < typeInfos.size())
      levelProps[i].put<ListLabelTypeInfo>(typeInfos[i]);
    if (i < labelIndents.size())
      levelProps[i].put<ListLabelIndent>(labelIndents[i]);
    if (i < textIndents.size())
      levelProps[i].put<ListTextIndent>(textIndents[i]);
  }
  IWORKListLevels_t levelsStyle;
  for (std::size_t i = 0; i != levels; ++i)
    levelsStyle[unsigned(i)] = std::make_shared<IWORKStyle>(levelProps[i], boost::none, boost::none);
  m_props.put<ListLevelStyles>(levelsStyle);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
