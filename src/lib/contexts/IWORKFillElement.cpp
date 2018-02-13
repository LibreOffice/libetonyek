/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <deque>

#include "IWORKFillElement.h"

#include "IWORKColorElement.h"
#include "IWORKContainerContext.h"
#include "IWORKDictionary.h"
#include "IWORKFilteredImageElement.h"
#include "IWORKImageElement.h"
#include "IWORKDirectCollector.h"
#include "IWORKPositionElement.h"
#include "IWORKProperties.h"
#include "IWORKRefContext.h"
#include "IWORKSizeElement.h"
#include "IWORKToken.h"
#include "IWORKTokenizer.h"
#include "IWORKXMLParserState.h"

#include "libetonyek_utils.h"
#include "libetonyek_xml.h"

namespace libetonyek
{

namespace
{

class GradientStopElement : public IWORKXMLElementContextBase
{
public:
  GradientStopElement(IWORKXMLParserState &state, std::deque<IWORKGradientStop> &stops);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  std::deque<IWORKGradientStop> &m_stops;
  boost::optional<IWORKColor> m_color;
  boost::optional<double> m_fraction;
  boost::optional<double> m_inflection;
};

GradientStopElement::GradientStopElement(IWORKXMLParserState &state, std::deque<IWORKGradientStop> &stops)
  : IWORKXMLElementContextBase(state)
  , m_stops(stops)
  , m_color()
  , m_fraction()
  , m_inflection()
{
}

void GradientStopElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::fraction :
    m_fraction = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::inflection :
    m_inflection = double_cast(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("GradientStopElement::attribute[IWORKFillElement.cpp]: find unknown attribute\n"));
  }
}

IWORKXMLContextPtr_t GradientStopElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::color :
    return std::make_shared<IWORKColorElement>(getState(), m_color);
  default:
    ETONYEK_DEBUG_MSG(("GradientStopElement::element[IWORKFillElement.cpp]: find unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

void GradientStopElement::endOfElement()
{
  if (m_fraction)
  {
    m_stops.push_back(IWORKGradientStop());
    m_stops.back().m_color = get_optional_value_or(m_color, IWORKColor());
    m_stops.back().m_fraction = get(m_fraction);
    m_stops.back().m_inflection = get_optional_value_or(m_inflection, 0.5);

    if (getId())
      getState().getDictionary().m_gradientStops.insert(IWORKGradientStopMap_t::value_type(get(getId()),m_stops.back()));
  }
}

}

namespace
{

typedef IWORKContainerContext<IWORKGradientStop, GradientStopElement, IWORKDirectCollector, IWORKToken::NS_URI_SF | IWORKToken::gradient_stop, IWORKToken::NS_URI_SF | IWORKToken::gradient_stop_ref> StopsElement;

}

namespace
{

class AngleGradientElement : public IWORKXMLElementContextBase
{
public:
  AngleGradientElement(IWORKXMLParserState &state, boost::optional<IWORKGradient> &value);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  boost::optional<IWORKGradient> &m_value;
  boost::optional<IWORKGradientType> m_type;
  boost::optional<double> m_opacity;
  boost::optional<double> m_angle;
  std::deque<IWORKGradientStop> m_stops;
};

AngleGradientElement::AngleGradientElement(IWORKXMLParserState &state, boost::optional<IWORKGradient> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
  , m_type()
  , m_opacity()
  , m_angle()
  , m_stops()
{
}

void AngleGradientElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::angle :
    m_angle = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::opacity :
    m_opacity = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::type :
    switch (getState().getTokenizer().getId(value))
    {
    case IWORKToken::linear :
      m_type = IWORK_GRADIENT_TYPE_LINEAR;
      break;
    case IWORKToken::radial :
      m_type = IWORK_GRADIENT_TYPE_RADIAL;
      break;
    default:
      ETONYEK_DEBUG_MSG(("AngleGradientElement::attribute[IWORKFillElement.cpp]: unknown type attribute\n"));
    }
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("AngleGradientElement::attribute[IWORKFillElement.cpp]: unknown attribute\n"));
  }
}

IWORKXMLContextPtr_t AngleGradientElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::stops :
    return std::make_shared<StopsElement>(getState(), getState().getDictionary().m_gradientStops, m_stops);
  default:
    ETONYEK_DEBUG_MSG(("AngleGradientElement::element[IWORKFillElement.cpp]: unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

void AngleGradientElement::endOfElement()
{
  if (m_type && !m_stops.empty())
  {
    IWORKGradient value;
    value.m_type = get(m_type);
    value.m_angle = get(m_angle);
    value.m_stops = m_stops;
    m_value = value;
    if (getId())
      getState().getDictionary().m_gradients.insert(IWORKGradientMap_t::value_type(get(getId()),value));
  }
}

}

namespace
{

class TransformGradientElement : public IWORKXMLElementContextBase
{
public:
  TransformGradientElement(IWORKXMLParserState &state, boost::optional<IWORKGradient> &value);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  boost::optional<IWORKGradient> &m_value;
  boost::optional<double> m_opacity;
  boost::optional<IWORKGradientType> m_type;
  std::deque<IWORKGradientStop> m_stops;
  boost::optional<IWORKPosition> m_startPosition;
  boost::optional<IWORKPosition> m_endPosition;
  boost::optional<IWORKSize> m_baseSize;
};

TransformGradientElement::TransformGradientElement(IWORKXMLParserState &state, boost::optional<IWORKGradient> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
  , m_opacity()
  , m_type()
  , m_stops()
  , m_startPosition()
  , m_endPosition()
  , m_baseSize()
{
}

void TransformGradientElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::opacity :
    m_opacity = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::type :
    switch (getState().getTokenizer().getId(value))
    {
    case IWORKToken::linear :
      m_type = IWORK_GRADIENT_TYPE_LINEAR;
      break;
    case IWORKToken::radial :
      m_type = IWORK_GRADIENT_TYPE_RADIAL;
      break;
    default:
      ETONYEK_DEBUG_MSG(("TransformGradientElement::attribute[IWORKFillElement.cpp]: unknown type\n"));
    }
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("TransformGradientElement::attribute[IWORKFillElement.cpp]: unknown attribute\n"));
  }
}

