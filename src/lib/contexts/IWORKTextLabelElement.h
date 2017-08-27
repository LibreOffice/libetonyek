/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTEXTLABELELEMENT_H_INCLUDED
#define IWORKTEXTLABELELEMENT_H_INCLUDED

#include "IWORKXMLContextBase.h"

#include "IWORKTypes.h"

namespace libetonyek
{


class IWORKTextLabelElement : public IWORKXMLEmptyContextBase
{
public:
  IWORKTextLabelElement(IWORKXMLParserState &state, boost::optional<IWORKListLabelTypeInfo_t> &value);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;

private:
  boost::optional<IWORKListLabelTypeInfo_t> &m_value;
  bool m_bullet;
  boost::optional<IWORKLabelNumFormat> m_type;
  std::string m_format;
  boost::optional<unsigned> m_first;
};

}

#endif // IWORKTEXTLABELELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
