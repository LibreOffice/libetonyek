/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYOUTPUT_H_INCLUDED
#define KEYOUTPUT_H_INCLUDED

#include <librevenge/librevenge.h>

#include "KEYStyle.h"

namespace libetonyek
{

class KEYStyleContext;

/** A context for drawing instances of @c KEYObject.
  */
class KEYOutput
{
  // disable assignment
  KEYOutput &operator=(const KEYOutput &other);

public:
  /** Create an initial @c KEYOutput.
    *
    * This should only be used once at the beginning of an output
    * operation.
    *
    * @arg[in] painter a painter
    */
  explicit KEYOutput(librevenge::RVNGPresentationInterface *painter);

  /** Create a @c KEYOutput as a copy of an existing one.
    *
    * @arg[in] other the copied @c KEYOutput instance
    */
  KEYOutput(const KEYOutput &other);

  /** Destroy a @c KEYOutput instance.
    *
    * If this @c KEYOutput instance pushed a style to the style context,
    * it will be popped.
    */
  ~KEYOutput();

  /** Get the painter.
    *
    * @return the painter
    */
  librevenge::RVNGPresentationInterface *getPainter() const;

private:
  librevenge::RVNGPresentationInterface *const m_painter;
};

}

#endif // KEYOUTPUT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
