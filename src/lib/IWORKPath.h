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

#include <deque>
#include <string>

#include <librevenge/librevenge.h>

#include "libetonyek_utils.h"
#include "IWORKPath_fwd.h"

namespace libetonyek
{

class IWORKTransformation;

class IWORKPath
{
  friend bool approxEqual(const IWORKPath &left, const IWORKPath &right, const double eps);

public:
  class Element;

public:
  IWORKPath();
  explicit IWORKPath(const std::string &path);
  IWORKPath(const IWORKPath &other);
  ~IWORKPath();
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
  void operator*=(const IWORKTransformation &tr);

  /** Create WPG representation of this path.
    */
  librevenge::RVNGPropertyListVector toWPG() const;

private:
  std::deque<Element *> m_elements;
  bool m_closed;
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
IWORKPath operator*(const IWORKPath &path, const IWORKTransformation &tr);

}

#endif //  IWORKPATH_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
