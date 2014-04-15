/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>
#include <utility>

#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include "libetonyek_utils.h"
#include "libetonyek_xml.h"
#include "KEY2Parser.h"
#include "KEY2StyleParser.h"
#include "KEY2TableParser.h"
#include "KEY2Token.h"
#include "KEYCollector.h"
#include "KEYPath.h"
#include "KEYStyle.h"
#include "KEYTypes.h"
#include "KEYXMLReader.h"

using boost::get_optional_value_or;
using boost::lexical_cast;
using boost::numeric_cast;
using boost::optional;

using std::pair;
using std::string;

namespace libetonyek
{

namespace
{

unsigned getVersion(const int token)
{
  switch (token)
  {
  case KEY2Token::VERSION_STR_2 :
    return 2;
  case KEY2Token::VERSION_STR_3 :
    return 3;
  case KEY2Token::VERSION_STR_4 :
    return 4;
  case KEY2Token::VERSION_STR_5 :
    return 5;
  }

  return 0;
}

}

KEY2Parser::KEY2Parser(const WPXInputStreamPtr_t &input, const WPXInputStreamPtr_t &package, KEYCollector *const collector, const KEYDefaults &defaults)
  : KEYParser(input, collector, defaults)
  , KEY2ParserUtils()
  , m_package(package)
  , m_version(0)
{
}

KEY2Parser::~KEY2Parser()
{
}

void KEY2Parser::processXmlNode(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::presentation, KEY2Token::NS_URI_KEY));

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (attr.getNamespace())
    {
      switch (getNamespaceId(attr))
      {
      case KEY2Token::NS_URI_KEY :
        switch (getNameId(attr))
        {
        case KEY2Token::version :
          m_version = getVersion(getValueId(attr));
          if (0 == m_version)
          {
            KEY_DEBUG_MSG(("unknown version %s\n", attr.getValue()));
          }
          break;
        default :
          break;
        }
        break;
      default :
        break;
      }
    }
  }

  optional<KEYSize> size;

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_KEY == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::size :
        size = readSize(reader);
        break;
      case KEY2Token::theme_list :
        parseThemeList(reader);
        break;
      case KEY2Token::slide_list :
        parseSlideList(reader);
        break;
      case KEY2Token::metadata :
        parseMetadata(reader);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else if ((KEY2Token::NS_URI_SF == getNamespaceId(element)) && (KEY2Token::calc_engine == getNameId(element)))
    {
      skipElement(element);
    }
    else
    {
      skipElement(element);
    }
  }

  getCollector()->collectPresentation(size);
}

KEYXMLReader::TokenizerFunction_t KEY2Parser::getTokenizer() const
{
  return KEY2Tokenizer();
}

void KEY2Parser::parseDrawables(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::drawables, KEY2Token::NS_URI_SF));

  getCollector()->startLevel();

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::connection_line :
        parseConnectionLine(element);
        break;
      case KEY2Token::group :
        parseGroup(element);
        break;
      case KEY2Token::image :
        parseImage(element);
        break;
      case KEY2Token::line :
        parseLine(element);
        break;
      case KEY2Token::media :
        parseMedia(element);
        break;
      case KEY2Token::shape :
        parseShape(element);
        break;
      case KEY2Token::sticky_note :
        parseStickyNote(element);
        break;
      case KEY2Token::tabular_info :
      {
        KEY2TableParser parser(*this);
        parser.parse(element);
        break;
      }
      case KEY2Token::body_placeholder_ref :
      {
        const optional<ID_t> id = readRef(reader);
        getCollector()->collectTextPlaceholder(id, false, true);
        break;
      }
      case KEY2Token::title_placeholder_ref :
      {
        const optional<ID_t> id = readRef(reader);
        getCollector()->collectTextPlaceholder(id, true, true);
        break;
      }
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }

  getCollector()->endLevel();
}

void KEY2Parser::parseLayer(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::layer, KEY2Token::NS_URI_SF));

  const optional<ID_t> id = readID(reader);

  getCollector()->startLayer();

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::drawables :
        parseDrawables(reader);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectLayer(id, false);
  getCollector()->endLayer();
}

void KEY2Parser::parseLayers(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::layers, KEY2Token::NS_URI_SF));

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::layer :
        parseLayer(reader);
        break;
      case KEY2Token::proxy_master_layer :
        parseProxyMasterLayer(reader);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }
}

