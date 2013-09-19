/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>
#include <utility>

#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include "libkeynote_utils.h"
#include "libkeynote_xml.h"
#include "KN2Parser.h"
#include "KN2StyleParser.h"
#include "KN2Token.h"
#include "KNCollector.h"
#include "KNPath.h"
#include "KNStyle.h"
#include "KNTypes.h"
#include "KNXMLReader.h"

using boost::get_optional_value_or;
using boost::lexical_cast;
using boost::numeric_cast;
using boost::optional;

using std::pair;
using std::string;

namespace libkeynote
{

namespace
{

bool bool_cast(const char *const value)
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

template <typename T1, typename T2>
pair<T1, T2>
readAttributePair(const KNXMLReader &reader, const int name1, const int ns1, const int name2, const int ns2, const bool empty = true)
{
  optional<T1> a1;
  optional<T2> a2;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((ns1 == getNamespaceId(attr)) && (name1 == getNameId(attr)))
    {
      a1 = lexical_cast<T1>(attr.getValue());
    }
    else if ((ns2 == getNamespaceId(attr)) && (name2 == getNameId(attr)))
    {
      a2 = lexical_cast<T2>(attr.getValue());
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
    }
  }

  if (empty)
    checkEmptyElement(reader);

  if (!a1 || !a2)
    throw GenericException();

  return pair<T1, T2>(get(a1), get(a2));
}

pair<optional<double>, optional<double> > readPoint(const KNXMLReader &reader)
{
  pair<optional<double>, optional<double> > point;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (KN2Token::NS_URI_SFA == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KN2Token::x :
        point.first = lexical_cast<double>(attr.getValue());
        break;
      case KN2Token::y :
        point.second = lexical_cast<double>(attr.getValue());
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
    }
  }

  checkEmptyElement(reader);

  return point;
}

KNPosition readPosition(const KNXMLReader &reader)
{
  pair<double, double> point = readAttributePair<double, double>(reader, KN2Token::x, KN2Token::NS_URI_SFA, KN2Token::y, KN2Token::NS_URI_SFA);

  return KNPosition(point.first, point.second);
}

KNSize readSize(const KNXMLReader &reader)
{
  const pair<double, double> size = readAttributePair<double, double>(reader, KN2Token::h, KN2Token::NS_URI_SFA, KN2Token::w, KN2Token::NS_URI_SFA);
  return KNSize(size.first, size.second);
}

ID_t readRef(const KNXMLReader &reader)
{
  optional<ID_t> id = readOnlyElementAttribute(reader, KN2Token::IDREF, KN2Token::NS_URI_SFA);
  if (!id)
    throw GenericException();
  return get(id);
}

optional<ID_t> readID(const KNXMLReader &reader)
{
  optional<ID_t> id;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SFA == getNamespaceId(attr)) && (KN2Token::ID == getNameId(attr)))
    {
      id = attr.getValue();
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
    }
  }

  return id;
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

namespace
{

struct Tokenizer
{
  int operator()(const char *const token) const
  {
    return getKN2TokenID(token);
  }
};

}

KN2Parser::KN2Parser(const WPXInputStreamPtr_t &input, KNCollector *const collector, const KNDefaults &defaults)
  : KNParser(input, collector, defaults)
  , m_version(0)
{
}

KN2Parser::~KN2Parser()
{
}

void KN2Parser::processXmlNode(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::presentation, KN2Token::NS_URI_KEY));

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (attr.getNamespace())
    {
      switch (getNamespaceId(attr))
      {
      case KN2Token::NS_URI_KEY :
        switch (getNameId(attr))
        {
        case KN2Token::all_images_bundled :
        case KN2Token::compatible_version :
        case KN2Token::kiosk_build_delay :
        case KN2Token::kiosk_slide_delay :
        case KN2Token::mode :
        case KN2Token::play_mode :
        case KN2Token::sticky_visibility :
          KN_DEBUG_XML_TODO("attribute", attr.getName(), attr.getNamespace());
          break;
        case KN2Token::version :
          m_version = getVersion(getKN2TokenID(attr.getValue()));
          if (0 == m_version)
          {
            KN_DEBUG_MSG(("unknown version %s\n", attr.getValue()));
          }
          break;
        default :
          KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
          break;
        }
        break;
      case KN2Token::NS_URI_SFA :
        if (KN2Token::ID == getNameId(attr))
        {
          KN_DEBUG_XML_TODO("attribute", attr.getName(), attr.getNamespace());
        }
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
    }
  }

  optional<KNSize> size;

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if (KN2Token::NS_URI_KEY == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::size :
        size = readSize(reader);
        break;
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
      case KN2Token::soundtrack_list :
      case KN2Token::calculation_engine :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(element);
        break;

      case KN2Token::ui_state :
      case KN2Token::version_history :
        skipElement(element);
        break;

      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        break;
      }
    }
    else if ((KN2Token::NS_URI_SF == getKN2TokenID(ns)) && (KN2Token::calc_engine == getKN2TokenID(name)))
    {
      KN_DEBUG_XML_TODO("element", name, ns);
      skipElement(element);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }

  getCollector()->collectPresentation(size);
}

KNXMLReader::TokenizerFunction_t KN2Parser::getTokenizer() const
{
  return Tokenizer();
}

