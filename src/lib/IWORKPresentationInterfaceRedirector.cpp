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

#include "IWORKPresentationInterfaceRedirector.h"

#include <cassert>

namespace libetonyek
{

IWORKPresentationInterfaceRedirector::IWORKPresentationInterfaceRedirector(librevenge::RVNGPresentationInterface *const iface)
  : m_iface(iface)
{
}

void IWORKPresentationInterfaceRedirector::setDocumentMetaData(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::startDocument(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::endDocument()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::definePageStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::defineEmbeddedFont(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::openPageSpan(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closePageSpan()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::startPage(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::endPage()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::startMasterPage(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::endMasterPage()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::setStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::startLayer(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::endLayer()
{
  assert(0);
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

void IWORKPresentationInterfaceRedirector::defineParagraphStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::openParagraph(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeParagraph()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::defineCharacterStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::openSpan(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeSpan()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::openLink(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeLink()
{
  assert(0);
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
  assert(0);
}
void IWORKPresentationInterfaceRedirector::insertSpace()
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::insertText(const librevenge::RVNGString &/*text*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::insertLineBreak()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::insertField(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::openOrderedListLevel(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::openUnorderedListLevel(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeOrderedListLevel()
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeUnorderedListLevel()
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::openListElement(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeListElement()
{
  assert(0);
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

void IWORKPresentationInterfaceRedirector::openComment(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeComment()
{
  assert(0);
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

void IWORKPresentationInterfaceRedirector::openTable(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::openTableRow(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeTableRow()
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::openTableCell(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeTableCell()
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::insertCoveredTableCell(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeTable()
{
  assert(0);
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

void IWORKPresentationInterfaceRedirector::openGroup(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeGroup()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::defineGraphicStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::drawRectangle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::drawEllipse(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::drawPolygon(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::drawPolyline(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::drawPath(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::drawGraphicObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::drawConnector(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::startTextObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::endTextObject()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::startNotes(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::endNotes()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::defineChartStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::openChart(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeChart()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::openChartTextObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeChartTextObject()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::openChartPlotArea(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeChartPlotArea()
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::insertChartAxis(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::openChartSeries(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeChartSeries()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::openAnimationSequence(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeAnimationSequence()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::openAnimationGroup(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeAnimationGroup()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::openAnimationIteration(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::closeAnimationIteration()
{
  assert(0);
}

void IWORKPresentationInterfaceRedirector::insertMotionAnimation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::insertColorAnimation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::insertAnimation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKPresentationInterfaceRedirector::insertEffect(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
