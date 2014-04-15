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
#include "KEY2StyleParser.h"
#include "KEY2Token.h"
#include "KEYCollector.h"
#include "KEYDefaults.h"
#include "KEYStyles.h"
#include "KEYTypes.h"
#include "KEYXMLReader.h"

using boost::any;
using boost::lexical_cast;
using boost::optional;

using std::string;

namespace libetonyek
{

namespace
{

template<typename T, typename C>
optional<T> readNumber(const KEYXMLReader &reader, const int type, const C converter)
{
  optional<T> retval;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    switch (getId(attr))
    {
    case KEY2Token::NS_URI_SFA | KEY2Token::number :
      retval = converter(attr.getValue());
      break;
    case KEY2Token::NS_URI_SFA | KEY2Token::type :
      if (getValueId(attr) != type)
      {
        KEY_DEBUG_MSG(("invalid number type %s\n", attr.getValue()));
      }
      break;
    }
  }

  return retval;
}

optional<bool> readBool(const KEYXMLReader &reader, const int type)
{
  return readNumber<bool>(reader, type, &KEY2ParserUtils::bool_cast);
}

optional<double> readDouble(const KEYXMLReader &reader)
{
  return readNumber<double>(reader, KEY2Token::f, &KEY2ParserUtils::double_cast);
}

optional<int> readInt(const KEYXMLReader &reader)
{
  return readNumber<int>(reader, KEY2Token::i, &KEY2ParserUtils::double_cast);
}

optional<KEYColor> readColor(const KEYXMLReader &reader)
{
  KEYColor color;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    switch (getId(attr))
    {
    case KEY2Token::NS_URI_SFA | KEY2Token::a :
      color.alpha = lexical_cast<double>(attr.getValue());
      break;
    case KEY2Token::NS_URI_SFA | KEY2Token::b :
      color.blue = lexical_cast<double>(attr.getValue());
      break;
    case KEY2Token::NS_URI_SFA | KEY2Token::g :
      color.green = lexical_cast<double>(attr.getValue());
      break;
    case KEY2Token::NS_URI_SFA | KEY2Token::r :
      color.red = lexical_cast<double>(attr.getValue());
      break;
    }
  }

  // Ignore sfa:custom-space-color, if present. The color profile is not part of the document anyway.
  skipElement(reader);

  return color;
}

