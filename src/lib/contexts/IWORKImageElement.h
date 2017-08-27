/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKIMAGEELEMENT_H_INCLUDED
#define IWORKIMAGEELEMENT_H_INCLUDED

#include "IWORKTypes.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKImageElement : public IWORKXMLElementContextBase
{
public:
  explicit IWORKImageElement(IWORKXMLParserState &state);

private:
  void startOfElement() override;
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKImagePtr_t m_image;
};

}

#endif // IWORKIMAGEELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
