/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKFILTEREDIMAGEELEMENT_H_INCLUDED
#define IWORKFILTEREDIMAGEELEMENT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKTypes_fwd.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKFilteredImageElement : public IWORKXMLElementContextBase
{
public:
  IWORKFilteredImageElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKMediaContentPtr_t &m_content;
  boost::optional<ID_t> m_unfilteredId;
  IWORKMediaContentPtr_t m_unfiltered;
  IWORKMediaContentPtr_t m_filtered;
  IWORKMediaContentPtr_t m_leveled;
};

}

#endif // IWORKFILTEREDIMAGEELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
