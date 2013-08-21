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

#include "KNObject.h"
#include "KNPath_fwd.h"

namespace libkeynote
{

struct KNPoint;

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
  explicit KNPath(const KNGeometryPtr_t &geometry);
  KNPath(const KNPath &other);
  ~KNPath();
  KNPath &operator=(const KNPath &other);

  void swap(KNPath &other);

  void clear();

  void setGeometry(const KNGeometryPtr_t &geometry);

  void appendMoveTo(const KNPoint &point);
  void appendLineTo(const KNPoint &point);
  void appendCurveTo(const KNPoint &controlPoint1, const KNPoint &controlPoint2, const KNPoint &point);
  void appendClose();

private:
  std::deque<Element *> m_elements;
  KNGeometryPtr_t m_geometry;
};

KNObjectPtr_t makeObject(const KNPathPtr_t &path);

}

#endif //  KNPATH_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
