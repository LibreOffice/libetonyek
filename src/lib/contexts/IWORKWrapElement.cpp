/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKWrapElement.h"

#include <boost/lexical_cast.hpp>

#include "libetonyek_xml.h"
#include "IWORKBezierElement.h"
#include "IWORKGeometryElement.h"
#include "IWORKPath.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{
namespace
{

class PathElement : public IWORKXMLEmptyContextBase
{
public:
  PathElement(IWORKXMLParserState &state, IWORKPathPtr_t &path);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
private:
  IWORKPathPtr_t &m_path;
};

PathElement::PathElement(IWORKXMLParserState &state, IWORKPathPtr_t &path)
  : IWORKXMLEmptyContextBase(state)
  , m_path(path)
{
}

void PathElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::path :
    try
    {
      m_path.reset(new IWORKPath(value));
    }
    catch (const IWORKPath::InvalidException &)
    {
      ETONYEK_DEBUG_MSG(("PathElement::attribute[IWORKWrapElement.cpp]: '%s' is not a valid path\n", value));
    }
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID : // USEME
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("PathElement::attribute[IWORKWrapElement.cpp]: unexpected attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t PathElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::bezier :
    return makeContext<IWORKBezierElement>(getState(), m_path);
  default:
    ETONYEK_DEBUG_MSG(("PathElement::element[IWORKWrapElement.cpp]: find unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}
}

IWORKWrapElement::IWORKWrapElement(IWORKXMLParserState &state, boost::optional<IWORKWrap> &wrap)
  : IWORKXMLElementContextBase(state)
  , m_wrap(wrap)
{
  m_wrap=IWORKWrap();
}

void IWORKWrapElement::attribute(const int /*name*/, const char *const /*value*/)
{
  ETONYEK_DEBUG_MSG(("IWORKWrapElement::attribute: find unknown attribute\n"));
}

IWORKXMLContextPtr_t IWORKWrapElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::path :
    return makeContext<PathElement>(getState(), get(m_wrap).m_path);
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState(), get(m_wrap).m_geometry);
  default:
    ETONYEK_DEBUG_MSG(("IWORKWrapElement::element: find unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
