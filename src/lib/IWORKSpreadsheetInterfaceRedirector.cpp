/* -*- Mode: C++
{
assert(0);
} tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKSpreadsheetInterfaceRedirector.h"

#include <cassert>

namespace libetonyek
{

IWORKSpreadsheetInterfaceRedirector::IWORKSpreadsheetInterfaceRedirector(librevenge::RVNGSpreadsheetInterface *const iface)
  : m_iface(iface)
{
}

void IWORKSpreadsheetInterfaceRedirector::setDocumentMetaData(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::startDocument(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::endDocument()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::definePageStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::defineEmbeddedFont(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openPageSpan(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closePageSpan()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::startPage(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::endPage()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::startMasterPage(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::endMasterPage()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::setStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::startLayer(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::endLayer()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openHeader(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeHeader()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openFooter(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeFooter()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::defineParagraphStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openParagraph(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeParagraph()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::defineCharacterStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openSpan(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeSpan()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openLink(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeLink()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::defineSectionStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openSection(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeSection()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::insertTab()
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::insertSpace()
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::insertText(const librevenge::RVNGString &/*text*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::insertLineBreak()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::insertField(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openOrderedListLevel(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::openUnorderedListLevel(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeOrderedListLevel()
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeUnorderedListLevel()
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::openListElement(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeListElement()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openFootnote(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeFootnote()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openEndnote(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeEndnote()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openComment(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeComment()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openTextBox(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeTextBox()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::defineSheetNumberingStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openTable(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::openTableRow(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeTableRow()
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::openTableCell(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeTableCell()
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::insertCoveredTableCell(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeTable()
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::openFrame(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeFrame()
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::insertBinaryObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::insertEquation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openGroup(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeGroup()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::defineGraphicStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::drawRectangle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::drawEllipse(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::drawPolygon(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::drawPolyline(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::drawPath(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::drawGraphicObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::drawConnector(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::startTextObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::endTextObject()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::startNotes(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::endNotes()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::defineChartStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openChart(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeChart()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openChartTextObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeChartTextObject()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openChartPlotArea(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeChartPlotArea()
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::insertChartAxis(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::openChartSeries(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeChartSeries()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openAnimationSequence(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeAnimationSequence()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openAnimationGroup(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeAnimationGroup()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::openAnimationIteration(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::closeAnimationIteration()
{
  assert(0);
}

void IWORKSpreadsheetInterfaceRedirector::insertMotionAnimation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::insertColorAnimation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::insertAnimation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetInterfaceRedirector::insertEffect(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
