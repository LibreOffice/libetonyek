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
#include <libwpg/libwpg.h>
#include <libkeynote/libkeynote.h>

#include "KEYDirectoryStream.h"

class TextPainter : public libwpg::WPGPaintInterface
{
public:
  TextPainter();

  void startGraphics(const ::WPXPropertyList &propList);
  void endGraphics();
  void startLayer(const ::WPXPropertyList &propList);
  void endLayer();
  void startEmbeddedGraphics(const ::WPXPropertyList &propList);
  void endEmbeddedGraphics();

  void setStyle(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &gradient);

  void drawRectangle(const ::WPXPropertyList &propList);
  void drawEllipse(const ::WPXPropertyList &propList);
  void drawPolyline(const ::WPXPropertyListVector &vertices);
  void drawPolygon(const ::WPXPropertyListVector &vertices);
  void drawPath(const ::WPXPropertyListVector &path);
  void drawGraphicObject(const ::WPXPropertyList &propList, const ::WPXBinaryData &binaryData);
  void startTextObject(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &path);
  void endTextObject();
  void startTextLine(const ::WPXPropertyList &propList);
  void endTextLine();
  void startTextSpan(const ::WPXPropertyList &propList);
  void endTextSpan();
  void insertText(const ::WPXString &str);
};

TextPainter::TextPainter()
{
}

void TextPainter::startGraphics(const ::WPXPropertyList &)
{
}

void TextPainter::endGraphics()
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

void TextPainter::startTextObject(const ::WPXPropertyList &, const ::WPXPropertyListVector &)
{
}

void TextPainter::endTextObject()
{
  printf("\n");
}

void TextPainter::startTextLine(const ::WPXPropertyList &)
{
}

void TextPainter::endTextLine()
{
  printf("\n");
}

void TextPainter::startTextSpan(const ::WPXPropertyList &)
{
}

void TextPainter::endTextSpan()
{
}

void TextPainter::insertText(const ::WPXString &str)
{
  printf(str.cstr());
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
