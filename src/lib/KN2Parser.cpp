/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include "libkeynote_utils.h"
#include "libkeynote_xml.h"
#include "KN2Parser.h"
#include "KN2Token.h"
#include "KNXMLAttributeIterator.h"

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

void KN2Parser::processXmlNode(xmlTextReaderPtr reader)
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

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
