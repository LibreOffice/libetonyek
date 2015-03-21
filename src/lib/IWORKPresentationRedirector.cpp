/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKPresentationRedirector.h"

#include <cassert>

namespace libetonyek
{

IWORKPresentationRedirector::IWORKPresentationRedirector(librevenge::RVNGPresentationInterface *const iface)
  : m_iface(iface)
{
}

void IWORKPresentationRedirector::setDocumentMetaData(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->setDocumentMetaData(propList);
}

void IWORKPresentationRedirector::startDocument(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->startDocument(propList);
}

void IWORKPresentationRedirector::endDocument()
{
  if (m_iface)
    m_iface->endDocument();
}

void IWORKPresentationRedirector::definePageStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationRedirector::defineEmbeddedFont(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->defineEmbeddedFont(propList);
}

void IWORKPresentationRedirector::openPageSpan(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationRedirector::closePageSpan()
{
  assert(0);
}

void IWORKPresentationRedirector::startSlide(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->startSlide(propList);
}

void IWORKPresentationRedirector::endSlide()
{
  if (m_iface)
    m_iface->endSlide();
}

void IWORKPresentationRedirector::startMasterSlide(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->startMasterSlide(propList);
}

void IWORKPresentationRedirector::endMasterSlide()
{
  if (m_iface)
    m_iface->endMasterSlide();
}

void IWORKPresentationRedirector::setStyle(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->setStyle(propList);
}

void IWORKPresentationRedirector::startLayer(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->startLayer(propList);
}

void IWORKPresentationRedirector::endLayer()
{
  if (m_iface)
    m_iface->endLayer();
}

void IWORKPresentationRedirector::openHeader(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationRedirector::closeHeader()
{
  assert(0);
}

void IWORKPresentationRedirector::openFooter(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationRedirector::closeFooter()
{
  assert(0);
}

void IWORKPresentationRedirector::defineParagraphStyle(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->defineParagraphStyle(propList);
}

void IWORKPresentationRedirector::openParagraph(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openParagraph(propList);
}

void IWORKPresentationRedirector::closeParagraph()
{
  if (m_iface)
    m_iface->closeParagraph();
}

void IWORKPresentationRedirector::defineCharacterStyle(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->defineCharacterStyle(propList);
}

void IWORKPresentationRedirector::openSpan(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openSpan(propList);
}

void IWORKPresentationRedirector::closeSpan()
{
  if (m_iface)
    m_iface->closeSpan();
}

void IWORKPresentationRedirector::openLink(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openLink(propList);
}

void IWORKPresentationRedirector::closeLink()
{
  if (m_iface)
    m_iface->closeLink();
}

void IWORKPresentationRedirector::defineSectionStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationRedirector::openSection(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationRedirector::closeSection()
{
  assert(0);
}

void IWORKPresentationRedirector::insertTab()
{
  if (m_iface)
    m_iface->insertTab();
}

void IWORKPresentationRedirector::insertSpace()
{
  if (m_iface)
    m_iface->insertSpace();
}

void IWORKPresentationRedirector::insertText(const librevenge::RVNGString &text)
{
  if (m_iface)
    m_iface->insertText(text);
}

void IWORKPresentationRedirector::insertLineBreak()
{
  if (m_iface)
    m_iface->insertLineBreak();
}

void IWORKPresentationRedirector::insertField(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->insertField(propList);
}

void IWORKPresentationRedirector::openOrderedListLevel(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openOrderedListLevel(propList);
}

void IWORKPresentationRedirector::openUnorderedListLevel(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openUnorderedListLevel(propList);
}

void IWORKPresentationRedirector::closeOrderedListLevel()
{
  if (m_iface)
    m_iface->closeOrderedListLevel();
}

void IWORKPresentationRedirector::closeUnorderedListLevel()
{
  if (m_iface)
    m_iface->closeUnorderedListLevel();
}

void IWORKPresentationRedirector::openListElement(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openListElement(propList);
}

void IWORKPresentationRedirector::closeListElement()
{
  if (m_iface)
    m_iface->closeListElement();
}

void IWORKPresentationRedirector::openFootnote(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationRedirector::closeFootnote()
{
  assert(0);
}

void IWORKPresentationRedirector::openEndnote(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationRedirector::closeEndnote()
{
  assert(0);
}

void IWORKPresentationRedirector::openComment(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->startComment(propList);
}

void IWORKPresentationRedirector::closeComment()
{
  if (m_iface)
    m_iface->endComment();
}

void IWORKPresentationRedirector::openTextBox(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationRedirector::closeTextBox()
{
  assert(0);
}

void IWORKPresentationRedirector::defineSheetNumberingStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationRedirector::openTable(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->startTableObject(propList);
}

void IWORKPresentationRedirector::openTableRow(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openTableRow(propList);
}

void IWORKPresentationRedirector::closeTableRow()
{
  if (m_iface)
    m_iface->closeTableRow();
}

void IWORKPresentationRedirector::openTableCell(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openTableCell(propList);
}

void IWORKPresentationRedirector::closeTableCell()
{
  if (m_iface)
    m_iface->closeTableCell();
}

void IWORKPresentationRedirector::insertCoveredTableCell(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->insertCoveredTableCell(propList);
}

void IWORKPresentationRedirector::closeTable()
{
  if (m_iface)
    m_iface->endTableObject();
}

void IWORKPresentationRedirector::openFrame(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationRedirector::closeFrame()
{
  assert(0);
}
void IWORKPresentationRedirector::insertBinaryObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationRedirector::insertEquation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationRedirector::openGroup(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openGroup(propList);
}

void IWORKPresentationRedirector::closeGroup()
{
  if (m_iface)
    m_iface->closeGroup();
}

void IWORKPresentationRedirector::defineGraphicStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationRedirector::drawRectangle(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->drawRectangle(propList);
}

void IWORKPresentationRedirector::drawEllipse(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->drawEllipse(propList);
}

void IWORKPresentationRedirector::drawPolygon(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->drawPolygon(propList);
}

void IWORKPresentationRedirector::drawPolyline(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->drawPolyline(propList);
}

void IWORKPresentationRedirector::drawPath(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->drawPath(propList);
}

void IWORKPresentationRedirector::drawGraphicObject(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->drawGraphicObject(propList);
}

void IWORKPresentationRedirector::drawConnector(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->drawConnector(propList);
}

void IWORKPresentationRedirector::startTextObject(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->startTextObject(propList);
}

void IWORKPresentationRedirector::endTextObject()
{
  if (m_iface)
    m_iface->endTextObject();
}

void IWORKPresentationRedirector::startNotes(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->startNotes(propList);
}

void IWORKPresentationRedirector::endNotes()
{
  if (m_iface)
    m_iface->endNotes();
}

void IWORKPresentationRedirector::defineChartStyle(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->defineChartStyle(propList);
}

void IWORKPresentationRedirector::openChart(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openChart(propList);
}

void IWORKPresentationRedirector::closeChart()
{
  if (m_iface)
    m_iface->closeChart();
}

void IWORKPresentationRedirector::openChartTextObject(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openChartTextObject(propList);
}

void IWORKPresentationRedirector::closeChartTextObject()
{
  if (m_iface)
    m_iface->closeChartTextObject();
}

void IWORKPresentationRedirector::openChartPlotArea(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openChartPlotArea(propList);
}

void IWORKPresentationRedirector::closeChartPlotArea()
{
  if (m_iface)
    m_iface->closeChartPlotArea();
}

void IWORKPresentationRedirector::insertChartAxis(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->insertChartAxis(propList);
}

void IWORKPresentationRedirector::openChartSeries(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openChartSeries(propList);
}

void IWORKPresentationRedirector::closeChartSeries()
{
  if (m_iface)
    m_iface->closeChartSeries();
}

void IWORKPresentationRedirector::openAnimationSequence(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openAnimationSequence(propList);
}

void IWORKPresentationRedirector::closeAnimationSequence()
{
  if (m_iface)
    m_iface->closeAnimationSequence();
}

void IWORKPresentationRedirector::openAnimationGroup(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openAnimationGroup(propList);
}

void IWORKPresentationRedirector::closeAnimationGroup()
{
  if (m_iface)
    m_iface->closeAnimationGroup();
}

void IWORKPresentationRedirector::openAnimationIteration(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->openAnimationIteration(propList);
}

void IWORKPresentationRedirector::closeAnimationIteration()
{
  if (m_iface)
    m_iface->closeAnimationIteration();
}

void IWORKPresentationRedirector::insertMotionAnimation(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->insertMotionAnimation(propList);
}

void IWORKPresentationRedirector::insertColorAnimation(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->insertColorAnimation(propList);
}

void IWORKPresentationRedirector::insertAnimation(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->insertAnimation(propList);
}

void IWORKPresentationRedirector::insertEffect(const librevenge::RVNGPropertyList &propList)
{
  if (m_iface)
    m_iface->insertEffect(propList);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
