/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKOUTPUTELEMENTSREDIRECTOR_H_INCLUDED
#define IWORKOUTPUTELEMENTSREDIRECTOR_H_INCLUDED

#include "IWORKDocumentInterface.h"

namespace libetonyek
{

class IWORKOutputElements;

class IWORKOutputElementsRedirector : public IWORKDocumentInterface
{
public:
  explicit IWORKOutputElementsRedirector(IWORKOutputElements &elements);

  virtual void setDocumentMetaData(const librevenge::RVNGPropertyList &propList);

  virtual void startDocument(const librevenge::RVNGPropertyList &propList);
  virtual void endDocument();

  virtual void definePageStyle(const librevenge::RVNGPropertyList &propList);

  virtual void defineEmbeddedFont(const librevenge::RVNGPropertyList &propList);

  virtual void openPageSpan(const librevenge::RVNGPropertyList &propList);
  virtual void closePageSpan();

  virtual void startSlide(const librevenge::RVNGPropertyList &propList);
  virtual void endSlide();

  virtual void startMasterSlide(const librevenge::RVNGPropertyList &propList);
  virtual void endMasterSlide();

  virtual void setStyle(const librevenge::RVNGPropertyList &propList);

  virtual void startLayer(const librevenge::RVNGPropertyList &propList);
  virtual void endLayer();

  virtual void openHeader(const librevenge::RVNGPropertyList &propList);
  virtual void closeHeader();

  virtual void openFooter(const librevenge::RVNGPropertyList &propList);
  virtual void closeFooter();

  virtual void defineParagraphStyle(const librevenge::RVNGPropertyList &propList);

  virtual void openParagraph(const librevenge::RVNGPropertyList &propList);
  virtual void closeParagraph();

  virtual void defineCharacterStyle(const librevenge::RVNGPropertyList &propList);

  virtual void openSpan(const librevenge::RVNGPropertyList &propList);
  virtual void closeSpan();

  virtual void openLink(const librevenge::RVNGPropertyList &propList);
  virtual void closeLink();

  virtual void defineSectionStyle(const librevenge::RVNGPropertyList &propList);

  virtual void openSection(const librevenge::RVNGPropertyList &propList);
  virtual void closeSection();

  virtual void insertTab();
  virtual void insertSpace();
  virtual void insertText(const librevenge::RVNGString &text);
  virtual void insertLineBreak();

  virtual void insertField(const librevenge::RVNGPropertyList &propList);

  virtual void openOrderedListLevel(const librevenge::RVNGPropertyList &propList);
  virtual void openUnorderedListLevel(const librevenge::RVNGPropertyList &propList);
  virtual void closeOrderedListLevel();
  virtual void closeUnorderedListLevel();
  virtual void openListElement(const librevenge::RVNGPropertyList &propList);
  virtual void closeListElement();

  virtual void openFootnote(const librevenge::RVNGPropertyList &propList);
  virtual void closeFootnote();

  virtual void openEndnote(const librevenge::RVNGPropertyList &propList);
  virtual void closeEndnote();

  virtual void openComment(const librevenge::RVNGPropertyList &propList);
  virtual void closeComment();

  virtual void openTextBox(const librevenge::RVNGPropertyList &propList);
  virtual void closeTextBox();

  virtual void defineSheetNumberingStyle(const librevenge::RVNGPropertyList &propList);

  virtual void openTable(const librevenge::RVNGPropertyList &propList);
  virtual void openTableRow(const librevenge::RVNGPropertyList &propList);
  virtual void closeTableRow();
  virtual void openTableCell(const librevenge::RVNGPropertyList &propList);
  virtual void closeTableCell();
  virtual void insertCoveredTableCell(const librevenge::RVNGPropertyList &propList);
  virtual void closeTable();
  virtual void openFrame(const librevenge::RVNGPropertyList &propList);
  virtual void closeFrame();
  virtual void insertBinaryObject(const librevenge::RVNGPropertyList &propList);
  virtual void insertEquation(const librevenge::RVNGPropertyList &propList);

  virtual void openGroup(const librevenge::RVNGPropertyList &propList);
  virtual void closeGroup();

  virtual void defineGraphicStyle(const librevenge::RVNGPropertyList &propList);

  virtual void drawRectangle(const librevenge::RVNGPropertyList &propList);
  virtual void drawEllipse(const librevenge::RVNGPropertyList &propList);
  virtual void drawPolygon(const librevenge::RVNGPropertyList &propList);
  virtual void drawPolyline(const librevenge::RVNGPropertyList &propList);
  virtual void drawPath(const librevenge::RVNGPropertyList &propList);

  virtual void drawGraphicObject(const librevenge::RVNGPropertyList &propList);

  virtual void drawConnector(const librevenge::RVNGPropertyList &propList);

  virtual void startTextObject(const librevenge::RVNGPropertyList &propList);
  virtual void endTextObject();

  virtual void startNotes(const librevenge::RVNGPropertyList &propList);
  virtual void endNotes();

  virtual void defineChartStyle(const librevenge::RVNGPropertyList &propList);

  virtual void openChart(const librevenge::RVNGPropertyList &propList);
  virtual void closeChart();

  virtual void openChartTextObject(const librevenge::RVNGPropertyList &propList);
  virtual void closeChartTextObject();

  virtual void openChartPlotArea(const librevenge::RVNGPropertyList &propList);
  virtual void closeChartPlotArea();
  virtual void insertChartAxis(const librevenge::RVNGPropertyList &propList);
  virtual void openChartSeries(const librevenge::RVNGPropertyList &propList);
  virtual void closeChartSeries();

  virtual void openAnimationSequence(const librevenge::RVNGPropertyList &propList);
  virtual void closeAnimationSequence();

  virtual void openAnimationGroup(const librevenge::RVNGPropertyList &propList);
  virtual void closeAnimationGroup();

  virtual void openAnimationIteration(const librevenge::RVNGPropertyList &propList);
  virtual void closeAnimationIteration();

  virtual void insertMotionAnimation(const librevenge::RVNGPropertyList &propList);
  virtual void insertColorAnimation(const librevenge::RVNGPropertyList &propList);
  virtual void insertAnimation(const librevenge::RVNGPropertyList &propList);
  virtual void insertEffect(const librevenge::RVNGPropertyList &propList);

private:
  IWORKOutputElements &m_elements;
};

}

#endif // IWORKOUTPUTELEMENTSREDIRECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
