/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBKEYNOTE_KEYNOTEDOCUMENT_H_INCLUDED
#define LIBKEYNOTE_KEYNOTEDOCUMENT_H_INCLUDED

#include <libwpd/libwpd.h>
#include <libwpg/libwpg.h>

#include <libkeynote/KNStringVector.h>

class WPXInputStream;

namespace libkeynote
{

class KeyNoteDocument
{
public:

  static bool isSupported(WPXInputStream *input);

  static bool parse(WPXInputStream *input, libwpg::WPGPaintInterface *painter);

  static bool generateSVG(WPXInputStream *input, KNStringVector &output);
};

} // namespace libkeynote

#endif // LIBKEYNOTE_KEYNOTEDOCUMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
