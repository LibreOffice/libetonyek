/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/lexical_cast.hpp>

#include "libetonyek_xml.h"
#include "IWORKToken.h"
#include "IWORKXMLReader.h"
#include "KEY2StyleParser.h"
#include "KEY2Token.h"
#include "KEYCollector.h"
#include "KEYDefaults.h"
#include "KEYStyles.h"
#include "KEYTypes.h"

using boost::any;
using boost::lexical_cast;
using boost::optional;

using std::string;

namespace libetonyek
{

namespace
{

template<typename T, typename C>
optional<T> readNumber(const IWORKXMLReader &reader, const int type, const C converter)
{
  optional<T> retval;

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    switch (getId(attr))
    {
    case IWORKToken::NS_URI_SFA | IWORKToken::number :
      retval = converter(attr.getValue());
      break;
    case IWORKToken::NS_URI_SFA | IWORKToken::type :
      if (getValueId(attr) != type)
      {
        ETONYEK_DEBUG_MSG(("invalid number type %s\n", attr.getValue()));
      }
      break;
    }
  }

  return retval;
}

optional<bool> readBool(const IWORKXMLReader &reader, const int type)
{
  return readNumber<bool>(reader, type, &KEY2ParserUtils::bool_cast);
}

optional<double> readDouble(const IWORKXMLReader &reader)
{
  return readNumber<double>(reader, IWORKToken::f, &KEY2ParserUtils::double_cast);
}

optional<int> readInt(const IWORKXMLReader &reader)
{
  return readNumber<int>(reader, KEY2Token::i, &KEY2ParserUtils::double_cast);
}

optional<IWORKColor> readColor(const IWORKXMLReader &reader)
{
  IWORKColor color;

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    switch (getId(attr))
    {
    case IWORKToken::NS_URI_SFA | IWORKToken::a :
      color.alpha = lexical_cast<double>(attr.getValue());
      break;
    case IWORKToken::NS_URI_SFA | IWORKToken::b :
      color.blue = lexical_cast<double>(attr.getValue());
      break;
    case IWORKToken::NS_URI_SFA | IWORKToken::g :
      color.green = lexical_cast<double>(attr.getValue());
      break;
    case IWORKToken::NS_URI_SFA | IWORKToken::r :
      color.red = lexical_cast<double>(attr.getValue());
      break;
    }
  }

  // Ignore sfa:custom-space-color, if present. The color profile is not part of the document anyway.
  skipElement(reader);

  return color;
}

optional<string> readString(const IWORKXMLReader &reader)
{
  return readOnlyAttribute(reader, IWORKToken::string, IWORKToken::NS_URI_SFA);
}

}

KEY2StyleParser::KEY2StyleParser(const int id, KEYCollector *const collector, const KEYDefaults &defaults, const bool nested)
  : KEY2ParserUtils()
  , m_id(id)
  , m_nested(nested)
  , m_collector(collector)
  , m_defaults(defaults)
  , m_props()
{
}

