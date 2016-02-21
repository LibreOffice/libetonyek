/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKCOLORELEMENT_H_INCLUDED
#define IWORKCOLORELEMENT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKTypes.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKColorElement : public IWORKXMLEmptyContextBase
{
public:
  IWORKColorElement(IWORKXMLParserState &state, boost::optional<IWORKColor> &color);

protected:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  boost::optional<IWORKColor> &m_color;
  std::string m_type;
  double m_r;
  double m_g;
  double m_b;
  double m_w;
  double m_c;
  double m_m;
  double m_y;
  double m_k;
  double m_a;
};

}

#endif // IWORKCOLORELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
