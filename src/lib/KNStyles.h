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

#include <boost/optional.hpp>
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
  KNStyleBase(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);
  virtual ~KNStyleBase();

  virtual const KNPropertyMap &getPropertyMap() const;

private:
  KNPropertyMap m_props;

  const boost::optional<std::string> m_ident;
  const boost::optional<std::string> m_parentIdent;
  KNStylePtr_t m_parent;
};

/** Represents a cell style.
  */
class KNCellStyle : public KNStyleBase
{
public:
  KNCellStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);
};

/** Represents a character style.
  */
class KNCharacterStyle : public KNStyleBase
{
public:
  KNCharacterStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);
};

/** Represents a connection style.
  */
class KNConnectionStyle : public KNStyleBase
{
public:
  KNConnectionStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);
};

/** Represents a graphic style.
  */
class KNGraphicStyle : public KNStyleBase
{
public:
  KNGraphicStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);
};

/** Represents a layout style.
  */
class KNLayoutStyle : public KNStyleBase
{
public:
  KNLayoutStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);
};

/** Represents a list style.
  */
class KNListStyle : public KNStyleBase
{
public:
  KNListStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);
};

/** Represents a paragraph style.
  */
class KNParagraphStyle : public KNStyleBase
{
public:
  KNParagraphStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);
};

/** Represents a placeholder style.
  */
class KNPlaceholderStyle : public KNStyleBase
{
public:
  KNPlaceholderStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);
};

/** Represents a slide style.
  */
class KNSlideStyle : public KNStyleBase
{
public:
  KNSlideStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);
};

/** Represents a tabular style.
  */
class KNTabularStyle : public KNStyleBase
{
public:
  KNTabularStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);
};

/** Represents a vector style.
  */
class KNVectorStyle : public KNStyleBase
{
public:
  KNVectorStyle(const KNPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);
};

}

#endif // KNSTYLES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
