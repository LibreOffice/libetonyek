/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KN2PARSERUTILS_H_INCLUDED
#define KN2PARSERUTILS_H_INCLUDED

#include <utility>

#include <boost/optional.hpp>

#include "KNTypes_fwd.h"

namespace libkeynote
{

class KNXMLReader;

/** A "mixin" containing a set of helper functions used in parser classes.
  *
  * @seealso KN2Parser, KN2StyleParser
  */
class KN2ParserUtils
{
  // disable copying
  KN2ParserUtils(const KN2ParserUtils &other);
  KN2ParserUtils &operator=(const KN2ParserUtils &other);

public:
  virtual ~KN2ParserUtils();

  /** Read sfa:ID attribute of an element.
    *
    * ID is expected to be the only attribute.
    *
    * @arg[in] reader the reader
    * @returns the ID
    */
  static boost::optional<ID_t> readID(const KNXMLReader &reader);

  /** Read a reference element.
    *
    * It is expected that the element is empty and does have a single
    * attribute: sfa:IDREF.
    *
    * @arg[in] reader the reader
    * @returns the value of the sfa:IDREF attribute
    * @throws a GenericException if sfa:IDREF attribute is not present
    */
  static ID_t readRef(const KNXMLReader &reader);

  /** Read sf:point element.
    *
    * @arg[in] reader the reader
    * @returns a pair of "coordinates"
    */
  static std::pair<boost::optional<double>, boost::optional<double> > readPoint(const KNXMLReader &reader);

  /** Read sf:position element.
    *
    * @arg[in] reader the reader
    * @returns the position
    */
  static KNPosition readPosition(const KNXMLReader &reader);

  /** Read sf:size element.
    *
    * @arg[in] reader the reader
    * @returns the size
    */
  static KNSize readSize(const KNXMLReader &reader);

  /** Convert string value to bool.
    *
    * @arg value the string
    * @returns the boolean value of the string
    */
  static bool bool_cast(const char *value);

  static double deg2rad(double value);

protected:
  KN2ParserUtils();
};

}

#endif // KN2PARSERUTILS_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
