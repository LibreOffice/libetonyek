/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKXMLCONTEXT_H_INCLUDED
#define IWORKXMLCONTEXT_H_INCLUDED

#include <memory>

namespace libetonyek
{

class IWORKXMLContext;

typedef std::shared_ptr<IWORKXMLContext> IWORKXMLContextPtr_t;

class IWORKXMLContext
{
public:
  virtual ~IWORKXMLContext() = 0;

  /** Signalize the start of an element.
    *
    * This can be used if initialization of the context needs virtual
    * function calls.
    */
  virtual void startOfElement() = 0;

  /** Process an attribute.
    *
    * Attributes are fed to the current element's context one by one.
    */
  virtual void attribute(int name, const char *value) = 0;

  /** Create a context for parsing a child element.
    *
    * @return A new context or an empty pointer if the child element cannot
    *         be handled.
    */
  virtual IWORKXMLContextPtr_t element(int name) = 0;

  /** Process textual content of an element.
    *
    * This function can be called more than once if the element has
    * mixed content.
    */
  virtual void text(const char *value) = 0;

  /** Signalize the end of an element.
    */
  virtual void endOfElement() = 0;
};

template<typename Context, typename Arg>
IWORKXMLContextPtr_t makeContext(Arg &arg)
{
  return IWORKXMLContextPtr_t(new Context(arg));
}

template<typename Context, typename Arg1, typename Arg2>
IWORKXMLContextPtr_t makeContext(Arg1 &arg1, Arg2 &arg2)
{
  return IWORKXMLContextPtr_t(new Context(arg1, arg2));
}

template<typename Context, typename Arg1, typename Arg2>
IWORKXMLContextPtr_t makeContext(Arg1 &arg1, const Arg2 &arg2)
{
  return IWORKXMLContextPtr_t(new Context(arg1, arg2));
}

template<typename Context, typename Arg1, typename Arg2, typename Arg3>
IWORKXMLContextPtr_t makeContext(Arg1 &arg1, Arg2 &arg2, const Arg3 &arg3)
{
  return IWORKXMLContextPtr_t(new Context(arg1, arg2, arg3));
}

template<typename Context, typename Arg1, typename Arg2, typename Arg3>
IWORKXMLContextPtr_t makeContext(Arg1 &arg1, Arg2 &arg2, Arg3 &arg3)
{
  return IWORKXMLContextPtr_t(new Context(arg1, arg2, arg3));
}

template<typename Context, typename Arg1, typename Arg2, typename Arg3>
IWORKXMLContextPtr_t makeContext(Arg1 &arg1, const Arg2 &arg2, Arg3 &arg3)
{
  return IWORKXMLContextPtr_t(new Context(arg1, arg2, arg3));
}

template<typename Context, typename Arg1, typename Arg2, typename Arg3>
IWORKXMLContextPtr_t makeContext(Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
{
  return IWORKXMLContextPtr_t(new Context(arg1, arg2, arg3));
}

template<typename Context, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
IWORKXMLContextPtr_t makeContext(Arg1 &arg1, Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
{
  return IWORKXMLContextPtr_t(new Context(arg1, arg2, arg3, arg4));
}

template<typename Context, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
IWORKXMLContextPtr_t makeContext(Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
{
  return IWORKXMLContextPtr_t(new Context(arg1, arg2, arg3, arg4));
}

}

#endif // IWORKXMLCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
