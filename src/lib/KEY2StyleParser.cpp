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
      done = parsePropertyImpl(element, key);
  }
}

bool KEY2StyleParser::parsePropertyImpl(const KEYXMLReader &reader, const char *const key)
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
    case KEY2Token::geometry :
    {
      const KEYGeometryPtr_t geometry = readGeometry(reader);
      if (geometry)
        prop = geometry;
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
      case KEY2Token::geometry :
        parseProperty(element, "geometry");
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
