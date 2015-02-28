/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYStyles.h"

#include "IWORKStyle.h"
#include "IWORKStylesheet.h"

using boost::any;
using boost::any_cast;
using boost::optional;

namespace libetonyek
{

namespace
{

template<typename T>
T extract(const any &property)
{
  T value;
  if (!property.empty())
    value = any_cast<T>(property);
  return value;
}

template<typename T>
optional<T> extractOptional(const any &property)
{
  optional<T> value;
  if (!property.empty())
    value = any_cast<T>(property);
  return value;
}

}

KEYLayoutStyle::KEYLayoutStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack)
  : m_style(style)
  , m_stack(stack)
{
}

KEYPlaceholderStyle::KEYPlaceholderStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack)
  : m_style(style)
  , m_stack(stack)
{
}

IWORKGeometryPtr_t KEYPlaceholderStyle::getGeometry() const
{
  return extract<IWORKGeometryPtr_t>(m_style->lookup("geometry", m_stack));
}

KEYSlideStyle::KEYSlideStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack)
  : m_style(style)
  , m_stack(stack)
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