void KEY2Parser::parseMasterSlide(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::master_slide, KEY2Token::NS_URI_KEY));

  optional<ID_t> id;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KEY2Token::ID | KEY2Token::NS_URI_SFA) == getId(attr))
      id = attr.getValue();
  }

  getCollector()->startPage();

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_KEY == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::page :
        parsePage(reader);
        break;
      case KEY2Token::stylesheet :
        parseStylesheet(reader);
        break;
      case KEY2Token::title_placeholder :
        parsePlaceholder(element, true);
        break;
      case KEY2Token::body_placeholder :
        parsePlaceholder(element);
        break;
      case KEY2Token::sticky_notes :
        parseStickyNotes(element);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectPage(id);
  getCollector()->endPage();
}

void KEY2Parser::parseMasterSlides(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::master_slides, KEY2Token::NS_URI_KEY));

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KEY2Token::NS_URI_KEY == getNamespaceId(element)) && (KEY2Token::master_slide == getNameId(element)))
      parseMasterSlide(reader);
    else
      skipElement(element);
  }
}

void KEY2Parser::parseMetadata(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::metadata, KEY2Token::NS_URI_KEY));

  skipElement(reader);
}

void KEY2Parser::parseNotes(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::notes, KEY2Token::NS_URI_KEY));

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KEY2Token::text_storage | KEY2Token::NS_URI_SF) == getId(element))
    {
      getCollector()->startText(false);
      parseTextStorage(element);
      getCollector()->collectNote();
      getCollector()->endText();
    }
    else
    {
      skipElement(reader);
    }
  }
}

void KEY2Parser::parsePage(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::page, KEY2Token::NS_URI_KEY));

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::size :
      {
        const KEYSize size = readSize(reader);
        // TODO: use size
        (void) size;
        break;
      }
      case KEY2Token::layers :
        parseLayers(reader);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }
}

void KEY2Parser::parseProxyMasterLayer(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::proxy_master_layer, KEY2Token::NS_URI_SF));

  optional<ID_t> ref;

  getCollector()->startLayer();

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::layer_ref :
        ref = readOnlyElementAttribute(reader, KEY2Token::IDREF, KEY2Token::NS_URI_SFA);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectLayer(ref, true);
  getCollector()->endLayer();
}

void KEY2Parser::parseSlide(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::slide, KEY2Token::NS_URI_KEY));

  const optional<ID_t> id = readID(reader);

  getCollector()->startPage();

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_KEY == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::notes :
        parseNotes(reader);
        break;
      case KEY2Token::page :
        parsePage(reader);
        break;
      case KEY2Token::stylesheet :
        parseStylesheet(reader);
        break;
      case KEY2Token::title_placeholder :
        parsePlaceholder(element, true);
        break;
      case KEY2Token::body_placeholder :
        parsePlaceholder(element);
        break;
      case KEY2Token::sticky_notes :
        parseStickyNotes(element);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectPage(id);
  getCollector()->endPage();
}

void KEY2Parser::parseSlideList(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::slide_list, KEY2Token::NS_URI_KEY));

  getCollector()->startSlides();

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KEY2Token::NS_URI_KEY == getNamespaceId(element)) && (KEY2Token::slide == getNameId(element)))
      parseSlide(reader);
    else
      skipElement(element);
  }

  getCollector()->endSlides();
}

void KEY2Parser::parseStickyNotes(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::sticky_notes, KEY2Token::NS_URI_KEY));

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KEY2Token::NS_URI_KEY == getNamespaceId(element)) && (KEY2Token::sticky_note == getNameId(element)))
      parseStickyNote(element);
    else
      skipElement(element);
  }
}