void KN2Parser::parseDrawables(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::drawables, KN2Token::NS_URI_SF));

  readID(reader);

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::connection_line :
        parseConnectionLine(element);
        break;
      case KN2Token::group :
        parseGroup(element);
        break;
      case KN2Token::image :
        parseImage(element);
        break;
      case KN2Token::line :
        parseLine(element);
        break;
      case KN2Token::media :
        parseMedia(element);
        break;
      case KN2Token::shape :
        parseShape(element);
        break;
      case KN2Token::body_placeholder_ref :
      case KN2Token::slide_number_placeholder_ref :
      case KN2Token::table_info :
      case KN2Token::title_placeholder_ref :
      case KN2Token::chart_info :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }
}

void KN2Parser::parseLayer(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::layer, KN2Token::NS_URI_SF));

  const optional<ID_t> id = readID(reader);

  getCollector()->startLayer();

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::drawables :
        parseDrawables(reader);
        break;
      case KN2Token::type :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(element);
        break;
      case KN2Token::guides :
        // ignore
        skipElement(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }

  getCollector()->collectLayer(id, false);
  getCollector()->endLayer();
}

void KN2Parser::parseLayers(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::layers, KN2Token::NS_URI_SF));

  readID(reader);

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

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
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }
}

void KN2Parser::parseMasterSlide(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::master_slide, KN2Token::NS_URI_KEY));

  optional<ID_t> id;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (attr.getNamespace())
    {
      switch (getNamespaceId(attr))
      {
      case KN2Token::NS_URI_KEY :
        switch (getNameId(attr))
        {
        case KN2Token::layer :
        case KN2Token::name :
          KN_DEBUG_XML_TODO("attribute", attr.getName(), attr.getNamespace());
          break;
        default :
          KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
          break;
        }
      case KN2Token::NS_URI_SFA :
        if (KN2Token::ID == getNameId(attr))
        {
          id = attr.getValue();
        }
        else
        {
          KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
        }
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
    }
  }

  getCollector()->startPage();

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

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
      case KN2Token::title_placeholder :
        parsePlaceholder(element, true);
        break;
      case KN2Token::body_placeholder :
        parsePlaceholder(element);
        break;
      case KN2Token::sticky_notes :
        parseStickyNotes(element);
        break;
      case KN2Token::style_ref :
      case KN2Token::object_placeholder :
      case KN2Token::slide_number_placeholder :
      case KN2Token::bullets :
      case KN2Token::thumbnails :
      case KN2Token::build_chunks :
      case KN2Token::events :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }

  getCollector()->collectPage(id);
  getCollector()->endPage();
}

void KN2Parser::parseMasterSlides(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::master_slides, KN2Token::NS_URI_KEY));

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SFA == getNamespaceId(attr)) && (KN2Token::ID == getNameId(attr)))
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
      break;
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
    }
  }

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if ((KN2Token::NS_URI_KEY == getKN2TokenID(ns)) && (KN2Token::master_slide == getKN2TokenID(name)))
    {
      parseMasterSlide(reader);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }
}

void KN2Parser::parseMetadata(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::metadata, KN2Token::NS_URI_KEY));

  // there are no attributes for key:metadata
  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
  }

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if (KN2Token::NS_URI_KEY == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::authors :
      case KN2Token::keywords :
      case KN2Token::title :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }
}

void KN2Parser::parsePage(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::page, KN2Token::NS_URI_KEY));

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SFA == getNamespaceId(attr)) && (KN2Token::ID == getNameId(attr)))
    {
      KN_DEBUG_XML_TODO("attribute", attr.getName(), attr.getNamespace());
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
    }
  }

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::size :
      {
        const KNSize size = readSize(reader);
        // TODO: use size
        (void) size;
        break;
      }
      case KN2Token::layers :
        parseLayers(reader);
        break;
      case KN2Token::geometry :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }
}

void KN2Parser::parseProxyMasterLayer(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::proxy_master_layer, KN2Token::NS_URI_SF));

  readID(reader);

  optional<ID_t> ref;

  getCollector()->startLayer();

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::layer_ref :
        ref = readOnlyElementAttribute(reader, KN2Token::IDREF, KN2Token::NS_URI_SFA);
        break;
      case KN2Token::type :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }

  getCollector()->collectLayer(ref, true);
  getCollector()->endLayer();
}

void KN2Parser::parseSlide(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::slide, KN2Token::NS_URI_KEY));

  optional<ID_t> id;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (attr.getNamespace())
    {
      switch (getNamespaceId(attr))
      {
      case KN2Token::NS_URI_KEY :
        switch (getNameId(attr))
        {
        case KN2Token::collapsed :
        case KN2Token::depth :
        case KN2Token::hidden :
          KN_DEBUG_XML_TODO("attribute", attr.getName(), attr.getNamespace());
          break;
        default :
          KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
          break;
        }
      case KN2Token::NS_URI_SFA :
        if (KN2Token::ID == getNameId(attr))
        {
          id = attr.getValue();
        }
        else
        {
          KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
        }
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
    }
  }

  getCollector()->startPage();

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

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
      case KN2Token::title_placeholder :
        parsePlaceholder(element, true);
        break;
      case KN2Token::body_placeholder :
        parsePlaceholder(element);
        break;
      case KN2Token::sticky_notes :
        parseStickyNotes(element);
        break;
      case KN2Token::style_ref :
      case KN2Token::object_placeholder :
      case KN2Token::slide_number_placeholder :
      case KN2Token::bullets :
      case KN2Token::thumbnails :
      case KN2Token::build_chunks :
      case KN2Token::master_ref :
      case KN2Token::notes :
      case KN2Token::dummy_body_headline :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }

  getCollector()->collectPage(id);
  getCollector()->endPage();
}

