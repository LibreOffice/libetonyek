/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKFORMATELEMENT_H_INCLUDED
#define IWORKFORMATELEMENT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKXMLContextBase.h"

#include "IWORKPropertyMap.h"
#include "IWORKTypes.h"

namespace libetonyek
{
class IWORKDateTimeFormatElement : public IWORKXMLEmptyContextBase
{
public:
  IWORKDateTimeFormatElement(IWORKXMLParserState &state, boost::optional<IWORKDateTimeFormat> &value);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;

private:
  boost::optional<IWORKDateTimeFormat> &m_value;
};

class IWORKDurationFormatElement : public IWORKXMLEmptyContextBase
{
public:
  IWORKDurationFormatElement(IWORKXMLParserState &state, boost::optional<IWORKDurationFormat> &value);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;

private:
  boost::optional<IWORKDurationFormat> &m_value;
};

class IWORKNumberFormatElement : public IWORKXMLEmptyContextBase
{
public:
  IWORKNumberFormatElement(IWORKXMLParserState &state, boost::optional<IWORKNumberFormat> &value);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;

private:
  boost::optional<IWORKNumberFormat> &m_value;
};

}

#endif // IWORKFORMATELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
