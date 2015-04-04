/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTOKENIZER_H_INCLUDED
#define IWORKTOKENIZER_H_INCLUDED

namespace libetonyek
{

class IWORKTokenizer
{
  // disable copying
  IWORKTokenizer(const IWORKTokenizer &);
  IWORKTokenizer &operator=(const IWORKTokenizer &);

public:
  virtual ~IWORKTokenizer() = 0;

  virtual int getId(const char *name) const = 0;
  virtual int getQualifiedId(const char *name, const char *ns) const = 0;

protected:
  IWORKTokenizer();
};

}

#endif // IWORKTOKENIZER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
