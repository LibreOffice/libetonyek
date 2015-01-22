/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKENUM_H_INCLUDED
#define IWORKENUM_H_INCLUDED

namespace libetonyek
{

enum IWORKAlignment
{
  IWORK_ALIGNMENT_LEFT,
  IWORK_ALIGNMENT_RIGHT,
  IWORK_ALIGNMENT_CENTER,
  IWORK_ALIGNMENT_JUSTIFY
};

enum IWORKBaseline
{
  IWORK_BASELINE_NORMAL,
  IWORK_BASELINE_SUB,
  IWORK_BASELINE_SUPER
};

enum IWORKCapitalization
{
  IWORK_CAPITALIZATION_NONE,
  IWORK_CAPITALIZATION_ALL_CAPS,
  IWORK_CAPITALIZATION_SMALL_CAPS,
  IWORK_CAPITALIZATION_TITLE
};

}

#endif //  IWORKENUM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
