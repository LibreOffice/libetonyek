/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNSTYLES_H_INCLUDED
#define KNSTYLES_H_INCLUDED

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include "KNPropertyMap.h"
#include "KNStyle.h"
#include "KNStyles_fwd.h"

namespace libkeynote
{

/** A common implementation of a style.
  */
class KNStyleBase : public KNStyle
{
public:
  virtual bool link();

  virtual void flatten();

// ensure that this class cannot be instantiated
protected:
  KNStyleBase(const KNPropertyMap &props, const std::string &ident, const std::string &parentIdent);
  virtual ~KNStyleBase();

  virtual const KNPropertyMap &getPropertyMap() const;

private:
  KNPropertyMap m_props;

  const std::string m_ident;
  const std::string m_parentIdent;
  KNStylePtr_t m_parent;
};

/** Represents a cell style.
  */
class KNCellStyle : public KNStyleBase
{
public:
  explicit KNCellStyle(const KNPropertyMap &props, const std::string &ident = "", const std::string &parentIdent = "");
};

/** Represents a character style.
  */
class KNCharacterStyle : public KNStyleBase
{
public:
  explicit KNCharacterStyle(const KNPropertyMap &props, const std::string &ident = "", const std::string &parentIdent = "");
};

/** Represents a connection style.
  */
class KNConnectionStyle : public KNStyleBase
{
public:
  explicit KNConnectionStyle(const KNPropertyMap &props, const std::string &ident = "", const std::string &parentIdent = "");
};

/** Represents a graphic style.
  */
class KNGraphicStyle : public KNStyleBase
{
public:
  explicit KNGraphicStyle(const KNPropertyMap &props, const std::string &ident = "", const std::string &parentIdent = "");
};

/** Represents a layout style.
  */
class KNLayoutStyle : public KNStyleBase
{
public:
  explicit KNLayoutStyle(const KNPropertyMap &props, const std::string &ident = "", const std::string &parentIdent = "");
};

/** Represents a list style.
  */
class KNListStyle : public KNStyleBase
{
public:
  explicit KNListStyle(const KNPropertyMap &props, const std::string &ident = "", const std::string &parentIdent = "");
};

/** Represents a paragraph style.
  */
class KNParagraphStyle : public KNStyleBase
{
public:
  explicit KNParagraphStyle(const KNPropertyMap &props, const std::string &ident = "", const std::string &parentIdent = "");
};

/** Represents a placeholder style.
  */
class KNPlaceholderStyle : public KNStyleBase
{
public:
  explicit KNPlaceholderStyle(const KNPropertyMap &props, const std::string &ident = "", const std::string &parentIdent = "");
};

/** Represents a slide style.
  */
class KNSlideStyle : public KNStyleBase
{
public:
  explicit KNSlideStyle(const KNPropertyMap &props, const std::string &ident = "", const std::string &parentIdent = "");
};

/** Represents a tabular style.
  */
class KNTabularStyle : public KNStyleBase
{
public:
  explicit KNTabularStyle(const KNPropertyMap &props, const std::string &ident = "", const std::string &parentIdent = "");
};

/** Represents a vector style.
  */
class KNVectorStyle : public KNStyleBase
{
public:
  explicit KNVectorStyle(const KNPropertyMap &props, const std::string &ident = "", const std::string &parentIdent = "");
};

}

#endif // KNSTYLES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
