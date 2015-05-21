/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKOutputElements.h"

#include <boost/make_shared.hpp>

#include "IWORKDocumentInterface.h"

namespace libetonyek
{

using boost::make_shared;

class IWORKOutputElement
{
  // disable copying
  IWORKOutputElement(const IWORKOutputElement &);
  IWORKOutputElement &operator=(const IWORKOutputElement &);

public:
  IWORKOutputElement() {}
  virtual ~IWORKOutputElement() {}
  virtual void write(IWORKDocumentInterface *iface) const = 0;
};

namespace
{

class CloseCommentElement : public IWORKOutputElement
{
public:
  CloseCommentElement() {}
  ~CloseCommentElement() {}
  void write(IWORKDocumentInterface *iface) const;
};

class CloseEndnoteElement : public IWORKOutputElement
{
public:
  CloseEndnoteElement() {}
  ~CloseEndnoteElement() {}
  void write(IWORKDocumentInterface *iface) const;
};

class CloseFooterElement : public IWORKOutputElement
{
public:
  CloseFooterElement() {}
  ~CloseFooterElement() {}
  void write(IWORKDocumentInterface *iface) const;
};

class CloseFootnoteElement : public IWORKOutputElement
{
public:
  CloseFootnoteElement() {}
  ~CloseFootnoteElement() {}
  void write(IWORKDocumentInterface *iface) const;
};

class CloseFrameElement : public IWORKOutputElement
{
public:
  CloseFrameElement() {}
  ~CloseFrameElement() {}
  void write(IWORKDocumentInterface *iface) const;
};

class CloseHeaderElement : public IWORKOutputElement
{
public:
  CloseHeaderElement() {}
  ~CloseHeaderElement() {}
  void write(IWORKDocumentInterface *iface) const;
};

class CloseLinkElement : public IWORKOutputElement
{
public:
  CloseLinkElement() {}
  ~CloseLinkElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new CloseLinkElement();
  }
};

class CloseListElementElement : public IWORKOutputElement
{
public:
  CloseListElementElement() {}
  ~CloseListElementElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new CloseListElementElement();
  }
};

class CloseOrderedListLevelElement : public IWORKOutputElement
{
public:
  CloseOrderedListLevelElement() {}
  ~CloseOrderedListLevelElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new CloseOrderedListLevelElement();
  }
};

class ClosePageSpanElement : public IWORKOutputElement
{
public:
  ClosePageSpanElement() {}
  ~ClosePageSpanElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new ClosePageSpanElement();
  }
};

class CloseParagraphElement : public IWORKOutputElement
{
public:
  CloseParagraphElement() {}
  ~CloseParagraphElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new CloseParagraphElement();
  }
};

class CloseSectionElement : public IWORKOutputElement
{
public:
  CloseSectionElement() {}
  ~CloseSectionElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new CloseSectionElement();
  }
};

class CloseSpanElement : public IWORKOutputElement
{
public:
  CloseSpanElement() {}
  ~CloseSpanElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new CloseSpanElement();
  }
};

class CloseTableElement : public IWORKOutputElement
{
public:
  CloseTableElement() {}
  ~CloseTableElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new CloseTableElement();
  }
};

class CloseTableCellElement : public IWORKOutputElement
{
public:
  CloseTableCellElement() {}
  ~CloseTableCellElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new CloseTableCellElement();
  }
};

class CloseTableRowElement : public IWORKOutputElement
{
public:
  CloseTableRowElement() {}
  ~CloseTableRowElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new CloseTableRowElement();
  }
};

class CloseUnorderedListLevelElement : public IWORKOutputElement
{
public:
  CloseUnorderedListLevelElement() {}
  ~CloseUnorderedListLevelElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new CloseUnorderedListLevelElement();
  }
};

