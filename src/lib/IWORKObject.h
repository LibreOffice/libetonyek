/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKOBJECT_H_INCLUDED
#define IWORKOBJECT_H_INCLUDED

#include <deque>

#include <boost/shared_ptr.hpp>

namespace libetonyek
{

class IWORKDocumentInterface;

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
class IWORKObject
{
public:
  virtual ~IWORKObject() = 0;

  /** Draw the object using @c document.
    *
    * @arg[in] document the document to use
    */
  virtual void draw(IWORKDocumentInterface *document) = 0;
};

typedef boost::shared_ptr<IWORKObject> IWORKObjectPtr_t;

/** A list of presentation objects.
  */
typedef std::deque<IWORKObjectPtr_t> IWORKObjectList_t;

/** Draw all objects of list @list using @c document.
  *
  * @arg[in] list the object list
  * @arg[in] document the document to use
  */
void drawAll(const IWORKObjectList_t &list, IWORKDocumentInterface *document);

}

#endif // IWORKOBJECT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
