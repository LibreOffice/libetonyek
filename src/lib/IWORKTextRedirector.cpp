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

void IWORKTextRedirector::setDocumentMetaData(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextRedirector::startDocument(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::endDocument()
{
  assert(0);
}

void IWORKTextRedirector::definePageStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextRedirector::defineEmbeddedFont(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextRedirector::openPageSpan(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closePageSpan()
{
  assert(0);
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

void IWORKTextRedirector::openHeader(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeHeader()
{
  assert(0);
}

void IWORKTextRedirector::openFooter(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeFooter()
{
  assert(0);
}

void IWORKTextRedirector::defineParagraphStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextRedirector::openParagraph(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeParagraph()
{
  assert(0);
}

void IWORKTextRedirector::defineCharacterStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextRedirector::openSpan(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeSpan()
{
  assert(0);
}

void IWORKTextRedirector::openLink(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeLink()
{
  assert(0);
}

void IWORKTextRedirector::defineSectionStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextRedirector::openSection(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeSection()
{
  assert(0);
}

void IWORKTextRedirector::insertTab()
{
  assert(0);
}
void IWORKTextRedirector::insertSpace()
{
  assert(0);
}
void IWORKTextRedirector::insertText(const librevenge::RVNGString &/*text*/)
{
  assert(0);
}
void IWORKTextRedirector::insertLineBreak()
{
  assert(0);
}

void IWORKTextRedirector::insertField(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextRedirector::openOrderedListLevel(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::openUnorderedListLevel(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeOrderedListLevel()
{
  assert(0);
}
void IWORKTextRedirector::closeUnorderedListLevel()
{
  assert(0);
}
void IWORKTextRedirector::openListElement(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeListElement()
{
  assert(0);
}

void IWORKTextRedirector::openFootnote(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeFootnote()
{
  assert(0);
}

void IWORKTextRedirector::openEndnote(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeEndnote()
{
  assert(0);
}

void IWORKTextRedirector::openComment(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeComment()
{
  assert(0);
}

void IWORKTextRedirector::openTextBox(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeTextBox()
{
  assert(0);
}

void IWORKTextRedirector::defineSheetNumberingStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextRedirector::openTable(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::openTableRow(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeTableRow()
{
  assert(0);
}
void IWORKTextRedirector::openTableCell(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeTableCell()
{
  assert(0);
}
void IWORKTextRedirector::insertCoveredTableCell(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeTable()
{
  assert(0);
}
void IWORKTextRedirector::openFrame(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeFrame()
{
  assert(0);
}
void IWORKTextRedirector::insertBinaryObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::insertEquation(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextRedirector::openGroup(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::closeGroup()
{
  assert(0);
}

void IWORKTextRedirector::defineGraphicStyle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextRedirector::drawRectangle(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::drawEllipse(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::drawPolygon(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::drawPolyline(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}
void IWORKTextRedirector::drawPath(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextRedirector::drawGraphicObject(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
}

void IWORKTextRedirector::drawConnector(const librevenge::RVNGPropertyList &/*propList*/)
{
  assert(0);
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
