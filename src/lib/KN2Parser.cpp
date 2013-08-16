/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include <boost/lexical_cast.hpp>

#include "libkeynote_utils.h"
#include "libkeynote_xml.h"
#include "KN2Parser.h"
#include "KN2Token.h"
#include "KNCollector.h"
#include "KNStyle.h"
#include "KNTypes.h"
#include "KNXMLAttributeIterator.h"

using boost::lexical_cast;
using boost::optional;

using std::string;

namespace libkeynote
{

namespace
{

bool asBool(const char *const value)
{
  switch (getKN2TokenID(value))
  {
  case KN2Token::_1 :
  case KN2Token::true_ :
    return true;
  case KN2Token::_0 :
  case KN2Token::false_ :
  default :
    return false;
  }

  return false;
}

double asDouble(const char *const value)
{
  return lexical_cast<double>(value);
}

bool checkElement(xmlTextReaderPtr reader, const int name, const int ns, const bool start = true)
{
  return isElement(reader)
         && getNamespace(reader) && (getKN2TokenID(getNamespace(reader)) == ns)
         && (getKN2TokenID(getName(reader)) == name)
         && (isStartElement(reader) == start);
}

bool checkNoAttributes(const xmlTextReaderPtr reader)
{
  unsigned count = 0;

  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
  }

  return 0 == count;
}

bool checkEmptyElement(const xmlTextReaderPtr reader)
{
  if (-1 == xmlTextReaderMoveToElement(reader))
    throw GenericException();

  bool empty = true;

  if (!isEmptyElement(reader))
  {
    empty = false;
    // there are no elements
    while (moveToNextNode(reader))
    {
      KN_DEBUG_XML_UNKNOWN("element", getName(reader), getNamespace(reader));
      skipElement(reader);
    }
  }

  return empty;
}

string readOnlyAttribute(const xmlTextReaderPtr reader, const int name, const int ns)
{
  optional<string> value;

  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if ((getKN2TokenID(attr->ns) == ns) && (getKN2TokenID(attr->name) == name))
      value = attr->value;
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
    }
  }

  if (!value)
    throw GenericException();

  return get(value);
}

string readOnlyElementAttribute(const xmlTextReaderPtr reader, const int name, const int ns)
{
  const char *const elementName = getName(reader);
  const char *const elementNs = getNamespace(reader);

  const string value = readOnlyAttribute(reader, name, ns);

  if (!isEmptyElement(reader))
  {
    KN_DEBUG_XML_NOT_EMPTY(elementName, elementNs);
    skipElement(reader);
  }

  return value;
}

KNPoint readPoint(const xmlTextReaderPtr reader)
{
  KNPoint point;

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if (KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns))
    {
      switch (getKN2TokenID(attr->name))
      {
      case KN2Token::x :
        point.x = asDouble(attr->value);
        break;
      case KN2Token::y :
        point.y = asDouble(attr->value);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
    }
  }

  checkEmptyElement(reader);

  return point;
}

KNPosition readPosition(const xmlTextReaderPtr reader)
{
  KNPosition position;

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if (KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns))
    {
      switch (getKN2TokenID(attr->name))
      {
      case KN2Token::x :
        position.x = asDouble(attr->value);
        break;
      case KN2Token::y :
        position.y = asDouble(attr->value);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
    }
  }

  checkEmptyElement(reader);

  return position;
}

KNSize readSize(const xmlTextReaderPtr reader)
{
  KNSize size;

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if (KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns))
    {
      switch (getKN2TokenID(attr->name))
      {
      case KN2Token::h :
        size.height = lexical_cast<unsigned>(attr->value);
        break;
      case KN2Token::w :
        size.width = lexical_cast<unsigned>(attr->value);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
    }
  }

  checkEmptyElement(reader);

  return size;
}

unsigned getVersion(const int token)
{
  switch (token)
  {
  case KN2Token::VERSION_STR_2 :
    return 2;
  case KN2Token::VERSION_STR_3 :
    return 3;
  case KN2Token::VERSION_STR_4 :
    return 4;
  case KN2Token::VERSION_STR_5 :
    return 5;
  }

  return 0;
}

}

KN2Parser::KN2Parser(WPXInputStream *const input, KNCollector *const collector)
  : KNParser(input, collector)
  , m_version(0)
{
}

KN2Parser::~KN2Parser()
{
}

void KN2Parser::processXmlNode(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::presentation, KN2Token::NS_URI_KEY));

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if (attr->ns)
    {
      switch (getKN2TokenID(attr->ns))
      {
      case KN2Token::NS_URI_KEY :
        switch (getKN2TokenID(attr->name))
        {
        case KN2Token::all_images_bundled :
        case KN2Token::compatible_version :
        case KN2Token::kiosk_build_delay :
        case KN2Token::kiosk_slide_delay :
        case KN2Token::mode :
        case KN2Token::play_mode :
        case KN2Token::sticky_visibility :
          KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
          break;
        case KN2Token::version :
          m_version = getVersion(getKN2TokenID(attr->value));
          if (0 == m_version)
          {
            KN_DEBUG_MSG(("unknown version %s\n", attr->value));
          }
          break;
        default :
          KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
          break;
        }
        break;
      case KN2Token::NS_URI_SFA :
        if (KN2Token::ID == getKN2TokenID(attr->name))
        {
          KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
        }
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
    }
  }

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::presentation, KN2Token::NS_URI_KEY, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_KEY == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::size :
      {
        KNSize size;
        parseSize(reader, size);
        getCollector()->collectSize(size);
        break;
      }
      case KN2Token::theme_list :
        parseThemeList(reader);
        break;
      case KN2Token::slide_list :
        parseSlideList(reader);
        break;
      case KN2Token::metadata :
        parseMetadata(reader);
        break;
      case KN2Token::master_order :
      case KN2Token::ui_state :
      case KN2Token::soundtrack_list :
      case KN2Token::calculation_engine :
      case KN2Token::version_history :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else if ((KN2Token::NS_URI_SF == getKN2TokenID(ns)) && (KN2Token::calc_engine == getKN2TokenID(name)))
    {
      KN_DEBUG_XML_TODO("element", name, ns);
      skipElement(reader);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }
}

void KN2Parser::parseDrawables(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::drawables, KN2Token::NS_URI_SF));

  readOnlyAttribute(reader, KN2Token::ID, KN2Token::NS_URI_SFA);

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::NS_URI_SF, KN2Token::drawables, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::group :
      case KN2Token::image :
      case KN2Token::line :
      case KN2Token::media :
      case KN2Token::shape :
      case KN2Token::body_placeholder_ref :
      case KN2Token::slide_number_placeholder_ref :
      case KN2Token::table_info :
      case KN2Token::title_placeholder_ref :
      case KN2Token::chart_info :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }
}

void KN2Parser::parseLayer(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::layer, KN2Token::NS_URI_SF));

  const ID_t id = readOnlyAttribute(reader, KN2Token::ID, KN2Token::NS_URI_SFA);

  getCollector()->startLayer();

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::layer, KN2Token::NS_URI_SF, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::drawables :
        parseDrawables(reader);
        break;
      case KN2Token::type :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;
      case KN2Token::guides :
        // ignore
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }

  getCollector()->collectLayer(id, false);
  getCollector()->endLayer();
}