void KN2Parser::parseSlideList(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::slide_list, KN2Token::NS_URI_KEY));

  getCollector()->startSlides();

  readID(reader);

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if ((KN2Token::NS_URI_KEY == getKN2TokenID(ns)) && (KN2Token::slide == getKN2TokenID(name)))
    {
      parseSlide(reader);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }

  getCollector()->endSlides();
}

void KN2Parser::parseStickyNotes(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::sticky_notes, KN2Token::NS_URI_KEY));

  readID(reader);

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KN2Token::NS_URI_KEY == getNamespaceId(element)) && (KN2Token::sticky_note == getNameId(element)))
    {
      parseStickyNote(element);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }
}

void KN2Parser::parseStyles(const KNXMLReader &reader, const bool anonymous)
{
  const int type = anonymous ? KN2Token::anon_styles : KN2Token::styles;
  assert(checkElement(reader, type, KN2Token::NS_URI_SF));

  checkNoAttributes(reader);

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::characterstyle :
      case KN2Token::graphic_style :
      case KN2Token::headline_style :
      case KN2Token::layoutstyle :
      case KN2Token::paragraphstyle :
      {
        KN2StyleParser parser(getKN2TokenID(name), getKN2TokenID(ns), getCollector());
        parser.parse(element);
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
        skipElement(element);
        break;

      default :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }
}

void KN2Parser::parseStylesheet(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::stylesheet, KN2Token::NS_URI_KEY));

  const optional<ID_t> id = readID(reader);

  optional<ID_t> parent;

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

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
        parent = readRef(element);
        break;
      default :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }

  getCollector()->collectStylesheet(id, parent);
}

void KN2Parser::parseTheme(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::theme, KN2Token::NS_URI_KEY));

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (attr.getNamespace())
    {
      switch (getNamespaceId(attr))
      {
      case KN2Token::NS_URI_KEY :
        switch (getNameId(attr))
        {
        case KN2Token::decimal_tab :
        case KN2Token::group_uuid :
        case KN2Token::name :
          KN_DEBUG_XML_TODO("attribute", attr.getName(), attr.getNamespace());
          break;
        default :
          KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
          break;
        }
      case KN2Token::NS_URI_SFA :
        if (KN2Token::ID == getNameId(attr))
        {
          KN_DEBUG_XML_TODO("attribute", attr.getName(), attr.getNamespace());
        }
        else
        {
          KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
        }
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
    }
  }

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if (KN2Token::NS_URI_KEY == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::size :
      {
        const KNSize size = readSize(reader);
        // TODO: use size
        (void) size;
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
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }
}

void KN2Parser::parseThemeList(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::theme_list, KN2Token::NS_URI_KEY));

  getCollector()->startThemes();

  readID(reader);

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if ((KN2Token::NS_URI_KEY == getKN2TokenID(ns)) && (KN2Token::theme == getKN2TokenID(name)))
    {
      parseTheme(reader);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }

  getCollector()->endThemes();
}

void KN2Parser::parseBezier(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::bezier, KN2Token::NS_URI_SF));

  optional<ID_t> id;
  KNPathPtr_t path;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SFA == getNamespaceId(attr)))
    {
      switch (getNameId(attr))
      {
      case KN2Token::ID :
        id = attr.getValue();
        break;
      case KN2Token::path :
        path.reset(new KNPath(attr.getValue()));
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
    }
  }

  checkEmptyElement(reader);

  getCollector()->collectBezier(id, path, false);
}

void KN2Parser::parseConnectionLine(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::connection_line, KN2Token::NS_URI_SF));

  optional<ID_t> id;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SF == getNamespaceId(attr)) && (KN2Token::locked == getNameId(attr)))
    {
      KN_DEBUG_XML_TODO_ATTRIBUTE(attr);
    }
    else if (KN2Token::NS_URI_SFA == getNamespaceId(attr) && (KN2Token::ID == getKN2TokenID (attr.getName())))
    {
      id = attr.getValue();
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
    }
  }

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KN2Token::geometry :
        parseGeometry(element);
        break;
      case KN2Token::path :
        parsePath(element);
        break;
      case KN2Token::style :
        KN_DEBUG_XML_TODO_ELEMENT(element);
        skipElement(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }

  getCollector()->collectShape(id);
}

void KN2Parser::parseGeometry(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::geometry, KN2Token::NS_URI_SF));

  optional<ID_t> id;
  KNGeometryPtr_t geometry(new KNGeometry());

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KN2Token::angle :
        geometry->angle = lexical_cast<double>(attr.getValue());
        break;
      case KN2Token::aspectRatioLocked :
        geometry->aspectRatioLocked = bool_cast(attr.getValue());
        break;
      case KN2Token::horizontalFlip :
        geometry->horizontalFlip = bool_cast(attr.getValue());
        break;
      case KN2Token::shearXAngle :
        geometry->shearXAngle = lexical_cast<double>(attr.getValue());
        break;
      case KN2Token::shearYAngle :
        geometry->shearYAngle = lexical_cast<double>(attr.getValue());
        break;
      case KN2Token::sizesLocked :
        geometry->sizesLocked = bool_cast(attr.getValue());
        break;
      case KN2Token::verticalFlip :
        geometry->verticalFlip = bool_cast(attr.getValue());
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
        break;
      }
    }
    else if (KN2Token::NS_URI_SFA == getNamespaceId(attr) && (KN2Token::ID == getKN2TokenID (attr.getName())))
    {
      id = attr.getValue();
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
    }
  }

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::naturalSize :
        geometry->naturalSize = readSize(reader);
        break;
      case KN2Token::size :
        geometry->size = readSize(reader);
        break;
      case KN2Token::position :
        geometry->position = readPosition(reader);
        break;
      case KN2Token::geometry :
        // Huh? I need to find the file that actually contains this...
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }

  getCollector()->collectGeometry(id, geometry);
}