IWORKXMLContextPtr_t TransformGradientElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::baseSize :
    return std::make_shared<IWORKSizeElement>(getState(), m_baseSize);
  case IWORKToken::NS_URI_SF | IWORKToken::end :
    return std::make_shared<IWORKPositionElement>(getState(), m_endPosition);
  case IWORKToken::NS_URI_SF | IWORKToken::stops :
    return std::make_shared<StopsElement>(getState(), getState().getDictionary().m_gradientStops, m_stops);
  case IWORKToken::NS_URI_SF | IWORKToken::start :
    return std::make_shared<IWORKPositionElement>(getState(), m_startPosition);
  default:
    ETONYEK_DEBUG_MSG(("TransformGradientElement::element[IWORKFillElement.cpp]: unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

void TransformGradientElement::endOfElement()
{
  if (m_type && !m_stops.empty())
  {
    IWORKGradient value;
    value.m_type = get(m_type);
    value.m_stops = m_stops;
    m_value = value;
    if (getId())
      getState().getDictionary().m_gradients.insert(IWORKGradientMap_t::value_type(get(getId()),value));
  }
}

}

namespace
{

class TexturedFillElement : public IWORKXMLElementContextBase
{
public:
  TexturedFillElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKMediaContentPtr_t &m_content;
  boost::optional<IWORKImageType> m_type;
  boost::optional<IWORKColor> m_color;
  boost::optional<ID_t> m_filteredImageRef;
  boost::optional<ID_t> m_imageRef;
};

TexturedFillElement::TexturedFillElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content)
  : IWORKXMLElementContextBase(state)
  , m_content(content)
  , m_type()
  , m_color()
  , m_filteredImageRef()
  , m_imageRef()
{
}

void TexturedFillElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_XSI | IWORKToken::type :
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::technique :
    switch (getState().getTokenizer().getId(value))
    {
    case IWORKToken::natural :
      m_type = IWORK_IMAGE_TYPE_ORIGINAL_SIZE;
      break;
    case IWORKToken::stretch :
      m_type = IWORK_IMAGE_TYPE_STRETCH;
      break;
    case IWORKToken::tile :
      m_type = IWORK_IMAGE_TYPE_TILE;
      break;
    case IWORKToken::fill :
      m_type = IWORK_IMAGE_TYPE_SCALE_TO_FILL;
      break;
    case IWORKToken::fit :
      m_type = IWORK_IMAGE_TYPE_SCALE_TO_FIT;
      break;
    default:
      ETONYEK_DEBUG_MSG(("TexturedFillElement::attribute[IWORKFillElement.cpp]: unknown technique\n"));
      break;
    }
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("TexturedFillElement::attribute[IWORKFillElement.cpp]: unknown attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t TexturedFillElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::color :
    return std::make_shared<IWORKColorElement>(getState(), m_color);
  case IWORKToken::NS_URI_SF | IWORKToken::filtered_image :
    return std::make_shared<IWORKFilteredImageElement>(getState(), m_content);
  case IWORKToken::NS_URI_SF | IWORKToken::filtered_image_ref :
    return std::make_shared<IWORKRefContext>(getState(), m_filteredImageRef);
  case IWORKToken::NS_URI_SF | IWORKToken::image :
    return std::make_shared<IWORKImageElement>(getState(), m_content);
  case IWORKToken::NS_URI_SF | IWORKToken::image_ref :
    return std::make_shared<IWORKRefContext>(getState(), m_imageRef);
  default:
    ETONYEK_DEBUG_MSG(("TexturedFillElement::element[IWORKFillElement.cpp]: unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void TexturedFillElement::endOfElement()
{
  if (m_filteredImageRef)
  {
    const IWORKMediaContentMap_t::const_iterator it = getState().getDictionary().m_filteredImages.find(get(m_filteredImageRef));
    if (it != getState().getDictionary().m_filteredImages.end())
      m_content = it->second;
    else
    {
      ETONYEK_DEBUG_MSG(("TexturedFillElement::endOfElement[IWORKFillElement.cpp]: can not find filtered image %s\n", get(m_filteredImageRef).c_str()));
    }
  }
  if (m_imageRef)
  {
    const IWORKMediaContentMap_t::const_iterator it = getState().getDictionary().m_images.find(get(m_imageRef));
    if (it != getState().getDictionary().m_images.end())
      m_content = it->second;
    else
    {
      ETONYEK_DEBUG_MSG(("TexturedFillElement::endOfElement[IWORKFillElement.cpp]: can not find image %s\n", get(m_imageRef).c_str()));
    }
  }
  if (bool(m_content))
  {
    if (m_type) m_content->m_type=get(m_type);
    if (m_color) m_content->m_fillColor=get(m_color);
    if (getId())
      getState().getDictionary().m_texturedFillImages.insert(IWORKMediaContentMap_t::value_type(get(getId()),m_content));
  }
  else
    m_content.reset();
}

}

IWORKFillElement::IWORKFillElement(IWORKXMLParserState &state, boost::optional<IWORKFill> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
  , m_color()
  , m_gradient()
  , m_bitmap()
  , m_gradientRef()
  , m_texturedFillRef()
{
}

IWORKXMLContextPtr_t IWORKFillElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::angle_gradient :
    return std::make_shared<AngleGradientElement>(getState(), m_gradient);
  case IWORKToken::NS_URI_SF | IWORKToken::angle_gradient_ref :
    return std::make_shared<IWORKRefContext>(getState(), m_gradientRef);
  case IWORKToken::NS_URI_SF | IWORKToken::color :
    return std::make_shared<IWORKColorElement>(getState(), m_color);
  case IWORKToken::NS_URI_SF | IWORKToken::texture_fill : // CHECKME: a dictionary
  case IWORKToken::NS_URI_SF | IWORKToken::texture_fill_ref : // CHECKME: ref to previous element
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::textured_fill :
    return std::make_shared<TexturedFillElement>(getState(), m_bitmap);
  case IWORKToken::NS_URI_SF | IWORKToken::textured_fill_ref :
    return std::make_shared<IWORKRefContext>(getState(), m_texturedFillRef);
  case IWORKToken::NS_URI_SF | IWORKToken::transform_gradient :
    return std::make_shared<TransformGradientElement>(getState(), m_gradient);
  case IWORKToken::NS_URI_SF | IWORKToken::null :
    break;
  default:
    ETONYEK_DEBUG_MSG(("IWORKFillElement::element: unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void IWORKFillElement::endOfElement()
{
  if (m_color)
    m_value=get(m_color);
  else if (m_gradient)
    m_value=get(m_gradient);
  else if (m_gradientRef)
  {
    const IWORKGradientMap_t::const_iterator it = getState().getDictionary().m_gradients.find(get(m_gradientRef));
    if (it != getState().getDictionary().m_gradients.end())
    {
      m_value = it->second;
    }
    else
    {
      ETONYEK_DEBUG_MSG(("IWORKFillElement::endOfElement: can not find gradient %s\n", get(m_gradientRef).c_str()));
    }
  }
  else if (m_bitmap)
    m_value=*m_bitmap;
  else if (m_texturedFillRef)
  {
    const IWORKMediaContentMap_t::const_iterator it = getState().getDictionary().m_texturedFillImages.find(get(m_texturedFillRef));
    if (it != getState().getDictionary().m_texturedFillImages.end())
    {
      if (it->second) m_value = *it->second;
    }
    else
    {
      ETONYEK_DEBUG_MSG(("IWORKFillElement::endOfElement: can not find textured fill %s\n", get(m_texturedFillRef).c_str()));
    }
  }
  if (getId() && m_value)
    getState().getDictionary().m_fills[get(getId())]=get(m_value);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
