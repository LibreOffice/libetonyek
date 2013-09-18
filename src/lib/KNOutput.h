/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNOUTPUT_H_INCLUDED
#define KNOUTPUT_H_INCLUDED

#include <libwpg/libwpg.h>

#include "KNStyle.h"
#include "KNTransformation.h"

namespace libkeynote
{

class KNStyleContext;

/** A context for drawing instances of @c KNObject.
  */
class KNOutput
{
public:
  /** Create an initial @c KNOutput.
    *
    * This should only be used once at the beginning of an output
    * operation.
    *
    * @arg[in] painter a painter
    * @arg[in] context a style context
    */
  KNOutput(libwpg::WPGPaintInterface *painter, KNStyleContext &context);

  /** Apply a transformation to the current output.
    *
    * @arg[in] output the current output
    * @arg[in] tr a transformation
    */
  KNOutput(const KNOutput &output, const KNTransformation &tr);

  /** Apply a style to the current output.
    *
    * The style is pushed to the style context.
    *
    * @arg[in] output the current output
    * @arg[in] style a style
    *
    * @seealso KNStyleContext
    */
  KNOutput(const KNOutput &output, const KNStylePtr_t &style);

  /** Apply a transformation and a style to the current output.
    *
    * The style is pushed to the style context.
    *
    * @arg[in] output the current output
    * @arg[in] tr a transformation
    * @arg[in] style a style
    *
    * @seealso KNStyleContext
    */
  KNOutput(const KNOutput &output, const KNTransformation &tr, const KNStylePtr_t &style);

  /** Destroy a @c KNOutput instance.
    *
    * If this @c KNOutput instance pushed a style to the style context,
    * it will be popped.
    */
  ~KNOutput();

  /** Get the painter.
    *
    * @return the painter
    */
  libwpg::WPGPaintInterface *getPainter() const;

  /** Get the current transformation.
    *
    * @return the currently active transformation
    */
  const KNTransformation &getTransformation() const;

  /** Get the style context.
    *
    * @return the style context.
    */
  const KNStyleContext &getStyleContext() const;

private:
  libwpg::WPGPaintInterface *const m_painter;
  KNStyleContext &m_styleContext;
  KNTransformation m_transformation;
  bool m_stylePushed;
};

}

#endif // KNOUTPUT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
