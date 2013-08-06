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

enum
{
  INVALID_TOKEN = 0,

  // namespaces
  XML_NS_KEY,
  XML_NS_SF,
  XML_NS_SFA,
  XML_NS_XSI,
  XML_XMLNS,

  // elements
  XML_PRESENTATION,

  // attributes
  XML_ID,
  XML_KIOSK_BUILD_DELAY,
  XML_KIOSK_SLIDE_DELAY,
  XML_MODE,
  XML_PLAY_MODE,
  XML_STICKY_VISIBILITY,
  XML_VERSION,

  // attribute values

  LAST_TOKEN
};

struct KNToken;

const KNToken *getKNToken(const char *str, const unsigned length);
const KNToken *getKNToken(const char *str);
const KNToken *getKNToken(const unsigned char *str);

int getKNTokenID(const KNToken &token);
int getKNTokenID(const KNToken *token);
int getKNTokenID(const char *str, const unsigned length);
int getKNTokenID(const char *str);
int getKNTokenID(const unsigned char *str);

const char *getKNTokenName(const KNToken &token);
const char *getKNTokenName(const KNToken *token);
const char *getKNTokenName(const char *str, const unsigned length);
const char *getKNTokenName(const char *str);
const char *getKNTokenName(const unsigned char *str);

}

#endif // KNTOKEN_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