void KN2Parser::parseLayers(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::layers, KN2Token::NS_URI_SF));

  readOnlyAttribute(reader, KN2Token::ID, KN2Token::NS_URI_SFA);

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::layers, KN2Token::NS_URI_SF, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::layer :
        parseLayer(reader);
        break;
      case KN2Token::proxy_master_layer :
        parseProxyMasterLayer(reader);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }
}

void KN2Parser::parseMasterSlide(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::master_slide, KN2Token::NS_URI_KEY));

  ID_t id;

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if (attr->ns)
    {
      switch (getKN2TokenID(attr->ns))
      {
      case KN2Token::NS_URI_KEY :
        switch (getKN2TokenID(attr->name))
        {
        case KN2Token::layer :
        case KN2Token::name :
          KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
          break;
        default :
          KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
          break;
        }
      case KN2Token::NS_URI_SFA :
        if (KN2Token::ID == getKN2TokenID(attr->name))
        {
          id = attr->value;
        }
        else
        {
          KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
        }
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
    }
  }

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::master_slide, KN2Token::NS_URI_KEY, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_KEY == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::page :
        parsePage(reader);
        break;
      case KN2Token::stylesheet :
        parseStylesheet(reader);
        break;
      case KN2Token::style_ref :
      case KN2Token::title_placeholder :
      case KN2Token::body_placeholder :
      case KN2Token::object_placeholder :
      case KN2Token::slide_number_placeholder :
      case KN2Token::bullets :
      case KN2Token::thumbnails :
      case KN2Token::build_chunks :
      case KN2Token::sticky_notes :
      case KN2Token::events :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }

  getCollector()->collectPage(id);
}

void KN2Parser::parseMasterSlides(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::master_slides, KN2Token::NS_URI_KEY));

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns)) && (KN2Token::ID == getKN2TokenID(attr->name)))
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
      break;
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
    }
  }

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::master_slides, KN2Token::NS_URI_KEY, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if ((KN2Token::NS_URI_KEY == getKN2TokenID(ns)) && (KN2Token::master_slide == getKN2TokenID(name)))
    {
      KN_DEBUG_XML_TODO("element", name, ns);
      parseMasterSlide(reader);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }
}

void KN2Parser::parseMetadata(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::metadata, KN2Token::NS_URI_KEY));

  // there are no attributes for key:metadata
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
  }

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::presentation, KN2Token::NS_URI_KEY, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_KEY == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::authors :
      case KN2Token::keywords :
      case KN2Token::title :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }
}

void KN2Parser::parsePage(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::page, KN2Token::NS_URI_KEY));

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns)) && (KN2Token::ID == getKN2TokenID(attr->name)))
    {
      KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
    }
  }

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::page, KN2Token::NS_URI_KEY, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::size :
      {
        KNSize size;
        parseSize(reader, size);
        // TODO: use size
        break;
      }
      case KN2Token::geometry :
      case KN2Token::layers :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }
}

