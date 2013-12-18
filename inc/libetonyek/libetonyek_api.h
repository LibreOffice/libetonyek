/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBETONYEK_LIBETONYEK_API_H_INCLUDED
#define LIBETONYEK_LIBETONYEK_API_H_INCLUDED

#ifdef DLL_EXPORT
#ifdef LIBETONYEK_BUILD
#define KEYAPI __declspec(dllexport)
#else
#define KEYAPI __declspec(dllimport)
#endif
#else
#define KEYAPI
#endif

#endif // LIBETONYEK_LIBETONYEK_API_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
