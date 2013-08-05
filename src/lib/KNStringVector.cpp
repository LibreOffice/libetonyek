/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <vector>

#include <libkeynote/KNStringVector.h>

namespace libkeynote
{

class KNStringVectorImpl
{
public:
  KNStringVectorImpl() : m_strings() {}
  KNStringVectorImpl(const KNStringVectorImpl &impl) : m_strings(impl.m_strings) {}
  ~KNStringVectorImpl() {}

  std::vector<WPXString> m_strings;
};

KNStringVector::KNStringVector()
  : m_pImpl(new KNStringVectorImpl())
{
}

KNStringVector::KNStringVector(const KNStringVector &vec)
  : m_pImpl(new KNStringVectorImpl(*(vec.m_pImpl)))
{
}

KNStringVector::~KNStringVector()
{
}

KNStringVector &KNStringVector::operator=(const KNStringVector &vec)
{
  if (m_pImpl)
    delete m_pImpl;
  m_pImpl = new KNStringVectorImpl(*(vec.m_pImpl));
  return *this;
}

unsigned KNStringVector::size() const
{
  return (unsigned)(m_pImpl->m_strings.size());
}

bool KNStringVector::empty() const
{
  return m_pImpl->m_strings.empty();
}

const WPXString &KNStringVector::operator[](unsigned idx) const
{
  return m_pImpl->m_strings[idx];
}

void KNStringVector::append(const WPXString &str)
{
  m_pImpl->m_strings.push_back(str);
}

void KNStringVector::clear()
{
  m_pImpl->m_strings.clear();
}

} // namespace libkeynote

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
