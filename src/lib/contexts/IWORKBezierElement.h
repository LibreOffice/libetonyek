/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKBEZIERELEMENT_H_INCLUDED
#define IWORKBEZIERELEMENT_H_INCLUDED

#include "IWORKXMLContextBase.h"
#include "IWORKPath_fwd.h"

namespace libetonyek
{

class IWORKBezierElement : public IWORKXMLEmptyContextBase
{
public:
  explicit IWORKBezierElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;

private:
  IWORKPathPtr_t m_path;
};

}

#endif // IWORKBEZIERELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
