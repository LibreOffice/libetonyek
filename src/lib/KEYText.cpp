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

#include <libwpd/libwpd.h>

#include <libetonyek/KEYPresentationInterface.h>

#include "KEYOutput.h"
#include "KEYPath.h"
#include "KEYStyles.h"
#include "KEYText.h"
#include "KEYTypes.h"

using boost::optional;

using std::string;

namespace libetonyek
{

struct KEYText::Paragraph
{
  KEYParagraphStylePtr_t style;
  KEYObjectList_t objects;

  Paragraph();
};

namespace
{

WPXString makeColor(const KEYColor &color)
{
  // TODO: alpha

  const unsigned r = color.red * 256 - 0.5;
  const unsigned g = color.green * 256 - 0.5;
  const unsigned b = color.blue * 256 - 0.5;

  WPXString str;
  str.sprintf("#%.2x%.2x%.2x", r, g, b);

  return str;
}

WPXPropertyListVector makeTabStops(const KEYParagraphStylePtr_t &style, const KEYStyleContext &context)
{
  WPXPropertyListVector tabs;

  if (bool(style))
  {
    const optional<KEYTabStops_t> &tabStops = style->getTabs(context);

    if (bool(tabStops))
    {
      for (KEYTabStops_t::const_iterator it = get(tabStops).begin(); get(tabStops).end() != it; ++it)
      {
        WPXPropertyList tab;
        tab.insert("style:position", pt2in(it->pos));
        tab.insert("style:type", "left");
      }
    }
  }

  return tabs;
}

void fillCharPropList(WPXPropertyList &props, const KEYCharacterStyle &style, const KEYStyleContext &context)
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

  const optional<KEYCapitalization> capitalization = style.getCapitalization(context);
  if (capitalization)
  {
    if (KEY_CAPITALIZATION_SMALL_CAPS == get(capitalization))
      props.insert("fo:font-variant", "small-caps");
  }

  const optional<string> fontName = style.getFontName(context);
  if (fontName)
    props.insert("style:font-name", WPXString(get(fontName).c_str()));

  const optional<double> fontSize = style.getFontSize(context);
  if (fontSize)
    props.insert("fo:font-size", pt2in(get(fontSize)));