void KEY2Parser::parseStyles(const KEYXMLReader &reader, const bool anonymous)
{
  assert(checkElement(reader, anonymous ? KEY2Token::anon_styles : KEY2Token::styles, KEY2Token::NS_URI_SF));

  checkNoAttributes(reader);

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      const int elementToken = getNameId(element);

      switch (elementToken)
      {
      case KEY2Token::cell_style :
      case KEY2Token::characterstyle :
      case KEY2Token::connection_style :
      case KEY2Token::graphic_style :
      case KEY2Token::headline_style :
      case KEY2Token::layoutstyle :
      case KEY2Token::liststyle :
      case KEY2Token::placeholder_style :
      case KEY2Token::paragraphstyle :
      case KEY2Token::slide_style :
      case KEY2Token::tabular_style :
      case KEY2Token::vector_style :
      {
        KEY2StyleParser parser(getNameId(element), getNamespaceId(element), getCollector(), getDefaults());
        parser.parse(element);
        break;
      }

      case KEY2Token::cell_style_ref :
      case KEY2Token::characterstyle_ref :
      case KEY2Token::layoutstyle_ref :
      case KEY2Token::liststyle_ref :
      case KEY2Token::paragraphstyle_ref :
      case KEY2Token::vector_style_ref :
      {
        const optional<ID_t> id = readRef(element);
        const optional<KEYPropertyMap> dummyProps;
        const optional<string> dummyIdent;

        switch (elementToken)
        {
        case KEY2Token::cell_style_ref :
          getCollector()->collectCellStyle(id, dummyProps, dummyIdent, dummyIdent, true, anonymous);
          break;
        case KEY2Token::characterstyle_ref :
          getCollector()->collectCharacterStyle(id, dummyProps, dummyIdent, dummyIdent, true, anonymous);
          break;
        case KEY2Token::layoutstyle_ref :
          getCollector()->collectLayoutStyle(id, dummyProps, dummyIdent, dummyIdent, true, anonymous);
          break;
        case KEY2Token::liststyle_ref :
          getCollector()->collectListStyle(id, dummyProps, dummyIdent, dummyIdent, true, anonymous);
          break;
        case KEY2Token::paragraphstyle_ref :
          getCollector()->collectParagraphStyle(id, dummyProps, dummyIdent, dummyIdent, true, anonymous);
          break;
        case KEY2Token::vector_style_ref :
          getCollector()->collectVectorStyle(id, dummyProps, dummyIdent, dummyIdent, true, anonymous);
          break;
        default :
          assert(0);
          break;
        }
        break;
      }

      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }
}

void KEY2Parser::parseStylesheet(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::stylesheet, KEY2Token::NS_URI_KEY));

  const optional<ID_t> id = readID(reader);

  optional<ID_t> parent;

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::styles :
        parseStyles(reader, false);
        break;
      case KEY2Token::anon_styles :
        parseStyles(reader, true);
        break;
      case KEY2Token::parent_ref :
        parent = readRef(element);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectStylesheet(id, parent);
}

void KEY2Parser::parseTheme(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::theme, KEY2Token::NS_URI_KEY));

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_KEY == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::size :
      {
        const KEYSize size = readSize(reader);
        // TODO: use size
        (void) size;
        break;
      }
      case KEY2Token::stylesheet :
        parseStylesheet(reader);
        break;
      case KEY2Token::master_slides :
        parseMasterSlides(reader);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }
}

void KEY2Parser::parseThemeList(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::theme_list, KEY2Token::NS_URI_KEY));

  getCollector()->startThemes();

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KEY2Token::NS_URI_KEY == getNamespaceId(element)) && (KEY2Token::theme == getNameId(element)))
      parseTheme(reader);
    else
      skipElement(element);
  }

  getCollector()->endThemes();
}

void KEY2Parser::parseBezier(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::bezier, KEY2Token::NS_URI_SF));

  optional<ID_t> id;
  KEYPathPtr_t path;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KEY2Token::NS_URI_SFA == getNamespaceId(attr)))
    {
      switch (getNameId(attr))
      {
      case KEY2Token::ID :
        id = attr.getValue();
        break;
      case KEY2Token::path :
        path.reset(new KEYPath(attr.getValue()));
        break;
      default :
        break;
      }
    }
  }

  checkEmptyElement(reader);

  getCollector()->collectBezier(id, path, false);
}

void KEY2Parser::parseConnectionLine(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::connection_line, KEY2Token::NS_URI_SF));

  const optional<ID_t> id = readID(reader);

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::geometry :
        parseGeometry(element);
        break;
      case KEY2Token::path :
        parsePath(element);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectShape(id);
}