void parsePropertyMap(xmlTextReaderPtr reader, KNStyle &style)
{
  assert(checkElement(reader, KN2Token::property_map, KN2Token::NS_URI_SF));

  checkNoAttributes(reader);

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::property_map, KN2Token::NS_URI_SF, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::BGBuildDurationProperty :
      case KN2Token::SFC2DAntialiasingModeProperty :
      case KN2Token::SFC2DAreaDataPointFillProperty :
      case KN2Token::SFC2DAreaDataPointStrokeProperty :
      case KN2Token::SFC2DAreaDataPointSymbolProperty :
      case KN2Token::SFC2DAreaFillProperty :
      case KN2Token::SFC2DAreaShadowProperty :
      case KN2Token::SFC2DAreaStrokeProperty :
      case KN2Token::SFC2DAreaUseSeriesFillForDataPointFillProperty :
      case KN2Token::SFC2DAreaUseStrokeColorForDataPointFillProperty :
      case KN2Token::SFC2DBarFillProperty :
      case KN2Token::SFC2DBarShadowProperty :
      case KN2Token::SFC2DBarStrokeProperty :
      case KN2Token::SFC2DBottomBorderOpacityProperty :
      case KN2Token::SFC2DBottomBorderShadowProperty :
      case KN2Token::SFC2DBottomBorderStrokeProperty :
      case KN2Token::SFC2DCategoryDirectionGridLineOpacityProperty :
      case KN2Token::SFC2DCategoryDirectionGridLineShadowProperty :
      case KN2Token::SFC2DCategoryDirectionGridLineStrokeProperty :
      case KN2Token::SFC2DCategoryDirectionMinorGridLineIntervalProperty :
      case KN2Token::SFC2DCategoryDirectionMinorGridLineOpacityProperty :
      case KN2Token::SFC2DCategoryDirectionMinorGridLineShadowProperty :
      case KN2Token::SFC2DCategoryDirectionMinorGridLineStrokeProperty :
      case KN2Token::SFC2DChartBackgroundFillProperty :
      case KN2Token::SFC2DChartBackgroundOpacityProperty :
      case KN2Token::SFC2DColumnFillProperty :
      case KN2Token::SFC2DCombineLayersProperty :
      case KN2Token::SFC2DLeftBorderOpacityProperty :
      case KN2Token::SFC2DLeftBorderShadowProperty :
      case KN2Token::SFC2DLeftBorderStrokeProperty :
      case KN2Token::SFC2DLineConnectedPointsProperty :
      case KN2Token::SFC2DLineDataPointFillProperty :
      case KN2Token::SFC2DLineDataPointStrokeProperty :
      case KN2Token::SFC2DLineDataPointSymbolProperty :
      case KN2Token::SFC2DLineShadowProperty :
      case KN2Token::SFC2DLineStrokeProperty :
      case KN2Token::SFC2DLineUseSeriesFillForDataPointFillProperty :
      case KN2Token::SFC2DLineUseStrokeColorForDataPointFillProperty :
      case KN2Token::SFC2DMixedAreaDataPointFillProperty :
      case KN2Token::SFC2DMixedAreaDataPointStrokeProperty :
      case KN2Token::SFC2DMixedAreaDataPointSymbolProperty :
      case KN2Token::SFC2DMixedAreaFillProperty :
      case KN2Token::SFC2DMixedAreaShadowProperty :
      case KN2Token::SFC2DMixedAreaStrokeProperty :
      case KN2Token::SFC2DMixedAreaUseSeriesFillForDataPointFillProperty :
      case KN2Token::SFC2DMixedAreaUseStrokeColorForDataPointFillProperty :
      case KN2Token::SFC2DMixedColumnFillProperty :
      case KN2Token::SFC2DMixedColumnShadowProperty :
      case KN2Token::SFC2DMixedColumnStrokeProperty :
      case KN2Token::SFC2DMixedConnectedPointsProperty :
      case KN2Token::SFC2DMixedLineDataPointFillProperty :
      case KN2Token::SFC2DMixedLineDataPointStrokeProperty :
      case KN2Token::SFC2DMixedLineDataPointSymbolProperty :
      case KN2Token::SFC2DMixedLineShadowProperty :
      case KN2Token::SFC2DMixedLineStrokeProperty :
      case KN2Token::SFC2DMixedLineUseStrokeColorForDataPointFillProperty :
      case KN2Token::SFC2DOpacityProperty :
      case KN2Token::SFC2DPieFillProperty :
      case KN2Token::SFC2DPieShadowProperty :
      case KN2Token::SFC2DPieStrokeProperty :
      case KN2Token::SFC2DRightBorderOpacityProperty :
      case KN2Token::SFC2DRightBorderShadowProperty :
      case KN2Token::SFC2DRightBorderStrokeProperty :
      case KN2Token::SFC2DScatterDataPointFillProperty :
      case KN2Token::SFC2DScatterDataPointStrokeProperty :
      case KN2Token::SFC2DScatterDataPointSymbolProperty :
      case KN2Token::SFC2DScatterDataPointSymbolSizeProperty :
      case KN2Token::SFC2DScatterShadowProperty :
      case KN2Token::SFC2DScatterStrokeProperty :
      case KN2Token::SFC2DShowBottomBorderProperty :
      case KN2Token::SFC2DShowBottomTicksProperty :
      case KN2Token::SFC2DShowCategoryDirectionGridLinesProperty :
      case KN2Token::SFC2DShowCategoryDirectionMinorGridLinesProperty :
      case KN2Token::SFC2DShowLeftBorderProperty :
      case KN2Token::SFC2DShowLeftTicksProperty :
      case KN2Token::SFC2DShowRightBorderProperty :
      case KN2Token::SFC2DShowRightTicksProperty :
      case KN2Token::SFC2DShowTopBorderProperty :
      case KN2Token::SFC2DShowTopTicksProperty :
      case KN2Token::SFC2DShowValueDirectionGridLinesProperty :
      case KN2Token::SFC2DShowValueDirectionMinorGridLinesProperty :
      case KN2Token::SFC2DTopBorderOpacityProperty :
      case KN2Token::SFC2DTopBorderShadowProperty :
      case KN2Token::SFC2DTopBorderStrokeProperty :
      case KN2Token::SFC2DValueDirectionGridLineOpacityProperty :
      case KN2Token::SFC2DValueDirectionGridLineShadowProperty :
      case KN2Token::SFC2DValueDirectionGridLineStrokeProperty :
      case KN2Token::SFC2DValueDirectionMinorGridLineIntervalProperty :
      case KN2Token::SFC2DValueDirectionMinorGridLineOpacityProperty :
      case KN2Token::SFC2DValueDirectionMinorGridLineShadowProperty :
      case KN2Token::SFC2DValueDirectionMinorGridLineStrokeProperty :
      case KN2Token::SFC3DAreaChartScaleProperty :
      case KN2Token::SFC3DAreaFillProperty :
      case KN2Token::SFC3DAreaLightingPackageProperty :
      case KN2Token::SFC3DAreaShadowProperty :
      case KN2Token::SFC3DBarChartScaleProperty :
      case KN2Token::SFC3DBarFillProperty :
      case KN2Token::SFC3DBarLightingPackageProperty :
      case KN2Token::SFC3DBarShadowProperty :
      case KN2Token::SFC3DBarShapeProperty :
      case KN2Token::SFC3DBevelEdgesProperty :
      case KN2Token::SFC3DCameraTypeProperty :
      case KN2Token::SFC3DCategoryDirectionGridLineOpacityProperty :
      case KN2Token::SFC3DCategoryDirectionGridLineStrokeProperty :
      case KN2Token::SFC3DChartRotationProperty :
      case KN2Token::SFC3DChartScaleProperty :
      case KN2Token::SFC3DColumnBevelEdgesProperty :
      case KN2Token::SFC3DColumnChartScaleProperty :
      case KN2Token::SFC3DColumnFillProperty :
      case KN2Token::SFC3DColumnLightingPackageProperty :
      case KN2Token::SFC3DFloorDepthProperty :
      case KN2Token::SFC3DFloorEmissiveColorProperty :
      case KN2Token::SFC3DFloorFillProperty :
      case KN2Token::SFC3DFloorSpecularColorProperty :
      case KN2Token::SFC3DInterSetDepthGapProperty :
      case KN2Token::SFC3DLabelThicknessProperty :
      case KN2Token::SFC3DLightingPackageProperty :
      case KN2Token::SFC3DLineChartScaleProperty :
      case KN2Token::SFC3DLineFillProperty :
      case KN2Token::SFC3DLineLightingPackageProperty :
      case KN2Token::SFC3DLineShadowProperty :
      case KN2Token::SFC3DMaterialPackageProperty :
      case KN2Token::SFC3DPieBevelEdgesProperty :
      case KN2Token::SFC3DPieChartScaleProperty :
      case KN2Token::SFC3DPieFillProperty :
      case KN2Token::SFC3DPieLightingPackageProperty :
      case KN2Token::SFC3DPieShadowProperty :
      case KN2Token::SFC3DReflectionsLevelProperty :
      case KN2Token::SFC3DSceneBackgroundColorProperty :
      case KN2Token::SFC3DSceneShowTextureProperty :
      case KN2Token::SFC3DSceneTextureTilingProperty :
      case KN2Token::SFC3DSeriesEmissiveColorProperty :
      case KN2Token::SFC3DSeriesShininessProperty :
      case KN2Token::SFC3DSeriesSpecularColorProperty :
      case KN2Token::SFC3DShadowCameraXProperty :
      case KN2Token::SFC3DShadowCameraYProperty :
      case KN2Token::SFC3DShadowProperty :
      case KN2Token::SFC3DShowBackWallProperty :
      case KN2Token::SFC3DShowFloorProperty :
      case KN2Token::SFC3DShowReflectionsProperty :
      case KN2Token::SFC3DTextureProperty :
      case KN2Token::SFC3DTextureTilingProperty :
      case KN2Token::SFC3DValueDirectionGridLineOpacityProperty :
      case KN2Token::SFC3DValueDirectionGridLineStrokeProperty :
      case KN2Token::SFC3DWallEmissiveColorProperty :
      case KN2Token::SFC3DWallFillProperty :
      case KN2Token::SFC3DWallSpecularColorProperty :
      case KN2Token::SFC3DWallsShininessProperty :
      case KN2Token::SFCAreaSeriesValueParagraphStyleProperty :
      case KN2Token::SFCAreaShowValueLabelProperty :
      case KN2Token::SFCAreaValueLabelPositionProperty :
      case KN2Token::SFCAxisLabelsOrientationProperty :
      case KN2Token::SFCBarSeriesValueParagraphStyleProperty :
      case KN2Token::SFCBarShowValueLabelProperty :
      case KN2Token::SFCBarValueLabelPositionProperty :
      case KN2Token::SFCCategoryAxisLabelsDepthProperty :
      case KN2Token::SFCCategoryAxisLabelsOpacityProperty :
      case KN2Token::SFCCategoryAxisLabelsOrientationProperty :
      case KN2Token::SFCCategoryAxisLabelsPositionProperty :
      case KN2Token::SFCCategoryAxisMajorTickLengthProperty :
      case KN2Token::SFCCategoryAxisMajorTickStrokeProperty :
      case KN2Token::SFCCategoryAxisMinorTickLengthProperty :
      case KN2Token::SFCCategoryAxisMinorTickStrokeProperty :
      case KN2Token::SFCCategoryAxisParagraphStyleProperty :
      case KN2Token::SFCCategoryAxisShowMajorTickMarksProperty :
      case KN2Token::SFCCategoryAxisShowMinorTickMarksProperty :
      case KN2Token::SFCCategoryAxisTickMarkLocationProperty :
      case KN2Token::SFCCategoryAxisTitleParagraphStyleProperty :
      case KN2Token::SFCCategoryAxisTitlePositionProperty :
      case KN2Token::SFCCategoryHorizontalSpacingProperty :
      case KN2Token::SFCCategoryVerticalSpacingProperty :
      case KN2Token::SFCChartTitleParagraphStyleProperty :
      case KN2Token::SFCChartTitlePositionProperty :
      case KN2Token::SFCDefaultChartAngleProperty :
      case KN2Token::SFCDefaultChartBoundsProperty :
      case KN2Token::SFCDefaultLegendAngleProperty :
      case KN2Token::SFCDefaultLegendBoundsProperty :
      case KN2Token::SFCDepthProperty :
      case KN2Token::SFCEmissiveColorProperty :
      case KN2Token::SFCErrorBarXAxisStrokeProperty :
      case KN2Token::SFCErrorBarXLineEndProperty :
      case KN2Token::SFCErrorBarYAxisShadowProperty :
      case KN2Token::SFCErrorBarYAxisStrokeProperty :
      case KN2Token::SFCErrorBarYLineEndProperty :
      case KN2Token::SFCHorizontalAxisLogarithmicProperty :
      case KN2Token::SFCHorizontalAxisNumberFormatSeparatorProperty :
      case KN2Token::SFCHorizontalAxisNumberOfDecadesProperty :
      case KN2Token::SFCHorizontalAxisNumberOfStepsProperty :
      case KN2Token::SFCHorizontalAxisShowMinimumValueProperty :
      case KN2Token::SFCInitialNumberOfSeriesProperty :
      case KN2Token::SFCInterBarGapProperty :
      case KN2Token::SFCInterSetGapProperty :
      case KN2Token::SFCLabelOpacityProperty :
      case KN2Token::SFCLabelShadowProperty :
      case KN2Token::SFCLegendFillProperty :
      case KN2Token::SFCLegendOpacityProperty :
      case KN2Token::SFCLegendParagraphStyleProperty :
      case KN2Token::SFCLegendPositionProperty :
      case KN2Token::SFCLegendShadowProperty :
      case KN2Token::SFCLegendStrokeProperty :
      case KN2Token::SFCLineSeriesValueParagraphStyleProperty :
      case KN2Token::SFCLineShowValueLabelProperty :
      case KN2Token::SFCLineValueLabelPositionProperty :
      case KN2Token::SFCMixedAreaValueLabelPositionProperty :
      case KN2Token::SFCMixedColumnValueLabelPositionProperty :
      case KN2Token::SFCMixedLineValueLabelPositionProperty :
      case KN2Token::SFCMixedSeriesValueParagraphStyleProperty :
      case KN2Token::SFCMixedShowValueLabelProperty :
      case KN2Token::SFCNumberOfDefinedSeriesStylesProperty :
      case KN2Token::SFCPieSeriesStartAngleProperty :
      case KN2Token::SFCPieSeriesValueParagraphStyleProperty :
      case KN2Token::SFCPieShowSeriesNameLabelProperty :
      case KN2Token::SFCPieShowValueLabelProperty :
      case KN2Token::SFCPieValueLabelPosition2Property :
      case KN2Token::SFCPieValueLabelPositionProperty :
      case KN2Token::SFCPieWedgeExplosionProperty :
      case KN2Token::SFCPieWedgeGroupingsProperty :
      case KN2Token::SFCSeriesValueLabelDateTimeFormatProperty :
      case KN2Token::SFCSeriesValueLabelDurationFormatProperty :
      case KN2Token::SFCSeriesValueLabelFormatTypeProperty :
      case KN2Token::SFCSeriesValueLabelNumberFormatProperty :
      case KN2Token::SFCSeriesValueLabelPercentageAxisNumberFormatProperty :
      case KN2Token::SFCSeriesValueLabelPieFormatTypeProperty :
      case KN2Token::SFCSeriesValueLabelPieNumberFormatProperty :
      case KN2Token::SFCShininessProperty :
      case KN2Token::SFCShowCategoryAxisLabelsProperty :
      case KN2Token::SFCShowCategoryAxisSeriesLabelsProperty :
      case KN2Token::SFCShowCategoryAxisTitleProperty :
      case KN2Token::SFCShowChartTitleProperty :
      case KN2Token::SFCShowLastCategoryAxisLabelsProperty :
      case KN2Token::SFCShowPieLabelAsPercentageProperty :
      case KN2Token::SFCShowValueAxisTitleProperty :
      case KN2Token::SFCShowValueY2AxisTitleProperty :
      case KN2Token::SFCSpecularColorProperty :
      case KN2Token::SFCStackedAreaValueLabelPositionProperty :
      case KN2Token::SFCStackedBarValueLabelPositionProperty :
      case KN2Token::SFCTrendLineShadowProperty :
      case KN2Token::SFCTrendLineStrokeProperty :
      case KN2Token::SFCTrendlineEquationOpacityProperty :
      case KN2Token::SFCTrendlineEquationParagraphStyleProperty :
      case KN2Token::SFCTrendlineRSquaredOpacityProperty :
      case KN2Token::SFCTrendlineRSquaredParagraphStyleProperty :
      case KN2Token::SFCValueAxisDateTimeFormatProperty :
      case KN2Token::SFCValueAxisDurationFormatProperty :
      case KN2Token::SFCValueAxisFormatTypeProperty :
      case KN2Token::SFCValueAxisHorizontalDateTimeFormatProperty :
      case KN2Token::SFCValueAxisHorizontalDurationFormatProperty :
      case KN2Token::SFCValueAxisHorizontalFormatTypeProperty :
      case KN2Token::SFCValueAxisHorizontalNumberFormatProperty :
      case KN2Token::SFCValueAxisLabelsDepthProperty :
      case KN2Token::SFCValueAxisLabelsOpacityProperty :
      case KN2Token::SFCValueAxisLabelsOrientationProperty :
      case KN2Token::SFCValueAxisLabelsPositionProperty :
      case KN2Token::SFCValueAxisLogarithmicProperty :
      case KN2Token::SFCValueAxisMajorTickLengthProperty :
      case KN2Token::SFCValueAxisMajorTickStrokeProperty :
      case KN2Token::SFCValueAxisMaximumValueProperty :
      case KN2Token::SFCValueAxisMinimumValueProperty :
      case KN2Token::SFCValueAxisMinorTickLengthProperty :
      case KN2Token::SFCValueAxisMinorTickStrokeProperty :
      case KN2Token::SFCValueAxisNumberFormatProperty :
      case KN2Token::SFCValueAxisNumberOfDecadesProperty :
      case KN2Token::SFCValueAxisNumberOfStepsProperty :
      case KN2Token::SFCValueAxisParagraphStyleProperty :
      case KN2Token::SFCValueAxisPercentageProperty :
      case KN2Token::SFCValueAxisShowMajorTickMarksProperty :
      case KN2Token::SFCValueAxisShowMinimumValueProperty :
      case KN2Token::SFCValueAxisShowMinorTickMarksProperty :
      case KN2Token::SFCValueAxisTickMarkLocationProperty :
      case KN2Token::SFCValueAxisTitleParagraphStyleProperty :
      case KN2Token::SFCValueAxisTitlePositionPropertya :
      case KN2Token::SFCValueAxisY2DateTimeFormatProperty :
      case KN2Token::SFCValueAxisY2DurationFormatProperty :
      case KN2Token::SFCValueAxisY2FormatTypeProperty :
      case KN2Token::SFCValueAxisY2NumberFormatProperty :
      case KN2Token::SFCValueAxisY2ParagraphStyleProperty :
      case KN2Token::SFCValueAxisY2TitleParagraphStyleProperty :
      case KN2Token::SFCValueNumberFormatDecimalPlacesProperty :
      case KN2Token::SFCValueNumberFormatPrefixProperty :
      case KN2Token::SFCValueNumberFormatSeparatorProperty :
      case KN2Token::SFCValueNumberFormatSuffixProperty :
      case KN2Token::SFCValueY2AxisLabelsDepthProperty :
      case KN2Token::SFCValueY2AxisLabelsOpacityProperty :
      case KN2Token::SFCValueY2AxisLabelsOrientationProperty :
      case KN2Token::SFCValueY2AxisLabelsPositionProperty :
      case KN2Token::SFCValueY2AxisLogarithmicProperty :
      case KN2Token::SFCValueY2AxisMajorTickLengthProperty :
      case KN2Token::SFCValueY2AxisMajorTickStrokeProperty :
      case KN2Token::SFCValueY2AxisMinorTickLengthProperty :
      case KN2Token::SFCValueY2AxisMinorTickStrokeProperty :
      case KN2Token::SFCValueY2AxisNumberOfDecadesProperty :
      case KN2Token::SFCValueY2AxisNumberOfStepsProperty :
      case KN2Token::SFCValueY2AxisPercentageProperty :
      case KN2Token::SFCValueY2AxisShowMajorTickMarksProperty :
      case KN2Token::SFCValueY2AxisShowMinimumValueProperty :
      case KN2Token::SFCValueY2AxisShowMinorTickMarksProperty :
      case KN2Token::SFCValueY2AxisTickMarkLocationProperty :
      case KN2Token::SFTAutoResizeProperty :
      case KN2Token::SFTBackgroundProperty :
      case KN2Token::SFTCellStylePropertyDateTimeFormat :
      case KN2Token::SFTCellStylePropertyDurationFormat :
      case KN2Token::SFTCellStylePropertyFormatType :
      case KN2Token::SFTCellStylePropertyImplicitFormatType :
      case KN2Token::SFTCellStylePropertyLayoutStyle :
      case KN2Token::SFTCellStylePropertyNumberFormat :
      case KN2Token::SFTCellStylePropertyParagraphStyle :
      case KN2Token::SFTCellTextWrapProperty :
      case KN2Token::SFTDefaultBodyCellStyleProperty :
      case KN2Token::SFTDefaultBodyVectorStyleProperty :
      case KN2Token::SFTDefaultBorderVectorStyleProperty :
      case KN2Token::SFTDefaultColumnCountProperty :
      case KN2Token::SFTDefaultFooterBodyVectorStyleProperty :
      case KN2Token::SFTDefaultFooterBorderVectorStyleProperty :
      case KN2Token::SFTDefaultFooterRowCellStyleProperty :
      case KN2Token::SFTDefaultFooterRowCountProperty :
      case KN2Token::SFTDefaultFooterSeparatorVectorStyleProperty :
      case KN2Token::SFTDefaultGeometryProperty :
      case KN2Token::SFTDefaultGroupingLevel0VectorStyleProperty :
      case KN2Token::SFTDefaultGroupingLevel1VectorStyleProperty :
      case KN2Token::SFTDefaultGroupingLevel2VectorStyleProperty :
      case KN2Token::SFTDefaultGroupingLevel3VectorStyleProperty :
      case KN2Token::SFTDefaultGroupingLevel4VectorStyleProperty :
      case KN2Token::SFTDefaultGroupingRow0CellStyleProperty :
      case KN2Token::SFTDefaultGroupingRow1CellStyleProperty :
      case KN2Token::SFTDefaultGroupingRow2CellStyleProperty :
      case KN2Token::SFTDefaultGroupingRow3CellStyleProperty :
      case KN2Token::SFTDefaultGroupingRow4CellStyleProperty :
      case KN2Token::SFTDefaultHeaderBodyVectorStyleProperty :
      case KN2Token::SFTDefaultHeaderBorderVectorStyleProperty :
      case KN2Token::SFTDefaultHeaderColumnCellStyleProperty :
      case KN2Token::SFTDefaultHeaderColumnCountProperty :
      case KN2Token::SFTDefaultHeaderRowCellStyleProperty :
      case KN2Token::SFTDefaultHeaderRowCountProperty :
      case KN2Token::SFTDefaultHeaderSeparatorVectorStyleProperty :
      case KN2Token::SFTDefaultRowCountProperty :
      case KN2Token::SFTDefaultTableNameIsDisplayedProperty :
      case KN2Token::SFTGroupingRowFillProperty :
      case KN2Token::SFTHeaderColumnRepeatsProperty :
      case KN2Token::SFTHeaderRowRepeatsProperty :
      case KN2Token::SFTStrokeProperty :
      case KN2Token::SFTTableBandedCellFillProperty :
      case KN2Token::SFTTableBandedRowsProperty :
      case KN2Token::SFTTableBehaviorProperty :
      case KN2Token::SFTableCellStylePropertyFill :
      case KN2Token::SFTableCellStylePropertyType :
      case KN2Token::SFTableStylePropertyBackgroundFill :
      case KN2Token::SFTableStylePropertyBorderVectorStyle :
      case KN2Token::SFTableStylePropertyCellLayoutStyle :
      case KN2Token::SFTableStylePropertyCellParagraphStyle :
      case KN2Token::SFTableStylePropertyCellStyle :
      case KN2Token::SFTableStylePropertyHeaderBorderVectorStyle :
      case KN2Token::SFTableStylePropertyHeaderColumnCellLayoutStyle :
      case KN2Token::SFTableStylePropertyHeaderColumnCellParagraphStyle :
      case KN2Token::SFTableStylePropertyHeaderColumnCellStyle :
      case KN2Token::SFTableStylePropertyHeaderRowCellLayoutStyle :
      case KN2Token::SFTableStylePropertyHeaderRowCellParagraphStyle :
      case KN2Token::SFTableStylePropertyHeaderRowCellStyle :
      case KN2Token::SFTableStylePropertyHeaderSeperatorVectorStyle :
      case KN2Token::SFTableStylePropertyHeaderVectorStyle :
      case KN2Token::SFTableStylePropertyPrototypeColumnCount :
      case KN2Token::SFTableStylePropertyPrototypeGeometry :
      case KN2Token::SFTableStylePropertyPrototypeIsHeaderColumn :
      case KN2Token::SFTableStylePropertyPrototypeIsHeaderRow :
      case KN2Token::SFTableStylePropertyPrototypeIsResize :
      case KN2Token::SFTableStylePropertyPrototypeRowCount :
      case KN2Token::SFTableStylePropertyVectorStyle :
      case KN2Token::Series_0 :
      case KN2Token::Series_1 :
      case KN2Token::Series_2 :
      case KN2Token::Series_3 :
      case KN2Token::Series_4 :
      case KN2Token::Series_5 :
      case KN2Token::TableCellStylePropertyFormatDecimals :
      case KN2Token::TableCellStylePropertyFormatEnabled :
      case KN2Token::TableCellStylePropertyFormatNegativeStyle :
      case KN2Token::TableCellStylePropertyFormatPrefix :
      case KN2Token::TableCellStylePropertyFormatSuffix :
      case KN2Token::TableCellStylePropertyFormatThousandsSeparator :
      case KN2Token::TableVectorStyleStrokeProperty :
      case KN2Token::TableVectorStyleTypeProperty :
      case KN2Token::alignment :
      case KN2Token::animationAutoPlay :
      case KN2Token::animationDelay :
      case KN2Token::animationDuration :
      case KN2Token::atsuFontFeatures :
      case KN2Token::baselineShift :
      case KN2Token::blendMode :
      case KN2Token::bodyPlaceholderVisibility :
      case KN2Token::bold :
      case KN2Token::bulletListStyle :
      case KN2Token::capitalization :
      case KN2Token::columns :
      case KN2Token::decimalTab :
      case KN2Token::defaultTabStops :
      case KN2Token::dropCap :
      case KN2Token::effect :
      case KN2Token::effectColor :
      case KN2Token::externalTextWrap :
      case KN2Token::fill :
      case KN2Token::filters :
      case KN2Token::firstLineIndent :
      case KN2Token::firstTopicNumber :
      case KN2Token::followingLayoutStyle :
      case KN2Token::followingParagraphStyle :
      case KN2Token::fontColor :
      case KN2Token::fontName :
      case KN2Token::fontSize :
      case KN2Token::geometry :
      case KN2Token::headLineEnd :
      case KN2Token::headOffset :
      case KN2Token::headlineIndent :
      case KN2Token::headlineParagraphStyle :
      case KN2Token::hidden :
      case KN2Token::hyphenate :
      case KN2Token::italic :
      case KN2Token::keepLinesTogether :
      case KN2Token::keepWithNext :
      case KN2Token::kerning :
      case KN2Token::labelCharacterStyle1 :
      case KN2Token::labelCharacterStyle2 :
      case KN2Token::labelCharacterStyle3 :
      case KN2Token::labelCharacterStyle4 :
      case KN2Token::labelCharacterStyle5 :
      case KN2Token::labelCharacterStyle6 :
      case KN2Token::labelCharacterStyle7 :
      case KN2Token::labelCharacterStyle8 :
      case KN2Token::labelCharacterStyle9 :
      case KN2Token::language :
      case KN2Token::layoutContinuous :
      case KN2Token::layoutMargins :
      case KN2Token::layoutParagraphStyle :
      case KN2Token::layoutStyle :
      case KN2Token::leftIndent :
      case KN2Token::ligatures :
      case KN2Token::lineSpacing :
      case KN2Token::listLabelGeometries:
      case KN2Token::listLabelIndents :
      case KN2Token::listLabelTypes :
      case KN2Token::listLevels :
      case KN2Token::listStyle :
      case KN2Token::listTextIndents :
      case KN2Token::minimumHorizontalInset :
      case KN2Token::objectPlaceholderVisibility :
      case KN2Token::opacity :
      case KN2Token::outline :
      case KN2Token::outlineColor :
      case KN2Token::outlineLevel :
      case KN2Token::outlineStyleType :
      case KN2Token::padding :
      case KN2Token::pageBreakBefore :
      case KN2Token::paragraphBorderType :
      case KN2Token::paragraphBorders :
      case KN2Token::paragraphFill :
      case KN2Token::paragraphRuleOffset :
      case KN2Token::paragraphStroke :
      case KN2Token::reflection :
      case KN2Token::rightIndent :
      case KN2Token::shadow :
      case KN2Token::showInTOC :
      case KN2Token::shrinkTextToFit :
      case KN2Token::slideNumberPlaceholderVisibility :
      case KN2Token::spaceAfter :
      case KN2Token::spaceBefore :
      case KN2Token::strikethru :
      case KN2Token::strikethruColor :
      case KN2Token::strikethruWidth :
      case KN2Token::stroke :
      case KN2Token::superscript :
      case KN2Token::tabs :
      case KN2Token::tailLineEnd :
      case KN2Token::tailOffset :
      case KN2Token::textBackground :
      case KN2Token::textBorders :
      case KN2Token::textShadow :
      case KN2Token::titlePlaceholderVisibility :
      case KN2Token::tocStyle :
      case KN2Token::tracking :
      case KN2Token::transition :
      case KN2Token::underline :
      case KN2Token::underlineColor :
      case KN2Token::underlineWidth :
      case KN2Token::verticalAlignment :
      case KN2Token::verticalText :
      case KN2Token::widowControl :
      case KN2Token::word_strikethrough :
      case KN2Token::word_underline :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        (void) style;
        break;

      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }
}

