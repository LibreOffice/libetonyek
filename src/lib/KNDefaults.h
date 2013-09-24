/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNDEFAULTS_H_INCLUDED
#define KNDEFAULTS_H_INCLUDED

#include <boost/optional.hpp>

#include "KNTypes.h"

namespace libkeynote
{

/** A representation of default values for various parts of the format.
  *
  * It serves to represent (and use) default values globally, not strewn
  * around over half a dozen source files, probably repeated. There are
  * two types of functions this class offers:
  * @li getX() - get the default value of @c X (e.g., paragraph style)
  * @li applyX(x...) - apply the default values for @c X for attributes
  *   passed by @c boost::optional, unless already set (e.g., shape size
  *   as @c width and @c height).
  */
class KNDefaults
{
public:
  virtual ~KNDefaults() = 0;

  virtual void applyPresentationSize(boost::optional<KNSize> &size) const = 0;

  virtual void applyGeometry(boost::optional<KNSize> &naturalSize, boost::optional<KNPosition> &position,
                             boost::optional<double> &angle,
                             boost::optional<double> &shearXAngle, boost::optional<double> &shearYAngle,
                             boost::optional<bool> &horizontalFlip, boost::optional<bool> &verticalFlip,
                             boost::optional<bool> &aspectRatioLocked, boost::optional<bool> &sizesLocked) const = 0;
};

}

#endif // KNDEFAULTS_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
