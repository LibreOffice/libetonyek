/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <vector>

#include <libetonyek/KEYStringVector.h>

namespace libetonyek
{

class KEYStringVectorImpl
{
public:
  KEYStringVectorImpl() : m_strings() {}
  KEYStringVectorImpl(const KEYStringVectorImpl &impl) : m_strings(impl.m_strings) {}
  ~KEYStringVectorImpl() {}

  std::vector<WPXString> m_strings;
};

KEYStringVector::KEYStringVector()
  : m_pImpl(new KEYStringVectorImpl())
{
}

KEYStringVector::KEYStringVector(const KEYStringVector &vec)
  : m_pImpl(new KEYStringVectorImpl(*(vec.m_pImpl)))
{
}

KEYStringVector::~KEYStringVector()
{
}

KEYStringVector &KEYStringVector::operator=(const KEYStringVector &vec)
{
  if (m_pImpl)
    delete m_pImpl;
  m_pImpl = new KEYStringVectorImpl(*(vec.m_pImpl));
  return *this;
}

unsigned KEYStringVector::size() const
{
  return (unsigned)(m_pImpl->m_strings.size());
}

bool KEYStringVector::empty() const
{
  return m_pImpl->m_strings.empty();
}

const WPXString &KEYStringVector::operator[](unsigned idx) const
{
  return m_pImpl->m_strings[idx];
}

void KEYStringVector::append(const WPXString &str)
{
  m_pImpl->m_strings.push_back(str);
}

void KEYStringVector::clear()
{
  m_pImpl->m_strings.clear();
}

} // namespace libetonyek

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