void KN2Parser::parseProxyMasterLayer(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::proxy_master_layer, KN2Token::NS_URI_SF));

  readOnlyAttribute(reader, KN2Token::ID, KN2Token::NS_URI_SFA);

  ID_t ref;

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::proxy_master_layer, KN2Token::NS_URI_SF, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::layer_ref :
        ref = readOnlyElementAttribute(reader, KN2Token::IDREF, KN2Token::NS_URI_SFA);
        break;
      case KN2Token::type :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }

  getCollector()->collectLayer(ref, true);
}

void KN2Parser::parseSize(const xmlTextReaderPtr reader, KNSize &size)
{
  assert(checkElement(reader, KN2Token::size, KN2Token::NS_URI_KEY));
  size = readSize(reader);
}

void KN2Parser::parseSlide(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::slide, KN2Token::NS_URI_KEY));

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if (attr->ns)
    {
      switch (getKN2TokenID(attr->ns))
      {
      case KN2Token::NS_URI_KEY :
        switch (getKN2TokenID(attr->name))
        {
        case KN2Token::collapsed :
        case KN2Token::depth :
        case KN2Token::hidden :
          KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
          break;
        default :
          KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
          break;
        }
      case KN2Token::NS_URI_SFA :
        if (KN2Token::ID == getKN2TokenID(attr->name))
        {
          KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
        }
        else
        {
          KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
        }
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
    }
  }

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::slide, KN2Token::NS_URI_KEY, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_KEY == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::page :
        parsePage(reader);
        break;
      case KN2Token::stylesheet :
        parseStylesheet(reader);
        break;
      case KN2Token::style_ref :
      case KN2Token::title_placeholder :
      case KN2Token::object_placeholder :
      case KN2Token::slide_number_placeholder :
      case KN2Token::bullets :
      case KN2Token::thumbnails :
      case KN2Token::build_chunks :
      case KN2Token::sticky_notes :
      case KN2Token::master_ref :
      case KN2Token::notes :
      case KN2Token::dummy_body_headline :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }
}

