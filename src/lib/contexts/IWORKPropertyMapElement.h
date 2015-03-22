/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKPROPERTYMAPELEMENT_H_INCLUDED
#define IWORKPROPERTYMAPELEMENT_H_INCLUDED

#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKPropertyMap;
class IWORKXMLParserState;

class IWORKPropertyMapElement : public IWORKXMLElementContextBase
{
public:
  IWORKPropertyMapElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

  virtual IWORKXMLContextPtr_t element(int name);

private:
  IWORKPropertyMap &m_propMap;
};

}

#endif // IWORKPROPERTYMAPELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
