/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKREFCONTEXT_H_INCLUDED
#define IWORKREFCONTEXT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKTypes_fwd.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKRefContext : public IWORKXMLEmptyContextBase
{
public:
  IWORKRefContext(IWORKXMLParserState &state, boost::optional<ID_t> &ref);

private:
  void endOfElement() override;

private:
  boost::optional<ID_t> &m_ref;
};

}

#endif // IWORKREFCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
