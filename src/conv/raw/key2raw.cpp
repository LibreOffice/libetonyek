/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <string.h>

#include <libwpd-stream/libwpd-stream.h>
#include <libwpd/libwpd.h>
#include <libetonyek/libetonyek.h>

#include "KEYDirectoryStream.h"

class RawPainter : public libetonyek::KEYPresentationInterface
{
public:
  RawPainter();

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

WPXString getPropString(const WPXPropertyList &propList)
{
  WPXString propString;
  WPXPropertyList::Iter i(propList);
  if (!i.last())
  {
    propString.append(i.key());
    propString.append(": ");
    propString.append(i()->getStr().cstr());
    for (; i.next(); )
    {
      propString.append(", ");
      propString.append(i.key());
      propString.append(": ");
      propString.append(i()->getStr().cstr());
    }
  }

  return propString;
}

WPXString getPropString(const WPXPropertyListVector &itemList)
{
  WPXString propString;

  propString.append("(");
  WPXPropertyListVector::Iter i(itemList);

  if (!i.last())
  {
    propString.append("(");
    propString.append(getPropString(i()));
    propString.append(")");

    for (; i.next();)
    {
      propString.append(", (");
      propString.append(getPropString(i()));
      propString.append(")");
    }

  }
  propString.append(")");

  return propString;
}

RawPainter::RawPainter()
{
}

void RawPainter::startDocument(const ::WPXPropertyList &propList)
{
  printf("RawPainter::startDocument(%s)\n", getPropString(propList).cstr());
}

void RawPainter::endDocument()
{
  printf("RawPainter::endDocument\n");
}

void RawPainter::setDocumentMetaData(const ::WPXPropertyList &propList)
{
  printf("RawPainter::setDocumentMetaData(%s)\n", getPropString(propList).cstr());
}

void RawPainter::startSlide(const ::WPXPropertyList &propList)
{
  printf("RawPainter::startSlide(%s)\n", getPropString(propList).cstr());
}

void RawPainter::endSlide()
{
  printf("RawPainter::endSlide\n");
}

void RawPainter::startLayer(const ::WPXPropertyList &propList)
{
  printf("RawPainter::startLayer (%s)\n", getPropString(propList).cstr());
}

void RawPainter::endLayer()
{
  printf("RawPainter::endLayer\n");
}

void RawPainter::startEmbeddedGraphics(const ::WPXPropertyList &propList)
{
  printf("RawPainter::startEmbeddedGraphics (%s)\n", getPropString(propList).cstr());
}

void RawPainter::endEmbeddedGraphics()
{
  printf("RawPainter::endEmbeddedGraphics \n");
}

void RawPainter::startGroup(const ::WPXPropertyList &propList)
{
  printf("RawPainter::startGroup(%s)\n", getPropString(propList).cstr());
}

void RawPainter::endGroup()
{
  printf("RawPainter::endGroup\n");
}

void RawPainter::setStyle(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &gradient)
{
  printf("RawPainter::setStyle(%s, gradient: (%s))\n", getPropString(propList).cstr(), getPropString(gradient).cstr());
}

void RawPainter::drawRectangle(const ::WPXPropertyList &propList)
{
  printf("RawPainter::drawRectangle (%s)\n", getPropString(propList).cstr());
}

void RawPainter::drawEllipse(const ::WPXPropertyList &propList)
{
  printf("RawPainter::drawEllipse (%s)\n", getPropString(propList).cstr());
}

void RawPainter::drawPolyline(const ::WPXPropertyListVector &vertices)
{
  printf("RawPainter::drawPolyline (%s)\n", getPropString(vertices).cstr());
}

void RawPainter::drawPolygon(const ::WPXPropertyListVector &vertices)
{
  printf("RawPainter::drawPolygon (%s)\n", getPropString(vertices).cstr());
}

void RawPainter::drawPath(const ::WPXPropertyListVector &path)
{
  printf("RawPainter::drawPath (%s)\n", getPropString(path).cstr());
}

void RawPainter::drawGraphicObject(const ::WPXPropertyList &propList, const ::WPXBinaryData & /*binaryData*/)
{
  printf("RawPainter::drawGraphicObject (%s)\n", getPropString(propList).cstr());
}

void RawPainter::drawConnector(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &path)
{
  printf("RawPainter::drawConnector(%s, path: (%s))\n", getPropString(propList).cstr(), getPropString(path).cstr());
}

void RawPainter::startTextObject(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &path)
{
  printf("RawPainter::startTextObject (%s, path: (%s))\n", getPropString(propList).cstr(), getPropString(path).cstr());
}

void RawPainter::endTextObject()
{
  printf("RawPainter::endTextObject\n");
}

void RawPainter::openParagraph(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &tabStops)
{
  printf("RawPainter::openParagraph (%s, tabStops: (%s))\n", getPropString(propList).cstr(), getPropString(tabStops).cstr());
}

void RawPainter::closeParagraph()
{
  printf("RawPainter::closeParagraph\n");
}

void RawPainter::openSpan(const ::WPXPropertyList &propList)
{
  printf("RawPainter::openSpan (%s)\n", getPropString(propList).cstr());
}

void RawPainter::closeSpan()
{
  printf("RawPainter::closeSpan\n");
}

void RawPainter::insertText(const ::WPXString &str)
{
  printf("RawPainter::insertText (%s)\n", str.cstr());
}

void RawPainter::insertTab()
{
  printf("RawPainter::insertTab\n");
}

void RawPainter::insertSpace()
{
  printf("RawPainter::insertSpace\n");
}

void RawPainter::insertLineBreak()
{
  printf("RawPainter::insertLineBreak\n");
}

void RawPainter::insertField(const WPXString &type, const ::WPXPropertyList &propList)
{
  printf("RawPainter::insertField(%s, %s)\n", type.cstr(), getPropString(propList).cstr());
}

void RawPainter::openOrderedListLevel(const ::WPXPropertyList &propList)
{
  printf("RawPainter::openOrderedListLevel(%s)\n", getPropString(propList).cstr());
}

void RawPainter::openUnorderedListLevel(const ::WPXPropertyList &propList)
{
  printf("RawPainter::openUnorderedListLevel(%s)\n", getPropString(propList).cstr());
}

void RawPainter::closeOrderedListLevel()
{
  printf("RawPainter::closeOrderedListLevel\n");
}

void RawPainter::closeUnorderedListLevel()
{
  printf("RawPainter::closeUnorderedListLevel\n");
}

void RawPainter::openListElement(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &tabStops)
{
  printf("RawPainter::openListElement(%s, tabStops: (%s))\n", getPropString(propList).cstr(), getPropString(tabStops).cstr());
}

void RawPainter::closeListElement()
{
  printf("RawPainter::closeListElement\n");
}

void RawPainter::openTable(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &columns)
{
  printf("RawPainter::openTable(%s, columns: (%s))\n", getPropString(propList).cstr(), getPropString(columns).cstr());
}

void RawPainter::openTableRow(const ::WPXPropertyList &propList)
{
  printf("RawPainter::openTableRow(%s)\n", getPropString(propList).cstr());
}

void RawPainter::closeTableRow()
{
  printf("RawPainter::closeTableRow\n");
}

void RawPainter::openTableCell(const ::WPXPropertyList &propList)
{
  printf("RawPainter::openTableCell(%s)\n", getPropString(propList).cstr());
}

void RawPainter::closeTableCell()
{
  printf("RawPainter::closeTableCell\n");
}

void RawPainter::insertCoveredTableCell(const ::WPXPropertyList &propList)
{
  printf("RawPainter::insertCoveredTableCell(%s)\n", getPropString(propList).cstr());
}

void RawPainter::closeTable()
{
  printf("RawPainter::closeTable\n");
}

void RawPainter::startComment(const ::WPXPropertyList &propList)
{
  printf("RawPainter::startComment(%s)\n", getPropString(propList).cstr());
}

void RawPainter::endComment()
{
  printf("RawPainter::endComment\n");
}

void RawPainter::startNotes(const ::WPXPropertyList &propList)
{
  printf("RawPainter::startNotes(%s)\n", getPropString(propList).cstr());
}

void RawPainter::endNotes()
{
  printf("RawPainter::endNotes\n");
}

namespace
{

int printUsage()
{
  printf("Usage: key2raw [OPTION] <KeyNote Document> | <KeyNote Directory>\n");
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

  libetonyek::KEYDocumentType type = libetonyek::KEY_DOCUMENT_TYPE_UNKNOWN;
  if (!libetonyek::KEYDocument::isSupported(input.get(), &type))
  {
    fprintf(stderr, "ERROR: Unsupported file format!\n");
    return 1;
  }

  if (libetonyek::KEY_DOCUMENT_TYPE_APXL_FILE == type)
  {
    path.remove_filename();
    input.reset(new conv::KEYDirectoryStream(path));
  }

  RawPainter painter;
  libetonyek::KEYDocument::parse(input.get(), &painter);

  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