void KN2Parser::parseGroup(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::group, KN2Token::NS_URI_SF));

  optional<ID_t> id;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SF == getNamespaceId(attr)) && (KN2Token::href == getNameId(attr)))
    {
      KN_DEBUG_XML_TODO("attribute", attr.getName(), attr.getNamespace());
    }
    else if ((KN2Token::NS_URI_SFA == getNamespaceId(attr)) && (KN2Token::ID == getNameId(attr)))
    {
      id = attr.getValue();
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
    }
  }

  KNGroupPtr_t group(new KNGroup());

  getCollector()->startGroup();

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::geometry :
        parseGeometry(reader);
        break;
      case KN2Token::group :
        parseGroup(reader);
        break;
      case KN2Token::image :
        parseImage(reader);
        break;
      case KN2Token::line :
        parseLine(reader);
        break;
      case KN2Token::media :
        parseMedia(reader);
        break;
      case KN2Token::shape :
        parseShape(reader);
        break;
      case KN2Token::wrap :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }

  getCollector()->collectGroup(id, group);
  getCollector()->endGroup();
}

void KN2Parser::parseImage(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::image, KN2Token::NS_URI_SF));

  optional<ID_t> id;
  KNImagePtr_t image(new KNImage());

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SF == getNamespaceId(attr)) && (KN2Token::locked == getNameId(attr)))
    {
      image->locked = bool_cast(attr.getValue());
    }
    else if (KN2Token::NS_URI_SFA == getNameId(attr))
    {
      switch (getNameId(attr))
      {
      case KN2Token::ID :
        id = attr.getValue();
        break;
      case KN2Token::version :
        KN_DEBUG_XML_TODO("attribute", attr.getName(), attr.getNamespace());
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
    }
  }

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::geometry :
        parseGeometry(reader);
        break;
      case KN2Token::size :
      case KN2Token::data :
      case KN2Token::style :
      case KN2Token::binary :
      case KN2Token::filtered_image :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }

  getCollector()->collectImage(id, image);
}

void KN2Parser::parseLine(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::line, KN2Token::NS_URI_SF));

  optional<ID_t> id;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SF == getNamespaceId(attr)) && (KN2Token::href == getNameId(attr)))
    {
      KN_DEBUG_XML_TODO("attribute", attr.getName(), attr.getNamespace());
    }
    else if ((KN2Token::NS_URI_SFA == getNamespaceId(attr)) && (KN2Token::ID == getNameId(attr)))
    {
      id = attr.getValue();
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
    }
  }

  KNLinePtr_t line(new KNLine());

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::geometry :
        parseGeometry(reader);
        break;
      case KN2Token::head :
      {
        const KNPosition head = readPosition(reader);
        line->x1 = head.x;
        line->y1 = head.y;
        break;
      }
      case KN2Token::tail :
      {
        const KNPosition tail = readPosition(reader);
        line->x2 = tail.x;
        line->y2 = tail.y;
        break;
      }
      case KN2Token::style :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }

  getCollector()->collectLine(id, line);
}

void KN2Parser::parseMedia(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::media, KN2Token::NS_URI_SF));

  optional<ID_t> id;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    switch (getNamespaceId(attr))
    {
    case KN2Token::NS_URI_KEY :
      switch (getNameId(attr))
      {
      case KN2Token::inheritance :
      case KN2Token::override_geometry_mask :
      case KN2Token::override_media :
      case KN2Token::tag :
        KN_DEBUG_XML_TODO("attribute", attr.getName(), attr.getNamespace());
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
        break;
      }
      break;
    case KN2Token::NS_URI_SF :
      switch (getNameId(attr))
      {
      case KN2Token::placeholder :
      case KN2Token::locked :
        KN_DEBUG_XML_TODO("attribute", attr.getName(), attr.getNamespace());
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
        break;
      }
      break;
    case KN2Token::NS_URI_SFA :
      switch (getNameId(attr))
      {
      case KN2Token::ID :
        id = attr.getValue();
        break;
      case KN2Token::version :
        KN_DEBUG_XML_TODO("attribute", attr.getName(), attr.getNamespace());
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
        break;
      }
      break;
    default :
      KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
      break;
    }
  }

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::geometry :
        parseGeometry(reader);
        break;
      case KN2Token::content :
        parseContent(element);
        break;
      case KN2Token::placeholder_size :
      case KN2Token::style :
      case KN2Token::masking_shape_path_source :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }

  getCollector()->collectMedia(id);
}

void KN2Parser::parsePath(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::path, KN2Token::NS_URI_SF));

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KN2Token::ID :
      case KN2Token::path :
        KN_DEBUG_XML_TODO_ATTRIBUTE(attr);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
    }
  }

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(element))
      {
      case KN2Token::bezier_path :
      case KN2Token::editable_bezier_path :
        parseBezierPath(element);
        break;
      case KN2Token::callout2_path :
        parseCallout2Path(element);
        break;
      case KN2Token::connection_path :
        parseConnectionPath(element);
        break;
      case KN2Token::point_path :
        parsePointPath(element);
        break;
      case KN2Token::scalar_path :
        parseScalarPath(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }
}

