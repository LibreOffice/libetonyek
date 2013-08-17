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
  virtual void processXmlNode(const KNXMLReader &reader);
  virtual KNXMLReader::TokenizerFunction_t getTokenizer() const;

  void parseDrawables(const KNXMLReader &reader);
  void parseLayer(const KNXMLReader &reader);
  void parseLayers(const KNXMLReader &reader);
  void parseMasterSlide(const KNXMLReader &reader);
  void parseMasterSlides(const KNXMLReader &reader);
  void parseMetadata(const KNXMLReader &reader);
  void parsePage(const KNXMLReader &reader);
  void parsePropertyMap(const KNXMLReader &reader, KNStyle &style);
  void parseProxyMasterLayer(const KNXMLReader &reader);
  void parseSize(const KNXMLReader &reader, KNSize &size);
  void parseSlide(const KNXMLReader &reader);
  void parseSlideList(const KNXMLReader &reader);
  ID_t parseStyle(const KNXMLReader &reader, KNStyle &style);
  void parseStyles(const KNXMLReader &reader, bool anonymous);
  void parseStylesheet(const KNXMLReader &reader);
  void parseTheme(const KNXMLReader &reader);
  void parseThemeList(const KNXMLReader &reader);

  ID_t parseGeometry(const KNXMLReader &reader);
  ID_t parseGroup(const KNXMLReader &reader);
  ID_t parseImage(const KNXMLReader &reader);
  ID_t parseLine(const KNXMLReader &reader);
  ID_t parseMedia(const KNXMLReader &reader);
  ID_t parseShape(const KNXMLReader &reader);

private:
  unsigned m_version;
};

}

#endif //  KN2PARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