class DrawGraphicObjectElement : public IWORKOutputElement
{
public:
  DrawGraphicObjectElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~DrawGraphicObjectElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new DrawGraphicObjectElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class DrawPathElement : public IWORKOutputElement
{
public:
  DrawPathElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~DrawPathElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new DrawPathElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class DrawPolylineElement : public IWORKOutputElement
{
public:
  DrawPolylineElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~DrawPolylineElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new DrawPolylineElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class EndTextObjectElement : public IWORKOutputElement
{
public:
  EndTextObjectElement() {}
  ~EndTextObjectElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new EndTextObjectElement();
  }
};

class InsertBinaryObjectElement : public IWORKOutputElement
{
public:
  InsertBinaryObjectElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~InsertBinaryObjectElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new InsertBinaryObjectElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class InsertCoveredTableCellElement : public IWORKOutputElement
{
public:
  InsertCoveredTableCellElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~InsertCoveredTableCellElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new InsertCoveredTableCellElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class InsertLineBreakElement : public IWORKOutputElement
{
public:
  InsertLineBreakElement() {}
  ~InsertLineBreakElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new InsertLineBreakElement();
  }
};

class InsertSpaceElement : public IWORKOutputElement
{
public:
  InsertSpaceElement() {}
  ~InsertSpaceElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new InsertSpaceElement();
  }
};

class InsertTabElement : public IWORKOutputElement
{
public:
  InsertTabElement() {}
  ~InsertTabElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new InsertTabElement();
  }
};

class InsertTextElement : public IWORKOutputElement
{
public:
  InsertTextElement(const librevenge::RVNGString &text) :
    m_text(text) {}
  ~InsertTextElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new InsertTextElement(m_text);
  }
private:
  librevenge::RVNGString m_text;
};

class OpenCommentElement : public IWORKOutputElement
{
public:
  OpenCommentElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenCommentElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new OpenCommentElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenEndnoteElement : public IWORKOutputElement
{
public:
  OpenEndnoteElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenEndnoteElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new OpenEndnoteElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenFooterElement : public IWORKOutputElement
{
public:
  OpenFooterElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenFooterElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new OpenFooterElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenFootnoteElement : public IWORKOutputElement
{
public:
  OpenFootnoteElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenFootnoteElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new OpenFootnoteElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenFrameElement : public IWORKOutputElement
{
public:
  OpenFrameElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenFrameElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new OpenFrameElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenHeaderElement : public IWORKOutputElement
{
public:
  OpenHeaderElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenHeaderElement() {}
  void write(IWORKDocumentInterface *iface) const;
  IWORKOutputElement *clone()
  {
    return new OpenHeaderElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenLinkElement : public IWORKOutputElement
{
public:
  OpenLinkElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenLinkElement() {}
  void write(IWORKDocumentInterface *iface) const;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenListElementElement : public IWORKOutputElement
{
public:
  OpenListElementElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenListElementElement() {}
  void write(IWORKDocumentInterface *iface) const;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenOrderedListLevelElement : public IWORKOutputElement
{
public:
  OpenOrderedListLevelElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenOrderedListLevelElement() {}
  void write(IWORKDocumentInterface *iface) const;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenPageSpanElement : public IWORKOutputElement
{
public:
  OpenPageSpanElement(const librevenge::RVNGPropertyList &propList)
    : m_propList(propList)
  {
  }
  ~OpenPageSpanElement() {}
  void write(IWORKDocumentInterface *iface) const;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenParagraphElement : public IWORKOutputElement
{
public:
  OpenParagraphElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenParagraphElement() {}
  void write(IWORKDocumentInterface *iface) const;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenSectionElement : public IWORKOutputElement
{
public:
  OpenSectionElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenSectionElement() {}
  void write(IWORKDocumentInterface *iface) const;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenSpanElement : public IWORKOutputElement
{
public:
  OpenSpanElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenSpanElement() {}
  void write(IWORKDocumentInterface *iface) const;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenTableElement : public IWORKOutputElement
{
public:
  OpenTableElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenTableElement() {}
  void write(IWORKDocumentInterface *iface) const;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenTableCellElement : public IWORKOutputElement
{
public:
  OpenTableCellElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenTableCellElement() {}
  void write(IWORKDocumentInterface *iface) const;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenTableRowElement : public IWORKOutputElement
{
public:
  OpenTableRowElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenTableRowElement() {}
  void write(IWORKDocumentInterface *iface) const;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenUnorderedListLevelElement : public IWORKOutputElement
{
public:
  OpenUnorderedListLevelElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenUnorderedListLevelElement() {}
  void write(IWORKDocumentInterface *iface) const;
private:
  librevenge::RVNGPropertyList m_propList;
};

class SetStyleElement : public IWORKOutputElement
{
public:
  SetStyleElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~SetStyleElement() {}
  void write(IWORKDocumentInterface *iface) const;
private:
  librevenge::RVNGPropertyList m_propList;
};

class StartTextObjectElement : public IWORKOutputElement
{
public:
  StartTextObjectElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~StartTextObjectElement() {}
  void write(IWORKDocumentInterface *iface) const;
private:
  librevenge::RVNGPropertyList m_propList;
};

void CloseCommentElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->closeComment();
}

void CloseEndnoteElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->closeEndnote();
}

void CloseFooterElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->closeFooter();
}

void CloseFootnoteElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->closeFootnote();
}

void CloseFrameElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->closeFrame();
}

void CloseHeaderElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->closeHeader();
}

void CloseLinkElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->closeLink();
}

void CloseListElementElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->closeListElement();
}

void CloseOrderedListLevelElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->closeOrderedListLevel();
}

void ClosePageSpanElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->closePageSpan();
}

void CloseParagraphElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->closeParagraph();
}

void CloseSectionElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->closeSection();
}

void CloseSpanElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->closeSpan();
}

void CloseTableElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->closeTable();
}

void CloseTableCellElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->closeTableCell();
}

void CloseTableRowElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->closeTableRow();
}

void CloseUnorderedListLevelElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->closeUnorderedListLevel();
}

void DrawGraphicObjectElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->drawGraphicObject(m_propList);
}

void DrawPathElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->drawPath(m_propList);
}

void DrawPolylineElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->drawPolyline(m_propList);
}

void EndTextObjectElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->endTextObject();
}

void InsertBinaryObjectElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->insertBinaryObject(m_propList);
}

void InsertCoveredTableCellElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->insertCoveredTableCell(m_propList);
}

void InsertLineBreakElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->insertLineBreak();
}

void InsertSpaceElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->insertSpace();
}

void InsertTabElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->insertTab();
}

void InsertTextElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->insertText(m_text);
}

void OpenCommentElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->openComment(m_propList);
}

void OpenEndnoteElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->openEndnote(m_propList);
}

void OpenFooterElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->openFooter(m_propList);
}

void OpenFootnoteElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->openFootnote(m_propList);
}

void OpenFrameElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->openFrame(m_propList);
}

void OpenHeaderElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->openHeader(m_propList);
}

void OpenLinkElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->openLink(m_propList);
}

void OpenListElementElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->openListElement(m_propList);
}

void OpenOrderedListLevelElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->openOrderedListLevel(m_propList);
}

void OpenPageSpanElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->openPageSpan(m_propList);

}

void OpenParagraphElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->openParagraph(m_propList);
}

void OpenSectionElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->openSection(m_propList);
}

void OpenSpanElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->openSpan(m_propList);
}

void OpenTableElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->openTable(m_propList);
}

void OpenTableCellElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->openTableCell(m_propList);
}

void OpenTableRowElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->openTableRow(m_propList);
}

void OpenUnorderedListLevelElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->openUnorderedListLevel(m_propList);
}

void SetStyleElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->setStyle(m_propList);
}

void StartTextObjectElement::write(IWORKDocumentInterface *iface) const
{
  if (iface)
    iface->startTextObject(m_propList);
}

}

IWORKOutputElements::IWORKOutputElements()
  : m_elements()
{
}

void IWORKOutputElements::append(const IWORKOutputElements &elements)
{
  m_elements.insert(m_elements.end(), elements.m_elements.begin(), elements.m_elements.end());
}

void IWORKOutputElements::write(IWORKDocumentInterface *iface) const
{
  ElementList_t::const_iterator iter;
  for (iter = m_elements.begin(); iter != m_elements.end(); ++iter)
    (*iter)->write(iface);
}

void IWORKOutputElements::clear()
{
  m_elements.clear();
}

bool IWORKOutputElements::empty() const
{
  return m_elements.empty();
}

void IWORKOutputElements::addCloseComment()
{
  m_elements.push_back(make_shared<CloseCommentElement>());
}

void IWORKOutputElements::addCloseEndnote()
{
  m_elements.push_back(make_shared<CloseEndnoteElement>());
}

void IWORKOutputElements::addCloseFooter()
{
  m_elements.push_back(make_shared<CloseFooterElement>());
}

void IWORKOutputElements::addCloseFootnote()
{
  m_elements.push_back(make_shared<CloseFootnoteElement>());
}

void IWORKOutputElements::addCloseFrame()
{
  m_elements.push_back(make_shared<CloseFrameElement>());
}

void IWORKOutputElements::addCloseHeader()
{
  m_elements.push_back(make_shared<CloseHeaderElement>());
}

void IWORKOutputElements::addCloseLink()
{
  m_elements.push_back(make_shared<CloseLinkElement>());
}