void KN2Parser::parseShape(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::shape, KN2Token::NS_URI_SF));

  optional<ID_t> id;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (!attr.getNamespace())
    {
      switch (getNameId(attr))
      {
      case KN2Token::can_autosize_h :
      case KN2Token::can_autosize_v :
        KN_DEBUG_XML_TODO("attribute", attr.getName(), attr.getNamespace());
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
        break;
      }
    }
    else
    {
      if (KN2Token::NS_URI_KEY == getNamespaceId(attr))
      {
        switch (getNameId(attr))
        {
        case KN2Token::inheritance :
        case KN2Token::override_autosize :
        case KN2Token::override_text :
        case KN2Token::tag :
          KN_DEBUG_XML_TODO("attribute", attr.getName(), attr.getNamespace());
          break;
        default :
          KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
          break;
        }
      }
      else if (KN2Token::NS_URI_SFA == getNamespaceId(attr) && (KN2Token::ID == getKN2TokenID (attr.getName())))
      {
        id = attr.getValue();
      }
      else
      {
        KN_DEBUG_XML_UNKNOWN("attribute", attr.getName(), attr.getNamespace());
      }
    }
  }

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    const char *const name = element.getName();
    const char *const ns = element.getNamespace();

    if (KN2Token::NS_URI_SF == getKN2TokenID(ns))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::path :
        parsePath(element);
        break;
      case KN2Token::geometry :
      case KN2Token::style :
      case KN2Token::text :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN("element", name, ns);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(element);
    }
  }
}

void KN2Parser::parseStickyNote(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::sticky_note, KN2Token::NS_URI_KEY));

  const optional<ID_t> id = readID(reader);

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KN2Token::path :
        parsePath(element);
        break;
      case KN2Token::geometry :
      case KN2Token::style :
      case KN2Token::text :
        KN_DEBUG_XML_TODO_ELEMENT(element);
        skipElement(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }

  // TODO: collect
}

void KN2Parser::parsePlaceholder(const KNXMLReader &reader, const bool title)
{
  assert(title
         ? checkElement(reader, KN2Token::title_placeholder, KN2Token::NS_URI_KEY)
         : checkElement(reader, KN2Token::body_placeholder, KN2Token::NS_URI_KEY));

  optional<ID_t> id;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (!title && (KN2Token::NS_URI_KEY == getNamespaceId(attr)) && (KN2Token::non_empty == getNameId(attr)))
    {
      KN_DEBUG_XML_TODO_ATTRIBUTE(attr);
    }
    else if ((KN2Token::NS_URI_SFA == getNamespaceId(attr)) && (KN2Token::ID == getNameId(attr)))
    {
      id = attr.getValue();
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
    }
  }

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KN2Token::NS_URI_KEY == getNamespaceId(element)) && (KN2Token::text == getNameId(element)))
      parseText(element);
    else if (KN2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KN2Token::geometry :
        // ignore; the real geometry comes from style
        skipElement(element);
        break;
      case KN2Token::style :
      {
        KNXMLReader readerStyle(element);

        checkNoAttributes(readerStyle);

        KNXMLReader::ElementIterator elementStyle(readerStyle);
        while (elementStyle.next())
        {
          if ((KN2Token::NS_URI_SF == getNamespaceId(elementStyle)) && (KN2Token::placeholder_style_ref == getNameId(elementStyle)))
          {
            const ID_t styleId = readRef(elementStyle);
            getCollector()->collectPlaceholderStyle(styleId, KNPlaceholderStylePtr_t(), true, false);
          }
          else
          {
            KN_DEBUG_XML_UNKNOWN_ELEMENT(elementStyle);
            skipElement(elementStyle);
          }
        }
        break;
      }
      default :
        KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
        skipElement(element);
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }

  getCollector()->collectSlideText(id, title);
}

void KN2Parser::parseBezierPath(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::bezier_path, KN2Token::NS_URI_SF)
         || checkElement(reader, KN2Token::editable_bezier_path, KN2Token::NS_URI_SF));

  const optional<ID_t> id = readID(reader);

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KN2Token::bezier :
        parseBezier(element);
        break;
      case KN2Token::bezier_ref :
      {
        const ID_t idref = readRef(element);
        getCollector()->collectBezier(idref, KNPathPtr_t(), true);
        break;
      }
      default :
        KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }

  getCollector()->collectBezierPath(id);
}

void KN2Parser::parseCallout2Path(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::callout2_path, KN2Token::NS_URI_SF));

  optional<ID_t> id;
  double cornerRadius(0);
  bool tailAtCenter(false);
  double tailPosX(0);
  double tailPosY(0);
  double tailSize(0);

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SF == getNamespaceId(attr)) && (KN2Token::ID == getNameId(attr)))
    {
      id = attr.getValue();
    }
    else if (KN2Token::NS_URI_SFA == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KN2Token::cornerRadius :
        cornerRadius = lexical_cast<double>(attr.getValue());
        break;
      case KN2Token::tailAtCenter :
        tailAtCenter = bool_cast(attr.getValue());
        break;
      case KN2Token::tailPositionX :
        tailPosX = lexical_cast<double>(attr.getValue());
        break;
      case KN2Token::tailPositionY :
        tailPosY = lexical_cast<double>(attr.getValue());
        break;
      case KN2Token::tailSize :
        tailSize = lexical_cast<double>(attr.getValue());
        break;
      case KN2Token::tempScale :
        KN_DEBUG_XML_TODO_ATTRIBUTE(attr);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
    }
  }

  KNSize size;

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KN2Token::NS_URI_SF == getNamespaceId(element)) && (KN2Token::size == getNameId(element)))
    {
      size = readSize(reader);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }

  getCollector()->collectCalloutPath(id, size, cornerRadius, tailSize, tailPosX, tailPosY, tailAtCenter);
}

