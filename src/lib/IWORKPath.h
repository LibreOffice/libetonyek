/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKPATH_H_INCLUDED
#define IWORKPATH_H_INCLUDED

#include "IWORKPath_fwd.h"

#include <string>

#include <glm/glm.hpp>

#include <librevenge/librevenge.h>

#include "libetonyek_utils.h"

namespace libetonyek
{

class IWORKPath
{
  friend bool approxEqual(const IWORKPath &left, const IWORKPath &right, const double eps);

public:
  struct Impl;
  struct InvalidException {};

public:
  IWORKPath();
  explicit IWORKPath(const std::string &path);
  IWORKPath(const IWORKPath &other);
  IWORKPath &operator=(const IWORKPath &other);

  void swap(IWORKPath &other);

  void clear();

  void appendMoveTo(double x, double y);
  void appendLineTo(double x, double y);
  void appendCurveTo(double x1, double y1, double x2, double y2, double x, double y);
  void appendClose();

  /** Transform all elements of the path.
    *
    * @arg[in] tr the transformation
    */
  void operator*=(const glm::dmat3 &tr);

  /* Create a string representation of this path.
   *
   * @return an SVG path representing this.
   */
  const std::string str() const;

  /** Create librevenge representation of this path.
    */
  void write(librevenge::RVNGPropertyListVector &vec) const;

private:
  std::shared_ptr<Impl> m_impl;
};

bool approxEqual(const IWORKPath &left, const IWORKPath &right, double eps = ETONYEK_EPSILON);
bool operator==(const IWORKPath &left, const IWORKPath &right);
bool operator!=(const IWORKPath &left, const IWORKPath &right);

/** Create a transformed path.
  *
  * @arg[in] path the path
  * @arg[in] tr the transformation
  * @returns the transformed path
  */
IWORKPath operator*(const IWORKPath &path, const glm::dmat3 &tr);

}

#endif //  IWORKPATH_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
