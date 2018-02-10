/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKSTROKEELEMENT_H_INCLUDED
#define IWORKSTROKEELEMENT_H_INCLUDED

#include <deque>
#include <boost/optional.hpp>

#include "IWORKXMLContextBase.h"

#include "IWORKTypes.h"

namespace libetonyek
{
class IWORKStrokeElement : public IWORKXMLElementContextBase
{
public:
  IWORKStrokeElement(IWORKXMLParserState &state, boost::optional<IWORKStroke> &value);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  boost::optional<IWORKStroke> &m_value;
  boost::optional<double> m_width;
  boost::optional<IWORKColor> m_color;
  boost::optional<IWORKLineCap> m_cap;
  boost::optional<IWORKLineJoin> m_join;
  boost::optional<IWORKPattern> m_pattern;
  boost::optional<ID_t> m_patternRef;
};

}

#endif // IWORKSTROKEELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