void KN2Parser::parseConnectionPath(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::connection_path, KN2Token::NS_URI_SF));

  const optional<ID_t> id = readID(reader);

  KNSize size;
  pair<optional<double>, optional<double> > point;

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KN2Token::point :
        point = readPoint(element);
        break;
      case KN2Token::size :
        size = readSize(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }

  getCollector()->collectConnectionPath(id, size, get_optional_value_or(point.first, 0), get_optional_value_or(point.second, 0));
}

void KN2Parser::parsePointPath(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::point_path, KN2Token::NS_URI_SF));

  optional<ID_t> id;
  bool star = false;
  // right arrow is the default (by my decree .-)
  bool doubleArrow = false;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SF == getNamespaceId(attr)) && (KN2Token::ID == getNameId(attr)))
    {
      switch (getKN2TokenID(attr.getValue()))
      {
      case KN2Token::double_ :
        doubleArrow = true;
        break;
      case KN2Token::right :
        break;
      case KN2Token::star :
        star = true;
        break;
      default :
        KN_DEBUG_MSG(("unknown point path type: %s\n", attr.getValue()));
        break;
      }
    }
    if ((KN2Token::NS_URI_SFA == getNamespaceId(attr)) && (KN2Token::ID == getNameId(attr)))
    {
      id = attr.getValue();
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
    }
  }

  KNSize size;
  pair<optional<double>, optional<double> > point;

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KN2Token::point :
        point = readPoint(element);
        break;
      case KN2Token::size :
        size = readSize(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
        skipElement(element);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }

  if (star)
    getCollector()->collectStarPath(id, size, numeric_cast<unsigned>(get_optional_value_or(point.first, 0.0)), get_optional_value_or(point.second, 0));
  else
    getCollector()->collectArrowPath(id, size, get_optional_value_or(point.first, 0), get_optional_value_or(point.second, 0), doubleArrow);
}

void KN2Parser::parseScalarPath(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::scalar_path, KN2Token::NS_URI_SF));

  optional<ID_t> id;
  bool polygon = false;
  double value = 0;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KN2Token::scalar :
        value = lexical_cast<double>(attr.getValue());
        break;
      case KN2Token::type :
      {
        switch (getKN2TokenID(attr.getValue()))
        {
        case KN2Token::_0 :
          break;
        case KN2Token::_1 :
          polygon = true;
          break;
        default :
          KN_DEBUG_MSG(("unknown scalar path type: %s\n", attr.getValue()));
          break;
        }
      }
      break;
      default :
        KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
        break;
      }
    }
    if ((KN2Token::NS_URI_SFA == getNamespaceId(attr)) && (KN2Token::ID == getNameId(attr)))
    {
      id = attr.getValue();
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
    }
  }

  KNSize size;

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KN2Token::NS_URI_SF == getNamespaceId(element)) && (KN2Token::size == getNameId(element)))
    {
      size = readSize(element);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }

  if (polygon)
    getCollector()->collectPolygonPath(id, size, numeric_cast<unsigned>(value));
  else
    getCollector()->collectRoundedRectanglePath(id, size, value);
}

void KN2Parser::parseContent(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::content, KN2Token::NS_URI_SF));

  checkNoAttributes(reader);

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KN2Token::image_media :
      case KN2Token::movie_media :
        KN_DEBUG_XML_TODO_ELEMENT(element);
        skipElement(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
        skipElement(element);
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }
}

void KN2Parser::parseData(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::data, KN2Token::NS_URI_SF));

  optional<ID_t> id;
  optional<string> displayName;
  WPXInputStreamPtr_t stream;
  optional<unsigned> type;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SFA == getNamespaceId(attr)) && (KN2Token::ID == getNameId(attr)))
    {
      KN_DEBUG_XML_TODO_ATTRIBUTE(attr);
    }
    else if (KN2Token::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KN2Token::displayname :
        displayName = attr.getValue();
        break;
      case KN2Token::hfs_type :
        type = lexical_cast<unsigned>(attr.getValue());
        break;
      case KN2Token::delay_archiving :
      case KN2Token::path :
      case KN2Token::resource_type :
      case KN2Token::sharable :
      case KN2Token::size :
        KN_DEBUG_XML_TODO_ATTRIBUTE(attr);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
        break;
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
    }
  }

  checkEmptyElement(reader);

  getCollector()->collectData(id, stream, displayName, type, false);
}

void KN2Parser::parseFiltered(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::filtered, KN2Token::NS_URI_SF));

  readID(reader);

  optional<KNSize> size;

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KN2Token::size :
        size = readSize(element);
        break;
      case KN2Token::data :
        parseData(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
        skipElement(element);
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }
}

