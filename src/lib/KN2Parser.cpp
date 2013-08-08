/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "libkeynote_utils.h"
#include "KN2Parser.h"
#include "KNToken.h"
#include "KNXMLAttributeIterator.h"

namespace libkeynote
{

namespace
{

unsigned getVersion(const int token)
{
  switch (token)
  {
  case XML_VERSION_STR_2 :
    return 2;
  case XML_VERSION_STR_3 :
    return 3;
  case XML_VERSION_STR_4 :
    return 4;
  case XML_VERSION_STR_5 :
    return 5;
  }

  return 0;
}

bool skipElement(xmlTextReaderPtr reader)
{
  int level = 1;

  int ret = xmlTextReaderRead(reader);
  while ((1 == ret) && (0 < level))
  {
    switch (xmlTextReaderNodeType(reader))
    {
      case XML_READER_TYPE_ELEMENT :
        if (!xmlTextReaderIsEmptyElement(reader))
          ++level;
        break;
      case XML_READER_TYPE_END_ELEMENT :
        --level;
        break;
    }

    ret = xmlTextReaderRead(reader);
  }

  return (-1 != ret) && (0 == level);
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
        && (XML_NS_KEY == getKNTokenID(xmlTextReaderConstNamespaceUri(reader)))
        && (XML_PRESENTATION == getKNTokenID(xmlTextReaderConstLocalName(reader)))))
    return false;

  // read attributes
  KNXMLAttributeIterator attr(reader);
  while (attr.next())
  {
    if (attr->ns)
    {
      switch (getKNTokenID(attr->ns))
      {
      case XML_NS_KEY :
        switch (getKNTokenID(attr->name))
        {
        case XML_ALL_IMAGES_BUNDLED :
        case XML_COMPATIBLE_VERSION :
        case XML_KIOSK_BUILD_DELAY :
        case XML_KIOSK_SLIDE_DELAY :
        case XML_MODE :
        case XML_PLAY_MODE :
        case XML_STICKY_VISIBILITY :
          // TODO: implement me
          break;
        case XML_VERSION :
          m_version = getVersion(getKNTokenID(attr->value));
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
      case XML_NS_SFA :
        if (XML_I_D != getKNTokenID(attr->name))
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
    if (ns && (XML_NS_KEY == getKNTokenID(ns)))
    {
      switch (getKNTokenID(name))
      {
      case XML_SIZE :
      case XML_THEME_LIST :
      case XML_MASTER_ORDER :
      case XML_SLIDE_LIST :
      case XML_UI_STATE :
      case XML_METADATA :
      case XML_SOUNDTRACK_LIST :
      case XML_CALCULATION_ENGINE :
      case XML_CALC_ENGINE :
      case XML_VERSION_HISTORY :
        // TODO: implement me
        if (!skipElement(reader))
        {
          KN_DEBUG_MSG(("failed to skip element {%s}%s\n", ns, name));
          return false;
        }
        break;

      case XML_PRESENTATION :
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