void KEY2StyleParser::parse(const IWORKXMLReader &reader)
{
  optional<ID_t> id;
  optional<string> ident;
  optional<string> parentIdent;

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    switch (getId(attr))
    {
    case IWORKToken::NS_URI_SF | IWORKToken::ident :
      ident = attr.getValue();
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::parent_ident :
      parentIdent = attr.getValue();
      break;
    case IWORKToken::NS_URI_SFA | IWORKToken::ID :
      ident = attr.getValue();
      break;
    default :
      break;
    }
  }

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((IWORKToken::NS_URI_SF | IWORKToken::property_map) == getId(element))
    {
      parsePropertyMap(reader);

      switch (m_id)
      {
      case IWORKToken::NS_URI_SF | IWORKToken::cell_style :
        m_collector->collectCellStyle(id, m_props, ident, parentIdent, false, m_nested);
        break;
      case IWORKToken::NS_URI_SF | IWORKToken::characterstyle :
        m_collector->collectCharacterStyle(id, m_props, ident, parentIdent, false, m_nested);
        break;
      case IWORKToken::NS_URI_SF | IWORKToken::connection_style :
        m_collector->collectConnectionStyle(id, m_props, ident, parentIdent, false, m_nested);
        break;
      case IWORKToken::NS_URI_SF | IWORKToken::graphic_style :
        m_collector->collectGraphicStyle(id, m_props, ident, parentIdent, false, m_nested);
        break;
      case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle :
        m_collector->collectLayoutStyle(id, m_props, ident, parentIdent, false, m_nested);
        break;
      case IWORKToken::NS_URI_SF | IWORKToken::liststyle :
        m_collector->collectListStyle(id, m_props, ident, parentIdent, false, m_nested);
        break;
      case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle :
        m_collector->collectParagraphStyle(id, m_props, ident, parentIdent, false, m_nested);
        break;
      case IWORKToken::NS_URI_SF | IWORKToken::placeholder_style :
        m_collector->collectPlaceholderStyle(id, m_props, ident, parentIdent, false, m_nested);
        break;
      case IWORKToken::NS_URI_SF | IWORKToken::slide_style :
        m_collector->collectSlideStyle(id, m_props, ident, parentIdent, false, m_nested);
        break;
      case IWORKToken::NS_URI_SF | IWORKToken::tabular_style :
        m_collector->collectTabularStyle(id, m_props, ident, parentIdent, false, m_nested);
        break;
      case IWORKToken::NS_URI_SF | IWORKToken::vector_style :
        m_collector->collectVectorStyle(id, m_props, ident, parentIdent, false, m_nested);
        break;
      default :
        ETONYEK_DEBUG_MSG(("unhandled style %d\n", m_id));
        break;
      }
    }
    else
    {
      skipElement(element);
    }
  }
}

void KEY2StyleParser::parseProperty(const IWORKXMLReader &reader, const char *const key)
{
  checkNoAttributes(reader);

  const int token = getId(reader);

  // Parse a property's value, ignoring any extra tags around (there
  // should be none, but can we on that?) Note that the property can be
  // empty.
  IWORKXMLReader::ElementIterator element(reader);
  bool done = false;
  while (element.next())
  {
    if (done)
      skipElement(element);
    else
      done = parsePropertyImpl(element, token, key);
  }
}