void KN2Parser::parseSlideList(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::slide_list, KN2Token::NS_URI_KEY));

  getCollector()->startSlides();

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns)) && (KN2Token::ID == getKN2TokenID(attr->name)))
    {
      KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
    }
  }

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::slide_list, KN2Token::NS_URI_KEY, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if ((KN2Token::NS_URI_KEY == getKN2TokenID(ns)) && (KN2Token::slide == getKN2TokenID(name)))
    {
      parseSlide(reader);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }

  getCollector()->endSlides();
}

ID_t KN2Parser::parseStyle(const xmlTextReaderPtr reader, KNStyle &style)
{
  const int type = getKN2TokenID(getName(reader));
  ID_t id;

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns)) && (KN2Token::ID == getKN2TokenID(attr->name)))
    {
      KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
    }
    else if (KN2Token::NS_URI_SF == getKN2TokenID(attr->ns))
    {
      switch (getKN2TokenID(attr->name))
      {
      case KN2Token::ID :
        id = attr->value;
        break;

      case KN2Token::ident :
      case KN2Token::parent_ident :
      case KN2Token::name :
      case KN2Token::cell_style_default_line_height :
      case KN2Token::locked :
      case KN2Token::seriesIndex :
      case KN2Token::sfclass :
      case KN2Token::tabular_style_name_internal :
        KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
    }
  }

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, type, KN2Token::NS_URI_SF, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if ((KN2Token::NS_URI_SF == getKN2TokenID(ns)) && (KN2Token::property_map == getKN2TokenID(name)))
    {
      parsePropertyMap(reader, style);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }

  return id;
}

