/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKSTRINGELEMENT_H_INCLUDED
#define IWORKSTRINGELEMENT_H_INCLUDED

#include <string>

#include <boost/optional.hpp>

#include "IWORKTypes.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKStringElement : public IWORKXMLEmptyContextBase
{
public:
  IWORKStringElement(IWORKXMLParserState &state, boost::optional<std::string> &str);

private:
  void attribute(int name, const char *value) override;

private:
  boost::optional<std::string> &m_string;
};

}

#endif // IWORKSTRINGELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
