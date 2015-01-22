/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY2DEFAULTS_H_INCLUDED
#define KEY2DEFAULTS_H_INCLUDED

#include "KEYDefaults.h"

namespace libetonyek
{

/** Default values for versions 2--5 of the format.
  */
class KEY2Defaults : public KEYDefaults
{
public:
  virtual ~KEY2Defaults();

  virtual void applyPresentationSize(boost::optional<IWORKSize> &size) const;

  virtual void applyGeometry(boost::optional<IWORKSize> &naturalSize, boost::optional<IWORKPosition> &position) const;
};

}

#endif // KEY2DEFAULTS_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
