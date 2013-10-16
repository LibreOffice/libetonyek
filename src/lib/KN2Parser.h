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

#include "KN2ParserUtils.h"
#include "KNParser.h"
#include "KNTypes.h"

namespace libkeynote
{

class KNStyle;

class KN2Parser : public KNParser, private KN2ParserUtils
{
public:
  KN2Parser(const WPXInputStreamPtr_t &input, const WPXInputStreamPtr_t &package, KNCollector *collector, const KNDefaults &defaults);
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
  void parseProxyMasterLayer(const KNXMLReader &reader);
  void parseSlide(const KNXMLReader &reader);
  void parseSlideList(const KNXMLReader &reader);
  void parseStickyNotes(const KNXMLReader &reader);
  void parseStyles(const KNXMLReader &reader, bool anonymous);
  void parseStylesheet(const KNXMLReader &reader);
  void parseTheme(const KNXMLReader &reader);
  void parseThemeList(const KNXMLReader &reader);

  void parseBezier(const KNXMLReader &reader);
  void parseConnectionLine(const KNXMLReader &reader);
  void parseGeometry(const KNXMLReader &reader);
  void parseGroup(const KNXMLReader &reader);
  void parseImage(const KNXMLReader &reader);
  void parseLine(const KNXMLReader &reader);
  void parseMedia(const KNXMLReader &reader);
  void parsePath(const KNXMLReader &reader);
  void parseShape(const KNXMLReader &reader);
  void parseStickyNote(const KNXMLReader &reader);
  void parsePlaceholder(const KNXMLReader &reader, bool title = false);

  void parseBezierPath(const KNXMLReader &reader);
  void parseCallout2Path(const KNXMLReader &reader);
  void parseConnectionPath(const KNXMLReader &reader);
  void parsePointPath(const KNXMLReader &reader);
  void parseScalarPath(const KNXMLReader &reader);

  void parseContent(const KNXMLReader &reader);
  void parseData(const KNXMLReader &reader);
  void parseFiltered(const KNXMLReader &reader);
  void parseFilteredImage(const KNXMLReader &reader);
  void parseImageMedia(const KNXMLReader &reader);
  void parseLeveled(const KNXMLReader &reader);
  void parseUnfiltered(const KNXMLReader &reader);
  void parseMovieMedia(const KNXMLReader &reader);
  void parseSelfContainedMovie(const KNXMLReader &reader);
  void parseOtherDatas(const KNXMLReader &reader);

  void parseBr(const KNXMLReader &reader);
  void parseLayout(const KNXMLReader &reader);
  void parseLink(const KNXMLReader &reader, bool ref = false);
  void parseP(const KNXMLReader &reader);
  void parseSpan(const KNXMLReader &reader);
  void parseTab(const KNXMLReader &reader);
  void parseText(const KNXMLReader &reader);
  void parseTextBody(const KNXMLReader &reader);
  void parseTextStorage(const KNXMLReader &reader);

  void emitLayoutStyle(const ID_t &id);

private:
  WPXInputStreamPtr_t m_package;
  unsigned m_version;
};

}

#endif //  KN2PARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
