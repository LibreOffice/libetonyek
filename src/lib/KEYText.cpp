/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include <libwpd/libwpd.h>

#include <libwpg/libwpg.h>

#include "KEYOutput.h"
#include "KEYPath.h"
#include "KEYText.h"

using std::string;

namespace libkeynote
{

struct KEYText::Paragraph
{
  KEYParagraphStylePtr_t style;
  KEYObjectList_t objects;

  Paragraph();
};

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
  WPXPropertyList props;
  // TODO: fill properties

  output.getPainter()->setStyle(WPXPropertyList(), WPXPropertyListVector());
  output.getPainter()->startTextSpan(props);
  output.getPainter()->insertText(WPXString(m_text.c_str()));
  output.getPainter()->endTextSpan();
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
  output.getPainter()->startTextSpan(WPXPropertyList());
  output.getPainter()->insertText(WPXString("\t"));
  output.getPainter()->endTextSpan();
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
  WPXPropertyList props;
  // TODO: fill from m_paraStyle

  output.getPainter()->endTextLine();
  output.getPainter()->startTextLine(props);
}

}

namespace
{

class TextObject : public KEYObject
{
public:
  TextObject(const KEYLayoutStylePtr_t &layoutStyle, const KEYText::ParagraphList_t &paragraphs);

private:
  virtual void draw(const KEYOutput &output);

private:
  const KEYLayoutStylePtr_t m_layoutStyle;
  const KEYText::ParagraphList_t m_paragraphs;
};

TextObject::TextObject(const KEYLayoutStylePtr_t &layoutStyle, const KEYText::ParagraphList_t &paragraphs)
  : m_layoutStyle(layoutStyle)
  , m_paragraphs(paragraphs)
{
}

void TextObject::draw(const KEYOutput &output)
{
  WPXPropertyList props;
  // TODO: fill properties

  KEYPath path;
  path.appendMoveTo(0, 0);
  path.appendLineTo(0, 1);
  path.appendLineTo(1, 1);
  path.appendLineTo(1, 0);
  path.appendClose();
  path *= output.getTransformation();

  output.getPainter()->startTextObject(props, path.toWPG());

  for(KEYText::ParagraphList_t::const_iterator it = m_paragraphs.begin(); m_paragraphs.end() != it; ++it)
  {
    output.getPainter()->startTextLine(WPXPropertyList());
    drawAll((*it)->objects, output);
    output.getPainter()->endTextLine();
  }

  output.getPainter()->endTextObject();
}

}

KEYText::KEYText()
  : m_layoutStyle()
  , m_paragraphs()
  , m_currentParagraph()
  , m_lineBreaks(0)
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
  const KEYObjectPtr_t object(new TextObject(text->getLayoutStyle(), text->getParagraphs()));
  return object;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */