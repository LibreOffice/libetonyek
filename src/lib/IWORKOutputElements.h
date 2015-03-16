/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKOUTPUTELEMENTS_H_INCLUDED
#define IWORKOUTPUTELEMENTS_H_INCLUDED

#include <deque>

#include <boost/shared_ptr.hpp>

#include <librevenge/librevenge.h>

namespace libetonyek
{

class IWORKDocumentInterface;
class IWORKOutputElement;

class IWORKOutputElements
{
  typedef std::deque<boost::shared_ptr<IWORKOutputElement> > ElementList_t;

public:
  IWORKOutputElements();

  void append(const IWORKOutputElements &elements);
  void write(IWORKDocumentInterface *iface) const;
  void clear();
  bool empty() const;

  void addCloseEndnote();
  void addCloseFooter();
  void addCloseFootnote();
  void addCloseFrame();
  void addCloseHeader();
  void addCloseLink();
  void addCloseListElement();
  void addCloseOrderedListLevel();
  void addClosePageSpan();
  void addCloseParagraph();
  void addCloseSection();
  void addCloseSpan();
  void addCloseTable();
  void addCloseTableCell();
  void addCloseTableRow();
  void addCloseUnorderedListLevel();
  void addDrawGraphicObject(const librevenge::RVNGPropertyList &propList);
  void addDrawPath(const librevenge::RVNGPropertyList &propList);
  void addDrawPolyline(const librevenge::RVNGPropertyList &propList);
  void addEndTextObject();
  void addInsertBinaryObject(const librevenge::RVNGPropertyList &propList);
  void addInsertCoveredTableCell(const librevenge::RVNGPropertyList &propList);
  void addInsertLineBreak();
  void addInsertSpace();
  void addInsertTab();
  void addInsertText(const librevenge::RVNGString &text);
  void addOpenEndnote(const librevenge::RVNGPropertyList &propList);
  void addOpenFooter(const librevenge::RVNGPropertyList &propList);
  void addOpenFootnote(const librevenge::RVNGPropertyList &propList);
  void addOpenFrame(const librevenge::RVNGPropertyList &propList);
  void addOpenHeader(const librevenge::RVNGPropertyList &propList);
  void addOpenLink(const librevenge::RVNGPropertyList &propList);
  void addOpenListElement(const librevenge::RVNGPropertyList &propList);
  void addOpenOrderedListLevel(const librevenge::RVNGPropertyList &propList);
  void addOpenPageSpan(const librevenge::RVNGPropertyList &propList);
  void addOpenParagraph(const librevenge::RVNGPropertyList &propList);
  void addOpenSection(const librevenge::RVNGPropertyList &propList);
  void addOpenSpan(const librevenge::RVNGPropertyList &propList);
  void addOpenTable(const librevenge::RVNGPropertyList &propList);
  void addOpenTableCell(const librevenge::RVNGPropertyList &propList);
  void addOpenTableRow(const librevenge::RVNGPropertyList &propList);
  void addOpenUnorderedListLevel(const librevenge::RVNGPropertyList &propList);
  void addSetStyle(const librevenge::RVNGPropertyList &propList);
  void addStartTextObject(const librevenge::RVNGPropertyList &propList);

private:
  ElementList_t m_elements;
};

}

#endif /* IWORKOUTPUTELEMENTS_H_INCLUDED */

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
