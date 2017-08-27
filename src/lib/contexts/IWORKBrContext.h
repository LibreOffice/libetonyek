/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKBRCONTEXT_H_INCLUDED
#define IWORKBRCONTEXT_H_INCLUDED

#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKBrContext : public IWORKXMLEmptyContextBase
{
public:
  explicit IWORKBrContext(IWORKXMLParserState &state);

private:
  void endOfElement() override;
};

}

#endif // IWORKBRCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
