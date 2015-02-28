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

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include "IWORKEnum.h"
#include "IWORKPropertyMap.h"
#include "IWORKStyle.h"
#include "IWORKTypes.h"
#include "KEYStyleContext.h"
#include "KEYStyles_fwd.h"
#include "KEYTypes.h"

namespace libetonyek
{

/** A common implementation of a style.
  */
class KEYStyleBase : public IWORKStyle
{
public:
  virtual bool link(const IWORKStylesheetPtr_t &stylesheet);

  virtual void flatten();

// ensure that this class cannot be instantiated
protected:
  KEYStyleBase(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);
  virtual ~KEYStyleBase();

  virtual const IWORKPropertyMap &getPropertyMap() const;

  virtual IWORKStylePtr_t find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const = 0;

  /** Look up a property in this style.
    *
    * @arg[in] property the property to look up
    * @arg[in] context the context for dynamic lookup
    * @returns the found property or empty boost::any
    */
  boost::any lookup(const char *property, const KEYStyleContext &context = KEYStyleContext()) const;

private:
  IWORKPropertyMap m_props;

  const boost::optional<std::string> m_ident;
  const boost::optional<std::string> m_parentIdent;
  IWORKStylePtr_t m_parent;
};

/** Represents a cell style.
  */
class KEYCellStyle : public KEYStyleBase
{
public:
  KEYCellStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);

private:
  virtual IWORKStylePtr_t find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const;
};

/** Represents a character style.
  */
class KEYCharacterStyle : public KEYStyleBase
{
public:
  KEYCharacterStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);

  boost::optional<IWORKBaseline> getBaseline(const KEYStyleContext &context = KEYStyleContext()) const;
  boost::optional<double> getBaselineShift(const KEYStyleContext &context = KEYStyleContext()) const;
  boost::optional<bool> getBold(const KEYStyleContext &context = KEYStyleContext()) const;
  boost::optional<IWORKCapitalization> getCapitalization(const KEYStyleContext &context = KEYStyleContext()) const;
  boost::optional<bool> getItalic(const KEYStyleContext &context = KEYStyleContext()) const;
  boost::optional<IWORKColor> getFontColor(const KEYStyleContext &context = KEYStyleContext()) const;
  boost::optional<double> getFontSize(const KEYStyleContext &context = KEYStyleContext()) const;
  boost::optional<std::string> getFontName(const KEYStyleContext &context = KEYStyleContext()) const;
  boost::optional<bool> getOutline(const KEYStyleContext &context = KEYStyleContext()) const;
  boost::optional<bool> getStrikethru(const KEYStyleContext &context = KEYStyleContext()) const;
  boost::optional<bool> getUnderline(const KEYStyleContext &context = KEYStyleContext()) const;

private:
  virtual IWORKStylePtr_t find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const;
};

/** Represents a connection style.
  */
class KEYConnectionStyle : public KEYStyleBase
{
public:
  KEYConnectionStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);

private:
  virtual IWORKStylePtr_t find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const;
};

/** Represents a graphic style.
  */
class KEYGraphicStyle : public KEYStyleBase
{
public:
  KEYGraphicStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);

private:
  virtual IWORKStylePtr_t find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const;
};

/** Represents a layout style.
  */
class KEYLayoutStyle : public KEYStyleBase
{
public:
  KEYLayoutStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);

private:
  virtual IWORKStylePtr_t find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const;
};

/** Represents a list style.
  */
class KEYListStyle : public KEYStyleBase
{
public:
  KEYListStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);

private:
  virtual IWORKStylePtr_t find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const;
};

/** Represents a paragraph style.
  */
class KEYParagraphStyle : public KEYCharacterStyle
{
public:
  KEYParagraphStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);

  boost::optional<IWORKAlignment> getAlignment(const KEYStyleContext &context = KEYStyleContext()) const;
  boost::optional<IWORKTabStops_t> getTabs(const KEYStyleContext &context = KEYStyleContext()) const;

private:
  virtual IWORKStylePtr_t find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const;
};

/** Represents a placeholder style.
  */
class KEYPlaceholderStyle : public KEYStyleBase
{
public:
  KEYPlaceholderStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);

  IWORKGeometryPtr_t getGeometry(const KEYStyleContext &context = KEYStyleContext()) const;

private:
  virtual IWORKStylePtr_t find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const;
};

/** Represents a slide style.
  */
class KEYSlideStyle : public KEYStyleBase
{
public:
  KEYSlideStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);

private:
  virtual IWORKStylePtr_t find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const;
};

/** Represents a tabular style.
  */
class KEYTabularStyle : public KEYStyleBase
{
public:
  KEYTabularStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);

private:
  virtual IWORKStylePtr_t find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const;
};

/** Represents a vector style.
  */
class KEYVectorStyle : public KEYStyleBase
{
public:
  KEYVectorStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);

private:
  virtual IWORKStylePtr_t find(const IWORKStylesheetPtr_t &stylesheet, const std::string &ident) const;
};

}

#endif // KEYSTYLES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