void KN2Parser::parseFilteredImage(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::filtered_image, KN2Token::NS_URI_SF));

  optional<ID_t> id;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SFA == getNamespaceId(attr)) && (KN2Token::ID == getNameId(attr)))
    {
      id = attr.getValue();
    }
    else if ((KN2Token::NS_URI_SF == getNamespaceId(attr)) && (KN2Token::is_leveled == getNameId(attr)))
    {
      KN_DEBUG_XML_TODO_ATTRIBUTE(attr);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
    }
  }

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KN2Token::unfiltered_ref :
      {
        optional<ID_t> idref = readRef(element);
        getCollector()->collectUnfiltered(idref, optional<KNSize>(), true);
        break;
      }
      case KN2Token::unfiltered :
        parseUnfiltered(element);
        break;
      case KN2Token::filtered :
        parseFiltered(element);
        break;
      case KN2Token::leveled :
        parseLeveled(element);
        break;
      case KN2Token::filter_properties :
      case KN2Token::extent :
        KN_DEBUG_XML_TODO_ELEMENT(element);
        skipElement(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
        skipElement(element);
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }

  getCollector()->collectFilteredImage(id, false);
}

void KN2Parser::parseImageMedia(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::image_media, KN2Token::NS_URI_SF));

  readID(reader);

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KN2Token::filtered_image :
        parseFilteredImage(element);
        break;
      case KN2Token::alpha_mask_path :
        KN_DEBUG_XML_TODO_ELEMENT(element);
        skipElement(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
        skipElement(element);
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }
}

void KN2Parser::parseLeveled(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::leveled, KN2Token::NS_URI_SF));

  const optional<ID_t> id = readID(reader);

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KN2Token::data :
        parseData(element);
        break;
      case KN2Token::size :
        KN_DEBUG_XML_TODO_ELEMENT(element);
        skipElement(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
        skipElement(element);
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }
}

void KN2Parser::parseUnfiltered(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::unfiltered, KN2Token::NS_URI_SF));

  const optional<ID_t> id = readID(reader);

  optional<KNSize> size;

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KN2Token::size :
        size = readSize(element);
        break;
      case KN2Token::data :
        parseData(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
        skipElement(element);
      }
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }

  getCollector()->collectUnfiltered(id, size, false);
}

void KN2Parser::parseBr(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::br, KN2Token::NS_URI_SF)
         || checkElement(reader, KN2Token::crbr, KN2Token::NS_URI_SF)
         || checkElement(reader, KN2Token::intratopicbr, KN2Token::NS_URI_SF)
         || checkElement(reader, KN2Token::lnbr, KN2Token::NS_URI_SF));

  checkNoAttributes(reader);
  checkEmptyElement(reader);

  getCollector()->collectLineBreak();
}

void KN2Parser::parseLayout(const KNXMLReader &reader, const boost::optional<ID_t> &layoutStyle)
{
  assert(checkElement(reader, KN2Token::layout, KN2Token::NS_URI_SF));

  optional<ID_t> style;

  if (layoutStyle)
    style = get(layoutStyle);

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SF == getNamespaceId(attr)) && (KN2Token::style == getNameId(attr)))
      style = attr.getValue();
    else
    {
      KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
    }
  }

  getCollector()->startTextLayout(style);

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KN2Token::NS_URI_SF == getNamespaceId(element)) && (KN2Token::p == getNameId(element)))
      parseP(element);
    else
    {
      KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }

  getCollector()->endTextLayout();
}

void KN2Parser::parseLink(const KNXMLReader &reader, const bool ref)
{
  assert(ref
         ? checkElement(reader, KN2Token::link-ref, KN2Token::NS_URI_SF)
         : checkElement(reader, KN2Token::link, KN2Token::NS_URI_SF));

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (!ref && !attr.getNamespace() && (KN2Token::href == getNameId(attr)))
    {
      KN_DEBUG_XML_TODO_ATTRIBUTE(attr);
    }
    else if (KN2Token::NS_URI_SFA == getNamespaceId(attr))
    {
      if (ref && (KN2Token::IDREF == getNameId(attr)))
      {
        KN_DEBUG_XML_TODO_ATTRIBUTE(attr);
      }
      if (!ref && (KN2Token::ID == getNameId(attr)))
      {
        KN_DEBUG_XML_TODO_ATTRIBUTE(attr);
      }
      else
      {
        KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
      }
    }
  }

  KNXMLReader::MixedIterator mixed(reader);
  while (mixed.next())
  {
    if (mixed.isElement())
    {
      if (KN2Token::NS_URI_SF == getNamespaceId(mixed))
      {
        switch (getNameId(mixed))
        {
        case KN2Token::br :
          parseBr(mixed);
          break;
        case KN2Token::span :
          parseSpan(mixed);
          break;
        default :
          KN_DEBUG_XML_UNKNOWN_ELEMENT(mixed);
          skipElement(mixed);
          break;
        }
      }
    }
    else
      getCollector()->collectText(optional<ID_t>(), mixed.getText());
  }
}

void KN2Parser::parseP(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::p, KN2Token::NS_URI_SF));

  optional<ID_t> style;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KN2Token::list_level :
        KN_DEBUG_XML_TODO_ATTRIBUTE(attr);
        break;
      case KN2Token::style :
        style = attr.getValue();
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
        break;
      }
    }
  }

  getCollector()->startParagraph(style);

  KNXMLReader::MixedIterator mixed(reader);
  while (mixed.next())
  {
    if (mixed.isElement())
    {
      if (KN2Token::NS_URI_SF == getNamespaceId(mixed))
      {
        switch (getNameId(mixed))
        {
        case KN2Token::br :
        case KN2Token::crbr :
        case KN2Token::intratopicbr :
        case KN2Token::lnbr :
          parseBr(mixed);
          break;
        case KN2Token::span :
          parseSpan(mixed);
          break;
        case KN2Token::tab :
          parseTab(mixed);
          break;
        case KN2Token::link :
          parseLink(mixed);
          break;
        case KN2Token::link_ref :
          parseLink(mixed, true);
          break;
        default :
          KN_DEBUG_XML_UNKNOWN_ELEMENT(mixed);
          skipElement(mixed);
          break;
        }
      }
    }
    else
    {
      getCollector()->collectText(style, mixed.getText());
    }
  }

  getCollector()->endParagraph();
}

