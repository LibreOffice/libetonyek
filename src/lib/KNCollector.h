/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNCOLLECTOR_H_INCLUDED
#define KNCOLLECTOR_H_INCLUDED

namespace libwpg
{

class WPGPaintInterface;

} // namespace libwpg

namespace libkeynote
{

class KNCollector
{
public:
  explicit KNCollector(::libwpg::WPGPaintInterface *painter);
  virtual ~KNCollector();

  // collector functions

private:
  KNCollector(const KNCollector &);
  KNCollector &operator=(const KNCollector &);

  // helper functions

private:
  libwpg::WPGPaintInterface *m_painter;
};

} // namespace libkeynote

#endif // KNCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
