/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PAG1FOOTNOTESELEMENT_H_INCLUDED
#define PAG1FOOTNOTESELEMENT_H_INCLUDED

#include "PAG1XMLContextBase.h"

namespace libetonyek
{

class PAG1FootnotesElement : public PAG1XMLElementContextBase
{
public:
  explicit PAG1FootnotesElement(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

}

#endif // PAG1FOOTNOTESELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