void KEY2Parser::parseGeometry(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::geometry, KEY2Token::NS_URI_SF));

  optional<ID_t> id;
  optional<KEYSize> naturalSize;
  optional<KEYSize> size;
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
        angle = deg2rad(lexical_cast<double>(attr.getValue()));
        break;
      case KEY2Token::aspectRatioLocked :
        aspectRatioLocked = bool_cast(attr.getValue());
        break;
      case KEY2Token::horizontalFlip :
        horizontalFlip = bool_cast(attr.getValue());
        break;
      case KEY2Token::shearXAngle :
        shearXAngle = deg2rad(lexical_cast<double>(attr.getValue()));
        break;
      case KEY2Token::shearYAngle :
        shearYAngle = deg2rad(lexical_cast<double>(attr.getValue()));
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
    else if (KEY2Token::NS_URI_SFA == getNamespaceId(attr) && (KEY2Token::ID == getNameId(attr)))
    {
      id = attr.getValue();
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
      case KEY2Token::size :
        size = readSize(reader);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectGeometry(id, naturalSize, size, pos, angle, shearXAngle, shearYAngle, horizontalFlip, verticalFlip, aspectRatioLocked, sizesLocked);
}

void KEY2Parser::parseGroup(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::group, KEY2Token::NS_URI_SF));

  getCollector()->startLevel();

  const optional<ID_t> id = readID(reader);

  KEYGroupPtr_t group(new KEYGroup());

  getCollector()->startGroup();

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::geometry :
        parseGeometry(reader);
        break;
      case KEY2Token::group :
        parseGroup(reader);
        break;
      case KEY2Token::image :
        parseImage(reader);
        break;
      case KEY2Token::line :
        parseLine(reader);
        break;
      case KEY2Token::media :
        parseMedia(reader);
        break;
      case KEY2Token::shape :
        parseShape(reader);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectGroup(id, group);
  getCollector()->endGroup();
  getCollector()->endLevel();
}

void KEY2Parser::parseImage(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::image, KEY2Token::NS_URI_SF));

  optional<ID_t> id;
  KEYImagePtr_t image(new KEYImage());

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KEY2Token::NS_URI_SF == getNamespaceId(attr)) && (KEY2Token::locked == getNameId(attr)))
      image->locked = bool_cast(attr.getValue());
    else if ((KEY2Token::ID | KEY2Token::NS_URI_SFA) == getId(attr))
      id = attr.getValue();
  }

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::geometry :
        parseGeometry(reader);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectImage(id, image);
}

void KEY2Parser::parseLine(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::line, KEY2Token::NS_URI_SF));

  const optional<ID_t> id = readID(reader);

  KEYLinePtr_t line(new KEYLine());

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::geometry :
        parseGeometry(reader);
        break;
      case KEY2Token::head :
      {
        const KEYPosition head = readPosition(reader);
        line->x1 = head.x;
        line->y1 = head.y;
        break;
      }
      case KEY2Token::tail :
      {
        const KEYPosition tail = readPosition(reader);
        line->x2 = tail.x;
        line->y2 = tail.y;
        break;
      }
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectLine(id, line);
}

void KEY2Parser::parseMedia(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::media, KEY2Token::NS_URI_SF));

  getCollector()->startLevel();

  const optional<ID_t> id = readID(reader);

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::geometry :
        parseGeometry(reader);
        break;
      case KEY2Token::content :
        parseContent(element);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectMedia(id);
  getCollector()->endLevel();
}

void KEY2Parser::parsePath(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::path, KEY2Token::NS_URI_SF));

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::bezier_path :
      case KEY2Token::editable_bezier_path :
        parseBezierPath(element);
        break;
      case KEY2Token::callout2_path :
        parseCallout2Path(element);
        break;
      case KEY2Token::connection_path :
        parseConnectionPath(element);
        break;
      case KEY2Token::point_path :
        parsePointPath(element);
        break;
      case KEY2Token::scalar_path :
        parseScalarPath(element);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }
}

void KEY2Parser::parseShape(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::shape, KEY2Token::NS_URI_SF));

  getCollector()->startText(true);

  const optional<ID_t> id = readID(reader);

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::geometry :
        parseGeometry(element);
        break;
      case KEY2Token::path :
        parsePath(element);
        break;
      case KEY2Token::text :
        parseText(element);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectShape(id);
  getCollector()->endText();
}

