/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKPresentationInterfaceRedirector.h"

#include <cassert>

namespace libetonyek
{

IWORKPresentationInterfaceRedirector::IWORKPresentationInterfaceRedirector(librevenge::RVNGPresentationInterface *const iface)
  : m_iface(iface)
{
}

void IWORKPresentationInterfaceRedirector::setDocumentMetaData(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->setDocumentMetaData(propList);
}

void IWORKPresentationInterfaceRedirector::startDocument(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->startDocument(propList);
}

void IWORKPresentationInterfaceRedirector::endDocument()
{
  if (m_iface)
    m_iface->endDocument();
}

void IWORKPresentationInterfaceRedirector::definePageStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::defineEmbeddedFont(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->defineEmbeddedFont(propList);
}

void IWORKPresentationInterfaceRedirector::openPageSpan(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closePageSpan()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::startSlide(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->startSlide(propList);
}

void IWORKPresentationInterfaceRedirector::endSlide()
{
  if (m_iface)
    m_iface->endSlide();
}

void IWORKPresentationInterfaceRedirector::startMasterSlide(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->startMasterSlide(propList);
}

void IWORKPresentationInterfaceRedirector::endMasterSlide()
{
  if (m_iface)
    m_iface->endMasterSlide();
}

void IWORKPresentationInterfaceRedirector::setStyle(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->setStyle(propList);
}

void IWORKPresentationInterfaceRedirector::startLayer(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->startLayer(propList);
}

void IWORKPresentationInterfaceRedirector::endLayer()
{
  if (m_iface)
    m_iface->endLayer();
}

void IWORKPresentationInterfaceRedirector::openHeader(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeHeader()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::openFooter(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeFooter()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::defineParagraphStyle(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->defineParagraphStyle(propList);
}

void IWORKPresentationInterfaceRedirector::openParagraph(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openParagraph(propList);
}

void IWORKPresentationInterfaceRedirector::closeParagraph()
{
  if (m_iface)
    m_iface->closeParagraph();
}

void IWORKPresentationInterfaceRedirector::defineCharacterStyle(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->defineCharacterStyle(propList);
}

void IWORKPresentationInterfaceRedirector::openSpan(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openSpan(propList);
}

void IWORKPresentationInterfaceRedirector::closeSpan()
{
  if (m_iface)
    m_iface->closeSpan();
}

void IWORKPresentationInterfaceRedirector::openLink(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openLink(propList);
}

void IWORKPresentationInterfaceRedirector::closeLink()
{
  if (m_iface)
    m_iface->closeLink();
}

void IWORKPresentationInterfaceRedirector::defineSectionStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::openSection(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeSection()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::insertTab()
{
  if (m_iface)
    m_iface->insertTab();
}

void IWORKPresentationInterfaceRedirector::insertSpace()
{
  if (m_iface)
    m_iface->insertSpace();
}

void IWORKPresentationInterfaceRedirector::insertText(const librevenge::RVNGString &text)
{
  if (m_iface)
    m_iface->insertText(text);
}

void IWORKPresentationInterfaceRedirector::insertLineBreak()
{
  if (m_iface)
    m_iface->insertLineBreak();
}

void IWORKPresentationInterfaceRedirector::insertField(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->insertField(propList);
}

void IWORKPresentationInterfaceRedirector::openOrderedListLevel(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openOrderedListLevel(propList);
}

void IWORKPresentationInterfaceRedirector::openUnorderedListLevel(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openUnorderedListLevel(propList);
}

void IWORKPresentationInterfaceRedirector::closeOrderedListLevel()
{
  if (m_iface)
    m_iface->closeOrderedListLevel();
}

void IWORKPresentationInterfaceRedirector::closeUnorderedListLevel()
{
  if (m_iface)
    m_iface->closeUnorderedListLevel();
}

void IWORKPresentationInterfaceRedirector::openListElement(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openListElement(propList);
}

void IWORKPresentationInterfaceRedirector::closeListElement()
{
  if (m_iface)
    m_iface->closeListElement();
}

void IWORKPresentationInterfaceRedirector::openFootnote(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeFootnote()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::openEndnote(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeEndnote()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::openComment(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->startComment(propList);
}

void IWORKPresentationInterfaceRedirector::closeComment()
{
  if (m_iface)
    m_iface->endComment();
}

void IWORKPresentationInterfaceRedirector::openTextBox(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeTextBox()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::defineSheetNumberingStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::openTable(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->startTableObject(propList);
}

void IWORKPresentationInterfaceRedirector::openTableRow(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openTableRow(propList);
}

void IWORKPresentationInterfaceRedirector::closeTableRow()
{
  if (m_iface)
    m_iface->closeTableRow();
}

void IWORKPresentationInterfaceRedirector::openTableCell(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openTableCell(propList);
}

void IWORKPresentationInterfaceRedirector::closeTableCell()
{
  if (m_iface)
    m_iface->closeTableCell();
}

void IWORKPresentationInterfaceRedirector::insertCoveredTableCell(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->insertCoveredTableCell(propList);
}

void IWORKPresentationInterfaceRedirector::closeTable()
{
  if (m_iface)
    m_iface->endTableObject();
}

void IWORKPresentationInterfaceRedirector::openFrame(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeFrame()
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::insertBinaryObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::insertEquation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::openGroup(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openGroup(propList);
}

void IWORKPresentationInterfaceRedirector::closeGroup()
{
  if (m_iface)
    m_iface->closeGroup();
}

void IWORKPresentationInterfaceRedirector::defineGraphicStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::drawRectangle(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->drawRectangle(propList);
}

void IWORKPresentationInterfaceRedirector::drawEllipse(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->drawEllipse(propList);
}

void IWORKPresentationInterfaceRedirector::drawPolygon(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->drawPolygon(propList);
}

void IWORKPresentationInterfaceRedirector::drawPolyline(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->drawPolyline(propList);
}

void IWORKPresentationInterfaceRedirector::drawPath(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->drawPath(propList);
}

void IWORKPresentationInterfaceRedirector::drawGraphicObject(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->drawGraphicObject(propList);
}

void IWORKPresentationInterfaceRedirector::drawConnector(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->drawConnector(propList);
}

void IWORKPresentationInterfaceRedirector::startTextObject(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->startTextObject(propList);
}

void IWORKPresentationInterfaceRedirector::endTextObject()
{
  if (m_iface)
    m_iface->endTextObject();
}

void IWORKPresentationInterfaceRedirector::startNotes(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->startNotes(propList);
}

void IWORKPresentationInterfaceRedirector::endNotes()
{
  if (m_iface)
    m_iface->endNotes();
}

void IWORKPresentationInterfaceRedirector::defineChartStyle(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->defineChartStyle(propList);
}

void IWORKPresentationInterfaceRedirector::openChart(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openChart(propList);
}

void IWORKPresentationInterfaceRedirector::closeChart()
{
  if (m_iface)
    m_iface->closeChart();
}

void IWORKPresentationInterfaceRedirector::openChartTextObject(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openChartTextObject(propList);
}

void IWORKPresentationInterfaceRedirector::closeChartTextObject()
{
  if (m_iface)
    m_iface->closeChartTextObject();
}

void IWORKPresentationInterfaceRedirector::openChartPlotArea(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openChartPlotArea(propList);
}

void IWORKPresentationInterfaceRedirector::closeChartPlotArea()
{
  if (m_iface)
    m_iface->closeChartPlotArea();
}

void IWORKPresentationInterfaceRedirector::insertChartAxis(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->insertChartAxis(propList);
}

void IWORKPresentationInterfaceRedirector::openChartSeries(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openChartSeries(propList);
}

void IWORKPresentationInterfaceRedirector::closeChartSeries()
{
  if (m_iface)
    m_iface->closeChartSeries();
}

void IWORKPresentationInterfaceRedirector::openAnimationSequence(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openAnimationSequence(propList);
}

void IWORKPresentationInterfaceRedirector::closeAnimationSequence()
{
  if (m_iface)
    m_iface->closeAnimationSequence();
}

void IWORKPresentationInterfaceRedirector::openAnimationGroup(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openAnimationGroup(propList);
}

void IWORKPresentationInterfaceRedirector::closeAnimationGroup()
{
  if (m_iface)
    m_iface->closeAnimationGroup();
}

void IWORKPresentationInterfaceRedirector::openAnimationIteration(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openAnimationIteration(propList);
}

void IWORKPresentationInterfaceRedirector::closeAnimationIteration()
{
  if (m_iface)
    m_iface->closeAnimationIteration();
}

void IWORKPresentationInterfaceRedirector::insertMotionAnimation(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->insertMotionAnimation(propList);
}

void IWORKPresentationInterfaceRedirector::insertColorAnimation(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->insertColorAnimation(propList);
}

void IWORKPresentationInterfaceRedirector::insertAnimation(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->insertAnimation(propList);
}

void IWORKPresentationInterfaceRedirector::insertEffect(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->insertEffect(propList);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
