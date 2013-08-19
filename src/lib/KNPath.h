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

#include "KNTypes.h"

namespace libkeynote
{

class KNPath
{
public:
  class Element
  {
  public:
    virtual ~Element() = 0;

    virtual Element *clone() const = 0;
  };

public:
  KNPath();
  explicit KNPath(const KNGeometry &geometry);
  KNPath(const KNPath &other);
  ~KNPath();
  KNPath &operator=(const KNPath &other);

  void swap(KNPath &other);

  void clear();

  void setGeometry(const KNGeometry &geometry);

  void appendMoveTo(const KNPoint &point);
  void appendLineTo(const KNPoint &point);
  void appendCurveTo(const KNPoint &controlPoint1, const KNPoint &controlPoint2, const KNPoint &point);
  void appendClose();

private:
  std::deque<Element *> m_elements;
  KNGeometry m_geometry;
};

typedef boost::unordered_map<std::string, KNPath> KNPathMap_t;

}

#endif //  KNPATH_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