void KEY2Parser::parseStickyNote(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::sticky_note, KEY2Token::NS_URI_KEY)
         || checkElement(reader, KEY2Token::sticky_note, KEY2Token::NS_URI_SF));

  getCollector()->startText(false);
  getCollector()->startLevel();

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::geometry :
        parseGeometry(element);
        break;
      case KEY2Token::text :
        parseText(element);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectStickyNote();

  getCollector()->endLevel();
  getCollector()->endText();
}

void KEY2Parser::parsePlaceholder(const KEYXMLReader &reader, const bool title)
{
  assert(title
         ? checkElement(reader, KEY2Token::title_placeholder, KEY2Token::NS_URI_KEY)
         : checkElement(reader, KEY2Token::body_placeholder, KEY2Token::NS_URI_KEY));

  getCollector()->startText(true);

  const optional<ID_t> id = readID(reader);

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KEY2Token::NS_URI_KEY == getNamespaceId(element)) && (KEY2Token::text == getNameId(element)))
      parseText(element);
    else if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::geometry :
        // ignore; the real geometry comes from style
        skipElement(element);
        break;
      case KEY2Token::style :
      {
        KEYXMLReader readerStyle(element);

        checkNoAttributes(readerStyle);

        KEYXMLReader::ElementIterator elementStyle(readerStyle);
        while (elementStyle.next())
        {
          if ((KEY2Token::NS_URI_SF == getNamespaceId(elementStyle)) && (KEY2Token::placeholder_style_ref == getNameId(elementStyle)))
          {
            const ID_t styleId = readRef(elementStyle);
            const optional<string> none;
            getCollector()->collectPlaceholderStyle(styleId, optional<KEYPropertyMap>(), none, none, true, false);
          }
          else
          {
            skipElement(elementStyle);
          }
        }
        break;
      }
      default :
        skipElement(element);
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectTextPlaceholder(id, title, false);
  getCollector()->endText();
}

void KEY2Parser::parseBezierPath(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::bezier_path, KEY2Token::NS_URI_SF)
         || checkElement(reader, KEY2Token::editable_bezier_path, KEY2Token::NS_URI_SF));

  const optional<ID_t> id = readID(reader);

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::bezier :
        parseBezier(element);
        break;
      case KEY2Token::bezier_ref :
      {
        const ID_t idref = readRef(element);
        getCollector()->collectBezier(idref, KEYPathPtr_t(), true);
        break;
      }
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectBezierPath(id);
}

void KEY2Parser::parseCallout2Path(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::callout2_path, KEY2Token::NS_URI_SF));

  optional<ID_t> id;
  double cornerRadius(0);
  bool tailAtCenter(false);
  double tailPosX(0);
  double tailPosY(0);
  double tailSize(0);

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
      case KEY2Token::cornerRadius :
        cornerRadius = lexical_cast<double>(attr.getValue());
        break;
      case KEY2Token::tailAtCenter :
        tailAtCenter = bool_cast(attr.getValue());
        break;
      case KEY2Token::tailPositionX :
        tailPosX = lexical_cast<double>(attr.getValue());
        break;
      case KEY2Token::tailPositionY :
        tailPosY = lexical_cast<double>(attr.getValue());
        break;
      case KEY2Token::tailSize :
        tailSize = lexical_cast<double>(attr.getValue());
        break;
      default :
        break;
      }
    }
  }

  KEYSize size;

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KEY2Token::NS_URI_SF == getNamespaceId(element)) && (KEY2Token::size == getNameId(element)))
      size = readSize(reader);
    else
      skipElement(element);
  }

  getCollector()->collectCalloutPath(id, size, cornerRadius, tailSize, tailPosX, tailPosY, tailAtCenter);
}

void KEY2Parser::parseConnectionPath(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::connection_path, KEY2Token::NS_URI_SF));

  const optional<ID_t> id = readID(reader);

  KEYSize size;
  pair<optional<double>, optional<double> > point;

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::point :
        point = readPoint(element);
        break;
      case KEY2Token::size :
        size = readSize(element);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectConnectionPath(id, size, get_optional_value_or(point.first, 0), get_optional_value_or(point.second, 0));
}