bool KEY2StyleParser::parsePropertyImpl(const IWORKXMLReader &reader, const int propertyId, const char *const key)
{
  bool parsed = true;
  any prop;

  const int token = getId(reader);

  switch (token)
  {
  // nested styles
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle :
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle :
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle :
  case IWORKToken::NS_URI_SF | IWORKToken::vector_style :
  {
    KEY2StyleParser parser(token, m_collector, m_defaults, true);
    parser.parse(reader);
    // TODO: need to get the style
    break;
  }
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::vector_style_ref :
  {
    const optional<string> dummyIdent;
    const optional<IWORKPropertyMap> dummyProps;
    const optional<ID_t> id = readRef(reader);

    // TODO: need to get the style
    switch (token)
    {
    case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle_ref :
      m_collector->collectLayoutStyle(id, dummyProps, dummyIdent, dummyIdent, true, true);
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::liststyle_ref :
      m_collector->collectListStyle(id, dummyProps, dummyIdent, dummyIdent, true, true);
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle_ref :
      m_collector->collectParagraphStyle(id, dummyProps, dummyIdent, dummyIdent, true, true);
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::vector_style_ref :
      m_collector->collectVectorStyle(id, dummyProps, dummyIdent, dummyIdent, true, true);
      break;
    default :
      assert(0);
      break;
    }

    break;
  }

  // "normal" properties

  case IWORKToken::NS_URI_SF | IWORKToken::color :
  {
    const optional<IWORKColor> color = readColor(reader);
    if (color)
      prop = get(color);
    break;
  }
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
  {
    const IWORKGeometryPtr_t geometry = readGeometry(reader);
    if (geometry)
      prop = geometry;
    break;
  }

  case IWORKToken::NS_URI_SF | IWORKToken::number :
  {
    switch (propertyId)
    {
    case IWORKToken::NS_URI_SF | IWORKToken::alignment :
    {
      const optional<int> alignment = readInt(reader);
      if (alignment)
      {
        switch (get(alignment))
        {
        case 0 :
          prop = IWORK_ALIGNMENT_LEFT;
          break;
        case 1 :
          prop = IWORK_ALIGNMENT_RIGHT;
          break;
        case 2 :
          prop = IWORK_ALIGNMENT_CENTER;
          break;
        case 3 :
          prop = IWORK_ALIGNMENT_JUSTIFY;
          break;
        default :
          ETONYEK_DEBUG_MSG(("unknown alignment %d\n", get(alignment)));
        }
      }
      break;
    }

    case IWORKToken::NS_URI_SF | IWORKToken::baselineShift :
    case IWORKToken::NS_URI_SF | IWORKToken::fontSize :
    {
      const optional<double> d = readDouble(reader);
      if (d)
        prop = get(d);
      break;
    }

    case IWORKToken::NS_URI_SF | IWORKToken::bold :
    case IWORKToken::NS_URI_SF | IWORKToken::outline :
    case IWORKToken::NS_URI_SF | IWORKToken::strikethru :
    case IWORKToken::NS_URI_SF | IWORKToken::underline :
    {
      const optional<bool> b = readBool(reader, KEY2Token::i);
      if (b)
        prop = get(b);
      break;
    }

    case IWORKToken::NS_URI_SF | IWORKToken::capitalization :
    {
      const optional<int> capitalization = readInt(reader);
      if (capitalization)
      {
        switch (get(capitalization))
        {
        case 0 :
          prop = IWORK_CAPITALIZATION_NONE;
          break;
        case 1 :
          prop = IWORK_CAPITALIZATION_ALL_CAPS;
          break;
        case 2 :
          prop = IWORK_CAPITALIZATION_SMALL_CAPS;
          break;
        case 3 :
          prop = IWORK_CAPITALIZATION_TITLE;
          break;
        default :
          ETONYEK_DEBUG_MSG(("unknown capitalization %d\n", get(capitalization)));
        }
      }
      break;
    }

    case IWORKToken::NS_URI_SF | IWORKToken::italic :
    {
      const optional<bool> b = readBool(reader, KEY2Token::c);
      if (b)
        prop = get(b);
      break;
    }

    case IWORKToken::NS_URI_SF | IWORKToken::superscript :
    {
      const optional<int> superscript = readInt(reader);
      if (superscript)
      {
        switch (get(superscript))
        {
        case 1 :
          prop = IWORK_BASELINE_SUPER;
          break;
        case 2 :
          prop = IWORK_BASELINE_SUB;
          break;
        default :
          ETONYEK_DEBUG_MSG(("unknown superscript %d\n", get(superscript)));
        }
      }
      break;
    }
    default :
      break;
    }
    break;
  }

  case IWORKToken::NS_URI_SF | IWORKToken::string :
  {
    const optional<string> str = readString(reader);
    if (str)
      prop = get(str);
    break;
  }

  case IWORKToken::NS_URI_SF | IWORKToken::tabs :
  {
    IWORKTabStops_t tabStops;

    IWORKXMLReader::ElementIterator element(reader);
    while (element.next())
    {
      if ((IWORKToken::NS_URI_SF | IWORKToken::tabstop) == getId(element))
      {
        optional<double> pos;

        const IWORKXMLReader tabStopReader(element);

        IWORKXMLReader::AttributeIterator attr(tabStopReader);
        while (attr.next())
        {
          switch (getId(attr))
          {
          case IWORKToken::NS_URI_SF | IWORKToken::align :
            // TODO: parse
            break;
          case IWORKToken::NS_URI_SF | IWORKToken::pos :
            pos = lexical_cast<double>(attr.getValue());
            break;
          default :
            break;
          }
        }

        checkEmptyElement(tabStopReader);

        if (bool(pos))
          tabStops.push_back(IWORKTabStop(get(pos)));
      }
      else
      {
        skipElement(element);
      }
    }

    if (!tabStops.empty())
      prop = tabStops;

    break;
  }

  default :
    parsed = false;
    skipElement(reader);
    break;
  }

  if (parsed && key && !prop.empty())
    m_props.set(key, prop);

  return parsed;
}

