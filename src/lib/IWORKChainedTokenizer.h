/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKCHAINEDTOKENIZER_H_INCLUDED
#define IWORKCHAINEDTOKENIZER_H_INCLUDED

#include "IWORKTokenizer.h"

namespace libetonyek
{

class IWORKChainedTokenizer : public IWORKTokenizer
{
public:
  IWORKChainedTokenizer(const IWORKTokenizer &tokenizer, const IWORKTokenizer &next);

  int getId(const char *name) const override;
  int getQualifiedId(const char *name, const char *ns) const override;

private:
  const IWORKTokenizer &m_tokenizer;
  const IWORKTokenizer &m_next;
};

}

#endif // IWORKCHAINEDTOKENIZER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
