/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include <boost/optional.hpp>

#include <librevenge/librevenge.h>

#include "IWORKPath.h"
#include "IWORKTransformation.h"
#include "IWORKTypes.h"
#include "KEYStyles.h"
#include "KEYText.h"
#include "KEYTypes.h"

using boost::optional;

using std::string;

namespace libetonyek
{

struct KEYText::Paragraph
{
  KEYStyleContext m_styleContext;
  KEYParagraphStylePtr_t style;
  KEYObjectList_t objects;

  explicit Paragraph(const KEYStyleContext &styleContext);
};

namespace
{

librevenge::RVNGString makeColor(const IWORKColor &color)
{
  // TODO: alpha

  const unsigned r = color.red * 256 - 0.5;
  const unsigned g = color.green * 256 - 0.5;
  const unsigned b = color.blue * 256 - 0.5;

  librevenge::RVNGString str;
  str.sprintf("#%.2x%.2x%.2x", r, g, b);

  return str;
}

void fillCharPropList(librevenge::RVNGPropertyList &props, const KEYCharacterStyle &style, const KEYStyleContext &context)
{
  if (style.getItalic(context))
    props.insert("fo:font-style", "italic");
  if (style.getBold(context))
    props.insert("fo:font-weight", "bold");
  if (style.getUnderline(context))
    props.insert("style:text-underline-type", "single");
  if (style.getStrikethru(context))
    props.insert("style:text-line-through-type", "single");
  if (style.getOutline(context))
    props.insert("style:text-outline", true);

  const optional<IWORKCapitalization> capitalization = style.getCapitalization(context);
  if (capitalization)
  {
    if (IWORK_CAPITALIZATION_SMALL_CAPS == get(capitalization))
      props.insert("fo:font-variant", "small-caps");
  }

  const optional<string> fontName = style.getFontName(context);
  if (fontName)
    props.insert("style:font-name", librevenge::RVNGString(get(fontName).c_str()));

  const optional<double> fontSize = style.getFontSize(context);
  if (fontSize)
    props.insert("fo:font-size", pt2in(get(fontSize)));

  const optional<IWORKColor> fontColor = style.getFontColor(context);
  if (fontColor)
    props.insert("fo:color", makeColor(get(fontColor)));
}

KEYCharacterStyle makeEmptyStyle()
{
  optional<string> dummy;
  return KEYCharacterStyle(IWORKPropertyMap(), dummy, dummy);
}

librevenge::RVNGPropertyList makePropList(const KEYCharacterStylePtr_t &style, const KEYStyleContext &context)
{
  librevenge::RVNGPropertyList props;

  // Even if there is no character style for the span, there might still
  // be attributes inherited from the paragraph style through context.
  // We use an empty style so these can be picked up.
  fillCharPropList(props, bool(style) ? *style : makeEmptyStyle(), context);

  return props;
}

librevenge::RVNGPropertyList makePropList(const KEYParagraphStylePtr_t &style, const KEYStyleContext &context)
{
  librevenge::RVNGPropertyList props;

  if (bool(style))
  {
    const optional<IWORKAlignment> alignment(style->getAlignment(context));
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

    const optional<IWORKTabStops_t> &tabStops = style->getTabs(context);
    if (bool(tabStops))
    {
      for (IWORKTabStops_t::const_iterator it = get(tabStops).begin(); get(tabStops).end() != it; ++it)
      {
        librevenge::RVNGPropertyList tab;
        tab.insert("style:position", pt2in(it->pos));
        tab.insert("style:type", "left");
      }
    }
  }

  return props;
}

}

namespace
{

class TextSpanObject : public KEYObject
{
public:
  TextSpanObject(const KEYCharacterStylePtr_t &style, const KEYStyleContext &styleContext, const string &text);

private:
  virtual void draw(librevenge::RVNGPresentationInterface *painter);

private:
  const KEYCharacterStylePtr_t m_style;
  const KEYStyleContext m_styleContext;
  const string m_text;
};

TextSpanObject::TextSpanObject(const KEYCharacterStylePtr_t &style, const KEYStyleContext &styleContext, const string &text)
  : m_style(style)
  , m_styleContext(styleContext)
  , m_text(text)
{
}

void TextSpanObject::draw(librevenge::RVNGPresentationInterface *const painter)
{
  const librevenge::RVNGPropertyList props(makePropList(m_style, m_styleContext));
  painter->openSpan(props);
  painter->insertText(librevenge::RVNGString(m_text.c_str()));
  painter->closeSpan();
}

}

namespace
{

class TabObject : public KEYObject
{
private:
  virtual void draw(librevenge::RVNGPresentationInterface *painter);
};

void TabObject::draw(librevenge::RVNGPresentationInterface *const painter)
{
  painter->openSpan(librevenge::RVNGPropertyList());
  painter->insertTab();
  painter->closeSpan();
}

}

namespace
{

class LineBreakObject : public KEYObject
{
public:
  explicit LineBreakObject(const KEYStyleContext &styleContext);

private:
  virtual void draw(librevenge::RVNGPresentationInterface *painter);

private:
  const KEYStyleContext m_styleContext;
};

LineBreakObject::LineBreakObject(const KEYStyleContext &styleContext)
  : m_styleContext(styleContext)
{
}

void LineBreakObject::draw(librevenge::RVNGPresentationInterface *const painter)
{
  painter->closeParagraph();
  const librevenge::RVNGPropertyList props(makePropList(KEYParagraphStylePtr_t(), m_styleContext));
  painter->openParagraph(props);
}

}

namespace
{

class TextObject : public KEYObject
{
public:
  TextObject(const IWORKGeometryPtr_t &boundingBox, const KEYText::ParagraphList_t &paragraphs, bool object, const IWORKTransformation &trafo);

private:
  virtual void draw(librevenge::RVNGPresentationInterface *painter);

private:
  const IWORKGeometryPtr_t m_boundingBox;
  const KEYText::ParagraphList_t m_paragraphs;
  const bool m_object;
  const IWORKTransformation m_trafo;
};

TextObject::TextObject(const IWORKGeometryPtr_t &boundingBox, const KEYText::ParagraphList_t &paragraphs, const bool object, const IWORKTransformation &trafo)
  : m_boundingBox(boundingBox)
  , m_paragraphs(paragraphs)
  , m_object(object)
  , m_trafo(trafo)
{
}

void TextObject::draw(librevenge::RVNGPresentationInterface *const painter)
{
  librevenge::RVNGPropertyList props;

  double x = 0;
  double y = 0;
  m_trafo(x, y);
  props.insert("svg:x", pt2in(x));
  props.insert("svg:y", pt2in(y));

  if (bool(m_boundingBox))
  {
    double w = m_boundingBox->naturalSize.width;
    double h = m_boundingBox->naturalSize.height;
    m_trafo(w, h, true);

    props.insert("svg:width", pt2in(w));
    props.insert("svg:height", pt2in(h));
  }

  IWORKPath path;
  path.appendMoveTo(0, 0);
  path.appendLineTo(0, 1);
  path.appendLineTo(1, 1);
  path.appendLineTo(1, 0);
  path.appendClose();
  path *= m_trafo;

  props.insert("svg:d", path.toWPG());

  if (m_object)
    painter->startTextObject(props);

  for (KEYText::ParagraphList_t::const_iterator it = m_paragraphs.begin(); m_paragraphs.end() != it; ++it)
  {
    const librevenge::RVNGPropertyList paraProps(makePropList((*it)->style, (*it)->m_styleContext));
    painter->openParagraph(paraProps);
    drawAll((*it)->objects, painter);
    painter->closeParagraph();
  }

  if (m_object)
    painter->endTextObject();
}

}

KEYText::KEYText(const bool object)
  : m_styleContext()
  , m_layoutStyle()
  , m_paragraphs()
  , m_currentParagraph()
  , m_lineBreaks(0)
  , m_object(object)
  , m_boundingBox()
{
}

KEYText::Paragraph::Paragraph(const KEYStyleContext &styleContext)
  : m_styleContext(styleContext)
  , style()
  , objects()
{
}

void KEYText::setLayoutStyle(const KEYLayoutStylePtr_t &style)
{
  assert(!m_layoutStyle);

  m_layoutStyle = style;
  m_styleContext.push();
  m_styleContext.set(style);
}

const IWORKGeometryPtr_t &KEYText::getBoundingBox() const
{
  return m_boundingBox;
}

void KEYText::setBoundingBox(const IWORKGeometryPtr_t &boundingBox)
{
  m_boundingBox = boundingBox;
}

void KEYText::openParagraph(const KEYParagraphStylePtr_t &style)
{
  assert(!m_currentParagraph);

  m_currentParagraph.reset(new Paragraph(m_styleContext));
  m_currentParagraph->style = style;
  m_styleContext.push();
  m_styleContext.set(style);
}

void KEYText::closeParagraph()
{
  assert(bool(m_currentParagraph));

  m_paragraphs.push_back(m_currentParagraph);
  m_currentParagraph.reset();
  m_styleContext.pop();
}

void KEYText::insertText(const std::string &text, const KEYCharacterStylePtr_t &style)
{
  assert(bool(m_currentParagraph));

  const KEYObjectPtr_t object(new TextSpanObject(style, m_styleContext, text));
  m_currentParagraph->objects.push_back(object);
}

void KEYText::insertTab()
{
  assert(bool(m_currentParagraph));

  const KEYObjectPtr_t object(new TabObject());
  m_currentParagraph->objects.push_back(object);
}

void KEYText::insertLineBreak()
{
  assert(bool(m_currentParagraph));

  ++m_lineBreaks;
}

const KEYLayoutStylePtr_t &KEYText::getLayoutStyle() const
{
  return m_layoutStyle;
}

const KEYText::ParagraphList_t &KEYText::getParagraphs() const
{
  return m_paragraphs;
}

bool KEYText::isObject() const
{
  return m_object;
}

void KEYText::insertDeferredLineBreaks()
{
  assert(bool(m_currentParagraph));

  if (0 < m_lineBreaks)
  {
    const KEYObjectPtr_t object(new LineBreakObject(m_currentParagraph->m_styleContext));
    m_currentParagraph->objects.insert(m_currentParagraph->objects.end(), m_lineBreaks, object);
    m_lineBreaks = 0;
  }
}

bool KEYText::empty() const
{
  return m_paragraphs.empty();
}

KEYObjectPtr_t makeObject(const KEYTextPtr_t &text, const IWORKTransformation &trafo)
{
  const KEYObjectPtr_t object(new TextObject(text->getBoundingBox(), text->getParagraphs(), text->isObject(), trafo));
  return object;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