  const optional<KEYColor> fontColor = style.getFontColor(context);
  if (fontColor)
    props.insert("fo:color", makeColor(get(fontColor)));
}

KEYCharacterStyle makeEmptyStyle()
{
  optional<string> dummy;
  return KEYCharacterStyle(KEYPropertyMap(), dummy, dummy);
}

WPXPropertyList makePropList(const KEYCharacterStylePtr_t &style, const KEYStyleContext &context)
{
  WPXPropertyList props;

  // Even if there is no character style for the span, there might still
  // be attributes inherited from the paragraph style through context.
  // We use an empty style so these can be picked up.
  fillCharPropList(props, bool(style) ? *style : makeEmptyStyle(), context);

  return props;
}

WPXPropertyList makePropList(const KEYParagraphStylePtr_t &style, const KEYStyleContext &context)
{
  WPXPropertyList props;

  if (bool(style))
  {
    const optional<KEYAlignment> alignment(style->getAlignment(context));
    if (bool(alignment))
    {
      switch (get(alignment))
      {
      case KEY_ALIGNMENT_LEFT :
        props.insert("fo:text-align", "left");
        break;
      case KEY_ALIGNMENT_RIGHT :
        props.insert("fo:text-align", "right");
        break;
      case KEY_ALIGNMENT_CENTER :
        props.insert("fo:text-align", "center");
        break;
      case KEY_ALIGNMENT_JUSTIFY :
        props.insert("fo:text-align", "justify");
        break;
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
  TextSpanObject(const KEYCharacterStylePtr_t &style, const string &text);

private:
  virtual void draw(const KEYOutput &output);

private:
  const KEYCharacterStylePtr_t m_style;
  const string m_text;
};

TextSpanObject::TextSpanObject(const KEYCharacterStylePtr_t &style, const string &text)
  : m_style(style)
  , m_text(text)
{
}

void TextSpanObject::draw(const KEYOutput &output)
{
  const WPXPropertyList props(makePropList(m_style, output.getStyleContext()));
  output.getPainter()->openSpan(props);
  output.getPainter()->insertText(WPXString(m_text.c_str()));
  output.getPainter()->closeSpan();
}

}

namespace
{

class TabObject : public KEYObject
{
private:
  virtual void draw(const KEYOutput &output);
};

void TabObject::draw(const KEYOutput &output)
{
  output.getPainter()->openSpan(WPXPropertyList());
  output.getPainter()->insertTab();
  output.getPainter()->closeSpan();
}

}

namespace
{

class LineBreakObject : public KEYObject
{
public:
  explicit LineBreakObject(const KEYParagraphStylePtr_t &paraStyle);

private:
  virtual void draw(const KEYOutput &output);

private:
  const KEYParagraphStylePtr_t m_paraStyle;
};

LineBreakObject::LineBreakObject(const KEYParagraphStylePtr_t &paraStyle)
  : m_paraStyle(paraStyle)
{
}

void LineBreakObject::draw(const KEYOutput &output)
{
  output.getPainter()->closeParagraph();
  const WPXPropertyList props(makePropList(m_paraStyle, output.getStyleContext()));
  output.getPainter()->openParagraph(props, WPXPropertyListVector());
}

}

namespace
{

class TextObject : public KEYObject
{
public:
  TextObject(const KEYLayoutStylePtr_t &layoutStyle, const KEYGeometryPtr_t &boundingBox, const KEYText::ParagraphList_t &paragraphs, bool object);

private:
  virtual void draw(const KEYOutput &output);

private:
  const KEYLayoutStylePtr_t m_layoutStyle;
  const KEYGeometryPtr_t m_boundingBox;
  const KEYText::ParagraphList_t m_paragraphs;
  const bool m_object;
};

TextObject::TextObject(const KEYLayoutStylePtr_t &layoutStyle, const KEYGeometryPtr_t &boundingBox, const KEYText::ParagraphList_t &paragraphs, const bool object)
  : m_layoutStyle(layoutStyle)
  , m_boundingBox(boundingBox)
  , m_paragraphs(paragraphs)
  , m_object(object)
{
}

void TextObject::draw(const KEYOutput &output)
{
  const KEYTransformation tr = output.getTransformation();

  WPXPropertyList props;

  double x = 0;
  double y = 0;
  tr(x, y);
  props.insert("svg:x", pt2in(x));
  props.insert("svg:y", pt2in(y));

  if (bool(m_boundingBox))
  {
    double w = m_boundingBox->naturalSize.width;
    double h = m_boundingBox->naturalSize.height;
    tr(w, h, true);

    props.insert("svg:width", pt2in(w));
    props.insert("svg:height", pt2in(h));
  }

  KEYPath path;
  path.appendMoveTo(0, 0);
  path.appendLineTo(0, 1);
  path.appendLineTo(1, 1);
  path.appendLineTo(1, 0);
  path.appendClose();
  path *= tr;

  if (m_object)
    output.getPainter()->startTextObject(props, path.toWPG());

  for (KEYText::ParagraphList_t::const_iterator it = m_paragraphs.begin(); m_paragraphs.end() != it; ++it)
  {
    const WPXPropertyList paraProps(makePropList((*it)->style, output.getStyleContext()));
    const WPXPropertyListVector tabStops(makeTabStops((*it)->style, output.getStyleContext()));
    output.getPainter()->openParagraph(paraProps, tabStops);
    const KEYOutput paraOutput(output, (*it)->style);
    drawAll((*it)->objects, paraOutput);
    output.getPainter()->closeParagraph();
  }

  if (m_object)
    output.getPainter()->endTextObject();
}

}

KEYText::KEYText(const bool object)
  : m_layoutStyle()
  , m_paragraphs()
  , m_currentParagraph()
  , m_lineBreaks(0)
  , m_object(object)
  , m_boundingBox()
{
}

KEYText::Paragraph::Paragraph()
  : style()
  , objects()
{
}

void KEYText::setLayoutStyle(const KEYLayoutStylePtr_t &style)
{
  m_layoutStyle = style;
}

const KEYGeometryPtr_t &KEYText::getBoundingBox() const
{
  return m_boundingBox;
}

void KEYText::setBoundingBox(const KEYGeometryPtr_t &boundingBox)
{
  m_boundingBox = boundingBox;
}

void KEYText::openParagraph(const KEYParagraphStylePtr_t &style)
{
  assert(!m_currentParagraph);

  m_currentParagraph.reset(new Paragraph());
  m_currentParagraph->style = style;
}

void KEYText::closeParagraph()
{
  assert(bool(m_currentParagraph));

  m_paragraphs.push_back(m_currentParagraph);
  m_currentParagraph.reset();
}

void KEYText::insertText(const std::string &text, const KEYCharacterStylePtr_t &style)
{
  assert(bool(m_currentParagraph));

  const KEYObjectPtr_t object(new TextSpanObject(style, text));
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
    const KEYObjectPtr_t object(new LineBreakObject(m_currentParagraph->style));
    m_currentParagraph->objects.insert(m_currentParagraph->objects.end(), m_lineBreaks, object);
    m_lineBreaks = 0;
  }
}

bool KEYText::empty() const
{
  return m_paragraphs.empty();
}

KEYObjectPtr_t makeObject(const KEYTextPtr_t &text)
{
  const KEYObjectPtr_t object(new TextObject(text->getLayoutStyle(), text->getBoundingBox(), text->getParagraphs(), text->isObject()));
  return object;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
