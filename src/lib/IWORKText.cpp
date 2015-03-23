/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKText.h"

#include <cassert>

#include <boost/make_shared.hpp>
#include <boost/optional.hpp>

#include <librevenge/librevenge.h>

#include "IWORKDocumentInterface.h"
#include "IWORKPath.h"
#include "IWORKStyles.h"
#include "IWORKTransformation.h"
#include "IWORKTypes.h"

using boost::optional;

using std::string;

namespace libetonyek
{

struct IWORKText::Paragraph
{
  IWORKStyleStack m_styleStack;
  IWORKStylePtr_t style;
  IWORKObjectList_t objects;

  explicit Paragraph(const IWORKStyleStack &styleStack);
};

namespace
{

librevenge::RVNGString makeColor(const IWORKColor &color)
{
  // TODO: alpha

  const unsigned r = color.m_red * 256 - 0.5;
  const unsigned g = color.m_green * 256 - 0.5;
  const unsigned b = color.m_blue * 256 - 0.5;

  librevenge::RVNGString str;
  str.sprintf("#%.2x%.2x%.2x", r, g, b);

  return str;
}

void fillCharPropList(librevenge::RVNGPropertyList &props, const IWORKCharacterStyle &style)
{
  if (style.getItalic())
    props.insert("fo:font-style", "italic");
  if (style.getBold())
    props.insert("fo:font-weight", "bold");
  if (style.getUnderline())
    props.insert("style:text-underline-type", "single");
  if (style.getStrikethru())
    props.insert("style:text-line-through-type", "single");
  if (style.getOutline())
    props.insert("style:text-outline", true);

  const optional<IWORKCapitalization> capitalization = style.getCapitalization();
  if (capitalization)
  {
    if (IWORK_CAPITALIZATION_SMALL_CAPS == get(capitalization))
      props.insert("fo:font-variant", "small-caps");
  }

  const optional<string> fontName = style.getFontName();
  if (fontName)
    props.insert("style:font-name", librevenge::RVNGString(get(fontName).c_str()));

  const optional<double> fontSize = style.getFontSize();
  if (fontSize)
    props.insert("fo:font-size", pt2in(get(fontSize)));

  const optional<IWORKColor> fontColor = style.getFontColor();
  if (fontColor)
    props.insert("fo:color", makeColor(get(fontColor)));
}

IWORKStylePtr_t makeEmptyStyle()
{
  optional<string> dummy;
  return boost::make_shared<IWORKStyle>(IWORKPropertyMap(), dummy, dummy);
}

librevenge::RVNGPropertyList makeCharPropList(const IWORKStylePtr_t &style, const IWORKStyleStack &context)
{
  librevenge::RVNGPropertyList props;

  // Even if there is no character style for the span, there might still
  // be attributes inherited from the paragraph style through context.
  // We use an empty style so these can be picked up.
  fillCharPropList(props, IWORKCharacterStyle(bool(style) ? style : makeEmptyStyle(), context));

  return props;
}

librevenge::RVNGPropertyList makeParaPropList(const IWORKStylePtr_t &style, const IWORKStyleStack &context)
{
  librevenge::RVNGPropertyList props;

  if (bool(style))
  {
    const IWORKParagraphStyle paraStyle(style, context);

    const optional<IWORKAlignment> alignment(paraStyle.getAlignment());
    if (bool(alignment))
    {
      switch (get(alignment))
      {
      case IWORK_ALIGNMENT_LEFT :
        props.insert("fo:text-align", "left");
        break;
      case IWORK_ALIGNMENT_RIGHT :
        props.insert("fo:text-align", "right");
        break;
      case IWORK_ALIGNMENT_CENTER :
        props.insert("fo:text-align", "center");
        break;
      case IWORK_ALIGNMENT_JUSTIFY :
        props.insert("fo:text-align", "justify");
        break;
      }
    }

    const optional<IWORKTabStops_t> &tabStops = paraStyle.getTabs();
    if (bool(tabStops))
    {
      for (IWORKTabStops_t::const_iterator it = get(tabStops).begin(); get(tabStops).end() != it; ++it)
      {
        librevenge::RVNGPropertyList tab;
        tab.insert("style:position", pt2in(it->m_pos));
        tab.insert("style:type", "left");
      }
    }
  }

  return props;
}

}

void drawTextSpan(const IWORKStylePtr_t &style, const IWORKStyleStack &styleStack, const string &text, IWORKOutputElements &elements)
{
  const librevenge::RVNGPropertyList props(makeCharPropList(style, styleStack));
  elements.addOpenSpan(props);
  elements.addInsertText(librevenge::RVNGString(text.c_str()));
  elements.addCloseSpan();
}

void drawTab(IWORKOutputElements &elements)
{
  elements.addOpenSpan(librevenge::RVNGPropertyList());
  elements.addInsertTab();
  elements.addCloseSpan();
}

void drawLineBreak(const IWORKStyleStack &styleStack, IWORKOutputElements &elements)
{
  elements.addCloseParagraph();
  const librevenge::RVNGPropertyList props(makeParaPropList(IWORKStylePtr_t(), styleStack));
  elements.addOpenParagraph(props);
}

void IWORKText::draw(const IWORKTransformation &trafo, IWORKOutputElements &elements) const
{
  librevenge::RVNGPropertyList props;

  double x = 0;
  double y = 0;
  trafo(x, y);
  props.insert("svg:x", pt2in(x));
  props.insert("svg:y", pt2in(y));

  if (bool(m_boundingBox))
  {
    double w = m_boundingBox->m_naturalSize.m_width;
    double h = m_boundingBox->m_naturalSize.m_height;
    trafo(w, h, true);

    props.insert("svg:width", pt2in(w));
    props.insert("svg:height", pt2in(h));
  }

  IWORKPath path;
  path.appendMoveTo(0, 0);
  path.appendLineTo(0, 1);
  path.appendLineTo(1, 1);
  path.appendLineTo(1, 0);
  path.appendClose();
  path *= trafo;

  props.insert("svg:d", path.toWPG());

  if (m_object)
    elements.addStartTextObject(props);

  for (IWORKText::ParagraphList_t::const_iterator it = m_paragraphs.begin(); m_paragraphs.end() != it; ++it)
  {
    const librevenge::RVNGPropertyList paraProps(makeParaPropList((*it)->style, (*it)->m_styleStack));
    elements.addOpenParagraph(paraProps);
    elements.addCloseParagraph();
  }

  if (m_object)
    elements.addEndTextObject();
}

IWORKText::IWORKText(const bool object)
  : m_styleStack()
  , m_layoutStyle()
  , m_paragraphs()
  , m_currentParagraph()
  , m_lineBreaks(0)
  , m_object(object)
  , m_boundingBox()
{
}

IWORKText::Paragraph::Paragraph(const IWORKStyleStack &styleStack)
  : m_styleStack(styleStack)
  , style()
  , objects()
{
}

void IWORKText::setLayoutStyle(const IWORKStylePtr_t &style)
{
  assert(!m_layoutStyle);

  m_layoutStyle = style;
  m_styleStack.push();
  m_styleStack.set(style);
}

const IWORKGeometryPtr_t &IWORKText::getBoundingBox() const
{
  return m_boundingBox;
}

void IWORKText::setBoundingBox(const IWORKGeometryPtr_t &boundingBox)
{
  m_boundingBox = boundingBox;
}

void IWORKText::openParagraph(const IWORKStylePtr_t &style)
{
  assert(!m_currentParagraph);

  m_currentParagraph.reset(new Paragraph(m_styleStack));
  m_currentParagraph->style = style;
  m_styleStack.push();
  m_styleStack.set(style);
}

void IWORKText::closeParagraph()
{
  assert(bool(m_currentParagraph));

  m_paragraphs.push_back(m_currentParagraph);
  m_currentParagraph.reset();
  m_styleStack.pop();
}

void IWORKText::insertText(const std::string &text, const IWORKStylePtr_t &style, IWORKOutputElements &elements)
{
  assert(bool(m_currentParagraph));

  // const IWORKObjectPtr_t object(new TextSpanObject(style, m_styleStack, text));
  // m_currentParagraph->objects.push_back(object);
  drawTextSpan(style, m_styleStack, text, elements);
}

void IWORKText::insertTab(IWORKOutputElements &elements)
{
  assert(bool(m_currentParagraph));

  drawTab(elements);
}

void IWORKText::insertLineBreak()
{
  assert(bool(m_currentParagraph));

  ++m_lineBreaks;
}

const IWORKStylePtr_t &IWORKText::getLayoutStyle() const
{
  return m_layoutStyle;
}

const IWORKText::ParagraphList_t &IWORKText::getParagraphs() const
{
  return m_paragraphs;
}

bool IWORKText::isObject() const
{
  return m_object;
}

void IWORKText::insertDeferredLineBreaks(IWORKOutputElements &elements)
{
  assert(bool(m_currentParagraph));

  if (0 < m_lineBreaks)
  {
    for (int i = 0; i < m_lineBreaks; ++i)
      drawLineBreak(m_currentParagraph->m_styleStack, elements);

    m_lineBreaks = 0;
  }
}

bool IWORKText::empty() const
{
  return m_paragraphs.empty();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
