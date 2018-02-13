/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY2StyleContext.h"

#include <string>

#include <boost/optional.hpp>

#include "libetonyek_xml.h"
#include "IWORKNumericPropertyContext.h"
#include "IWORKPropertyContext.h"
#include "IWORKPropertyMapElement.h"
#include "IWORKStyle.h"
#include "IWORKToken.h"
#include "IWORKValueContext.h"
#include "KEYProperties.h"
#include "KEYTypes.h"
#include "KEY2Dictionary.h"
#include "KEY2ParserState.h"
#include "KEY2Token.h"

namespace libetonyek
{

namespace
{
template<class Property>
class NumericPropertyContext : public IWORKPropertyContext<Property, IWORKNumberElement<typename IWORKPropertyInfo<Property>::ValueType>, IWORKToken::NS_URI_SF | IWORKToken::number, KEY2Token::NS_URI_KEY | KEY2Token::number>
{
  typedef IWORKPropertyContext<Property, IWORKNumberElement<typename IWORKPropertyInfo<Property>::ValueType>, IWORKToken::NS_URI_SF | IWORKToken::number, KEY2Token::NS_URI_KEY | KEY2Token::number> Parent_t;

public:
  NumericPropertyContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);
};

typedef IWORKValueContext<bool, IWORKNumberElement<bool>, IWORKToken::NS_URI_SF | IWORKToken::number, KEY2Token::NS_URI_KEY | KEY2Token::number> BoolProperty;
typedef IWORKValueContext<double, IWORKNumberElement<double>, IWORKToken::NS_URI_SF | IWORKToken::number, KEY2Token::NS_URI_KEY | KEY2Token::number> DoubleProperty;
typedef IWORKValueContext<int, IWORKNumberElement<int>, IWORKToken::NS_URI_SF | IWORKToken::number, KEY2Token::NS_URI_KEY | KEY2Token::number> IntProperty;
typedef IWORKNumericPropertyContext<property::AnimationAutoPlay, KEY2Token::NS_URI_KEY | KEY2Token::number> AnimationAutoPlayPropertyElement;
typedef IWORKNumericPropertyContext<property::AnimationDelay, KEY2Token::NS_URI_KEY | KEY2Token::number> AnimationDelayPropertyElement;
typedef IWORKNumericPropertyContext<property::AnimationDuration, KEY2Token::NS_URI_KEY | KEY2Token::number> AnimationDurationPropertyElement;
}

namespace
{
class TransitionAttributesElement : public KEY2XMLElementContextBase
{
public:
  TransitionAttributesElement(KEY2ParserState &state, boost::optional<KEYTransition> &transition);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

