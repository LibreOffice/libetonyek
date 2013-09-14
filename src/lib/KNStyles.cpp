/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KNStyles.h"

namespace libkeynote
{

bool KNStyleBase::link()
{
  // TODO: implement me
  return false;
}

void KNStyleBase::flatten()
{
  // TODO: implement me
}

KNStyleBase::KNStyleBase(const KNPropertyMap &props, const std::string &ident, const std::string &parentIdent)
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

KNCellStyle::KNCellStyle(const KNPropertyMap &props, const std::string &ident, const std::string &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNCharacterStyle::KNCharacterStyle(const KNPropertyMap &props, const std::string &ident, const std::string &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNConnectionStyle::KNConnectionStyle(const KNPropertyMap &props, const std::string &ident, const std::string &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNGraphicStyle::KNGraphicStyle(const KNPropertyMap &props, const std::string &ident, const std::string &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNLayoutStyle::KNLayoutStyle(const KNPropertyMap &props, const std::string &ident, const std::string &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNListStyle::KNListStyle(const KNPropertyMap &props, const std::string &ident, const std::string &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNParagraphStyle::KNParagraphStyle(const KNPropertyMap &props, const std::string &ident, const std::string &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNPlaceholderStyle::KNPlaceholderStyle(const KNPropertyMap &props, const std::string &ident, const std::string &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNSlideStyle::KNSlideStyle(const KNPropertyMap &props, const std::string &ident, const std::string &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNTabularStyle::KNTabularStyle(const KNPropertyMap &props, const std::string &ident, const std::string &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

KNVectorStyle::KNVectorStyle(const KNPropertyMap &props, const std::string &ident, const std::string &parentIdent)
  : KNStyleBase(props, ident, parentIdent)
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
