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

#include "IWORKTextInterfaceRedirector.h"

#include <cassert>

namespace libetonyek
{

IWORKTextInterfaceRedirector::IWORKTextInterfaceRedirector(librevenge::RVNGTextInterface *const iface)
  : m_iface(iface)
{
}

void IWORKTextInterfaceRedirector::setDocumentMetaData(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextInterfaceRedirector::startDocument(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::endDocument()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::definePageStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextInterfaceRedirector::defineEmbeddedFont(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openPageSpan(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closePageSpan()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::startSlide(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::endSlide()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::startMasterSlide(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::endMasterSlide()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::setStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextInterfaceRedirector::startLayer(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::endLayer()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openHeader(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeHeader()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openFooter(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeFooter()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::defineParagraphStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openParagraph(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeParagraph()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::defineCharacterStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openSpan(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeSpan()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openLink(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeLink()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::defineSectionStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openSection(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeSection()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::insertTab()
{
  assert(0);
}
void IWORKTextInterfaceRedirector::insertSpace()
{
  assert(0);
}
void IWORKTextInterfaceRedirector::insertText(const librevenge::RVNGString &/*text*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::insertLineBreak()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::insertField(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openOrderedListLevel(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::openUnorderedListLevel(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeOrderedListLevel()
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeUnorderedListLevel()
{
  assert(0);
}
void IWORKTextInterfaceRedirector::openListElement(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeListElement()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openFootnote(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeFootnote()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openEndnote(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeEndnote()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openComment(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeComment()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openTextBox(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeTextBox()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::defineSheetNumberingStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openTable(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::openTableRow(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeTableRow()
{
  assert(0);
}
void IWORKTextInterfaceRedirector::openTableCell(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeTableCell()
{
  assert(0);
}
void IWORKTextInterfaceRedirector::insertCoveredTableCell(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeTable()
{
  assert(0);
}
void IWORKTextInterfaceRedirector::openFrame(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeFrame()
{
  assert(0);
}
void IWORKTextInterfaceRedirector::insertBinaryObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::insertEquation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openGroup(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeGroup()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::defineGraphicStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextInterfaceRedirector::drawRectangle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::drawEllipse(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::drawPolygon(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::drawPolyline(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::drawPath(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextInterfaceRedirector::drawGraphicObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextInterfaceRedirector::drawConnector(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextInterfaceRedirector::startTextObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::endTextObject()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::startNotes(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::endNotes()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::defineChartStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openChart(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeChart()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openChartTextObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeChartTextObject()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openChartPlotArea(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeChartPlotArea()
{
  assert(0);
}
void IWORKTextInterfaceRedirector::insertChartAxis(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::openChartSeries(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeChartSeries()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openAnimationSequence(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeAnimationSequence()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openAnimationGroup(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeAnimationGroup()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::openAnimationIteration(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::closeAnimationIteration()
{
  assert(0);
}

void IWORKTextInterfaceRedirector::insertMotionAnimation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::insertColorAnimation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::insertAnimation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextInterfaceRedirector::insertEffect(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