void IWORKOutputElements::addCloseListElement()
{
  m_elements.push_back(make_shared<CloseListElementElement>());
}

void IWORKOutputElements::addCloseOrderedListLevel()
{
  m_elements.push_back(make_shared<CloseOrderedListLevelElement>());
}

void IWORKOutputElements::addClosePageSpan()
{
  m_elements.push_back(make_shared<ClosePageSpanElement>());
}

void IWORKOutputElements::addCloseParagraph()
{
  m_elements.push_back(make_shared<CloseParagraphElement>());
}

void IWORKOutputElements::addCloseSection()
{
  m_elements.push_back(make_shared<CloseSectionElement>());
}

void IWORKOutputElements::addCloseSpan()
{
  m_elements.push_back(make_shared<CloseSpanElement>());
}

void IWORKOutputElements::addCloseTable()
{
  m_elements.push_back(make_shared<CloseTableElement>());
}

void IWORKOutputElements::addCloseTableCell()
{
  m_elements.push_back(make_shared<CloseTableCellElement>());
}

void IWORKOutputElements::addCloseTableRow()
{
  m_elements.push_back(make_shared<CloseTableRowElement>());
}

void IWORKOutputElements::addCloseUnorderedListLevel()
{
  m_elements.push_back(make_shared<CloseUnorderedListLevelElement>());
}

void IWORKOutputElements::addDrawGraphicObject(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<DrawGraphicObjectElement>(propList));
}

void IWORKOutputElements::addDrawPath(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<DrawPathElement>(propList));
}

void IWORKOutputElements::addDrawPolyline(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<DrawPolylineElement>(propList));
}

void IWORKOutputElements::addEndTextObject()
{
  m_elements.push_back(make_shared<EndTextObjectElement>());
}

void IWORKOutputElements::addInsertBinaryObject(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<InsertBinaryObjectElement>(propList));
}

void IWORKOutputElements::addInsertCoveredTableCell(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<InsertCoveredTableCellElement>(propList));
}

void IWORKOutputElements::addInsertLineBreak()
{
  m_elements.push_back(make_shared<InsertLineBreakElement>());
}

void IWORKOutputElements::addInsertSpace()
{
  m_elements.push_back(make_shared<InsertSpaceElement>());
}

void IWORKOutputElements::addInsertTab()
{
  m_elements.push_back(make_shared<InsertTabElement>());
}

void IWORKOutputElements::addInsertText(const librevenge::RVNGString &text)
{
  m_elements.push_back(make_shared<InsertTextElement>(text));
}

void IWORKOutputElements::addOpenComment(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<OpenCommentElement>(propList));
}

void IWORKOutputElements::addOpenEndnote(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<OpenEndnoteElement>(propList));
}

void IWORKOutputElements::addOpenFooter(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<OpenFooterElement>(propList));
}

void IWORKOutputElements::addOpenFootnote(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<OpenFootnoteElement>(propList));
}

void IWORKOutputElements::addOpenFrame(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<OpenFrameElement>(propList));
}

void IWORKOutputElements::addOpenHeader(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<OpenHeaderElement>(propList));
}

void IWORKOutputElements::addOpenLink(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<OpenLinkElement>(propList));
}

void IWORKOutputElements::addOpenListElement(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<OpenListElementElement>(propList));
}

void IWORKOutputElements::addOpenOrderedListLevel(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<OpenOrderedListLevelElement>(propList));
}

void IWORKOutputElements::addOpenPageSpan(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<OpenPageSpanElement>(propList));
}

void IWORKOutputElements::addOpenParagraph(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<OpenParagraphElement>(propList));
}

void IWORKOutputElements::addOpenSection(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<OpenSectionElement>(propList));
}

void IWORKOutputElements::addOpenSpan(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<OpenSpanElement>(propList));
}

void IWORKOutputElements::addOpenTable(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<OpenTableElement>(propList));
}

void IWORKOutputElements::addOpenTableCell(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<OpenTableCellElement>(propList));
}

void IWORKOutputElements::addOpenTableRow(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<OpenTableRowElement>(propList));
}

void IWORKOutputElements::addOpenUnorderedListLevel(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<OpenUnorderedListLevelElement>(propList));
}

void IWORKOutputElements::addSetStyle(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<SetStyleElement>(propList));
}

void IWORKOutputElements::addStartTextObject(const librevenge::RVNGPropertyList &propList)
{
  m_elements.push_back(make_shared<StartTextObjectElement>(propList));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
