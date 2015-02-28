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

#include "IWORKEnum.h"
#include "IWORKStyle_fwd.h"
#include "IWORKStyleContext.h"
#include "IWORKTypes.h"
#include "KEYTypes.h"

namespace libetonyek
{

/** Represents a cell style.
  */
class KEYCellStyle
{
public:
  KEYCellStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context);

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleContext &m_context;
};

/** Represents a character style.
  */
class KEYCharacterStyle
{
public:
  KEYCharacterStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context);

  boost::optional<IWORKBaseline> getBaseline() const;
  boost::optional<double> getBaselineShift() const;
  boost::optional<bool> getBold() const;
  boost::optional<IWORKCapitalization> getCapitalization() const;
  boost::optional<bool> getItalic() const;
  boost::optional<IWORKColor> getFontColor() const;
  boost::optional<double> getFontSize() const;
  boost::optional<std::string> getFontName() const;
  boost::optional<bool> getOutline() const;
  boost::optional<bool> getStrikethru() const;
  boost::optional<bool> getUnderline() const;

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleContext &m_context;
};

/** Represents a connection style.
  */
class KEYConnectionStyle
{
public:
  KEYConnectionStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context);

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleContext &m_context;
};

/** Represents a graphic style.
  */
class KEYGraphicStyle
{
public:
  KEYGraphicStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context);

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleContext &m_context;
};

/** Represents a layout style.
  */
class KEYLayoutStyle
{
public:
  KEYLayoutStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context);

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleContext &m_context;
};

/** Represents a list style.
  */
class KEYListStyle
{
public:
  KEYListStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context);

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleContext &m_context;
};

/** Represents a paragraph style.
  */
class KEYParagraphStyle : public KEYCharacterStyle
{
public:
  KEYParagraphStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context);

  boost::optional<IWORKAlignment> getAlignment() const;
  boost::optional<IWORKTabStops_t> getTabs() const;

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleContext &m_context;
};

/** Represents a placeholder style.
  */
class KEYPlaceholderStyle
{
public:
  KEYPlaceholderStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context);

  IWORKGeometryPtr_t getGeometry() const;

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleContext &m_context;
};

/** Represents a slide style.
  */
class KEYSlideStyle
{
public:
  KEYSlideStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context);

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleContext &m_context;
};

/** Represents a tabular style.
  */
class KEYTabularStyle
{
public:
  KEYTabularStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context);

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleContext &m_context;
};

/** Represents a vector style.
  */
class KEYVectorStyle
{
public:
  KEYVectorStyle(const IWORKStylePtr_t &style, const IWORKStyleContext &context);

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleContext &m_context;
};

}

#endif // KEYSTYLES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
