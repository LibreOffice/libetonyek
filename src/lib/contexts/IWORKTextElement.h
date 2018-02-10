/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTEXTELEMENT_H_INCLUDED
#define IWORKTEXTELEMENT_H_INCLUDED

#include "IWORKXMLContextBase.h"

namespace libetonyek
{

// NOTE: isn't it wonderful that there are two text elements in two
// different namespaces, but with the same schema?
class IWORKTextElement : public IWORKXMLElementContextBase
{
public:
  explicit IWORKTextElement(IWORKXMLParserState &state);

protected:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
};

}

#endif // IWORKTEXTELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
