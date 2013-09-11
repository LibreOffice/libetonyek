/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNPATH_H_INCLUDED
#define KNPATH_H_INCLUDED

#include <deque>
#include <string>

#include "KNObject.h"
#include "KNPath_fwd.h"

namespace libkeynote
{

class KNPath
{
public:
  class Element;

public:
  KNPath();
  explicit KNPath(const std::string &path);
  KNPath(const KNPath &other);
  ~KNPath();
  KNPath &operator=(const KNPath &other);

  void swap(KNPath &other);

  void clear();

  void appendMoveTo(double x, double y);
  void appendLineTo(double x, double y);
  void appendCurveTo(double x1, double y1, double x2, double y2, double x, double y);
  void appendClose();

  /** Create SVG representation of this path.
    */
  std::string toSvg() const;

private:
  std::deque<Element *> m_elements;
};

KNObjectPtr_t makeObject(const KNPathPtr_t &path);

}

#endif //  KNPATH_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
