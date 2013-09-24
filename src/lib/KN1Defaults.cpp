/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KN1Defaults.h"

namespace libkeynote
{

KN1Defaults::~KN1Defaults()
{
}

void KN1Defaults::applyPresentationSize(boost::optional<KNSize> &size) const
{
  // TODO: implement me
  (void) size;
}

void KN1Defaults::applyGeometry(boost::optional<KNSize> &naturalSize, boost::optional<KNPosition> &position,
                                boost::optional<double> &angle,
                                boost::optional<double> &shearXAngle, boost::optional<double> &shearYAngle,
                                boost::optional<bool> &horizontalFlip, boost::optional<bool> &verticalFlip,
                                boost::optional<bool> &aspectRatioLocked, boost::optional<bool> &sizesLocked) const
{
  // TODO: implement me
  (void) naturalSize;
  (void) position;
  (void) angle;
  (void) shearXAngle;
  (void) shearYAngle;
  (void) horizontalFlip;
  (void) verticalFlip;
  (void) aspectRatioLocked;
  (void) sizesLocked;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
