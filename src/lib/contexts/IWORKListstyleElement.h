/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKLISTSTYLEELEMENT_H_INCLUDED
#define IWORKLISTSTYLEELEMENT_H_INCLUDED

#include <deque>

#include "IWORKPropertyHandler.h"
#include "IWORKTypes.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKListstyleElement : public IWORKXMLElementContextBase, public IWORKPropertyHandler
{
public:
  IWORKListstyleElement(IWORKXMLParserState &state, IWORKListStyle_t &style);
  explicit IWORKListstyleElement(IWORKXMLParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

  IWORKXMLContextPtr_t handle(int name) override;

private:
  IWORKListStyle_t m_localStyle;
  IWORKListStyle_t &m_style;
  std::deque<IWORKListLabelGeometry> m_labelGeometries;
  std::deque<IWORKListLabelTypeInfo_t> m_typeInfos;
  std::deque<double> m_labelIndents;
  std::deque<double> m_textIndents;
};

}

#endif // IWORKLISTSTYLEELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
