/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYENUM_H_INCLUDED
#define KEYENUM_H_INCLUDED

namespace libetonyek
{

enum KEYAlignment
{
  KEY_ALIGNMENT_LEFT,
  KEY_ALIGNMENT_RIGHT,
  KEY_ALIGNMENT_CENTER,
  KEY_ALIGNMENT_JUSTIFY
};

enum KEYBaseline
{
  KEY_BASELINE_NORMAL,
  KEY_BASELINE_SUB,
  KEY_BASELINE_SUPER
};

enum KEYCapitalization
{
  KEY_CAPITALIZATION_NONE,
  KEY_CAPITALIZATION_ALL_CAPS,
  KEY_CAPITALIZATION_SMALL_CAPS,
  KEY_CAPITALIZATION_TITLE
};

}

#endif //  KEYENUM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
