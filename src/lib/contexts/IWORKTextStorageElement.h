/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTEXTSTORAGEELEMENT_H_INCLUDED
#define IWORKTEXTSTORAGEELEMENT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKTypes_fwd.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKXMLParserState;

class IWORKTextStorageElement : public IWORKXMLElementContextBase
{
public:
  explicit IWORKTextStorageElement(IWORKXMLParserState &state);

protected:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

  void sendStylesheet();

private:
  boost::optional<ID_t> m_stylesheetId;
  bool m_hasStylesheet;
};

}

#endif // IWORKTEXTSTORAGEELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
