/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PAGPARSER_H_INCLUDED
#define PAGPARSER_H_INCLUDED

#include <libxml/xmlreader.h>

#include "libetonyek_utils.h"

namespace libetonyek
{

class PAGCollector;
class KEYXMLReader;

class PAGParser
{
  // -Weffc++
  PAGParser(const PAGParser &);
  PAGParser &operator=(const PAGParser &);

  enum TextStorageKind
  {
    TEXT_STORAGE_KIND_UNKNOWN,
    TEXT_STORAGE_KIND_BODY,
    TEXT_STORAGE_KIND_HEADER,
    TEXT_STORAGE_KIND_FOOTNOTE,
    TEXT_STORAGE_KIND_TEXTBOX,
    TEXT_STORAGE_KIND_NOTE,
    TEXT_STORAGE_KIND_CELL
  };

public:
  PAGParser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, PAGCollector *collector);

  bool parse();

private:
  void parseDocument(const KEYXMLReader &reader);
  void parseMetadata(const KEYXMLReader &reader);
  void parseSectionPrototypes(const KEYXMLReader &reader);
  void parseStylesheet(const KEYXMLReader &reader);
  void parseHeaders(const KEYXMLReader &reader);
  void parseFooters(const KEYXMLReader &reader);
  void parseTextStorage(const KEYXMLReader &reader);

  void parseTextBody(const KEYXMLReader &reader, TextStorageKind kind);
  void parseSection(const KEYXMLReader &reader);
  void parseLayout(const KEYXMLReader &reader);
  void parseP(const KEYXMLReader &reader);
  void parseSpan(const KEYXMLReader &reader);
  void parseTab(const KEYXMLReader &reader);
  void parseBr(const KEYXMLReader &reader);

private:
  RVNGInputStreamPtr_t m_input;
  RVNGInputStreamPtr_t m_package;
  PAGCollector *m_collector;
};

} // namespace libetonyek

#endif //  PAGPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