void KN2Parser::parseStyles(const xmlTextReaderPtr reader, const bool anonymous)
{
  const int type = anonymous ? KN2Token::anon_styles : KN2Token::styles;
  assert(checkElement(reader, type, KN2Token::NS_URI_SF));

  checkNoAttributes(reader);

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, type, KN2Token::NS_URI_SF, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::characterstyle :
      {
        KNStyle style;
        const ID_t id = parseStyle(reader, style);
        getCollector()->collectCharacterStyle(id, style);
        break;
      }
      case KN2Token::graphic_style :
      {
        KNStyle style;
        const ID_t id = parseStyle(reader, style);
        getCollector()->collectGraphicStyle(id, style);
        break;
      }
      case KN2Token::headline_style :
      {
        KNStyle style;
        const ID_t id = parseStyle(reader, style);
        getCollector()->collectHeadlineStyle(id, style);
        break;
      }
      case KN2Token::layoutstyle :
      {
        KNStyle style;
        const ID_t id = parseStyle(reader, style);
        getCollector()->collectLayoutStyle(id, style);
        break;
      }
      case KN2Token::paragraphstyle :
      {
        KNStyle style;
        const ID_t id = parseStyle(reader, style);
        getCollector()->collectParagraphStyle(id, style);
        break;
      }

      case KN2Token::cell_style :
      case KN2Token::chart_series_style :
      case KN2Token::chart_style :
      case KN2Token::connection_style :
      case KN2Token::liststyle :
      case KN2Token::placeholder_style :
      case KN2Token::slide_style :
      case KN2Token::table_cell_style :
      case KN2Token::table_style :
      case KN2Token::table_vector_style :
      case KN2Token::tabular_style :
      case KN2Token::vector_style :
      case KN2Token::cell_style_ref :
      case KN2Token::characterstyle_ref :
      case KN2Token::chart_series_style_ref :
      case KN2Token::layoutstyle_ref :
      case KN2Token::liststyle_ref :
      case KN2Token::paragraphstyle_ref :
      case KN2Token::table_cell_style_ref :
      case KN2Token::table_vector_style_ref :
      case KN2Token::vector_style_ref :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;

      default :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }
}

