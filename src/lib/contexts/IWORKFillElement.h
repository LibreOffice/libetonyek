/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKFILLELEMENT_H_INCLUDED
#define IWORKFILLELEMENT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKXMLContextBase.h"

#include "IWORKPropertyMap.h"
#include "IWORKTypes.h"

namespace libetonyek
{
class IWORKFillElement : public IWORKXMLElementContextBase
{
public:
  IWORKFillElement(IWORKXMLParserState &state, boost::optional<IWORKFill> &value);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  boost::optional<IWORKFill> &m_value;
  boost::optional<IWORKColor> m_color;
  boost::optional<IWORKGradient> m_gradient;
  IWORKMediaContentPtr_t m_bitmap;
  boost::optional<ID_t> m_gradientRef;
  boost::optional<ID_t> m_texturedFillRef;
};
}

#endif // IWORKFILLELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
