/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKSIZEELEMENT_H_INCLUDED
#define IWORKSIZEELEMENT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKTypes_fwd.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKSizeElement : public IWORKXMLEmptyContextBase
{
public:
  IWORKSizeElement(IWORKXMLParserState &state, boost::optional<IWORKSize> &size);

protected:
  virtual void attribute(int name, const char *value);
  virtual void endOfElement();

private:
  boost::optional<IWORKSize> &m_size;
  boost::optional<double> m_width;
  boost::optional<double> m_height;
};

}

#endif // IWORKSIZEELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
