/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKLineEndElement.h"

#include "libetonyek_xml.h"
#include "IWORKCollector.h"
#include "IWORKPath.h"
#include "IWORKPositionElement.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

namespace
{
class PathElement : public IWORKXMLEmptyContextBase
{
public:
  explicit PathElement(IWORKXMLParserState &state, boost::optional<std::string> &value);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;

  boost::optional<std::string> &m_value;
  boost::optional<ID_t> m_id;
};

PathElement::PathElement(IWORKXMLParserState &state, boost::optional<std::string> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
  , m_id()
{
}

void PathElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::ID | IWORKToken::NS_URI_SFA :
    m_id=value;
    break;
  case IWORKToken::path | IWORKToken::NS_URI_SFA :
    m_value=value;
    break;
  case IWORKToken::version | IWORKToken::NS_URI_SFA :
    break;
  default :
    ETONYEK_DEBUG_MSG(("PathElement::attribute[IWORKLineEndElement]: find unknown attribute\n"));
    IWORKXMLEmptyContextBase::attribute(name, value);
  }
}

IWORKXMLContextPtr_t PathElement::element(int /*name*/)
{
  ETONYEK_DEBUG_MSG(("PathElement::element[IWORKLineEndElement]: find unknown element\n"));
  return IWORKXMLContextPtr_t();
}
}

IWORKLineEndElement::IWORKLineEndElement(IWORKXMLParserState &state, boost::optional<IWORKMarker> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
  , m_id()
{
  m_value=IWORKMarker();
}

void IWORKLineEndElement::attribute(int name, const char *value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    /* checkme: do we need to store this element in the dictionary ?
       I never seen sf:line-end-ref so maybe not
     */
    m_id=value;
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::filled :
    m_value->m_filled = bool_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::identifier : // needed ?
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::scale :
    m_value->m_scale = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::path_join :
    m_value->m_pathJoin = int_cast(value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("IWORKLineEndElement::attribute: find unknown attribute\n"));
  }
}

IWORKXMLContextPtr_t IWORKLineEndElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::path :
    return std::make_shared<PathElement>(getState(), m_value->m_path);
  case IWORKToken::NS_URI_SF | IWORKToken::end_point :
    return std::make_shared<IWORKPositionElement>(getState(), m_value->m_endPoint);
  default:
    break;
  }

  ETONYEK_DEBUG_MSG(("IWORKLineEndElement::element: find unknown element\n"));
  return IWORKXMLContextPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
