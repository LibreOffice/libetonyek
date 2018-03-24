/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <deque>
#include <memory>

#include "KEY1FillElement.h"

#include "IWORKParser.h"
#include "IWORKTokenizer.h"
#include "KEY1Dictionary.h"
#include "KEY1ParserState.h"
#include "KEY1StringConverter.h"
#include "KEY1Token.h"

#include "libetonyek_utils.h"
#include "libetonyek_xml.h"

namespace libetonyek
{
KEY1FillElement::KEY1FillElement(KEY1ParserState &state, boost::optional<IWORKFill> &fill)
  : KEY1XMLElementContextBase(state)
  , m_fill(fill)
  , m_type()
  , m_color()
  , m_imageName()
  , m_imageType()
{
}

void KEY1FillElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::fill_color :
    m_color=KEY1StringConverter<IWORKColor>::convert(value);
    break;
  case KEY1Token::fill_type :
    switch (getState().getTokenizer().getId(value))
    {
    case KEY1Token::color :
      m_type=FILL_TYPE_COLOR;
      break;
    case KEY1Token::image :
      m_type=FILL_TYPE_IMAGE;
      break;
    case KEY1Token::none :
      m_type=FILL_TYPE_NONE;
      break;
    default :
      ETONYEK_DEBUG_MSG(("KEY1FillElement::attribute[KEY1Parser.cpp]: unexpected type %s\n", value));
      break;
    }
    break;
  case KEY1Token::id :
    setId(value);
    break;
  case KEY1Token::image_data :
    m_imageName=value;
    break;
  case KEY1Token::image_scale :
    switch (getState().getTokenizer().getId(value))
    {
    case KEY1Token::scale_to_fit :
      m_imageType=IWORK_IMAGE_TYPE_SCALE_TO_FIT;
      break;
    case KEY1Token::tile :
      m_imageType=IWORK_IMAGE_TYPE_TILE;
      break;
    default :
      ETONYEK_DEBUG_MSG(("KEY1FillElement::attribute[KEY1Parser.cpp]: unexpected imageType type %s\n", value));
      break;
    }
    break;
  case KEY1Token::byte_size : // the image size
    break;
  default :
    ETONYEK_DEBUG_MSG(("KEY1FillElement::attribute[KEY1Parser.cpp]: unexpected attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t KEY1FillElement::element(const int name)
{
  switch (name)
  {
  default :
    ETONYEK_DEBUG_MSG(("KEY1FillElement::element[KEY1Parser.cpp]: unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void KEY1FillElement::endOfElement()
{
  if (!m_type) return;
  switch (get(m_type))
  {
  case FILL_TYPE_COLOR :
    if (!m_color)
    {
      ETONYEK_DEBUG_MSG(("KEY1FillElement::endOfElement[KEY1Parser.cpp]: can not find the color\n"));
      break;
    }
    m_fill=get(m_color);
    break;
  case FILL_TYPE_IMAGE :
  {
    if (!m_imageName)
    {
      ETONYEK_DEBUG_MSG(("KEY1FillElement::endOfElement[KEY1Parser.cpp]: can not find the image name\n"));
      break;
    }
    IWORKMediaContent image;
    image.m_data = std::make_shared<IWORKData>();
    image.m_data->m_stream.reset(getState().getParser().getPackage()->getSubStreamByName(get(m_imageName).c_str()));
    if (m_imageType) image.m_type=get(m_imageType);
    image.m_fillColor=m_color;
    m_fill=image;
    break;
  }
  case FILL_TYPE_NONE :
    break;
  default:
    ETONYEK_DEBUG_MSG(("KEY1FillElement::endOfElement[KEY1Parser.cpp]: unknown type\n"));
  }
  if (getId() && m_fill)
    getState().getDictionary().m_fills[get(getId())]=get(m_fill);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