void KN2Parser::parseStylesheet(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::stylesheet, KN2Token::NS_URI_KEY));

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns)) && (KN2Token::ID == getKN2TokenID(attr->name)))
    {
      KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
    }
  }

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::stylesheet, KN2Token::NS_URI_KEY, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::styles :
        parseStyles(reader, false);
        break;
      case KN2Token::anon_styles :
        parseStyles(reader, true);
        break;
      case KN2Token::parent_ref :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;
      default :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }
}

void KN2Parser::parseTheme(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::theme, KN2Token::NS_URI_KEY));

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if (attr->ns)
    {
      switch (getKN2TokenID(attr->ns))
      {
      case KN2Token::NS_URI_KEY :
        switch (getKN2TokenID(attr->name))
        {
        case KN2Token::decimal_tab :
        case KN2Token::group_uuid :
        case KN2Token::name :
          KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
          break;
        default :
          KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
          break;
        }
      case KN2Token::NS_URI_SFA :
        if (KN2Token::ID == getKN2TokenID(attr->name))
        {
          KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
        }
        else
        {
          KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
        }
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
    }
  }

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::theme, KN2Token::NS_URI_KEY, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_KEY == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::size :
      {
        KNSize size;
        parseSize(reader, size);
        break;
      }
      case KN2Token::stylesheet :
        parseStylesheet(reader);
        break;
      case KN2Token::master_slides :
        parseMasterSlides(reader);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }
}

void KN2Parser::parseThemeList(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::theme_list, KN2Token::NS_URI_KEY));

  getCollector()->startThemes();

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns)) && (KN2Token::ID == getKN2TokenID(attr->name)))
    {
      KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
    }
  }

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::theme_list, KN2Token::NS_URI_KEY, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if ((KN2Token::NS_URI_KEY == getKN2TokenID(ns)) && (KN2Token::theme == getKN2TokenID(name)))
    {
      parseTheme(reader);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }

  getCollector()->endThemes();
}

