/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTOKENINFO_H_INCLUDED
#define IWORKTOKENINFO_H_INCLUDED

namespace libetonyek
{

template<class Parser>
struct IWORKTokenInfo
{
};

class IWORKParser;
class KEY1Parser;
class KEY2Parser;
class NUMParser;
class PAG1Parser;

template<>
struct IWORKTokenInfo<IWORKParser>
{
  static const int first = 1;
  static const int last = first + 1000;
};

template<>
struct IWORKTokenInfo<KEY1Parser>
{
  static const int first = IWORKTokenInfo<IWORKParser>::last + 1;
  static const int last = first + 1000;
};

template<>
struct IWORKTokenInfo<KEY2Parser>
{
  static const int first = IWORKTokenInfo<KEY1Parser>::last + 1;
  static const int last = first + 1000;
};

template<>
struct IWORKTokenInfo<NUMParser>
{
  static const int first = IWORKTokenInfo<KEY2Parser>::last + 1;
  static const int last = first + 1000;
};

template<>
struct IWORKTokenInfo<PAG1Parser>
{
  static const int first = IWORKTokenInfo<NUMParser>::last + 1;
  static const int last = first + 1000;
};

}

#endif // IWORKTOKENINFO_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
