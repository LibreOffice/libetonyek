/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYPATH_H_INCLUDED
#define KEYPATH_H_INCLUDED

#include <deque>
#include <string>

#include <libwpd/libwpd.h>

#include "libetonyek_utils.h"
#include "KEYObject.h"
#include "KEYPath_fwd.h"

namespace libetonyek
{

class KEYTransformation;

class KEYPath
{
  friend bool approxEqual(const KEYPath &left, const KEYPath &right, const double eps);

public:
  class Element;

public:
  KEYPath();
  explicit KEYPath(const std::string &path);
  KEYPath(const KEYPath &other);
  ~KEYPath();
  KEYPath &operator=(const KEYPath &other);

  void swap(KEYPath &other);

  void clear();

  void appendMoveTo(double x, double y);
  void appendLineTo(double x, double y);
  void appendCurveTo(double x1, double y1, double x2, double y2, double x, double y);
  void appendClose();

  /** Transform all elements of the path.
    *
    * @arg[in] tr the transformation
    */
  void operator*=(const KEYTransformation &tr);

  /** Create WPG representation of this path.
    */
  WPXPropertyListVector toWPG() const;

private:
  std::deque<Element *> m_elements;
};

bool approxEqual(const KEYPath &left, const KEYPath &right, double eps = KEY_EPSILON);
bool operator==(const KEYPath &left, const KEYPath &right);
bool operator!=(const KEYPath &left, const KEYPath &right);

/** Create a transformed path.
  *
  * @arg[in] path the path
  * @arg[in] tr the transformation
  * @returns the transformed path
  */
KEYPath operator*(const KEYPath &path, const KEYTransformation &tr);

}

#endif //  KEYPATH_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
