/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstring>

#include <libwpd-stream/libwpd-stream.h>

#include "libkeynote_xml.h"
#include "KNParser.h"
#include "KNCollector.h"
#include "KNToken.h"

namespace libkeynote
{

namespace
{

extern "C" int readFromStream(void *context, char *buffer, int len)
{
  try
  {
    WPXInputStream *const input = reinterpret_cast<WPXInputStream *>(context);

    unsigned long bytesRead = 0;
    const unsigned char *const bytes = input->read(len, bytesRead);

    std::memcpy(buffer, bytes, static_cast<int>(bytesRead));
    return static_cast<int>(bytesRead);
  }
  catch (...)
  {
  }

  return -1;
}

extern "C" int closeStream(void * /* context */)
{
  return 0;
}

}

KNParser::KNParser(WPXInputStream *const input, KNCollector *const collector)
  : m_input(input)
  , m_collector(collector)
{
}

KNParser::~KNParser()
{
}

bool KNParser::parse()
{
  m_input->seek(0, WPX_SEEK_SET);

  const xmlTextReaderPtr reader = xmlReaderForIO(readFromStream, closeStream, m_input, "", 0, 0);
  if (!reader)
    return false;

  const bool success = processXmlDocument(reader);

  xmlTextReaderClose(reader);
  xmlFreeTextReader(reader);

  return success;
}

KNCollector *KNParser::getCollector() const
{
  return m_collector;
}

bool KNParser::processXmlDocument(xmlTextReaderPtr reader) try
{
  if (moveToNextNode(reader))
  {
    // process root node
    processXmlNode(reader);
    return !moveToNextNode(reader); // the whole input has been read
  }
  return false;
}
catch (...)
{
  return false;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
