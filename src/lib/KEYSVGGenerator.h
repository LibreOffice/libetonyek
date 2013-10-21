/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYSVGGENERATOR_H_INCLUDED
#define KEYSVGGENERATOR_H_INCLUDED

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <libwpd/libwpd.h>
#include <libkeynote/libkeynote.h>

namespace libkeynote
{

class KEYSVGGenerator : public KEYPresentationInterface
{
public:
  explicit KEYSVGGenerator(KEYStringVector &vec);
  virtual ~KEYSVGGenerator();

  virtual void startDocument(const ::WPXPropertyList &propList);
  virtual void endDocument();

  virtual void setDocumentMetaData(const ::WPXPropertyList &propList);

  virtual void startSlide(const ::WPXPropertyList &propList);
  virtual void endSlide();

  virtual void setStyle(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &gradient);

  virtual void startLayer(const ::WPXPropertyList &propList);
  virtual void endLayer();

  virtual void startEmbeddedGraphics(const ::WPXPropertyList & /*propList*/) {}
  virtual void endEmbeddedGraphics() {}

  virtual void startGroup(const ::WPXPropertyList &propList);
  virtual void endGroup();

  virtual void drawRectangle(const ::WPXPropertyList &propList);
  virtual void drawEllipse(const ::WPXPropertyList &propList);
  virtual void drawPolyline(const ::WPXPropertyListVector &vertices);
  virtual void drawPolygon(const ::WPXPropertyListVector &vertices);
  virtual void drawPath(const ::WPXPropertyListVector &path);
  virtual void drawGraphicObject(const ::WPXPropertyList &propList, const ::WPXBinaryData &binaryData);

  virtual void drawConnector(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &path);

  virtual void startTextObject(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &path);
  virtual void endTextObject();
  virtual void insertTab();
  virtual void insertSpace();
  virtual void insertText(const WPXString &text);
  virtual void insertLineBreak();

  virtual void insertField(const WPXString &type, const ::WPXPropertyList &propList);

  virtual void openOrderedListLevel(const ::WPXPropertyList &propList);
  virtual void openUnorderedListLevel(const ::WPXPropertyList &propList);
  virtual void closeOrderedListLevel();
  virtual void closeUnorderedListLevel();
  virtual void openListElement(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &tabStops);
  virtual void closeListElement();

  virtual void openParagraph(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &tabStops);
  virtual void closeParagraph();
  virtual void openSpan(const ::WPXPropertyList &propList);
  virtual void closeSpan();

  virtual void openTable(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &columns);
  virtual void openTableRow(const ::WPXPropertyList &propList);
  virtual void closeTableRow();
  virtual void openTableCell(const ::WPXPropertyList &propList);
  virtual void closeTableCell();
  virtual void insertCoveredTableCell(const ::WPXPropertyList &propList);
  virtual void closeTable();

  virtual void startComment(const ::WPXPropertyList &propList);
  virtual void endComment();

  virtual void startNotes(const ::WPXPropertyList &propList);
  virtual void endNotes();

private:
  ::WPXPropertyListVector m_gradient;
  ::WPXPropertyList m_style;
  int m_gradientIndex;
  int m_patternIndex;
  int m_shadowIndex;
  void writeStyle(bool isClosed=true);
  void drawPolySomething(const ::WPXPropertyListVector &vertices, bool isClosed);

  std::ostringstream m_outputSink;
  KEYStringVector &m_vec;
};

} // namespace libkeynote

#endif // KEYSVGGENERATOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