void KEY2Parser::parsePointPath(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::point_path, KEY2Token::NS_URI_SF));

  optional<ID_t> id;
  bool star = false;
  // right arrow is the default (by my decree .-)
  bool doubleArrow = false;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KEY2Token::NS_URI_SF == getNamespaceId(attr)) && (KEY2Token::type == getNameId(attr)))
    {
      switch (getValueId(attr))
      {
      case KEY2Token::double_ :
        doubleArrow = true;
        break;
      case KEY2Token::right :
        break;
      case KEY2Token::star :
        star = true;
        break;
      default :
        KEY_DEBUG_MSG(("unknown point path type: %s\n", attr.getValue()));
        break;
      }
    }
    else if ((KEY2Token::NS_URI_SFA == getNamespaceId(attr)) && (KEY2Token::ID == getNameId(attr)))
    {
      id = attr.getValue();
    }
  }

  KEYSize size;
  pair<optional<double>, optional<double> > point;

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::point :
        point = readPoint(element);
        break;
      case KEY2Token::size :
        size = readSize(element);
        break;
      default :
        skipElement(element);
        break;
      }
    }
    else
      skipElement(element);
  }

  if (star)
    getCollector()->collectStarPath(id, size, numeric_cast<unsigned>(get_optional_value_or(point.first, 0.0)), get_optional_value_or(point.second, 0));
  else
    getCollector()->collectArrowPath(id, size, get_optional_value_or(point.first, 0), get_optional_value_or(point.second, 0), doubleArrow);
}

void KEY2Parser::parseScalarPath(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::scalar_path, KEY2Token::NS_URI_SF));

  optional<ID_t> id;
  bool polygon = false;
  double value = 0;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KEY2Token::scalar :
        value = lexical_cast<double>(attr.getValue());
        break;
      case KEY2Token::type :
      {
        switch (getValueId(attr))
        {
        case KEY2Token::_0 :
          break;
        case KEY2Token::_1 :
          polygon = true;
          break;
        default :
          KEY_DEBUG_MSG(("unknown scalar path type: %s\n", attr.getValue()));
          break;
        }
        break;
      }
      default :
        break;
      }
    }
    else if ((KEY2Token::NS_URI_SFA == getNamespaceId(attr)) && (KEY2Token::ID == getNameId(attr)))
    {
      id = attr.getValue();
    }
  }

  KEYSize size;

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KEY2Token::NS_URI_SF == getNamespaceId(element)) && (KEY2Token::size == getNameId(element)))
      size = readSize(element);
    else
      skipElement(element);
  }

  if (polygon)
    getCollector()->collectPolygonPath(id, size, numeric_cast<unsigned>(value));
  else
    getCollector()->collectRoundedRectanglePath(id, size, value);
}

void KEY2Parser::parseContent(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::content, KEY2Token::NS_URI_SF));

  checkNoAttributes(reader);

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::image_media :
        parseImageMedia(element);
        break;
      case KEY2Token::movie_media :
        parseMovieMedia(element);
        break;
      default :
        skipElement(element);
      }
    }
    else
      skipElement(element);
  }
}

void KEY2Parser::parseData(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::data, KEY2Token::NS_URI_SF));

  optional<ID_t> id;
  optional<string> displayName;
  WPXInputStreamPtr_t stream;
  optional<unsigned> type;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KEY2Token::NS_URI_SFA == getNamespaceId(attr)) && (KEY2Token::ID == getNameId(attr)))
    {
      KEY_DEBUG_XML_TODO_ATTRIBUTE(attr);
    }
    else if (KEY2Token::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KEY2Token::displayname :
        displayName = attr.getValue();
        break;
      case KEY2Token::hfs_type :
        type = lexical_cast<unsigned>(attr.getValue());
        break;
      case KEY2Token::path :
        stream.reset(m_package->getDocumentOLEStream(attr.getValue()));
        break;
      default :
        break;
      }
    }
  }

  checkEmptyElement(reader);

  getCollector()->collectData(id, stream, displayName, type, false);
}

void KEY2Parser::parseFiltered(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::filtered, KEY2Token::NS_URI_SF));

  optional<ID_t> id;
  optional<KEYSize> size;

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::size :
        size = readSize(element);
        break;
      case KEY2Token::data :
        parseData(element);
        break;
      default :
        skipElement(element);
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectFiltered(id, size);
}

void KEY2Parser::parseFilteredImage(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::filtered_image, KEY2Token::NS_URI_SF));

  const optional<ID_t> id = readID(reader);

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::unfiltered_ref :
      {
        optional<ID_t> idref = readRef(element);
        getCollector()->collectUnfiltered(idref, optional<KEYSize>(), true);
        break;
      }
      case KEY2Token::unfiltered :
        parseUnfiltered(element);
        break;
      case KEY2Token::filtered :
        parseFiltered(element);
        break;
      case KEY2Token::leveled :
        parseLeveled(element);
        break;
      default :
        skipElement(element);
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectFilteredImage(id, false);
}

