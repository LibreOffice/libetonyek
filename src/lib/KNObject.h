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

class KNOutput;

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

  /** Draw the object using @c output as context.
    *
    * @arg[in] output the output to use
    */
  virtual void draw(const KNOutput &output) = 0;
};

typedef boost::shared_ptr<KNObject> KNObjectPtr_t;

/** A list of presentation objects.
  */
typedef std::deque<KNObjectPtr_t> KNObjectList_t;

/** Draw all objects of list @list using @c output as context.
  *
  * @arg[in] list the object list
  * @arg[in] output the output to use
  */
void drawAll(const KNObjectList_t &list, const KNOutput &output);

}

#endif // KNOBJECT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
