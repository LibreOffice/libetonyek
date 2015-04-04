/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKChainedTokenizer.h"

namespace libetonyek
{

IWORKChainedTokenizer::IWORKChainedTokenizer(const IWORKTokenizer &tokenizer, const IWORKTokenizer &next)
  : IWORKTokenizer()
  , m_tokenizer(tokenizer)
  , m_next(next)
{
}

int IWORKChainedTokenizer::getId(const char *name) const
{
  const int token = m_tokenizer.getId(name);
  return token ? token : m_next.getId(name);
}

int IWORKChainedTokenizer::getQualifiedId(const char *name, const char *ns) const
{
  const int token = m_tokenizer.getQualifiedId(name, ns);
  return token ? token : m_next.getQualifiedId(name, ns);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
