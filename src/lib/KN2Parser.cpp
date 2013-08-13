/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "libkeynote_utils.h"
#include "libkeynote_xml.h"
#include "KN2Parser.h"
#include "KN2Token.h"
#include "KNXMLAttributeIterator.h"

namespace libkeynote
{

namespace
{

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

bool KN2Parser::processXmlNode(xmlTextReaderPtr reader)
{
  if (XML_READER_TYPE_ELEMENT != xmlTextReaderNodeType(reader))
    return false;

  if (!(xmlTextReaderConstNamespaceUri(reader)
        && (KN2Token::NS_URI_KEY == getKN2TokenID(xmlTextReaderConstNamespaceUri(reader)))
        && (KN2Token::presentation == getKN2TokenID(xmlTextReaderConstLocalName(reader)))))
    return false;

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
          // TODO: implement me
          break;
        case KN2Token::version :
          m_version = getVersion(getKN2TokenID(attr->value));
          if (0 == m_version)
          {
            KN_DEBUG_MSG(("unknown version %s\n", attr->value));
          }
          break;
        default :
          KN_DEBUG_MSG(("unprocessed attribute {%s}%s\n", attr->ns, attr->name));
          break;
        }
        break;
      case KN2Token::NS_URI_SFA :
        if (KN2Token::id != getKN2TokenID(attr->name))
        {
          KN_DEBUG_MSG(("unprocessed attribute {%s}%s\n", attr->ns, attr->name));
        }
        break;
      default :
        KN_DEBUG_MSG(("unprocessed attribute {%s}%s\n", attr->ns, attr->name));
        break;
      }
    }
    else
    {
      KN_DEBUG_MSG(("unprocessed attribute %s\n", attr->name));
    }
  }

  // read elements
  int ret = xmlTextReaderRead(reader);
  while (1 == ret)
  {
    const int nodeType = xmlTextReaderNodeType(reader);
    if (!((XML_READER_TYPE_ELEMENT == nodeType) || (XML_READER_TYPE_END_ELEMENT == nodeType)))
    {
      ret = xmlTextReaderRead(reader);
      continue;
    }

    const xmlChar *const ns = xmlTextReaderConstNamespaceUri(reader);
    const xmlChar *const name = xmlTextReaderConstLocalName(reader);
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
      case KN2Token::calc_engine :
      case KN2Token::version_history :
        // TODO: implement me
        if (!skipElement(reader))
        {
          KN_DEBUG_MSG(("failed to skip element {%s}%s\n", ns, name));
          return false;
        }
        break;

      case KN2Token::presentation :
        return XML_READER_TYPE_END_ELEMENT == nodeType;
      }
    }
    else
    {
      KN_DEBUG_MSG(("unprocessed element %s%s%s%s\n", ns ? "{" : "", ns, ns ? "}" : "", name));
      if (!skipElement(reader))
      {
        KN_DEBUG_MSG(("failed to skip element {%s}%s\n", ns, name));
        return false;
      }
    }

    ret = xmlTextReaderRead(reader);
  }

  return true;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
