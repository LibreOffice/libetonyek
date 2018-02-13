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
#include <memory>

#include "libetonyek_xml.h"
#include "IWORKBezierElement.h"
#include "IWORKDictionary.h"
#include "IWORKGeometryElement.h"
#include "IWORKPath.h"
#include "IWORKToken.h"
#include "IWORKTokenizer.h"
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
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
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
      m_path = std::make_shared<IWORKPath>(value);
    }
    catch (const IWORKPath::InvalidException &)
    {
      ETONYEK_DEBUG_MSG(("PathElement::attribute[IWORKWrapElement.cpp]: '%s' is not a valid path\n", value));
    }
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID : // USEME
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::version :
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

IWORKExternalTextWrapElement::IWORKExternalTextWrapElement(IWORKXMLParserState &state, boost::optional<IWORKExternalTextWrap> &wrap)
  : IWORKXMLElementContextBase(state)
  , m_wrap(wrap)
{
  m_wrap=IWORKExternalTextWrap();
}

void IWORKExternalTextWrapElement::attribute(int name, const char *value)
{
  try
  {
    switch (name)
    {
    case IWORKToken::NS_URI_SFA | IWORKToken::ID :
      IWORKXMLElementContextBase::attribute(name, value);
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::attachment_wrap_type :
      switch (getState().getTokenizer().getId(value))
      {
      case IWORKToken::aligned:
        get(m_wrap).m_aligned = true;
        break;
      case IWORKToken::unaligned:
        get(m_wrap).m_aligned = false;
        break;
      default:
        ETONYEK_DEBUG_MSG(("IWORKExternalTextWrapElement::attribute[IWORKWrapElement.cpp]: find unknown attachment type value=%s\n", value));
        break;
      }
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::alpha_threshold :
      get(m_wrap).m_alphaThreshold=double_cast(value);
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::direction :
      switch (getState().getTokenizer().getId(value))
      {
      case IWORKToken::both :
        get(m_wrap).m_direction=IWORK_WRAP_DIRECTION_BOTH;
        break;
      case IWORKToken::left :
        get(m_wrap).m_direction=IWORK_WRAP_DIRECTION_LEFT;
        break;
      case IWORKToken::right :
        get(m_wrap).m_direction=IWORK_WRAP_DIRECTION_RIGHT;
        break;
      default:
        ETONYEK_DEBUG_MSG(("IWORKExternalTextWrapElement::attribute[IWORKWrapElement.cpp]: find unknown direction value=%s\n", value));
        break;
      }
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::floating_wrap_enabled :
      get(m_wrap).m_floatingWrapEnabled=bool_cast(value);
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::floating_wrap_type :
      switch (getState().getTokenizer().getId(value))
      {
      case IWORKToken::directional:
        get(m_wrap).m_floatingType = IWORK_WRAP_TYPE_DIRECTIONAL;
        break;
      case IWORKToken::largest:
        get(m_wrap).m_floatingType = IWORK_WRAP_TYPE_LARGEST;
        break;
      case IWORKToken::neither:
        get(m_wrap).m_floatingType = IWORK_WRAP_TYPE_NEITHER;
        break;
      default:
        ETONYEK_DEBUG_MSG(("IWORKExternalTextWrapElement::attribute[IWORKWrapElement.cpp]: find unknown floating type value=%s\n", value));
        break;
      }
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::inline_wrap_enabled :
      get(m_wrap).m_inlineWrapEnabled=bool_cast(value);
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::margin :
      get(m_wrap).m_margin=double_cast(value);
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::wrap_style :
      switch (getState().getTokenizer().getId(value))
      {
      case IWORKToken::regular :
        get(m_wrap).m_style=IWORK_WRAP_STYLE_REGULAR;
        break;
      case IWORKToken::tight :
        get(m_wrap).m_style=IWORK_WRAP_STYLE_TIGHT;
        break;
      default:
        ETONYEK_DEBUG_MSG(("IWORKExternalTextWrapElement::attribute[IWORKWrapElement.cpp]: find unknown style value=%s\n", value));
        break;
      }
      break;
    default:
      ETONYEK_DEBUG_MSG(("IWORKExternalTextWrapElement::attribute[IWORKWrapElement.cpp]: find unknown attribute\n"));
      break;
    }
  }
  catch (...)
  {
    ETONYEK_DEBUG_MSG(("IWORKExternalTextWrapElement::attribute[IWORKWrapElement.cpp]: catch some exception\n"));
  }

}

#ifndef DEBUG
IWORKXMLContextPtr_t IWORKExternalTextWrapElement::element(const int)
#else
IWORKXMLContextPtr_t IWORKExternalTextWrapElement::element(const int name)
#endif
{
  ETONYEK_DEBUG_MSG(("IWORKExternalTextWrapElement::element[IWORKWrapElement.cpp]: find unknown element %d\n", name));

  return IWORKXMLContextPtr_t();
}

void IWORKExternalTextWrapElement::endOfElement()
{
  if (getId())
    getState().getDictionary().m_externalTextWraps[get(getId())]=get(m_wrap);
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