optional<string> readString(const KEYXMLReader &reader)
{
  return readOnlyAttribute(reader, KEY2Token::string, KEY2Token::NS_URI_SFA);
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

void KEY2StyleParser::parse(const KEYXMLReader &reader)
{
  optional<ID_t> id;
  optional<string> ident;
  optional<string> parentIdent;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KEY2Token::NS_URI_SFA == getNamespaceId(attr)) && (KEY2Token::ID == getNameId(attr)))
    {
      id = attr.getValue();
    }
    else if (KEY2Token::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KEY2Token::ident :
        ident = attr.getValue();
        break;
      case KEY2Token::parent_ident :
        parentIdent = attr.getValue();
        break;
      default :
        break;
      }
    }
  }

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KEY2Token::NS_URI_SF == getNamespaceId(element)) && (KEY2Token::property_map == getNameId(element)))
    {
      parsePropertyMap(reader);

      if (KEY2Token::NS_URI_SF == m_nsId)
      {
        switch (m_nameId)
        {
        case KEY2Token::cell_style :
          m_collector->collectCellStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::characterstyle :
          m_collector->collectCharacterStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::connection_style :
          m_collector->collectConnectionStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::graphic_style :
          m_collector->collectGraphicStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::layoutstyle :
          m_collector->collectLayoutStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::liststyle :
          m_collector->collectListStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::paragraphstyle :
          m_collector->collectParagraphStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::placeholder_style :
          m_collector->collectPlaceholderStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::slide_style :
          m_collector->collectSlideStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::tabular_style :
          m_collector->collectTabularStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        case KEY2Token::vector_style :
          m_collector->collectVectorStyle(id, m_props, ident, parentIdent, false, m_nested);
          break;
        default :
          KEY_DEBUG_MSG(("unhandled style %d:%d\n", m_nsId, m_nameId));
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

void KEY2StyleParser::parseProperty(const KEYXMLReader &reader, const char *const key)
{
  checkNoAttributes(reader);

  const int token = getId(reader);

  // Parse a property's value, ignoring any extra tags around (there
  // should be none, but can we on that?) Note that the property can be
  // empty.
  KEYXMLReader::ElementIterator element(reader);
  bool done = false;
  while (element.next())
  {
    if (done)
      skipElement(element);
    else
      done = parsePropertyImpl(element, token, key);
  }
}

bool KEY2StyleParser::parsePropertyImpl(const KEYXMLReader &reader, const int propertyId, const char *const key)
{
  bool parsed = true;
  any prop;

  const int nsToken = getNamespaceId(reader);

  if (KEY2Token::NS_URI_SF == nsToken)
  {
    const int nameToken = getNameId(reader);

    switch (nameToken)
    {
    // nested styles
    case KEY2Token::layoutstyle :
    case KEY2Token::liststyle :
    case KEY2Token::paragraphstyle :
    case KEY2Token::vector_style :
    {
      KEY2StyleParser parser(nameToken, nsToken, m_collector, m_defaults, true);
      parser.parse(reader);
      // TODO: need to get the style
      break;
    }
    case KEY2Token::layoutstyle_ref :
    case KEY2Token::liststyle_ref :
    case KEY2Token::paragraphstyle_ref :
    case KEY2Token::vector_style_ref :
    {
      const optional<string> dummyIdent;
      const optional<KEYPropertyMap> dummyProps;
      const optional<ID_t> id = readRef(reader);

      // TODO: need to get the style
      switch (nameToken)
      {
      case KEY2Token::layoutstyle_ref :
        m_collector->collectLayoutStyle(id, dummyProps, dummyIdent, dummyIdent, true, true);
        break;
      case KEY2Token::liststyle_ref :
        m_collector->collectListStyle(id, dummyProps, dummyIdent, dummyIdent, true, true);
        break;
      case KEY2Token::paragraphstyle_ref :
        m_collector->collectParagraphStyle(id, dummyProps, dummyIdent, dummyIdent, true, true);
        break;
      case KEY2Token::vector_style_ref :
        m_collector->collectVectorStyle(id, dummyProps, dummyIdent, dummyIdent, true, true);
        break;
      default :
        assert(0);
        break;
      }

      break;
    }

    // "normal" properties

    case KEY2Token::color :
    {
      const optional<KEYColor> color = readColor(reader);
      if (color)
        prop = get(color);
      break;
    }
    case KEY2Token::geometry :
    {
      const KEYGeometryPtr_t geometry = readGeometry(reader);
      if (geometry)
        prop = geometry;
      break;
    }

    case KEY2Token::number :
    {
      switch (propertyId)
      {
      case KEY2Token::NS_URI_SF | KEY2Token::alignment :
      {
        const optional<int> alignment = readInt(reader);
        if (alignment)
        {
          switch (get(alignment))
          {
          case 0 :
            prop = KEY_ALIGNMENT_LEFT;
            break;
          case 1 :
            prop = KEY_ALIGNMENT_RIGHT;
            break;
          case 2 :
            prop = KEY_ALIGNMENT_CENTER;
            break;
          case 3 :
            prop = KEY_ALIGNMENT_JUSTIFY;
            break;
          default :
            KEY_DEBUG_MSG(("unknown alignment %d\n", get(alignment)));
          }
        }
        break;
      }

      case KEY2Token::NS_URI_SF | KEY2Token::baselineShift :
      case KEY2Token::NS_URI_SF | KEY2Token::fontSize :
      {
        const optional<double> d = readDouble(reader);
        if (d)
          prop = get(d);
        break;
      }

      case KEY2Token::NS_URI_SF | KEY2Token::bold :
      case KEY2Token::NS_URI_SF | KEY2Token::outline :
      case KEY2Token::NS_URI_SF | KEY2Token::strikethru :
      case KEY2Token::NS_URI_SF | KEY2Token::underline :
      {
        const optional<bool> b = readBool(reader, KEY2Token::i);
        if (b)
          prop = get(b);
        break;
      }

      case KEY2Token::NS_URI_SF | KEY2Token::capitalization :
      {
        const optional<int> capitalization = readInt(reader);
        if (capitalization)
        {
          switch (get(capitalization))
          {
          case 0 :
            prop = KEY_CAPITALIZATION_NONE;
            break;
          case 1 :
            prop = KEY_CAPITALIZATION_ALL_CAPS;
            break;
          case 2 :
            prop = KEY_CAPITALIZATION_SMALL_CAPS;
            break;
          case 3 :
            prop = KEY_CAPITALIZATION_TITLE;
            break;
          default :
            KEY_DEBUG_MSG(("unknown capitalization %d\n", get(capitalization)));
          }
        }
        break;
      }

      case KEY2Token::NS_URI_SF | KEY2Token::italic :
      {
        const optional<bool> b = readBool(reader, KEY2Token::c);
        if (b)
          prop = get(b);
        break;
      }

      case KEY2Token::NS_URI_SF | KEY2Token::superscript :
      {
        const optional<int> superscript = readInt(reader);
        if (superscript)
        {
          switch (get(superscript))
          {
          case 1 :
            prop = KEY_BASELINE_SUPER;
            break;
          case 2 :
            prop = KEY_BASELINE_SUB;
            break;
          default :
            KEY_DEBUG_MSG(("unknown superscript %d\n", get(superscript)));
          }
        }
        break;
      }
      default :
        break;
      }
      break;
    }

    case KEY2Token::string :
    {
      const optional<string> str = readString(reader);
      if (str)
        prop = get(str);
      break;
    }

    case KEY2Token::tabs :
    {
      KEYTabStops_t tabStops;

      KEYXMLReader::ElementIterator element(reader);
      while (element.next())
      {
        if ((KEY2Token::NS_URI_SF | KEY2Token::tabstop) == getId(element))
        {
          optional<double> pos;

          const KEYXMLReader tabStopReader(element);

          KEYXMLReader::AttributeIterator attr(tabStopReader);
          while (attr.next())
          {
            switch (getId(attr))
            {
            case KEY2Token::NS_URI_SF | KEY2Token::align :
              // TODO: parse
              break;
            case KEY2Token::NS_URI_SF | KEY2Token::pos :
              pos = lexical_cast<double>(attr.getValue());
              break;
            default :
              break;
            }
          }

          checkEmptyElement(tabStopReader);

          if (bool(pos))
            tabStops.push_back(KEYTabStop(get(pos)));
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

void KEY2StyleParser::parsePropertyMap(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::property_map, KEY2Token::NS_URI_SF));

  checkNoAttributes(reader);

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::SFTCellStylePropertyLayoutStyle :
      case KEY2Token::SFTCellStylePropertyParagraphStyle :
      case KEY2Token::SFTableStylePropertyBorderVectorStyle :
      case KEY2Token::SFTableStylePropertyCellLayoutStyle :
      case KEY2Token::SFTableStylePropertyCellParagraphStyle :
      case KEY2Token::SFTableStylePropertyCellStyle :
      case KEY2Token::SFTableStylePropertyHeaderBorderVectorStyle :
      case KEY2Token::SFTableStylePropertyHeaderColumnCellLayoutStyle :
      case KEY2Token::SFTableStylePropertyHeaderColumnCellParagraphStyle :
      case KEY2Token::SFTableStylePropertyHeaderColumnCellStyle :
      case KEY2Token::SFTableStylePropertyHeaderRowCellLayoutStyle :
      case KEY2Token::SFTableStylePropertyHeaderRowCellParagraphStyle :
      case KEY2Token::SFTableStylePropertyHeaderRowCellStyle :
      case KEY2Token::SFTableStylePropertyHeaderSeperatorVectorStyle :
      case KEY2Token::SFTableStylePropertyHeaderVectorStyle :
      case KEY2Token::SFTableStylePropertyVectorStyle :
      case KEY2Token::TableCellStylePropertyFormatNegativeStyle :
      case KEY2Token::bulletListStyle :
      case KEY2Token::followingLayoutStyle :
      case KEY2Token::followingParagraphStyle :
      case KEY2Token::headlineParagraphStyle :
      case KEY2Token::layoutParagraphStyle :
      case KEY2Token::layoutStyle :
      case KEY2Token::listStyle :
      case KEY2Token::tocStyle :
        parseProperty(element);
        break;

      case KEY2Token::alignment :
        parseProperty(element, "alignment");
        break;
      case KEY2Token::baselineShift :
        parseProperty(element, "baselineShift");
        break;
      case KEY2Token::bold :
        parseProperty(element, "bold");
        break;
      case KEY2Token::capitalization :
        parseProperty(element, "capitalization");
        break;
      case KEY2Token::fontColor :
        parseProperty(element, "fontColor");
        break;
      case KEY2Token::fontName :
        parseProperty(element, "fontName");
        break;
      case KEY2Token::fontSize :
        parseProperty(element, "fontSize");
        break;
      case KEY2Token::geometry :
        parseProperty(element, "geometry");
        break;
      case KEY2Token::italic :
        parseProperty(element, "italic");
        break;
      case KEY2Token::outline :
        parseProperty(element, "outline");
        break;
      case KEY2Token::strikethru :
        parseProperty(element, "strikethru");
        break;
      case KEY2Token::superscript :
        parseProperty(element, "superscript");
        break;
      case KEY2Token::tabs :
        parseProperty(element, "tabs");
        break;
      case KEY2Token::underline :
        parseProperty(element, "underline");
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

KEYGeometryPtr_t KEY2StyleParser::readGeometry(const KEYXMLReader &reader)
{
  optional<KEYSize> naturalSize;
  optional<KEYPosition> pos;
  optional<double> angle;
  optional<double> shearXAngle;
  optional<double> shearYAngle;
  optional<bool> aspectRatioLocked;
  optional<bool> sizesLocked;
  optional<bool> horizontalFlip;
  optional<bool> verticalFlip;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KEY2Token::angle :
        angle = lexical_cast<double>(attr.getValue());
        break;
      case KEY2Token::aspectRatioLocked :
        aspectRatioLocked = bool_cast(attr.getValue());
        break;
      case KEY2Token::horizontalFlip :
        horizontalFlip = bool_cast(attr.getValue());
        break;
      case KEY2Token::shearXAngle :
        shearXAngle = lexical_cast<double>(attr.getValue());
        break;
      case KEY2Token::shearYAngle :
        shearYAngle = lexical_cast<double>(attr.getValue());
        break;
      case KEY2Token::sizesLocked :
        sizesLocked = bool_cast(attr.getValue());
        break;
      case KEY2Token::verticalFlip :
        verticalFlip = bool_cast(attr.getValue());
        break;
      default :
        break;
      }
    }
  }

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::naturalSize :
        naturalSize = readSize(reader);
        break;
      case KEY2Token::position :
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

  const KEYGeometryPtr_t geometry(new KEYGeometry());
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
