/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY2PARSERUTILS_H_INCLUDED
#define KEY2PARSERUTILS_H_INCLUDED

#include <utility>

#include <boost/optional.hpp>

#include "KEYTypes_fwd.h"

namespace libetonyek
{

class KEYXMLReader;

/** A "mixin" containing a set of helper functions used in parser classes.
  *
  * @seealso KEY2Parser, KEY2StyleParser
  */
class KEY2ParserUtils
{
  // disable copying
  KEY2ParserUtils(const KEY2ParserUtils &other);
  KEY2ParserUtils &operator=(const KEY2ParserUtils &other);

public:
  virtual ~KN2ParserUtils();

  /** Read sfa:ID attribute of an element.
    *
    * ID is expected to be the only attribute.
    *
    * @arg[in] reader the reader
    * @returns the ID
    */
  static boost::optional<ID_t> readID(const KEYXMLReader &reader);

  /** Read a reference element.
    *
    * It is expected that the element is empty and does have a single
    * attribute: sfa:IDREF.
    *
    * @arg[in] reader the reader
    * @returns the value of the sfa:IDREF attribute
    * @throws a GenericException if sfa:IDREF attribute is not present
    */
  static ID_t readRef(const KEYXMLReader &reader);

  /** Read sf:point element.
    *
    * @arg[in] reader the reader
    * @returns a pair of "coordinates"
    */
  static std::pair<boost::optional<double>, boost::optional<double> > readPoint(const KEYXMLReader &reader);

  /** Read sf:position element.
    *
    * @arg[in] reader the reader
    * @returns the position
    */
  static KEYPosition readPosition(const KEYXMLReader &reader);

  /** Read sf:size element.
    *
    * @arg[in] reader the reader
    * @returns the size
    */
  static KEYSize readSize(const KEYXMLReader &reader);

  /** Convert string value to bool.
    *
    * @arg value the string
    * @returns the boolean value of the string
    */
  static bool bool_cast(const char *value);

  static double deg2rad(double value);

protected:
  KEY2ParserUtils();
};

}

#endif // KEY2PARSERUTILS_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