void KEY2Parser::parseImageMedia(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::image_media, KEY2Token::NS_URI_SF));

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::filtered_image :
        parseFilteredImage(element);
        break;
      default :
        skipElement(element);
      }
    }
    else
      skipElement(element);
  }
}

void KEY2Parser::parseLeveled(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::leveled, KEY2Token::NS_URI_SF));

  const optional<ID_t> id = readID(reader);

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::data :
        parseData(element);
        break;
      case KEY2Token::size :
        KEY_DEBUG_XML_TODO_ELEMENT(element);
        skipElement(element);
        break;
      default :
        skipElement(element);
      }
    }
    else
      skipElement(element);
  }

  getCollector()->collectLeveled(id, optional<KEYSize>());
}

void KEY2Parser::parseUnfiltered(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::unfiltered, KEY2Token::NS_URI_SF));

  const optional<ID_t> id = readID(reader);

  optional<KEYSize> size;

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::size :
        size = readSize(element);
        break;
      case KEY2Token::data :
        parseData(element);
        break;
      default :
        skipElement(element);
      }
    }
    else
    {
      skipElement(element);
    }
  }

  getCollector()->collectUnfiltered(id, size, false);
}

void KEY2Parser::parseMovieMedia(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::movie_media, KEY2Token::NS_URI_SF));

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KEY2Token::NS_URI_SF | KEY2Token::self_contained_movie) == getId(element))
      parseSelfContainedMovie(element);
    else
      skipElement(element);
  }

  getCollector()->collectMovieMedia(optional<ID_t>());
}

void KEY2Parser::parseSelfContainedMovie(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::self_contained_movie, KEY2Token::NS_URI_SF));

  checkNoAttributes(reader);

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KEY2Token::NS_URI_SF | KEY2Token::other_datas) == getId(element))
      parseOtherDatas(element);
    else
      skipElement(element);
  }
}

void KEY2Parser::parseOtherDatas(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::other_datas, KEY2Token::NS_URI_SF));

  checkNoAttributes(reader);

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case KEY2Token::NS_URI_SF | KEY2Token::data :
      parseData(element);
      break;
    case KEY2Token::NS_URI_SF | KEY2Token::data_ref :
    {
      const ID_t idref = readRef(element);
      getCollector()->collectData(idref, WPXInputStreamPtr_t(), optional<string>(), optional<unsigned>(), true);
      break;
    }
    default :
      skipElement(element);
    }
  }
}

void KEY2Parser::parseBr(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::br, KEY2Token::NS_URI_SF)
         || checkElement(reader, KEY2Token::crbr, KEY2Token::NS_URI_SF)
         || checkElement(reader, KEY2Token::intratopicbr, KEY2Token::NS_URI_SF)
         || checkElement(reader, KEY2Token::lnbr, KEY2Token::NS_URI_SF));

  checkNoAttributes(reader);
  checkEmptyElement(reader);

  getCollector()->collectLineBreak();
}

void KEY2Parser::parseLayout(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::layout, KEY2Token::NS_URI_SF));

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KEY2Token::NS_URI_SF == getNamespaceId(attr)) && (KEY2Token::style == getNameId(attr)))
      emitLayoutStyle(attr.getValue());
  }

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KEY2Token::NS_URI_SF == getNamespaceId(element)) && (KEY2Token::p == getNameId(element)))
      parseP(element);
    else
      skipElement(element);
  }
}

void KEY2Parser::parseLink(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::link, KEY2Token::NS_URI_SF));

  KEYXMLReader::MixedIterator mixed(reader);
  while (mixed.next())
  {
    if (mixed.isElement())
    {
      if (KEY2Token::NS_URI_SF == getNamespaceId(mixed))
      {
        switch (getNameId(mixed))
        {
        case KEY2Token::br :
          parseBr(mixed);
          break;
        case KEY2Token::span :
          parseSpan(mixed);
          break;
        default :
          skipElement(mixed);
          break;
        }
      }
      else
      {
        skipElement(mixed);
      }
    }
    else
      getCollector()->collectText(optional<ID_t>(), mixed.getText());
  }
}

