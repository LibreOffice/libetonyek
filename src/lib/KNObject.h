/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNOBJECT_H_INCLUDED
#define KNOBJECT_H_INCLUDED

#include <deque>

#include <boost/shared_ptr.hpp>

namespace libwpg
{
class WPGPaintInterface;
}

namespace libkeynote
{

struct KNDictionary;
class KNTransformation;

/** An interface for saved presentation objects.
  *
  * All objects on a slide's layer are collected into a list and then
  * painted in one sweep when the layer is finished. The collected
  * objects implement this interface.
  *
  * @note This is really only necessary for master pages' objects, but
  * it is convenient to use the same mechanism for processing regular
  * slides too.
  */
class KNObject
{
public:
  virtual ~KNObject() = 0;

  virtual void draw(libwpg::WPGPaintInterface *painter, const KNDictionary &dict, const KNTransformation &tr) = 0;
};

typedef boost::shared_ptr<KNObject> KNObjectPtr_t;

/** A list of presentation objects.
  */
typedef std::deque<KNObjectPtr_t> KNObjectList_t;

}

#endif // KNOBJECT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
