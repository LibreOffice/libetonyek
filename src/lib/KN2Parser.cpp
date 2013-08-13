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
#include "KNTypes.h"
#include "KNXMLAttributeIterator.h"

using boost::lexical_cast;

namespace libkeynote
{

namespace
{

bool checkElement(xmlTextReaderPtr reader, const int name, const int ns, const bool start = true)
{
  return isElement(reader)
         && getNamespace(reader) && (getKN2TokenID(getNamespace(reader)) == ns)
         && (getKN2TokenID(getName(reader)) == name)
         && (isStartElement(reader) == start);
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
        if (KN2Token::ID != getKN2TokenID(attr->name))
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

    if (ns && (KN2Token::NS_URI_KEY == getKN2TokenID(ns)))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::size :
      case KN2Token::theme_list :
      case KN2Token::master_order :
      case KN2Token::slide_list :
      case KN2Token::ui_state :
      case KN2Token::metadata :
      case KN2Token::soundtrack_list :
      case KN2Token::calculation_engine :
      case KN2Token::version_history :
        KN_DEBUG_XML_TODO("element", name, ns);
        skipElement(reader);
        break;
      }
    }
    else if (ns && (KN2Token::NS_URI_SF == getKN2TokenID(ns)) && (KN2Token::calc_engine == getKN2TokenID(name)))
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

void KN2Parser::parseMasterSlides(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::NS_URI_KEY, KN2Token::master_slides));

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if (attr->ns && (KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns)) && (KN2Token::ID == getKN2TokenID(attr->name)))
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

    if (checkElement(reader, KN2Token::NS_URI_KEY, KN2Token::master_slides, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (ns && (KN2Token::NS_URI_KEY == getKN2TokenID(ns)) && (KN2Token::master_slide == getKN2TokenID(name)))
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

void KN2Parser::parseMetadata(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::NS_URI_KEY, KN2Token::metadata));

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

    if (checkElement(reader, KN2Token::NS_URI_KEY, KN2Token::presentation, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (ns && (KN2Token::NS_URI_KEY == getKN2TokenID(ns)))
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
  assert(checkElement(reader, KN2Token::NS_URI_KEY, KN2Token::page));

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if (attr->ns && (KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns)) && (KN2Token::ID == getKN2TokenID(attr->name)))
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

    if (checkElement(reader, KN2Token::NS_URI_KEY, KN2Token::page, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (ns && (KN2Token::NS_URI_SF == getKN2TokenID(ns)))
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

void KN2Parser::parseSize(const xmlTextReaderPtr reader, KNSize &size)
{
  assert(checkElement(reader, KN2Token::NS_URI_KEY, KN2Token::size));

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if (attr->ns && KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns))
    {
      switch (getKN2TokenID(attr->ns))
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

  if (!isEmptyElement(reader))
  {
    // there are no elements
    while (moveToNextNode(reader))
    {
      KN_DEBUG_XML_UNKNOWN("element", getName(reader), getNamespace(reader));
      skipElement(reader);
    }
  }
}

void KN2Parser::parseSlide(const xmlTextReaderPtr reader)
{
  assert(checkElement(reader, KN2Token::NS_URI_KEY, KN2Token::slide));

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
        if (KN2Token::ID != getKN2TokenID(attr->name))
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

    if (checkElement(reader, KN2Token::NS_URI_KEY, KN2Token::slide, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (ns && (KN2Token::NS_URI_KEY == getKN2TokenID(ns)))
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
  assert(checkElement(reader, KN2Token::NS_URI_KEY, KN2Token::slide_list));

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if (attr->ns && (KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns)) && (KN2Token::ID == getKN2TokenID(attr->name)))
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

    if (checkElement(reader, KN2Token::NS_URI_KEY, KN2Token::slide_list, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (ns && (KN2Token::NS_URI_KEY == getKN2TokenID(ns)) && (KN2Token::slide == getKN2TokenID(name)))
    {
      parseSlide(reader);
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
  assert(checkElement(reader, KN2Token::NS_URI_KEY, KN2Token::stylesheet));

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if (attr->ns && (KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns)) && (KN2Token::ID == getKN2TokenID(attr->name)))
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

    if (checkElement(reader, KN2Token::NS_URI_KEY, KN2Token::stylesheet, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (ns && (KN2Token::NS_URI_SF == getKN2TokenID(ns)))
    {
      switch (getKN2TokenID(name))
      {
      case KN2Token::styles :
      case KN2Token::anon_styles :
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
  assert(checkElement(reader, KN2Token::NS_URI_KEY, KN2Token::theme));

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

    if (checkElement(reader, KN2Token::NS_URI_KEY, KN2Token::theme, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (ns && (KN2Token::NS_URI_KEY == getKN2TokenID(ns)))
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
  assert(checkElement(reader, KN2Token::NS_URI_KEY, KN2Token::theme_list));

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if (attr->ns && (KN2Token::NS_URI_SFA == getKN2TokenID(attr->ns)) && (KN2Token::ID == getKN2TokenID(attr->name)))
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

    if (checkElement(reader, KN2Token::NS_URI_KEY, KN2Token::theme_list, false))
      break;

    if (isEndElement(reader))
      throw GenericException();

    if (ns && (KN2Token::NS_URI_KEY == getKN2TokenID(ns)) && (KN2Token::theme == getKN2TokenID(name)))
    {
      parseTheme(reader);
    }
    else
    {
      KN_DEBUG_XML_UNKNOWN("element", name, ns);
      skipElement(reader);
    }
  }
}


}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