void KEY2StyleParser::parsePropertyMap(const IWORKXMLReader &reader)
{
  assert(checkElement(reader, IWORKToken::property_map, IWORKToken::NS_URI_SF));

  checkNoAttributes(reader);

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyLayoutStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyParagraphStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyBorderVectorStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyCellLayoutStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyCellParagraphStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyCellStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderBorderVectorStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderColumnCellLayoutStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderColumnCellParagraphStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderColumnCellStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderRowCellLayoutStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderRowCellParagraphStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderRowCellStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderSeperatorVectorStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderVectorStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyVectorStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::TableCellStylePropertyFormatNegativeStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::bulletListStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::followingLayoutStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::followingParagraphStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::layoutParagraphStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::layoutStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::listStyle :
    case IWORKToken::NS_URI_SF | IWORKToken::tocStyle :
      parseProperty(element);
      break;

    case IWORKToken::NS_URI_SF | IWORKToken::alignment :
      parseProperty(element, "alignment");
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::baselineShift :
      parseProperty(element, "baselineShift");
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::bold :
      parseProperty(element, "bold");
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::capitalization :
      parseProperty(element, "capitalization");
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::fontColor :
      parseProperty(element, "fontColor");
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::fontName :
      parseProperty(element, "fontName");
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::fontSize :
      parseProperty(element, "fontSize");
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::geometry :
      parseProperty(element, "geometry");
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::italic :
      parseProperty(element, "italic");
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::outline :
      parseProperty(element, "outline");
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::strikethru :
      parseProperty(element, "strikethru");
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::superscript :
      parseProperty(element, "superscript");
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::tabs :
      parseProperty(element, "tabs");
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::underline :
      parseProperty(element, "underline");
      break;

    case KEY2Token::NS_URI_KEY | KEY2Token::headlineParagraphStyle :
      parseProperty(element);
      break;

    default :
      skipElement(element);
      break;
    }
  }
}

IWORKGeometryPtr_t KEY2StyleParser::readGeometry(const IWORKXMLReader &reader)
{
  optional<IWORKSize> naturalSize;
  optional<IWORKPosition> pos;
  optional<double> angle;
  optional<double> shearXAngle;
  optional<double> shearYAngle;
  optional<bool> aspectRatioLocked;
  optional<bool> sizesLocked;
  optional<bool> horizontalFlip;
  optional<bool> verticalFlip;

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    switch (getId(attr))
    {
    case IWORKToken::NS_URI_SF | IWORKToken::angle :
      angle = lexical_cast<double>(attr.getValue());
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::aspectRatioLocked :
      aspectRatioLocked = bool_cast(attr.getValue());
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::horizontalFlip :
      horizontalFlip = bool_cast(attr.getValue());
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::shearXAngle :
      shearXAngle = lexical_cast<double>(attr.getValue());
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::shearYAngle :
      shearYAngle = lexical_cast<double>(attr.getValue());
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::sizesLocked :
      sizesLocked = bool_cast(attr.getValue());
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::verticalFlip :
      verticalFlip = bool_cast(attr.getValue());
      break;
    default :
      break;
    }
  }

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case IWORKToken::NS_URI_SF | IWORKToken::naturalSize :
      naturalSize = readSize(reader);
      break;
    case IWORKToken::NS_URI_SF | IWORKToken::position :
      pos = readPosition(reader);
      break;
    default :
      skipElement(element);
      break;
    }
  }

  m_defaults.applyGeometry(naturalSize, pos);
  assert(naturalSize && pos);

  const IWORKGeometryPtr_t geometry(new IWORKGeometry());
  geometry->naturalSize = get(naturalSize);
  geometry->position = get(pos);
  geometry->angle = angle;
  geometry->shearXAngle = shearXAngle;
  geometry->shearYAngle = shearYAngle;
  geometry->horizontalFlip = horizontalFlip;
  geometry->verticalFlip = verticalFlip;
  geometry->aspectRatioLocked = aspectRatioLocked;
  geometry->sizesLocked = sizesLocked;

  return geometry;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
