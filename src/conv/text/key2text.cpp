/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <string.h>

#include <libwpd-stream/libwpd-stream.h>
#include <libwpd/libwpd.h>
#include <libkeynote/libkeynote.h>

#include "KEYDirectoryStream.h"

class TextPainter : public libkeynote::KEYPresentationInterface
{
public:
  TextPainter();

  virtual void startDocument(const ::WPXPropertyList &propList);
  virtual void endDocument();
  virtual void setDocumentMetaData(const ::WPXPropertyList &propList);
  virtual void startSlide(const ::WPXPropertyList &propList);
  virtual void endSlide();
  virtual void startLayer(const ::WPXPropertyList &propList);
  virtual void endLayer();
  virtual void startEmbeddedGraphics(const ::WPXPropertyList &propList);
  virtual void endEmbeddedGraphics();
  virtual void startGroup(const ::WPXPropertyList &propList);
  virtual void endGroup();

  virtual void setStyle(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &gradient);

  virtual void drawRectangle(const ::WPXPropertyList &propList);
  virtual void drawEllipse(const ::WPXPropertyList &propList);
  virtual void drawPolyline(const ::WPXPropertyListVector &vertices);
  virtual void drawPolygon(const ::WPXPropertyListVector &vertices);
  virtual void drawPath(const ::WPXPropertyListVector &path);
  virtual void drawGraphicObject(const ::WPXPropertyList &propList, const ::WPXBinaryData &binaryData);
  virtual void drawConnector(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &path);

  virtual void startTextObject(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &path);
  virtual void endTextObject();
  virtual void openParagraph(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &tabStops);
  virtual void closeParagraph();
  virtual void openSpan(const ::WPXPropertyList &propList);
  virtual void closeSpan();
  virtual void insertTab();
  virtual void insertSpace();
  virtual void insertText(const ::WPXString &str);
  virtual void insertLineBreak();

  virtual void insertField(const WPXString &type, const ::WPXPropertyList &propList);

  virtual void openOrderedListLevel(const ::WPXPropertyList &propList);
  virtual void openUnorderedListLevel(const ::WPXPropertyList &propList);
  virtual void closeOrderedListLevel();
  virtual void closeUnorderedListLevel();
  virtual void openListElement(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &tabStops);
  virtual void closeListElement();

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
};

TextPainter::TextPainter()
{
}

void TextPainter::startDocument(const ::WPXPropertyList &)
{
}

void TextPainter::endDocument()
{
}

void TextPainter::setDocumentMetaData(const ::WPXPropertyList &)
{
}

void TextPainter::startSlide(const ::WPXPropertyList &)
{
}

void TextPainter::endSlide()
{
  printf("\n");
}

void TextPainter::startLayer(const ::WPXPropertyList &)
{
}

void TextPainter::endLayer()
{
}

void TextPainter::startEmbeddedGraphics(const ::WPXPropertyList &)
{
}

void TextPainter::endEmbeddedGraphics()
{
}

void TextPainter::startGroup(const ::WPXPropertyList &)
{
}

void TextPainter::endGroup()
{
}

void TextPainter::setStyle(const ::WPXPropertyList &, const ::WPXPropertyListVector &)
{
}

void TextPainter::drawRectangle(const ::WPXPropertyList &)
{
}

void TextPainter::drawEllipse(const ::WPXPropertyList &)
{
}

void TextPainter::drawPolyline(const ::WPXPropertyListVector &)
{
}

void TextPainter::drawPolygon(const ::WPXPropertyListVector &)
{
}

void TextPainter::drawPath(const ::WPXPropertyListVector &)
{
}

void TextPainter::drawGraphicObject(const ::WPXPropertyList &, const ::WPXBinaryData &)
{
}

void TextPainter::drawConnector(const ::WPXPropertyList &, const ::WPXPropertyListVector &)
{
}

void TextPainter::startTextObject(const ::WPXPropertyList &, const ::WPXPropertyListVector &)
{
}

void TextPainter::endTextObject()
{
  printf("\n");
}

void TextPainter::openParagraph(const ::WPXPropertyList &, const ::WPXPropertyListVector &)
{
}

void TextPainter::closeParagraph()
{
  printf("\n");
}

void TextPainter::openSpan(const ::WPXPropertyList &)
{
}

void TextPainter::closeSpan()
{
}

void TextPainter::insertText(const ::WPXString &str)
{
  printf(str.cstr());
}

void TextPainter::insertTab()
{
  printf("\t");
}

void TextPainter::insertSpace()
{
  printf(" ");
}

void TextPainter::insertLineBreak()
{
  printf("\n");
}

void TextPainter::insertField(const WPXString &, const ::WPXPropertyList &)
{
}

void TextPainter::openOrderedListLevel(const ::WPXPropertyList &)
{
}

void TextPainter::openUnorderedListLevel(const ::WPXPropertyList &)
{
}

void TextPainter::closeOrderedListLevel()
{
}

void TextPainter::closeUnorderedListLevel()
{
}

void TextPainter::openListElement(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &tabStops)
{
  openParagraph(propList, tabStops);
}

void TextPainter::closeListElement()
{
  closeParagraph();
}

void TextPainter::openTable(const ::WPXPropertyList &, const ::WPXPropertyListVector &)
{
}

void TextPainter::openTableRow(const ::WPXPropertyList &)
{
}

void TextPainter::closeTableRow()
{
}

void TextPainter::openTableCell(const ::WPXPropertyList &)
{
}

void TextPainter::closeTableCell()
{
}

void TextPainter::insertCoveredTableCell(const ::WPXPropertyList &)
{
}

void TextPainter::closeTable()
{
}

void TextPainter::startComment(const ::WPXPropertyList &)
{
}

void TextPainter::endComment()
{
}

void TextPainter::startNotes(const ::WPXPropertyList &)
{
}

void TextPainter::endNotes()
{
}

namespace
{

int printUsage()
{
  printf("Usage: key2text [OPTION] <KeyNote Document> | <KeyNote Directory>\n");
  printf("\n");
  printf("Options:\n");
  printf("--help                Shows this help message\n");
  return -1;
}

} // anonymous namespace

int main(int argc, char *argv[])
{
  if (argc < 2)
    return printUsage();

  char *file = 0;

  for (int i = 1; i < argc; i++)
  {
    if (!file && strncmp(argv[i], "--", 2))
      file = argv[i];
    else
      return printUsage();
  }

  if (!file)
    return printUsage();

  using boost::shared_ptr;
  namespace fs = boost::filesystem;

  fs::path path(file);
  shared_ptr<WPXInputStream> input;
  if (is_directory(path))
    input.reset(new conv::KEYDirectoryStream(path));
  else
    input.reset(new WPXFileStream(file));

  libkeynote::KEYDocumentType type = libkeynote::KEY_DOCUMENT_TYPE_UNKNOWN;
  if (!libkeynote::KEYDocument::isSupported(input.get(), &type))
  {
    fprintf(stderr, "ERROR: Unsupported file format!\n");
    return 1;
  }

  if (libkeynote::KEY_DOCUMENT_TYPE_APXL_FILE == type)
  {
    path.remove_filename();
    input.reset(new conv::KEYDirectoryStream(path));
  }

  TextPainter painter;
  libkeynote::KEYDocument::parse(input.get(), &painter);

  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