  boost::optional<KEYTransition> &m_transition;
};

TransitionAttributesElement::TransitionAttributesElement(KEY2ParserState &state, boost::optional<KEYTransition> &transition)
  : KEY2XMLElementContextBase(state)
  , m_transition(transition)
{
  m_transition=KEYTransition();
}

void TransitionAttributesElement::attribute(int name, const char *value)
{
  switch (name)
  {
  case IWORKToken::ID | IWORKToken::NS_URI_SFA :
    KEY2XMLElementContextBase::attribute(name, value);
    break;
  case KEY2Token::type | KEY2Token::NS_URI_KEY :
  {
    std::string val(value);
    if (val=="none")
      get(m_transition).m_type=KEY_TRANSITION_STYLE_TYPE_NONE;
    else if (val=="inherited") // Checkme
      get(m_transition).m_type=KEY_TRANSITION_STYLE_TYPE_INHERITED;
    else
    {
      get(m_transition).m_type=KEY_TRANSITION_STYLE_TYPE_NAMED;
      get(m_transition).m_name=value;
      static bool first=true;
      if (first)
      {
        first=false;
        ETONYEK_DEBUG_MSG(("TransitionAttributesElement::attribute[KEY2StyleContext.cpp]: find some unexpected type=%s\n", value));
      }
    }
    break;
  }
  default :
    ETONYEK_DEBUG_MSG(("TransitionAttributesElement::attribute[KEY2StyleContext.cpp]: find some unknown attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t TransitionAttributesElement::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::animationAuto :
    return std::make_shared<BoolProperty>(getState(), get(m_transition).m_automatic);
  case KEY2Token::NS_URI_KEY | KEY2Token::animationDelay :
    return std::make_shared<DoubleProperty>(getState(), get(m_transition).m_delay);
  case KEY2Token::NS_URI_KEY | KEY2Token::animationDuration :
    return std::make_shared<DoubleProperty>(getState(), get(m_transition).m_duration);
  case KEY2Token::NS_URI_KEY | KEY2Token::direction :
    return std::make_shared<IntProperty>(getState(), get(m_transition).m_direction);
  case KEY2Token::NS_URI_KEY | KEY2Token::BGBuildDurationProperty :
    break;
  default:
  {
    /* also <key:com.apple.iWork.Keynote.BLTFadeThruColor.color>
       <key:com.apple.iWork.Keynote.BLTMosaicFlip.numberOfParticles>
       <key:com.apple.iWork.Keynote.BLTMosaicFlip.type>
       <key:com.apple.iWork.Keynote.BUKTwist.twist>
       <key:com.apple.iWork.Keynote.KLNSparkle.color>
       <key:com.apple.iWork.Keynote.KLNSwap.angle>
       <key:com.apple.iWork.Keynote.KLNSwap.spacing*/
    static bool first=true;
    if (first)
    {
      first=false;
      ETONYEK_DEBUG_MSG(("TransitionAttributesElement::element[KEY2StyleContext.cpp]: found some unexpected element\n"));
    }
    break;
  }
  }

  return IWORKXMLContextPtr_t();
}

void TransitionAttributesElement::endOfElement()
{
  if (getId())
    getState().getDictionary().m_transitions[get(getId())]=get(m_transition);
}
}

namespace
{
class TransitionElement : public KEY2XMLElementContextBase
{
public:
  TransitionElement(KEY2ParserState &state, boost::optional<KEYTransition> &transition);

private:
  IWORKXMLContextPtr_t element(int name) override;

  boost::optional<KEYTransition> &m_transition;
};

TransitionElement::TransitionElement(KEY2ParserState &state, boost::optional<KEYTransition> &transition)
  : KEY2XMLElementContextBase(state)
  , m_transition(transition)
{
}

IWORKXMLContextPtr_t TransitionElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::transition_attributes :
    return std::make_shared<TransitionAttributesElement>(getState(), m_transition);
  default:
    ETONYEK_DEBUG_MSG(("TransitionElement::element[KEY2StyleContext.cpp]: found unexpected element %d\n", name));
    break;
  }

  return IWORKXMLContextPtr_t();
}
}

namespace
{

class PropertyMapElement : public KEY2XMLElementContextBase
{
public:
  PropertyMapElement(KEY2ParserState &state, IWORKPropertyMap &propMap);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKPropertyMapElement m_base;
  IWORKPropertyMap &m_propMap;
  boost::optional<KEYTransition> m_transition;
};

PropertyMapElement::PropertyMapElement(KEY2ParserState &state, IWORKPropertyMap &propMap)
  : KEY2XMLElementContextBase(state)
  , m_base(state, propMap)
  , m_propMap(propMap)
  , m_transition()
{
}

IWORKXMLContextPtr_t PropertyMapElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::animationAutoPlay :
  case KEY2Token::NS_URI_KEY | KEY2Token::animationAutoPlay :
    return std::make_shared<AnimationAutoPlayPropertyElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::animationDelay :
  case KEY2Token::NS_URI_KEY | KEY2Token::animationDelay :
    return std::make_shared<AnimationDelayPropertyElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::animationDuration :
  case KEY2Token::NS_URI_KEY | KEY2Token::animationDuration :
    return std::make_shared<AnimationDurationPropertyElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::transition :
    return std::make_shared<TransitionElement>(getState(), m_transition);
  default:
    break;
  }

  return m_base.element(name);
}

void PropertyMapElement::endOfElement()
{
  if (m_transition)
    m_propMap.template put<property::Transition>(get(m_transition));
}
}

KEY2StyleContext::KEY2StyleContext(KEY2ParserState &state, IWORKStyleMap_t *const styleMap, const char *const defaultParent, const bool /*nested*/)
  : KEY2XMLElementContextBase(state)
  , m_props()
  , m_base(state, m_props, styleMap, defaultParent)
  , m_ident()
  , m_parentIdent()
{
}

void KEY2StyleContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::ident :
    m_ident = value;
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::parent_ident :
    m_parentIdent = value;
    break;
  default :
    KEY2XMLElementContextBase::attribute(name, value);
    break;
  }

  m_base.attribute(name, value);
}

IWORKXMLContextPtr_t KEY2StyleContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::property_map :
    return std::make_shared<PropertyMapElement>(getState(), m_props);
  default:
    ETONYEK_DEBUG_MSG(("KEY2StyleContext::element: found unexpected element %d\n", name));
  }

  return IWORKXMLContextPtr_t();
}

void KEY2StyleContext::endOfElement()
{
  m_base.endOfElement();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
