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

void IWORKSpreadsheetRedirector::setDocumentMetaData(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetRedirector::startDocument(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::endDocument()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::definePageStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetRedirector::defineEmbeddedFont(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openPageSpan(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closePageSpan()
{
  assert(0);
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

void IWORKSpreadsheetRedirector::setStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetRedirector::startLayer(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::endLayer()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openHeader(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeHeader()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openFooter(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeFooter()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::defineParagraphStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openParagraph(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeParagraph()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::defineCharacterStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openSpan(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeSpan()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openLink(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeLink()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::defineSectionStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openSection(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeSection()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::insertTab()
{
  assert(0);
}
void IWORKSpreadsheetRedirector::insertSpace()
{
  assert(0);
}
void IWORKSpreadsheetRedirector::insertText(const librevenge::RVNGString &/*text*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::insertLineBreak()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::insertField(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openOrderedListLevel(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::openUnorderedListLevel(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeOrderedListLevel()
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeUnorderedListLevel()
{
  assert(0);
}
void IWORKSpreadsheetRedirector::openListElement(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeListElement()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openFootnote(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeFootnote()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openEndnote(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeEndnote()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openComment(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeComment()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openTextBox(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeTextBox()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::defineSheetNumberingStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openTable(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::openTableRow(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeTableRow()
{
  assert(0);
}
void IWORKSpreadsheetRedirector::openTableCell(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeTableCell()
{
  assert(0);
}
void IWORKSpreadsheetRedirector::insertCoveredTableCell(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeTable()
{
  assert(0);
}
void IWORKSpreadsheetRedirector::openFrame(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeFrame()
{
  assert(0);
}
void IWORKSpreadsheetRedirector::insertBinaryObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::insertEquation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openGroup(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeGroup()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::defineGraphicStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetRedirector::drawRectangle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::drawEllipse(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::drawPolygon(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::drawPolyline(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::drawPath(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetRedirector::drawGraphicObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetRedirector::drawConnector(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
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

void IWORKSpreadsheetRedirector::defineChartStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openChart(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeChart()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openChartTextObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeChartTextObject()
{
  assert(0);
}

void IWORKSpreadsheetRedirector::openChartPlotArea(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKSpreadsheetRedirector::closeChartPlotArea()
{
  assert(0);
}
void IWORKSpreadsheetRedirector::insertChartAxis(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
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
