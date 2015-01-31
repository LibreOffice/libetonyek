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

KEY2StyleParser::KEY2StyleParser(const int nameId, const int nsId, KEYCollector *const collector, const KEYDefaults &defaults, const bool nested)
  : KEY2ParserUtils()
  , m_nameId(nameId)
  , m_nsId(nsId)
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
    if ((IWORKToken::NS_URI_SFA == getNamespaceId(attr)) && (IWORKToken::ID == getNameId(attr)))
    {
      id = attr.getValue();
    }
    else if (IWORKToken::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case IWORKToken::ident :
        ident = attr.getValue();
        break;
      case IWORKToken::parent_ident :
        parentIdent = attr.getValue();
        break;
      default :
        break;
      }
    }
  }

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((IWORKToken::NS_URI_SF == getNamespaceId(element)) && (IWORKToken::property_map == getNameId(element)))
    {
      parsePropertyMap(reader);

      if (IWORKToken::NS_URI_SF == m_nsId)
      {
        switch (m_nameId)
        {
        case IWORKToken::cell_style :
          m_collector->collectCellStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case IWORKToken::characterstyle :
          m_collector->collectCharacterStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case IWORKToken::connection_style :
          m_collector->collectConnectionStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case IWORKToken::graphic_style :
          m_collector->collectGraphicStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case IWORKToken::layoutstyle :
          m_collector->collectLayoutStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case IWORKToken::liststyle :
          m_collector->collectListStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case IWORKToken::paragraphstyle :
          m_collector->collectParagraphStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case IWORKToken::placeholder_style :
          m_collector->collectPlaceholderStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case IWORKToken::slide_style :
          m_collector->collectSlideStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case IWORKToken::tabular_style :
          m_collector->collectTabularStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case IWORKToken::vector_style :
          m_collector->collectVectorStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        default :
          ETONYEK_DEBUG_MSG(("unhandled style %d:%d\n", m_nsId, m_nameId));
          break;
        }
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

  const int nsToken = getNamespaceId(reader);

  if (IWORKToken::NS_URI_SF == nsToken)
  {
    const int nameToken = getNameId(reader);

    switch (nameToken)
    {
    // nested styles
    case IWORKToken::layoutstyle :
    case IWORKToken::liststyle :
    case IWORKToken::paragraphstyle :
    case IWORKToken::vector_style :
    {
      KEY2StyleParser parser(nameToken, nsToken, m_collector, m_defaults, true);
      parser.parse(reader);
      // TODO: need to get the style
      break;
    }
    case IWORKToken::layoutstyle_ref :
    case IWORKToken::liststyle_ref :
    case IWORKToken::paragraphstyle_ref :
    case IWORKToken::vector_style_ref :
    {
      const optional<string> dummyIdent;
      const optional<IWORKPropertyMap> dummyProps;
      const optional<ID_t> id = readRef(reader);

      // TODO: need to get the style
      switch (nameToken)
      {
      case IWORKToken::layoutstyle_ref :
        m_collector->collectLayoutStyle(id, dummyProps, dummyIdent, dummyIdent, true, true);
        break;
      case IWORKToken::liststyle_ref :
        m_collector->collectListStyle(id, dummyProps, dummyIdent, dummyIdent, true, true);
        break;
      case IWORKToken::paragraphstyle_ref :
        m_collector->collectParagraphStyle(id, dummyProps, dummyIdent, dummyIdent, true, true);
        break;
      case IWORKToken::vector_style_ref :
        m_collector->collectVectorStyle(id, dummyProps, dummyIdent, dummyIdent, true, true);
        break;
      default :
        assert(0);
        break;
      }

      break;
    }

    // "normal" properties

    case IWORKToken::color :
    {
      const optional<IWORKColor> color = readColor(reader);
      if (color)
        prop = get(color);
      break;
    }
    case IWORKToken::geometry :
    {
      const IWORKGeometryPtr_t geometry = readGeometry(reader);
      if (geometry)
        prop = geometry;
      break;
    }

    case IWORKToken::number :
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

    case IWORKToken::string :
    {
      const optional<string> str = readString(reader);
      if (str)
        prop = get(str);
      break;
    }

    case IWORKToken::tabs :
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
  }
  else
  {
    parsed = false;
    skipElement(reader);
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
    if (IWORKToken::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case IWORKToken::SFTCellStylePropertyLayoutStyle :
      case IWORKToken::SFTCellStylePropertyParagraphStyle :
      case IWORKToken::SFTableStylePropertyBorderVectorStyle :
      case IWORKToken::SFTableStylePropertyCellLayoutStyle :
      case IWORKToken::SFTableStylePropertyCellParagraphStyle :
      case IWORKToken::SFTableStylePropertyCellStyle :
      case IWORKToken::SFTableStylePropertyHeaderBorderVectorStyle :
      case IWORKToken::SFTableStylePropertyHeaderColumnCellLayoutStyle :
      case IWORKToken::SFTableStylePropertyHeaderColumnCellParagraphStyle :
      case IWORKToken::SFTableStylePropertyHeaderColumnCellStyle :
      case IWORKToken::SFTableStylePropertyHeaderRowCellLayoutStyle :
      case IWORKToken::SFTableStylePropertyHeaderRowCellParagraphStyle :
      case IWORKToken::SFTableStylePropertyHeaderRowCellStyle :
      case IWORKToken::SFTableStylePropertyHeaderSeperatorVectorStyle :
      case IWORKToken::SFTableStylePropertyHeaderVectorStyle :
      case IWORKToken::SFTableStylePropertyVectorStyle :
      case IWORKToken::TableCellStylePropertyFormatNegativeStyle :
      case IWORKToken::bulletListStyle :
      case IWORKToken::followingLayoutStyle :
      case IWORKToken::followingParagraphStyle :
      case IWORKToken::layoutParagraphStyle :
      case IWORKToken::layoutStyle :
      case IWORKToken::listStyle :
      case IWORKToken::tocStyle :
        parseProperty(element);
        break;

      case IWORKToken::alignment :
        parseProperty(element, "alignment");
        break;
      case IWORKToken::baselineShift :
        parseProperty(element, "baselineShift");
        break;
      case IWORKToken::bold :
        parseProperty(element, "bold");
        break;
      case IWORKToken::capitalization :
        parseProperty(element, "capitalization");
        break;
      case IWORKToken::fontColor :
        parseProperty(element, "fontColor");
        break;
      case IWORKToken::fontName :
        parseProperty(element, "fontName");
        break;
      case IWORKToken::fontSize :
        parseProperty(element, "fontSize");
        break;
      case IWORKToken::geometry :
        parseProperty(element, "geometry");
        break;
      case IWORKToken::italic :
        parseProperty(element, "italic");
        break;
      case IWORKToken::outline :
        parseProperty(element, "outline");
        break;
      case IWORKToken::strikethru :
        parseProperty(element, "strikethru");
        break;
      case IWORKToken::superscript :
        parseProperty(element, "superscript");
        break;
      case IWORKToken::tabs :
        parseProperty(element, "tabs");
        break;
      case IWORKToken::underline :
        parseProperty(element, "underline");
        break;

      default :
        skipElement(element);
        break;
      }
    }
    else if (KEY2Token::NS_URI_KEY == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::headlineParagraphStyle :
        parseProperty(element);
        break;

      default :
        skipElement(element);
        break;
      }
    }
    else
    {
      skipElement(element);
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
    if (IWORKToken::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case IWORKToken::angle :
        angle = lexical_cast<double>(attr.getValue());
        break;
      case IWORKToken::aspectRatioLocked :
        aspectRatioLocked = bool_cast(attr.getValue());
        break;
      case IWORKToken::horizontalFlip :
        horizontalFlip = bool_cast(attr.getValue());
        break;
      case IWORKToken::shearXAngle :
        shearXAngle = lexical_cast<double>(attr.getValue());
        break;
      case IWORKToken::shearYAngle :
        shearYAngle = lexical_cast<double>(attr.getValue());
        break;
      case IWORKToken::sizesLocked :
        sizesLocked = bool_cast(attr.getValue());
        break;
      case IWORKToken::verticalFlip :
        verticalFlip = bool_cast(attr.getValue());
        break;
      default :
        break;
      }
    }
  }

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (IWORKToken::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case IWORKToken::naturalSize :
        naturalSize = readSize(reader);
        break;
      case IWORKToken::position :
        pos = readPosition(reader);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
    {
      skipElement(element);
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
