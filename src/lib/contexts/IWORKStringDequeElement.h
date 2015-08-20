/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKSTRINGDEQUELEMENT_H_INCLUDED
#define IWORKSTRINGDEQUEELEMENT_H_INCLUDED

#include <deque>
#include <string>

#include "IWORKTypes.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKStringDequeElement : public IWORKXMLEmptyContextBase
{
public:
  IWORKStringDequeElement(IWORKXMLParserState &state, std::deque<std::string> &deque);

private:
  virtual void attribute(int name, const char *value);

private:
  std::deque<std::string> &m_deque;
};

}

#endif // IWORKSTRINGDEQUEELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
