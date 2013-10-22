/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBETONYEK_KEYSTRINGVECTOR_H_INCLUDED
#define LIBETONYEK_KEYSTRINGVECTOR_H_INCLUDED

#include <libwpd/libwpd.h>

namespace libetonyek
{

class KEYStringVectorImpl;

class KEYStringVector
{
public:
  KEYStringVector();
  KEYStringVector(const KEYStringVector &vec);
  ~KEYStringVector();

  KEYStringVector &operator=(const KEYStringVector &vec);

  unsigned size() const;
  bool empty() const;
  const WPXString &operator[](unsigned idx) const;
  void append(const WPXString &str);
  void clear();

private:
  KEYStringVectorImpl *m_pImpl;
};

} // namespace libetonyek

#endif // LIBETONYEK_KEYSTRINGVECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
