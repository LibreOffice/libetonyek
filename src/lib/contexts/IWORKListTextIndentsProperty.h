/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKLISTTEXTINDENTSPROPERTY_H_INCLUDED
#define IWORKLISTTEXTINDENTSPROPERTY_H_INCLUDED

#include <deque>

#include <boost/optional.hpp>

#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKListTextIndentsProperty : public IWORKXMLElementContextBase
{
public:
  IWORKListTextIndentsProperty(IWORKXMLParserState &state, std::deque<double> &elements);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  std::deque<double> &m_elements;
  boost::optional<ID_t> m_ref;
};

}

#endif // IWORKLISTTEXTINDENTSPROPERTY_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
