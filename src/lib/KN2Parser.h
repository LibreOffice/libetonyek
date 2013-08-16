/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KN2PARSER_H_INCLUDED
#define KN2PARSER_H_INCLUDED

#include "KNParser.h"
#include "KNTypes.h"

namespace libkeynote
{

class KNStyle;

class KN2Parser : public KNParser
{
public:
  KN2Parser(WPXInputStream *input, KNCollector *collector);
  virtual ~KN2Parser();

private:
  virtual void processXmlNode(xmlTextReaderPtr reader);

  void parseDrawables(xmlTextReaderPtr reader);
  void parseLayer(xmlTextReaderPtr reader);
  void parseLayers(xmlTextReaderPtr reader);
  void parseMasterSlide(xmlTextReaderPtr reader);
  void parseMasterSlides(xmlTextReaderPtr reader);
  void parseMetadata(xmlTextReaderPtr reader);
  void parsePage(xmlTextReaderPtr reader);
  void parsePropertyMap(xmlTextReaderPtr reader, KNStyle &style);
  void parseProxyMasterLayer(xmlTextReaderPtr reader);
  void parseSize(xmlTextReaderPtr reader, KNSize &size);
  void parseSlide(xmlTextReaderPtr reader);
  void parseSlideList(xmlTextReaderPtr reader);
  ID_t parseStyle(xmlTextReaderPtr reader, KNStyle &style);
  void parseStyles(xmlTextReaderPtr reader, bool anonymous);
  void parseStylesheet(xmlTextReaderPtr reader);
  void parseTheme(xmlTextReaderPtr reader);
  void parseThemeList(xmlTextReaderPtr reader);

  ID_t parseGeometry(xmlTextReaderPtr reader);
  ID_t parseGroup(xmlTextReaderPtr reader);
  ID_t parseImage(xmlTextReaderPtr reader);
  ID_t parseLine(xmlTextReaderPtr reader);
  ID_t parseMedia(xmlTextReaderPtr reader);
  ID_t parseShape(xmlTextReaderPtr reader);

private:
  unsigned m_version;
};

}

#endif //  KN2PARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
