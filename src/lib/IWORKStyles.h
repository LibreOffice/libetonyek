/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKSTYLES_H_INCLUDED
#define IWORKSTYLES_H_INCLUDED

#include <string>

#include "IWORKEnum.h"
#include "IWORKStyle_fwd.h"
#include "IWORKStyleStack.h"
#include "IWORKTypes.h"

namespace libetonyek
{

/** Represents a cell style.
  */
class IWORKCellStyle
{
public:
  IWORKCellStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack);

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleStack &m_stack;
};

/** Represents a character style.
  */
class IWORKCharacterStyle
{
public:
  IWORKCharacterStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack);

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
  const IWORKStyleStack &m_stack;
};

/** Represents a connection style.
  */
class IWORKConnectionStyle
{
public:
  IWORKConnectionStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack);

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleStack &m_stack;
};

/** Represents a graphic style.
  */
class IWORKGraphicStyle
{
public:
  IWORKGraphicStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack);

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleStack &m_stack;
};

/** Represents a list style.
  */
class IWORKListStyle
{
public:
  IWORKListStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack);

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleStack &m_stack;
};

/** Represents a paragraph style.
  */
class IWORKParagraphStyle : public IWORKCharacterStyle
{
public:
  IWORKParagraphStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack);

  boost::optional<IWORKAlignment> getAlignment() const;
  boost::optional<IWORKTabStops_t> getTabs() const;

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleStack &m_stack;
};

/** Represents a tabular style.
  */
class IWORKTabularStyle
{
public:
  IWORKTabularStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack);

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleStack &m_stack;
};

/** Represents a vector style.
  */
class IWORKVectorStyle
{
public:
  IWORKVectorStyle(const IWORKStylePtr_t &style, const IWORKStyleStack &stack);

private:
  const IWORKStylePtr_t &m_style;
  const IWORKStyleStack &m_stack;
};

}

#endif // IWORKSTYLES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
