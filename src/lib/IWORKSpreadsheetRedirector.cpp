/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKSpreadsheetRedirector.h"

#include <cassert>

namespace libetonyek
{

IWORKSpreadsheetRedirector::IWORKSpreadsheetRedirector(librevenge::RVNGSpreadsheetInterface *const iface)
  : m_iface(iface)
{
}

void IWORKSpreadsheetRedirector::setDocumentMetaData(const librevenge::RVNGPropertyList &propList)
{
  m_iface->setDocumentMetaData(propList);
}

void IWORKSpreadsheetRedirector::startDocument(const librevenge::RVNGPropertyList &propList)
{
  m_iface->startDocument(propList);
}
void IWORKSpreadsheetRedirector::endDocument()
{
  m_iface->endDocument();
}

void IWORKSpreadsheetRedirector::definePageStyle(const librevenge::RVNGPropertyList &propList)
{
  m_iface->definePageStyle(propList);
}

void IWORKSpreadsheetRedirector::defineEmbeddedFont(const librevenge::RVNGPropertyList &propList)
{
  m_iface->defineEmbeddedFont(propList);
}

void IWORKSpreadsheetRedirector::openPageSpan(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openPageSpan(propList);
}
void IWORKSpreadsheetRedirector::closePageSpan()
{
  m_iface->closePageSpan();
}

void IWORKSpreadsheetRedirector::startSlide(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::endSlide()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::startMasterSlide(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::endMasterSlide()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::setStyle(const librevenge::RVNGPropertyList &propList)
{
  m_iface->defineGraphicStyle(propList);
}

void IWORKSpreadsheetRedirector::startLayer(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::endLayer()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openHeader(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openHeader(propList);
}
void IWORKSpreadsheetRedirector::closeHeader()
{
  m_iface->closeHeader();
}

void IWORKSpreadsheetRedirector::openFooter(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openFooter(propList);
}
void IWORKSpreadsheetRedirector::closeFooter()
{
  m_iface->closeFooter();
}

void IWORKSpreadsheetRedirector::defineParagraphStyle(const librevenge::RVNGPropertyList &propList)
{
  m_iface->defineParagraphStyle(propList);
}

void IWORKSpreadsheetRedirector::openParagraph(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openParagraph(propList);
}
void IWORKSpreadsheetRedirector::closeParagraph()
{
  m_iface->closeParagraph();
}

void IWORKSpreadsheetRedirector::defineCharacterStyle(const librevenge::RVNGPropertyList &propList)
{
  m_iface->defineCharacterStyle(propList);
}

void IWORKSpreadsheetRedirector::openSpan(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openSpan(propList);
}
void IWORKSpreadsheetRedirector::closeSpan()
{
  m_iface->closeSpan();
}

void IWORKSpreadsheetRedirector::openLink(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openLink(propList);
}
void IWORKSpreadsheetRedirector::closeLink()
{
  m_iface->closeLink();
}

void IWORKSpreadsheetRedirector::defineSectionStyle(const librevenge::RVNGPropertyList &propList)
{
  m_iface->defineSectionStyle(propList);
}

void IWORKSpreadsheetRedirector::openSection(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openSection(propList);
}
void IWORKSpreadsheetRedirector::closeSection()
{
  m_iface->closeSection();
}

void IWORKSpreadsheetRedirector::insertTab()
{
  m_iface->insertTab();
}
void IWORKSpreadsheetRedirector::insertSpace()
{
  m_iface->insertSpace();
}
void IWORKSpreadsheetRedirector::insertText(const librevenge::RVNGString &text)
{
  m_iface->insertText(text);
}
void IWORKSpreadsheetRedirector::insertLineBreak()
{
  m_iface->insertLineBreak();
}

void IWORKSpreadsheetRedirector::insertField(const librevenge::RVNGPropertyList &propList)
{
  m_iface->insertField(propList);
}

void IWORKSpreadsheetRedirector::openOrderedListLevel(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openOrderedListLevel(propList);
}
void IWORKSpreadsheetRedirector::openUnorderedListLevel(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openUnorderedListLevel(propList);
}
void IWORKSpreadsheetRedirector::closeOrderedListLevel()
{
  m_iface->closeOrderedListLevel();
}
void IWORKSpreadsheetRedirector::closeUnorderedListLevel()
{
  m_iface->closeUnorderedListLevel();
}
void IWORKSpreadsheetRedirector::openListElement(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openListElement(propList);
}
void IWORKSpreadsheetRedirector::closeListElement()
{
  m_iface->closeListElement();
}

void IWORKSpreadsheetRedirector::openFootnote(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openFootnote(propList);
}
void IWORKSpreadsheetRedirector::closeFootnote()
{
  m_iface->closeFootnote();
}

void IWORKSpreadsheetRedirector::openEndnote(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeEndnote()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openComment(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openComment(propList);
}
void IWORKSpreadsheetRedirector::closeComment()
{
  m_iface->closeComment();
}

void IWORKSpreadsheetRedirector::openTextBox(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openTextBox(propList);
}
void IWORKSpreadsheetRedirector::closeTextBox()
{
  m_iface->closeTextBox();
}

void IWORKSpreadsheetRedirector::defineSheetNumberingStyle(const librevenge::RVNGPropertyList &propList)
{
  m_iface->defineSheetNumberingStyle(propList);
}

void IWORKSpreadsheetRedirector::openTable(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openSheet(propList);
}
void IWORKSpreadsheetRedirector::openTableRow(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openSheetRow(propList);
}
void IWORKSpreadsheetRedirector::closeTableRow()
{
  m_iface->closeSheetRow();
}
void IWORKSpreadsheetRedirector::openTableCell(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openSheetCell(propList);
}
void IWORKSpreadsheetRedirector::closeTableCell()
{
  m_iface->closeSheetCell();
}
void IWORKSpreadsheetRedirector::insertCoveredTableCell(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeTable()
{
  m_iface->closeSheet();
}
void IWORKSpreadsheetRedirector::openFrame(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openFrame(propList);
}
void IWORKSpreadsheetRedirector::closeFrame()
{
  m_iface->closeFrame();
}
void IWORKSpreadsheetRedirector::insertBinaryObject(const librevenge::RVNGPropertyList &propList)
{
  m_iface->insertBinaryObject(propList);
}
void IWORKSpreadsheetRedirector::insertEquation(const librevenge::RVNGPropertyList &propList)
{
  m_iface->insertEquation(propList);
}

void IWORKSpreadsheetRedirector::openGroup(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openGroup(propList);
}
void IWORKSpreadsheetRedirector::closeGroup()
{
  m_iface->closeGroup();
}

void IWORKSpreadsheetRedirector::defineGraphicStyle(const librevenge::RVNGPropertyList &propList)
{
  m_iface->defineGraphicStyle(propList);
}

void IWORKSpreadsheetRedirector::drawRectangle(const librevenge::RVNGPropertyList &propList)
{
  m_iface->drawRectangle(propList);
}
void IWORKSpreadsheetRedirector::drawEllipse(const librevenge::RVNGPropertyList &propList)
{
  m_iface->drawEllipse(propList);
}
void IWORKSpreadsheetRedirector::drawPolygon(const librevenge::RVNGPropertyList &propList)
{
  m_iface->drawPolygon(propList);
}
void IWORKSpreadsheetRedirector::drawPolyline(const librevenge::RVNGPropertyList &propList)
{
  m_iface->drawPolyline(propList);
}
void IWORKSpreadsheetRedirector::drawPath(const librevenge::RVNGPropertyList &propList)
{
  m_iface->drawPath(propList);
}

void IWORKSpreadsheetRedirector::drawGraphicObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetRedirector::drawConnector(const librevenge::RVNGPropertyList &propList)
{
  m_iface->drawConnector(propList);
}

void IWORKSpreadsheetRedirector::startTextObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::endTextObject()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::startNotes(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::endNotes()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::defineChartStyle(const librevenge::RVNGPropertyList &propList)
{
  m_iface->defineChartStyle(propList);
}

void IWORKSpreadsheetRedirector::openChart(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openChart(propList);
}
void IWORKSpreadsheetRedirector::closeChart()
{
  m_iface->closeChart();
}

void IWORKSpreadsheetRedirector::openChartTextObject(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openChartTextObject(propList);
}
void IWORKSpreadsheetRedirector::closeChartTextObject()
{
  m_iface->closeChartTextObject();
}

void IWORKSpreadsheetRedirector::openChartPlotArea(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openChartPlotArea(propList);
}
void IWORKSpreadsheetRedirector::closeChartPlotArea()
{
  m_iface->closeChartPlotArea();
}
void IWORKSpreadsheetRedirector::insertChartAxis(const librevenge::RVNGPropertyList &propList)
{
  m_iface->insertChartAxis(propList);
}
void IWORKSpreadsheetRedirector::openChartSeries(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeChartSeries()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openAnimationSequence(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeAnimationSequence()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openAnimationGroup(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeAnimationGroup()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openAnimationIteration(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeAnimationIteration()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::insertMotionAnimation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::insertColorAnimation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::insertAnimation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::insertEffect(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
