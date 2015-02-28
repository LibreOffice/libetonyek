/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYOBJECT_H_INCLUDED
#define KEYOBJECT_H_INCLUDED

#include <deque>

#include <boost/shared_ptr.hpp>

#include <librevenge/librevenge.h>

namespace libetonyek
{

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
class KEYObject
{
public:
  virtual ~KEYObject() = 0;

  /** Draw the object using @c painter.
    *
    * @arg[in] painter the painter to use
    */
  virtual void draw(librevenge::RVNGPresentationInterface *painter) = 0;
};

typedef boost::shared_ptr<KEYObject> KEYObjectPtr_t;

/** A list of presentation objects.
  */
typedef std::deque<KEYObjectPtr_t> KEYObjectList_t;

/** Draw all objects of list @list using @c painter.
  *
  * @arg[in] list the object list
  * @arg[in] painter the painter to use
  */
void drawAll(const KEYObjectList_t &list, librevenge::RVNGPresentationInterface *painter);

}

#endif // KEYOBJECT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
