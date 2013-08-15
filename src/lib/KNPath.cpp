/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include "KNPath.h"

namespace libkeynote
{

KNPath::KNPath()
  : m_geometry()
  , m_elements()
{
}

KNPath::KNPath(const KNGeometry &geometry)
  : m_geometry(geometry)
  , m_elements()
{
}

void KNPath::setGeometry(const KNGeometry &geometry)
{
  m_geometry = geometry;
}

void KNPath::appendMoveTo(const KNPoint &point)
{
  Element element;
  element.type = TYPE_MOVE_TO;
  element.value.point = point;
  m_elements.push_back(element);
}

void KNPath::appendLineTo(const KNPoint &point)
{
  Element element;
  element.type = TYPE_LINE_TO;
  element.value.point = point;
  m_elements.push_back(element);
}

void KNPath::appendCurveTo(const KNPoint &controlPoint1, const KNPoint &controlPoint2, const KNPoint &point)
{
  Element element;
  element.type = TYPE_CURVE_TO;
  element.curve.controlPoint1 = controlPoint1;
  element.curve.controlPoint2 = controlPoint2;
  element.curve.point = point;
  m_elements.push_back(element);
}

void KNPath::appendClose()
{
  Element element;
  element.type = TYPE_CLOSE;
  element.value.close = Close();
  m_elements.push_back(element);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
