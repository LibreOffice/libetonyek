/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYSTYLES_H_INCLUDED
#define KEYSTYLES_H_INCLUDED

#include <string>

#include "IWORKStyle_fwd.h"
#include "IWORKStyleStack.h"
#include "IWORKTypes.h"

namespace libetonyek
{

/** Represents a layout style.
  */
class KEYLayoutStyle
{
public:
  KEYLayoutStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack);

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleStack &m_stack;
};

/** Represents a placeholder style.
  */
class KEYPlaceholderStyle
{
public:
  KEYPlaceholderStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack);

  IWORKGeometryPtr_t getGeometry() const;

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleStack &m_stack;
};

/** Represents a slide style.
  */
class KEYSlideStyle
{
public:
  KEYSlideStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack);

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleStack &m_stack;
};

}

#endif // KEYSTYLES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
