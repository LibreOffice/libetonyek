/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNSVGGENERATOR_H_INCLUDED
#define KNSVGGENERATOR_H_INCLUDED

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <libwpd/libwpd.h>
#include <libwpg/libwpg.h>
#include <libkeynote/libkeynote.h>

namespace libkeynote
{

class KNSVGGenerator : public libwpg::WPGPaintInterface
{
public:
  KNSVGGenerator(KNStringVector &vec);
  ~KNSVGGenerator();

  void startGraphics(const ::WPXPropertyList &propList);
  void endGraphics();
  void startLayer(const ::WPXPropertyList &propList);
  void endLayer();
  void startEmbeddedGraphics(const ::WPXPropertyList & /*propList*/) {}
  void endEmbeddedGraphics() {}

  void setStyle(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &gradient);

  void drawRectangle(const ::WPXPropertyList &propList);
  void drawEllipse(const ::WPXPropertyList &propList);
  void drawPolyline(const ::WPXPropertyListVector &vertices);
  void drawPolygon(const ::WPXPropertyListVector &vertices);
  void drawPath(const ::WPXPropertyListVector &path);
  void drawGraphicObject(const ::WPXPropertyList &propList, const ::WPXBinaryData &binaryData);
  void startTextObject(const ::WPXPropertyList &propList, const ::WPXPropertyListVector &path);
  void endTextObject();
  void startTextLine(const ::WPXPropertyList & /* propList */) {}
  void endTextLine() {}
  void startTextSpan(const ::WPXPropertyList &propList);
  void endTextSpan();
  void insertText(const ::WPXString &str);

private:
  ::WPXPropertyListVector m_gradient;
  ::WPXPropertyList m_style;
  int m_gradientIndex;
  int m_patternIndex;
  int m_shadowIndex;
  void writeStyle(bool isClosed=true);
  void drawPolySomething(const ::WPXPropertyListVector &vertices, bool isClosed);

  std::ostringstream m_outputSink;
  KNStringVector &m_vec;
};

} // namespace libkeynote

#endif // KNSVGGENERATOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