void KEY2Parser::parseP(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::p, KEY2Token::NS_URI_SF));

  optional<ID_t> style;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KEY2Token::style :
        style = attr.getValue();
        break;
      default :
        break;
      }
    }
  }

  getCollector()->startParagraph(style);

  KEYXMLReader::MixedIterator mixed(reader);
  while (mixed.next())
  {
    if (mixed.isElement())
    {
      if (KEY2Token::NS_URI_SF == getNamespaceId(mixed))
      {
        switch (getNameId(mixed))
        {
        case KEY2Token::br :
        case KEY2Token::crbr :
        case KEY2Token::intratopicbr :
        case KEY2Token::lnbr :
          parseBr(mixed);
          break;
        case KEY2Token::span :
          parseSpan(mixed);
          break;
        case KEY2Token::tab :
          parseTab(mixed);
          break;
        case KEY2Token::link :
          parseLink(mixed);
          break;
        default :
          skipElement(mixed);
          break;
        }
      }
      else
      {
        skipElement(mixed);
      }
    }
    else
    {
      getCollector()->collectText(style, mixed.getText());
    }
  }

  getCollector()->endParagraph();
}

void KEY2Parser::parseSpan(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::span, KEY2Token::NS_URI_SF));

  optional<ID_t> style;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KEY2Token::NS_URI_SF == getNamespaceId(attr)) && (KEY2Token::style == getNameId(attr)))
      style = attr.getValue();
  }

  KEYXMLReader::MixedIterator mixed(reader);
  while (mixed.next())
  {
    if (mixed.isElement())
    {
      if (KEY2Token::NS_URI_KEY == getNamespaceId(mixed))
      {
        switch (getNameId(mixed))
        {
        case KEY2Token::br :
        case KEY2Token::crbr :
        case KEY2Token::intratopicbr :
        case KEY2Token::lnbr :
          parseBr(mixed);
          break;
        case KEY2Token::tab :
          parseTab(mixed);
          break;
        default :
          skipElement(mixed);
          break;
        }
      }
      else
      {
        skipElement(mixed);
      }
    }
    else
      getCollector()->collectText(style, mixed.getText());
  }
}

void KEY2Parser::parseTab(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::tab, KEY2Token::NS_URI_SF));

  checkNoAttributes(reader);
  checkEmptyElement(reader);

  getCollector()->collectTab();
}

void KEY2Parser::parseText(const KEYXMLReader &reader)
{
  // NOTE: isn't it wonderful that there are two text elements in two
  // different namespaces, but with the same schema?
  assert(checkElement(reader, KEY2Token::text, KEY2Token::NS_URI_KEY)
         || checkElement(reader, KEY2Token::text, KEY2Token::NS_URI_SF));

  optional<ID_t> layoutStyle;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KEY2Token::layoutstyle :
        emitLayoutStyle(attr.getValue());
        break;
      default :
        break;
      }
    }
  }

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KEY2Token::NS_URI_SF == getNamespaceId(element)) && (KEY2Token::text_storage == getNameId(element)))
      parseTextStorage(element);
    else
      skipElement(element);
  }
}

void KEY2Parser::parseTextBody(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::text_body, KEY2Token::NS_URI_SF));

  checkNoAttributes(reader);

  bool layout = false;
  bool para = false;
  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::layout :
        if (layout || para)
        {
          KEY_DEBUG_MSG(("layout following another element, not allowed, skipping\n"));
          skipElement(element);
        }
        else
        {
          parseLayout(element);
          layout = true;
        }
        break;
      case KEY2Token::p :
        if (layout)
        {
          KEY_DEBUG_MSG(("paragraph following layout, not allowed, skipping\n"));
          skipElement(element);
        }
        else if (para)
        {
          parseP(element);
        }
        else
        {
          para = true;
          parseP(element);
        }
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

void KEY2Parser::parseTextStorage(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::text_storage, KEY2Token::NS_URI_SF));

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KEY2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KEY2Token::text_body :
        parseTextBody(element);
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

void KEY2Parser::emitLayoutStyle(const ID_t &id)
{
  optional<KEYPropertyMap> dummyProps;
  optional<string> dummyIdent;
  getCollector()->collectLayoutStyle(id, dummyProps, dummyIdent, dummyIdent, true, false);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
