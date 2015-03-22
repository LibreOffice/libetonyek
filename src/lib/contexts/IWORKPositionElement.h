/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKPOSITIONELEMENT_H_INCLUDED
#define IWORKPOSITIONELEMENT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKTypes_fwd.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKPositionElement : public IWORKXMLEmptyContextBase
{
public:
  IWORKPositionElement(IWORKXMLParserState &state, boost::optional<IWORKPosition> &position);

protected:
  virtual void attribute(int name, const char *value);
  virtual void endOfElement();

private:
  boost::optional<IWORKPosition> &m_position;
  boost::optional<double> m_x;
  boost::optional<double> m_y;
};

}

#endif // IWORKPOSITIONELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
