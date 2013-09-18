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

#include "KNOutput.h"
#include "KNText.h"

using std::string;

namespace libkeynote
{

struct KNText::Paragraph
{
  KNParagraphStylePtr_t style;
  KNObjectList_t objects;
};

namespace
{

// TODO: this might need the paragraph/layout style as well
class TextSpanObject : public KNObject
{
public:
  TextSpanObject(const KNCharacterStylePtr_t &style, const string &text);

private:
  virtual void draw(const KNOutput &output);

private:
  const KNCharacterStylePtr_t m_style;
  const string m_text;
};

TextSpanObject::TextSpanObject(const KNCharacterStylePtr_t &style, const string &text)
  : m_style(style)
  , m_text(text)
{
}

void TextSpanObject::draw(const KNOutput &output)
{
  WPXPropertyList props;
  // TODO: fill properties

  output.getPainter()->startTextSpan(props);
  output.getPainter()->insertText(WPXString(m_text.c_str()));
  output.getPainter()->endTextSpan();
}

}

namespace
{

class TabObject : public KNObject
{
private:
  virtual void draw(const KNOutput &output);
};

void TabObject::draw(const KNOutput &output)
{
  output.getPainter()->startTextSpan(WPXPropertyList());
  output.getPainter()->insertText(WPXString("\t"));
  output.getPainter()->endTextSpan();
}

}

namespace
{

class LineBreakObject : public KNObject
{
public:
  explicit LineBreakObject(const KNParagraphStylePtr_t &paraStyle);

private:
  virtual void draw(const KNOutput &output);

private:
  const KNParagraphStylePtr_t m_paraStyle;
};

LineBreakObject::LineBreakObject(const KNParagraphStylePtr_t &paraStyle)
  : m_paraStyle(paraStyle)
{
}

void LineBreakObject::draw(const KNOutput &output)
{
  WPXPropertyList props;
  // TODO: fill from m_paraStyle

  output.getPainter()->endTextLine();
  output.getPainter()->startTextLine(props);
}

}

namespace
{

class TextObject : public KNObject
{
public:
  TextObject(const KNLayoutStylePtr_t &layoutStyle, const KNText::ParagraphList_t &paragraphs);

private:
  virtual void draw(const KNOutput &output);

private:
  const KNLayoutStylePtr_t m_layoutStyle;
  const KNText::ParagraphList_t m_paragraphs;
};

TextObject::TextObject(const KNLayoutStylePtr_t &layoutStyle, const KNText::ParagraphList_t &paragraphs)
  : m_layoutStyle(layoutStyle)
  , m_paragraphs(paragraphs)
{
}

void TextObject::draw(const KNOutput &output)
{
  WPXPropertyList props;
  // TODO: fill properties

  output.getPainter()->startTextObject(props, WPXPropertyListVector());

  for(KNText::ParagraphList_t::const_iterator it = m_paragraphs.begin(); m_paragraphs.end() != it; ++it)
  {
    output.getPainter()->startTextLine(WPXPropertyList());
    for (KNObjectList_t::const_iterator objIt = (*it)->objects.begin(); (*it)->objects.end() != objIt; ++objIt)
      (*objIt)->draw(output);
    output.getPainter()->endTextLine();
  }

  output.getPainter()->endTextObject();
}

}

KNText::KNText()
  : m_layoutStyle()
  , m_paragraphs()
  , m_currentParagraph()
  , m_lineBreaks(0)
{
}

void KNText::setLayoutStyle(const KNLayoutStylePtr_t &style)
{
  m_layoutStyle = style;
}

void KNText::openParagraph(const KNParagraphStylePtr_t &style)
{
  assert(!m_currentParagraph);

  m_currentParagraph.reset(new Paragraph());
  m_currentParagraph->style = style;
}

void KNText::closeParagraph()
{
  assert(bool(m_currentParagraph));

  m_paragraphs.push_back(m_currentParagraph);
  m_currentParagraph.reset();
}

void KNText::insertText(const std::string &text, const KNCharacterStylePtr_t &style)
{
  assert(bool(m_currentParagraph));

  const KNObjectPtr_t object(new TextSpanObject(style, text));
  m_currentParagraph->objects.push_back(object);
}

void KNText::insertTab()
{
  assert(bool(m_currentParagraph));

  const KNObjectPtr_t object(new TabObject());
  m_currentParagraph->objects.push_back(object);
}

void KNText::insertLineBreak()
{
  assert(bool(m_currentParagraph));

  ++m_lineBreaks;
}

const KNLayoutStylePtr_t &KNText::getLayoutStyle() const
{
  return m_layoutStyle;
}

const KNText::ParagraphList_t &KNText::getParagraphs() const
{
  return m_paragraphs;
}

void KNText::insertDeferredLineBreaks()
{
  assert(bool(m_currentParagraph));

  if (0 < m_lineBreaks)
  {
    const KNObjectPtr_t object(new LineBreakObject(m_currentParagraph->style));
    m_currentParagraph->objects.insert(m_currentParagraph->objects.end(), m_lineBreaks, object);
    m_lineBreaks = 0;
  }
}

KNObjectPtr_t makeObject(const KNTextPtr_t &text)
{
  const KNObjectPtr_t object(new TextObject(text->getLayoutStyle(), text->getParagraphs()));
  return object;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
