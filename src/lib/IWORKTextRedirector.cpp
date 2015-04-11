/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTextRedirector.h"

#include <cassert>

namespace libetonyek
{

IWORKTextRedirector::IWORKTextRedirector(librevenge::RVNGTextInterface *const iface)
  : m_iface(iface)
{
}

void IWORKTextRedirector::setDocumentMetaData(const librevenge::RVNGPropertyList &propList)
{
  m_iface->setDocumentMetaData(propList);
}

void IWORKTextRedirector::startDocument(const librevenge::RVNGPropertyList &propList)
{
  m_iface->startDocument(propList);
}
void IWORKTextRedirector::endDocument()
{
  m_iface->endDocument();
}

void IWORKTextRedirector::definePageStyle(const librevenge::RVNGPropertyList &propList)
{
  m_iface->definePageStyle(propList);
}

void IWORKTextRedirector::defineEmbeddedFont(const librevenge::RVNGPropertyList &propList)
{
  m_iface->defineEmbeddedFont(propList);
}

void IWORKTextRedirector::openPageSpan(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openPageSpan(propList);
}
void IWORKTextRedirector::closePageSpan()
{
  m_iface->closePageSpan();
}

void IWORKTextRedirector::startSlide(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::endSlide()
{
  assert(0);
}

void IWORKTextRedirector::startMasterSlide(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::endMasterSlide()
{
  assert(0);
}

void IWORKTextRedirector::setStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextRedirector::startLayer(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::endLayer()
{
  assert(0);
}

void IWORKTextRedirector::openHeader(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openHeader(propList);
}
void IWORKTextRedirector::closeHeader()
{
  m_iface->closeHeader();
}

void IWORKTextRedirector::openFooter(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openFooter(propList);
}
void IWORKTextRedirector::closeFooter()
{
  m_iface->closeFooter();
}

void IWORKTextRedirector::defineParagraphStyle(const librevenge::RVNGPropertyList &propList)
{
  m_iface->defineParagraphStyle(propList);
}

void IWORKTextRedirector::openParagraph(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openParagraph(propList);
}
void IWORKTextRedirector::closeParagraph()
{
  m_iface->closeParagraph();
}

void IWORKTextRedirector::defineCharacterStyle(const librevenge::RVNGPropertyList &propList)
{
  m_iface->defineCharacterStyle(propList);
}

void IWORKTextRedirector::openSpan(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openSpan(propList);
}
void IWORKTextRedirector::closeSpan()
{
  m_iface->closeSpan();
}

void IWORKTextRedirector::openLink(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openLink(propList);
}
void IWORKTextRedirector::closeLink()
{
  m_iface->closeLink();
}

void IWORKTextRedirector::defineSectionStyle(const librevenge::RVNGPropertyList &propList)
{
  m_iface->defineSectionStyle(propList);
}

void IWORKTextRedirector::openSection(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openSection(propList);
}
void IWORKTextRedirector::closeSection()
{
  m_iface->closeSection();
}

void IWORKTextRedirector::insertTab()
{
  m_iface->insertTab();
}
void IWORKTextRedirector::insertSpace()
{
  m_iface->insertSpace();
}
void IWORKTextRedirector::insertText(const librevenge::RVNGString &text)
{
  m_iface->insertText(text);
}
void IWORKTextRedirector::insertLineBreak()
{
  m_iface->insertLineBreak();
}

void IWORKTextRedirector::insertField(const librevenge::RVNGPropertyList &propList)
{
  m_iface->insertField(propList);
}

void IWORKTextRedirector::openOrderedListLevel(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openOrderedListLevel(propList);
}
void IWORKTextRedirector::openUnorderedListLevel(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openUnorderedListLevel(propList);
}
void IWORKTextRedirector::closeOrderedListLevel()
{
  m_iface->closeOrderedListLevel();
}
void IWORKTextRedirector::closeUnorderedListLevel()
{
  m_iface->closeUnorderedListLevel();
}
void IWORKTextRedirector::openListElement(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openListElement(propList);
}
void IWORKTextRedirector::closeListElement()
{
  m_iface->closeListElement();
}

void IWORKTextRedirector::openFootnote(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openFootnote(propList);
}
void IWORKTextRedirector::closeFootnote()
{
  m_iface->closeFootnote();
}

void IWORKTextRedirector::openEndnote(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openEndnote(propList);
}
void IWORKTextRedirector::closeEndnote()
{
  m_iface->closeEndnote();
}

void IWORKTextRedirector::openComment(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openComment(propList);
}
void IWORKTextRedirector::closeComment()
{
  m_iface->closeComment();
}

void IWORKTextRedirector::openTextBox(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openTextBox(propList);
}
void IWORKTextRedirector::closeTextBox()
{
  m_iface->closeTextBox();
}

void IWORKTextRedirector::defineSheetNumberingStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextRedirector::openTable(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openTable(propList);
}
void IWORKTextRedirector::openTableRow(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openTableRow(propList);
}
void IWORKTextRedirector::closeTableRow()
{
  m_iface->closeTableRow();
}
void IWORKTextRedirector::openTableCell(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openTableCell(propList);
}
void IWORKTextRedirector::closeTableCell()
{
  m_iface->closeTableCell();
}
void IWORKTextRedirector::insertCoveredTableCell(const librevenge::RVNGPropertyList &propList)
{
  m_iface->insertCoveredTableCell(propList);
}
void IWORKTextRedirector::closeTable()
{
  m_iface->closeTable();
}
void IWORKTextRedirector::openFrame(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openFrame(propList);
}
void IWORKTextRedirector::closeFrame()
{
  m_iface->closeFrame();
}
void IWORKTextRedirector::insertBinaryObject(const librevenge::RVNGPropertyList &propList)
{
  m_iface->insertBinaryObject(propList);
}
void IWORKTextRedirector::insertEquation(const librevenge::RVNGPropertyList &propList)
{
  m_iface->insertEquation(propList);
}

void IWORKTextRedirector::openGroup(const librevenge::RVNGPropertyList &propList)
{
  m_iface->openGroup(propList);
}
void IWORKTextRedirector::closeGroup()
{
  m_iface->closeGroup();
}

void IWORKTextRedirector::defineGraphicStyle(const librevenge::RVNGPropertyList &propList)
{
  m_iface->defineGraphicStyle(propList);
}

void IWORKTextRedirector::drawRectangle(const librevenge::RVNGPropertyList &propList)
{
  m_iface->drawRectangle(propList);
}
void IWORKTextRedirector::drawEllipse(const librevenge::RVNGPropertyList &propList)
{
  m_iface->drawEllipse(propList);
}
void IWORKTextRedirector::drawPolygon(const librevenge::RVNGPropertyList &propList)
{
  m_iface->drawPolygon(propList);
}
void IWORKTextRedirector::drawPolyline(const librevenge::RVNGPropertyList &propList)
{
  m_iface->drawPolyline(propList);
}
void IWORKTextRedirector::drawPath(const librevenge::RVNGPropertyList &propList)
{
  m_iface->drawPath(propList);
}

void IWORKTextRedirector::drawGraphicObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextRedirector::drawConnector(const librevenge::RVNGPropertyList &propList)
{
  m_iface->drawConnector(propList);
}

void IWORKTextRedirector::startTextObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::endTextObject()
{
  assert(0);
}

void IWORKTextRedirector::startNotes(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::endNotes()
{
  assert(0);
}

void IWORKTextRedirector::defineChartStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextRedirector::openChart(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeChart()
{
  assert(0);
}

void IWORKTextRedirector::openChartTextObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeChartTextObject()
{
  assert(0);
}

void IWORKTextRedirector::openChartPlotArea(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeChartPlotArea()
{
  assert(0);
}
void IWORKTextRedirector::insertChartAxis(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::openChartSeries(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeChartSeries()
{
  assert(0);
}

void IWORKTextRedirector::openAnimationSequence(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeAnimationSequence()
{
  assert(0);
}

void IWORKTextRedirector::openAnimationGroup(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeAnimationGroup()
{
  assert(0);
}

void IWORKTextRedirector::openAnimationIteration(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeAnimationIteration()
{
  assert(0);
}

void IWORKTextRedirector::insertMotionAnimation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::insertColorAnimation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::insertAnimation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::insertEffect(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
