/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKMEDIAELEMENT_H_INCLUDED
#define IWORKMEDIAELEMENT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKTypes.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKMediaElement : public IWORKXMLElementContextBase
{
public:
  explicit IWORKMediaElement(IWORKXMLParserState &state);

private:
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKMediaContentPtr_t m_content;
  IWORKDataPtr_t m_movieData;
  IWORKMediaContentPtr_t m_audioOnlyImage;
  IWORKMediaContentPtr_t m_posterImage;
  boost::optional<ID_t> m_audioOnlyImageRef;
  IWORKStylePtr_t m_style;
  IWORKGeometryPtr_t m_cropGeometry;
  boost::optional<IWORKWrap> m_wrap;
  boost::optional<IWORKSize> m_placeholderSize;
};

}

#endif // IWORKMEDIAELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
