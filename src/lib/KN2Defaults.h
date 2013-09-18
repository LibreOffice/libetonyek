/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KN2DEFAULTS_H_INCLUDED
#define KN2DEFAULTS_H_INCLUDED

#include "KNDefaults.h"

namespace libkeynote
{

/** Default values for versions 2--5 of the format.
  */
class KN2Defaults : public KNDefaults
{
public:
  virtual ~KN2Defaults();

  virtual void applyPresentationSize(boost::optional<KNSize> &size) const;
};

}

#endif // KN2DEFAULTS_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
