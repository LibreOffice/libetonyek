/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY2PARSER_H_INCLUDED
#define KEY2PARSER_H_INCLUDED

#include "KEY2ParserUtils.h"
#include "KEYParser.h"
#include "KEYTypes.h"

namespace libetonyek
{

class KEYStyle;

class KEY2Parser : public KEYParser, private KEY2ParserUtils
{
public:
  KEY2Parser(const WPXInputStreamPtr_t &input, const WPXInputStreamPtr_t &package, KEYCollector *collector, const KEYDefaults &defaults);
  virtual ~KEY2Parser();

  // NOTE: KEY2TableParser uses these to avoid code duplication
  void parseGeometry(const KEYXMLReader &reader);
  void parseTextBody(const KEYXMLReader &reader);

private:
  virtual void processXmlNode(const KEYXMLReader &reader);
  virtual KEYXMLReader::TokenizerFunction_t getTokenizer() const;

  void parseDrawables(const KEYXMLReader &reader);
  void parseLayer(const KEYXMLReader &reader);
  void parseLayers(const KEYXMLReader &reader);
  void parseMasterSlide(const KEYXMLReader &reader);
  void parseMasterSlides(const KEYXMLReader &reader);
  void parseMetadata(const KEYXMLReader &reader);
  void parseNotes(const KEYXMLReader &reader);
  void parsePage(const KEYXMLReader &reader);
  void parseProxyMasterLayer(const KEYXMLReader &reader);
  void parseSlide(const KEYXMLReader &reader);
  void parseSlideList(const KEYXMLReader &reader);
  void parseStickyNotes(const KEYXMLReader &reader);
  void parseStyles(const KEYXMLReader &reader, bool anonymous);
  void parseStylesheet(const KEYXMLReader &reader);
  void parseTheme(const KEYXMLReader &reader);
  void parseThemeList(const KEYXMLReader &reader);

  void parseBezier(const KEYXMLReader &reader);
  void parseConnectionLine(const KEYXMLReader &reader);
  void parseGroup(const KEYXMLReader &reader);
  void parseImage(const KEYXMLReader &reader);
  void parseLine(const KEYXMLReader &reader);
  void parseMedia(const KEYXMLReader &reader);
  void parsePath(const KEYXMLReader &reader);
  void parseShape(const KEYXMLReader &reader);
  void parseStickyNote(const KEYXMLReader &reader);
  void parsePlaceholder(const KEYXMLReader &reader, bool title = false);

  void parseBezierPath(const KEYXMLReader &reader);
  void parseCallout2Path(const KEYXMLReader &reader);
  void parseConnectionPath(const KEYXMLReader &reader);
  void parsePointPath(const KEYXMLReader &reader);
  void parseScalarPath(const KEYXMLReader &reader);

  void parseContent(const KEYXMLReader &reader);
  void parseData(const KEYXMLReader &reader);
  void parseFiltered(const KEYXMLReader &reader);
  void parseFilteredImage(const KEYXMLReader &reader);
  void parseImageMedia(const KEYXMLReader &reader);
  void parseLeveled(const KEYXMLReader &reader);
  void parseUnfiltered(const KEYXMLReader &reader);
  void parseMovieMedia(const KEYXMLReader &reader);
  void parseSelfContainedMovie(const KEYXMLReader &reader);
  void parseOtherDatas(const KEYXMLReader &reader);

  void parseBr(const KEYXMLReader &reader);
  void parseLayout(const KEYXMLReader &reader);
  void parseLink(const KEYXMLReader &reader);
  void parseP(const KEYXMLReader &reader);
  void parseSpan(const KEYXMLReader &reader);
  void parseTab(const KEYXMLReader &reader);
  void parseText(const KEYXMLReader &reader);
  void parseTextStorage(const KEYXMLReader &reader);

  void emitLayoutStyle(const ID_t &id);

private:
  WPXInputStreamPtr_t m_package;
  unsigned m_version;
};

}

#endif //  KEY2PARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
