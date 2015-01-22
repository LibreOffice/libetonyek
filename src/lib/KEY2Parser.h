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
  KEY2Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, KEYCollector *collector, const KEYDefaults &defaults);
  virtual ~KEY2Parser();

  // NOTE: KEY2TableParser uses these to avoid code duplication
  void parseGeometry(const IWORKXMLReader &reader);
  void parseTextBody(const IWORKXMLReader &reader);

private:
  virtual void processXmlNode(const IWORKXMLReader &reader);
  virtual IWORKXMLReader::TokenizerFunction_t getTokenizer() const;

  void parseDrawables(const IWORKXMLReader &reader);
  void parseLayer(const IWORKXMLReader &reader);
  void parseLayers(const IWORKXMLReader &reader);
  void parseMasterSlide(const IWORKXMLReader &reader);
  void parseMasterSlides(const IWORKXMLReader &reader);
  void parseMetadata(const IWORKXMLReader &reader);
  void parseNotes(const IWORKXMLReader &reader);
  void parsePage(const IWORKXMLReader &reader);
  void parseProxyMasterLayer(const IWORKXMLReader &reader);
  void parseSlide(const IWORKXMLReader &reader);
  void parseSlideList(const IWORKXMLReader &reader);
  void parseStickyNotes(const IWORKXMLReader &reader);
  void parseStyles(const IWORKXMLReader &reader, bool anonymous);
  void parseStylesheet(const IWORKXMLReader &reader);
  void parseTheme(const IWORKXMLReader &reader);
  void parseThemeList(const IWORKXMLReader &reader);

  void parseBezier(const IWORKXMLReader &reader);
  void parseConnectionLine(const IWORKXMLReader &reader);
  void parseGroup(const IWORKXMLReader &reader);
  void parseImage(const IWORKXMLReader &reader);
  void parseLine(const IWORKXMLReader &reader);
  void parseMedia(const IWORKXMLReader &reader);
  void parsePath(const IWORKXMLReader &reader);
  void parseShape(const IWORKXMLReader &reader);
  void parseStickyNote(const IWORKXMLReader &reader);
  void parsePlaceholder(const IWORKXMLReader &reader, bool title = false);

  void parseBezierPath(const IWORKXMLReader &reader);
  void parseCallout2Path(const IWORKXMLReader &reader);
  void parseConnectionPath(const IWORKXMLReader &reader);
  void parsePointPath(const IWORKXMLReader &reader);
  void parseScalarPath(const IWORKXMLReader &reader);

  void parseContent(const IWORKXMLReader &reader);
  void parseData(const IWORKXMLReader &reader);
  void parseFiltered(const IWORKXMLReader &reader);
  void parseFilteredImage(const IWORKXMLReader &reader);
  void parseImageMedia(const IWORKXMLReader &reader);
  void parseLeveled(const IWORKXMLReader &reader);
  void parseUnfiltered(const IWORKXMLReader &reader);
  void parseMovieMedia(const IWORKXMLReader &reader);
  void parseSelfContainedMovie(const IWORKXMLReader &reader);
  void parseOtherDatas(const IWORKXMLReader &reader);

  void parseBr(const IWORKXMLReader &reader);
  void parseLayout(const IWORKXMLReader &reader);
  void parseLink(const IWORKXMLReader &reader);
  void parseP(const IWORKXMLReader &reader);
  void parseSpan(const IWORKXMLReader &reader);
  void parseTab(const IWORKXMLReader &reader);
  void parseText(const IWORKXMLReader &reader);
  void parseTextStorage(const IWORKXMLReader &reader);

  void emitLayoutStyle(const ID_t &id);

private:
  RVNGInputStreamPtr_t m_package;
  unsigned m_version;
};

}

#endif //  KEY2PARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
