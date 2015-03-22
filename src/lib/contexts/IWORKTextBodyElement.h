/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTEXTBODYELEMENT_H_INCLUDED
#define IWORKTEXTBODYELEMENT_H_INCLUDED

#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKTextBodyElement : public IWORKXMLElementContextBase
{
public:
  explicit IWORKTextBodyElement(IWORKXMLParserState &state);

protected:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  bool m_layout;
  bool m_para;
};

}

#endif // IWORKTEXTBODYELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