ID_t KN2Parser::parseGeometry(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::geometry, KN2Token::NS_URI_SF));

  ID_t id;
  KNGeometry geometry;

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if (KN2Token::NS_URI_SF == getKN2TokenID(attr->ns))
    {
      switch (getKN2TokenID(attr->name))
      {
      case KN2Token::angle :
        geometry.angle = asDouble(attr->value);
        break;
      case KN2Token::aspectRatioLocked :
        geometry.aspectRatioLocked = asBool(attr->value);
        break;
      case KN2Token::horizontalFlip :
        geometry.horizontalFlip = asBool(attr->value);
        break;
      case KN2Token::shearXAngle :
        geometry.shearXAngle = asDouble(attr->value);
        break;
      case KN2Token::shearYAngle :
        geometry.shearYAngle = asDouble(attr->value);
        break;
      case KN2Token::sizesLocked :
        geometry.sizesLocked = asBool(attr->value);
        break;
      case KN2Token::verticalFlip :
        geometry.verticalFlip = asBool(attr->value);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
        break;
      }
    }
    else if (KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns) && (KN2Token::ID == getKN2TokenID (attr->name)))
    {
      id = attr->value;
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
    }
  }

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::geometry, KN2Token::NS_URI_SF, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::naturalSize :
        geometry.naturalSize = readSize(reader);
        break;
      case KN2Token::size :
        geometry.size = readSize(reader);
        break;
      case KN2Token::position :
        geometry.position = readPosition(reader);
        break;
      case KN2Token::geometry :
        // Huh? I need to find the file that actually contains this...
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }

  getCollector()->collectGeometry(id, geometry);

  return id;
}

ID_t KN2Parser::parseGroup(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::group, KN2Token::NS_URI_SF));

  ID_t id;

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SF == getKN2TokenID(attr->ns)) && (KN2Token::href == getKN2TokenID(attr->name)))
    {
      KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
    }
    else if ((KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns)) && (KN2Token::ID == getKN2TokenID(attr->name)))
    {
      id = attr->value;
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
    }
  }

  KNGroup group;

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::group, KN2Token::NS_URI_SF, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::geometry :
        group.geometries.push_back(parseGeometry(reader));
        break;
      case KN2Token::group :
        group.groups.push_back(parseGroup(reader));
        break;
      case KN2Token::image :
        group.images.push_back(parseImage(reader));
        break;
      case KN2Token::line :
        group.lines.push_back(parseLine(reader));
        break;
      case KN2Token::media :
        group.media.push_back(parseMedia(reader));
        break;
      case KN2Token::shape :
        group.shapes.push_back(parseShape(reader));
        break;
      case KN2Token::wrap :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }

  getCollector()->collectGroup(id, group);

  return id;
}

ID_t KN2Parser::parseImage(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::image, KN2Token::NS_URI_SF));

  ID_t id;
  KNImage image;

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SF == getKN2TokenID(attr->ns)) && (KN2Token::locked == getKN2TokenID(attr->name)))
    {
      image.locked = asBool(attr->value);
    }
    else if (KN2Token::NS_URI_SFA == getKN2TokenID(attr->name))
    {
      switch (getKN2TokenID(attr->name))
      {
      case KN2Token::ID :
        id = attr->value;
        break;
      case KN2Token::version :
        KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
    }
  }

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::image, KN2Token::NS_URI_SF, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::geometry :
        image.geometry = parseGeometry(reader);
        break;
      case KN2Token::size :
      case KN2Token::data :
      case KN2Token::style :
      case KN2Token::binary :
      case KN2Token::filtered_image :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }

  getCollector()->collectImage(id, image);

  return id;
}

ID_t KN2Parser::parseLine(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::line, KN2Token::NS_URI_SF));

  ID_t id;

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SF == getKN2TokenID(attr->ns)) && (KN2Token::href == getKN2TokenID(attr->name)))
    {
      KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
    }
    else if ((KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns)) && (KN2Token::ID == getKN2TokenID(attr->name)))
    {
      id = attr->value;
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
    }
  }

  KNLine line;

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::line, KN2Token::NS_URI_SF, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::geometry :
        line.geometry = parseGeometry(reader);
        break;
      case KN2Token::head :
        line.head = readPoint(reader);
        break;
      case KN2Token::tail :
        line.tail = readPoint(reader);
        break;
      case KN2Token::style :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }

  getCollector()->collectLine(id, line);

  return id;
}

ID_t KN2Parser::parseMedia(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::media, KN2Token::NS_URI_SF));

  ID_t id;
  KNMedia media;

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    switch (getKN2TokenID(attr->ns))
    {
    case KN2Token::NS_URI_KEY :
      switch (getKN2TokenID(attr->name))
      {
      case KN2Token::inheritance :
      case KN2Token::override_geometry_mask :
      case KN2Token::override_media :
      case KN2Token::tag :
        KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
        break;
      }
      break;
    case KN2Token::NS_URI_SF :
      switch (getKN2TokenID(attr->name))
      {
      case KN2Token::placeholder :
        media.placeholder = asBool(attr->value);
        break;
      case KN2Token::locked :
        KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
        break;
      }
      break;
    case KN2Token::NS_URI_SFA :
      switch (getKN2TokenID(attr->name))
      {
      case KN2Token::ID :
        id = attr->value;
        break;
      case KN2Token::version :
        KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
        break;
      }
      break;
    default :
      KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
      break;
    }
  }

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::media, KN2Token::NS_URI_SF, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::geometry :
        media.geometry = parseGeometry(reader);
        break;
      case KN2Token::placeholder_size :
        media.placeholderSize = readSize(reader);
        break;
      case KN2Token::style :
      case KN2Token::masking_shape_path_source :
      case KN2Token::crop_geometry :
      case KN2Token::content :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }

  getCollector()->collectMedia(id, media);

  return id;
}

ID_t KN2Parser::parseShape(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::shape, KN2Token::NS_URI_SF));

  ID_t id;

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if (attr->ns)
    {
      switch (getKN2TokenID(attr->name))
      {
      case KN2Token::can_autosize_h :
      case KN2Token::can_autosize_v :
        KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
        break;
      }
    }
    else
    {
      if (KN2Token::NS_URI_KEY == getKN2TokenID(attr->ns))
      {
        switch (getKN2TokenID(attr->name))
        {
        case KN2Token::inheritance :
        case KN2Token::override_autosize :
        case KN2Token::override_text :
        case KN2Token::tag :
          KN_DEBUG_XML_TODO("attribute", attr->name, attr->ns);
          break;
        default :
          KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
          break;
        }
      }
      else if (KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns) && (KN2Token::ID == getKN2TokenID (attr->name)))
      {
        id = attr->value;
      }
      else
      {
        KN_DEBUG_XML_UNKNOWN("attribute", attr->name, attr->ns);
      }
    }
  }

  // read elements
  while (moveToNextNode(reader))
  {
    const char *const name = getName(reader);
    const char *const ns = getNamespace(reader);

    if (checkElement(reader, KN2Token::group, KN2Token::NS_URI_SF, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::geometry :
      case KN2Token::style :
      case KN2Token::path :
      case KN2Token::text :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }

  return id;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
