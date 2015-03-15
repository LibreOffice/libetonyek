/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKOutputElementsRedirector.h"

#include <cassert>

#include "IWORKOutputElements.h"

namespace libetonyek
{

IWORKOutputElementsRedirector::IWORKOutputElementsRedirector(IWORKOutputElements &elements)
  : m_elements(elements)
{
}

void IWORKOutputElementsRedirector::setDocumentMetaData(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKOutputElementsRedirector::startDocument(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::endDocument()
{
  assert(0);
}

void IWORKOutputElementsRedirector::definePageStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKOutputElementsRedirector::defineEmbeddedFont(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKOutputElementsRedirector::openPageSpan(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closePageSpan()
{
  assert(0);
}

void IWORKOutputElementsRedirector::startSlide(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::endSlide()
{
  assert(0);
}

void IWORKOutputElementsRedirector::startMasterSlide(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::endMasterSlide()
{
  assert(0);
}

void IWORKOutputElementsRedirector::setStyle(const librevenge::RVNGPropertyList &propList)
{
  m_elements.addSetStyle(propList);
}

void IWORKOutputElementsRedirector::startLayer(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::endLayer()
{
  assert(0);
}

void IWORKOutputElementsRedirector::openHeader(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeHeader()
{
  assert(0);
}

void IWORKOutputElementsRedirector::openFooter(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeFooter()
{
  assert(0);
}

void IWORKOutputElementsRedirector::defineParagraphStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKOutputElementsRedirector::openParagraph(const librevenge::RVNGPropertyList &propList)
{
  m_elements.addOpenParagraph(propList);
}

void IWORKOutputElementsRedirector::closeParagraph()
{
  m_elements.addCloseParagraph();
}

void IWORKOutputElementsRedirector::defineCharacterStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKOutputElementsRedirector::openSpan(const librevenge::RVNGPropertyList &propList)
{
  m_elements.addOpenSpan(propList);
}

void IWORKOutputElementsRedirector::closeSpan()
{
  m_elements.addCloseSpan();
}

void IWORKOutputElementsRedirector::openLink(const librevenge::RVNGPropertyList &propList)
{
  m_elements.addOpenLink(propList);
}

void IWORKOutputElementsRedirector::closeLink()
{
  m_elements.addCloseLink();
}

void IWORKOutputElementsRedirector::defineSectionStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKOutputElementsRedirector::openSection(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeSection()
{
  assert(0);
}

void IWORKOutputElementsRedirector::insertTab()
{
  m_elements.addInsertTab();
}

void IWORKOutputElementsRedirector::insertSpace()
{
  m_elements.addInsertSpace();
}

void IWORKOutputElementsRedirector::insertText(const librevenge::RVNGString &text)
{
  m_elements.addInsertText(text);
}

void IWORKOutputElementsRedirector::insertLineBreak()
{
  m_elements.addInsertLineBreak();
}

void IWORKOutputElementsRedirector::insertField(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKOutputElementsRedirector::openOrderedListLevel(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::openUnorderedListLevel(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeOrderedListLevel()
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeUnorderedListLevel()
{
  assert(0);
}
void IWORKOutputElementsRedirector::openListElement(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeListElement()
{
  assert(0);
}

void IWORKOutputElementsRedirector::openFootnote(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeFootnote()
{
  assert(0);
}

void IWORKOutputElementsRedirector::openEndnote(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeEndnote()
{
  assert(0);
}

void IWORKOutputElementsRedirector::openComment(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeComment()
{
  assert(0);
}

void IWORKOutputElementsRedirector::openTextBox(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeTextBox()
{
  assert(0);
}

void IWORKOutputElementsRedirector::defineSheetNumberingStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKOutputElementsRedirector::openTable(const librevenge::RVNGPropertyList &propList)
{
  m_elements.addOpenTable(propList);
}

void IWORKOutputElementsRedirector::openTableRow(const librevenge::RVNGPropertyList &propList)
{
  m_elements.addOpenTableRow(propList);
}

void IWORKOutputElementsRedirector::closeTableRow()
{
  m_elements.addCloseTableRow();
}

void IWORKOutputElementsRedirector::openTableCell(const librevenge::RVNGPropertyList &propList)
{
  m_elements.addOpenTableCell(propList);
}

void IWORKOutputElementsRedirector::closeTableCell()
{
  m_elements.addCloseTableCell();
}
void IWORKOutputElementsRedirector::insertCoveredTableCell(const librevenge::RVNGPropertyList &propList)
{
  m_elements.addInsertCoveredTableCell(propList);
}

void IWORKOutputElementsRedirector::closeTable()
{
  m_elements.addCloseTable();
}

void IWORKOutputElementsRedirector::openFrame(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeFrame()
{
  assert(0);
}
void IWORKOutputElementsRedirector::insertBinaryObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::insertEquation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKOutputElementsRedirector::openGroup(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeGroup()
{
  assert(0);
}

void IWORKOutputElementsRedirector::defineGraphicStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKOutputElementsRedirector::drawRectangle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::drawEllipse(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::drawPolygon(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKOutputElementsRedirector::drawPolyline(const librevenge::RVNGPropertyList &propList)
{
  m_elements.addDrawPolyline(propList);
}

void IWORKOutputElementsRedirector::drawPath(const librevenge::RVNGPropertyList &propList)
{
  m_elements.addDrawPath(propList);
}

void IWORKOutputElementsRedirector::drawGraphicObject(const librevenge::RVNGPropertyList &propList)
{
  m_elements.addDrawGraphicObject(propList);
}

void IWORKOutputElementsRedirector::drawConnector(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKOutputElementsRedirector::startTextObject(const librevenge::RVNGPropertyList &propList)
{
  m_elements.addStartTextObject(propList);
}

void IWORKOutputElementsRedirector::endTextObject()
{
  m_elements.addEndTextObject();
}

void IWORKOutputElementsRedirector::startNotes(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::endNotes()
{
  assert(0);
}

void IWORKOutputElementsRedirector::defineChartStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKOutputElementsRedirector::openChart(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeChart()
{
  assert(0);
}

void IWORKOutputElementsRedirector::openChartTextObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeChartTextObject()
{
  assert(0);
}

void IWORKOutputElementsRedirector::openChartPlotArea(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeChartPlotArea()
{
  assert(0);
}
void IWORKOutputElementsRedirector::insertChartAxis(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::openChartSeries(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeChartSeries()
{
  assert(0);
}

void IWORKOutputElementsRedirector::openAnimationSequence(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeAnimationSequence()
{
  assert(0);
}

void IWORKOutputElementsRedirector::openAnimationGroup(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeAnimationGroup()
{
  assert(0);
}

void IWORKOutputElementsRedirector::openAnimationIteration(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::closeAnimationIteration()
{
  assert(0);
}

void IWORKOutputElementsRedirector::insertMotionAnimation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::insertColorAnimation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::insertAnimation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKOutputElementsRedirector::insertEffect(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
