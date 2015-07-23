/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKDOCUMENTINTERFACE_H_INCLUDED
#define IWORKDOCUMENTINTERFACE_H_INCLUDED

#include <librevenge/librevenge.h>

namespace libetonyek
{

class IWORKDocumentInterface
{
public:
  virtual ~IWORKDocumentInterface() = 0;

  virtual void setDocumentMetaData(const librevenge::RVNGPropertyList &propList) = 0;

  virtual void startDocument(const librevenge::RVNGPropertyList &propList) = 0;

  virtual void endDocument() = 0;

  virtual void definePageStyle(const librevenge::RVNGPropertyList &propList) = 0;

  virtual void defineEmbeddedFont(const librevenge::RVNGPropertyList &propList) = 0;

  virtual void openPageSpan(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closePageSpan() = 0;

  virtual void startSlide(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void endSlide() = 0;

  virtual void startMasterSlide(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void endMasterSlide() = 0;

  virtual void setStyle(const librevenge::RVNGPropertyList &propList) = 0;

  virtual void startLayer(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void endLayer() = 0;

  virtual void openHeader(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeHeader() = 0;

  virtual void openFooter(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeFooter() = 0;

  virtual void defineParagraphStyle(const librevenge::RVNGPropertyList &propList) = 0;

  virtual void openParagraph(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeParagraph() = 0;

  virtual void defineCharacterStyle(const librevenge::RVNGPropertyList &propList) = 0;

  virtual void openSpan(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeSpan() = 0;

  virtual void openLink(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeLink() = 0;

  virtual void defineSectionStyle(const librevenge::RVNGPropertyList &propList) = 0;

  virtual void openSection(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeSection() = 0;

  virtual void insertTab() = 0;
  virtual void insertSpace() = 0;
  virtual void insertText(const librevenge::RVNGString &text) = 0;
  virtual void insertLineBreak() = 0;

  virtual void insertField(const librevenge::RVNGPropertyList &propList) = 0;

  virtual void openOrderedListLevel(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void openUnorderedListLevel(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeOrderedListLevel() = 0;
  virtual void closeUnorderedListLevel() = 0;
  virtual void openListElement(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeListElement() = 0;

  virtual void openFootnote(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeFootnote() = 0;

  virtual void openEndnote(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeEndnote() = 0;

  virtual void openComment(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeComment() = 0;

  virtual void openTextBox(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeTextBox() = 0;

  virtual void defineSheetNumberingStyle(const librevenge::RVNGPropertyList &propList) = 0;

  virtual void openTable(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void openTableRow(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeTableRow() = 0;
  virtual void openTableCell(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeTableCell() = 0;
  virtual void insertCoveredTableCell(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeTable() = 0;
  virtual void openFrame(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeFrame() = 0;
  virtual void insertBinaryObject(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void insertEquation(const librevenge::RVNGPropertyList &propList) = 0;

  virtual void openGroup(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeGroup() = 0;

  virtual void defineGraphicStyle(const librevenge::RVNGPropertyList &propList) = 0;

  virtual void drawRectangle(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void drawEllipse(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void drawPolygon(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void drawPolyline(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void drawPath(const librevenge::RVNGPropertyList &propList) = 0;

  virtual void drawGraphicObject(const librevenge::RVNGPropertyList &propList) = 0;

  virtual void drawConnector(const librevenge::RVNGPropertyList &propList) = 0;

  virtual void startTextObject(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void endTextObject() = 0;

  virtual void startNotes(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void endNotes() = 0;

  virtual void defineChartStyle(const librevenge::RVNGPropertyList &propList) = 0;

  virtual void openChart(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeChart() = 0;

  virtual void openChartTextObject(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeChartTextObject() = 0;

  virtual void openChartPlotArea(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeChartPlotArea() = 0;
  virtual void insertChartAxis(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void openChartSeries(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeChartSeries() = 0;

  virtual void openAnimationSequence(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeAnimationSequence() = 0;

  virtual void openAnimationGroup(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeAnimationGroup() = 0;

  virtual void openAnimationIteration(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void closeAnimationIteration() = 0;

  virtual void insertMotionAnimation(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void insertColorAnimation(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void insertAnimation(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void insertEffect(const librevenge::RVNGPropertyList &propList) = 0;

  virtual void startGraphic(const librevenge::RVNGPropertyList &propList) = 0;
  virtual void endGraphic() = 0;
};

}

#endif // IWORKDOCUMENTINTERFACE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