void KN2Parser::parseSpan(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::span, KN2Token::NS_URI_SF));

  optional<ID_t> style;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((KN2Token::NS_URI_SF == getNamespaceId(attr)) && (KN2Token::style == getNameId(attr)))
      style = attr.getValue();
    else
    {
      KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
    }
  }

  KNXMLReader::MixedIterator mixed(reader);
  while (mixed.next())
  {
    if (mixed.isElement())
    {
      if (KN2Token::NS_URI_KEY == getNamespaceId(mixed))
      {
        switch (getNameId(mixed))
        {
        case KN2Token::br :
        case KN2Token::crbr :
        case KN2Token::intratopicbr :
        case KN2Token::lnbr :
          parseBr(mixed);
          break;
        case KN2Token::tab :
          parseTab(mixed);
          break;
        default :
          KN_DEBUG_XML_UNKNOWN_ELEMENT(mixed);
          skipElement(mixed);
          break;
        }
      }
    }
    else
      getCollector()->collectText(style, mixed.getText());
  }
}

void KN2Parser::parseTab(const KNXMLReader &reader)
{
  assert(checkElement(reader, KN2Token::tab, KN2Token::NS_URI_SF));

  checkNoAttributes(reader);
  checkEmptyElement(reader);

  getCollector()->collectTab();
}

void KN2Parser::parseText(const KNXMLReader &reader)
{
  // NOTE: isn't it wonderful that there are two text elements in two
  // different namespaces, but with the same schema?
  assert(checkElement(reader, KN2Token::text, KN2Token::NS_URI_KEY)
         || checkElement(reader, KN2Token::text, KN2Token::NS_URI_SF));

  optional<ID_t> layoutStyle;

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KN2Token::layoutstyle :
        layoutStyle = attr.getValue();
        break;
      case KN2Token::tscale :
        KN_DEBUG_XML_TODO_ATTRIBUTE(attr);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
        break;
      }
    }
    else if (KN2Token::NS_URI_SFA == getNamespaceId(attr) && (KN2Token::ID == getNameId(attr)))
    {
      KN_DEBUG_XML_TODO_ATTRIBUTE(attr);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
    }
  }

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((KN2Token::NS_URI_SF == getNamespaceId(element)) && (KN2Token::text_storage == getNameId(element)))
      parseTextStorage(element, layoutStyle);
    else
    {
      KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
      skipElement(element);
    }
  }
}

void KN2Parser::parseTextBody(const KNXMLReader &reader, const boost::optional<ID_t> &layoutStyle)
{
  assert(checkElement(reader, KN2Token::text_body, KN2Token::NS_URI_SF));

  checkNoAttributes(reader);

  bool layout = false;
  bool para = false;
  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KN2Token::layout :
        if (layout || para)
        {
          KN_DEBUG_MSG(("layout following another element, not allowed, skipping\n"));
          skipElement(element);
        }
        else
        {
          parseLayout(element, layoutStyle);
          layout = true;
        }
        break;
      case KN2Token::p :
        if (layout)
        {
          KN_DEBUG_MSG(("paragraph following layout, not allowed, skipping\n"));
          skipElement(element);
        }
        else if (para)
        {
          parseP(element);
        }
        else
        {
          optional<ID_t> layoutStyleId;
          if (layoutStyle)
            layoutStyleId = get(layoutStyle);
          getCollector()->startTextLayout(layoutStyleId);
          para = true;
          parseP(element);
        }
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
        skipElement(element);
        break;
      }
    }
  }

  if (para)
    getCollector()->endTextLayout();
}

void KN2Parser::parseTextStorage(const KNXMLReader &reader, const boost::optional<ID_t> &layoutStyle)
{
  assert(checkElement(reader, KN2Token::text_storage, KN2Token::NS_URI_SF));

  KNXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(attr))
    {
      switch (getNameId(attr))
      {
      case KN2Token::excl :
      case KN2Token::exclude_attachments :
      case KN2Token::exclude_charts :
      case KN2Token::exclude_shapes :
      case KN2Token::exclude_tables :
      case KN2Token::kind :
        KN_DEBUG_XML_TODO_ATTRIBUTE(attr);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
        break;
      }
    }
    else if ((KN2Token::NS_URI_SFA == getNamespaceId(attr)) && (KN2Token::ID == getNameId(attr)))
    {
      KN_DEBUG_XML_TODO_ATTRIBUTE(attr);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN_ATTRIBUTE(attr);
    }
  }

  KNXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if (KN2Token::NS_URI_SF == getNamespaceId(element))
    {
      switch (getNameId(element))
      {
      case KN2Token::text_body :
        parseTextBody(element, layoutStyle);
        break;
      case KN2Token::stylesheet_ref :
        KN_DEBUG_XML_TODO_ELEMENT(element);
        skipElement(element);
        break;
      default :
        KN_DEBUG_XML_UNKNOWN_ELEMENT(element);
        skipElement(element);
        break;
      }
    }
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
