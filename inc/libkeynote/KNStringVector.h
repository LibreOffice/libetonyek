/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBKEYNOTE_KNSTRINGVECTOR_H_INCLUDED
#define LIBKEYNOTE_KNSTRINGVECTOR_H_INCLUDED

#include <libwpd/libwpd.h>

namespace libkeynote
{

class KNStringVectorImpl;

class KNStringVector
{
public:
  KNStringVector();
  KNStringVector(const KNStringVector &vec);
  ~KNStringVector();

  KNStringVector &operator=(const KNStringVector &vec);

  unsigned size() const;
  bool empty() const;
  const WPXString &operator[](unsigned idx) const;
  void append(const WPXString &str);
  void clear();

private:
  KNStringVectorImpl *m_pImpl;
};

} // namespace libkeynote

#endif // LIBKEYNOTE_KNSTRINGVECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
