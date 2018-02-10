/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PAG1TEXTSTORAGEELEMENT_H_INCLUDED
#define PAG1TEXTSTORAGEELEMENT_H_INCLUDED

#include "PAGEnum.h"

#include "PAG1XMLContextBase.h"

#include "IWORKTextStorageElement.h"

namespace libetonyek
{

class PAG1TextStorageElement : public PAG1XMLContextBase<IWORKTextStorageElement>
{
public:
  explicit PAG1TextStorageElement(PAG1ParserState &state, const PAGTextStorageKind kind=PAG_TEXTSTORAGE_KIND_BASIC);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  PAGTextStorageKind m_kind;
  bool m_textOpened;
};

}

#endif // PAG1TEXTSTORAGEELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
