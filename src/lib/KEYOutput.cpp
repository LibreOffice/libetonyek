/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYOutput.h"
#include "KEYStyleContext.h"

namespace libkeynote
{

KEYOutput::KEYOutput(libwpg::WPGPaintInterface *const painter, KEYStyleContext &context)
  : m_painter(painter)
  , m_styleContext(context)
  , m_transformation()
  , m_stylePushed(false)
{
}

KEYOutput::KEYOutput(const KEYOutput &output, const KEYTransformation &tr)
  : m_painter(output.m_painter)
  , m_styleContext(output.m_styleContext)
  , m_transformation(tr *output.m_transformation)
  , m_stylePushed(false)
{
}

KEYOutput::KEYOutput(const KEYOutput &output, const KEYStylePtr_t &style)
  : m_painter(output.m_painter)
  , m_styleContext(output.m_styleContext)
  , m_transformation(output.m_transformation)
  , m_stylePushed(false)
{
  m_styleContext.push(style);
  m_stylePushed = true;
}

KEYOutput::KEYOutput(const KEYOutput &output, const KEYTransformation &tr, const KEYStylePtr_t &style)
  : m_painter(output.m_painter)
  , m_styleContext(output.m_styleContext)
  , m_transformation(tr *output.m_transformation)
  , m_stylePushed(false)
{
  m_styleContext.push(style);
  m_stylePushed = true;
}

KEYOutput::KEYOutput(const KEYOutput &other)
  : m_painter(other.m_painter)
  , m_styleContext(other.m_styleContext)
  , m_transformation(other.m_transformation)
  , m_stylePushed(false)
{
}

KEYOutput::~KEYOutput()
{
  if (m_stylePushed)
    m_styleContext.pop();
}

libwpg::WPGPaintInterface *KEYOutput::getPainter() const
{
  return m_painter;
}

const KEYTransformation &KEYOutput::getTransformation() const
{
  return m_transformation;
}

const KEYStyleContext &KEYOutput::getStyleContext() const
{
  return m_styleContext;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
