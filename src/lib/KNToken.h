/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNTOKEN_H_INCLUDED
#define KNTOKEN_H_INCLUDED

namespace libkeynote
{

#define KN_NO_NAMESPACE(ns) (0 == ns)

struct KNToken
{
  const char *name;
  int id;
};

}

#endif // KNTOKEN_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
