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

#include "KNText.h"

using std::string;

namespace libkeynote
{

struct KNText::Paragraph
{
  KNStylePtr_t style;
  KNObjectList_t objects;
};

namespace
{

// TODO: this might need the paragraph/layout style as well
class TextSpanObject : public KNObject
{
public:
  TextSpanObject(const KNStylePtr_t &style, const string &text);

private:
  virtual void draw(libwpg::WPGPaintInterface *painter, const KNTransformation &tr);

private:
  const KNStylePtr_t m_style;
  const string m_text;
};

TextSpanObject::TextSpanObject(const KNStylePtr_t &style, const string &text)
  : m_style(style)
  , m_text(text)
{
}

void TextSpanObject::draw(libwpg::WPGPaintInterface *const painter, const KNTransformation &)
{
  WPXPropertyList props;
  // TODO: fill properties

  painter->startTextSpan(props);
  painter->insertText(WPXString(m_text.c_str()));
  painter->endTextSpan();
}

}

namespace
{

class TabObject : public KNObject
{
private:
  virtual void draw(libwpg::WPGPaintInterface *painter, const KNTransformation &tr);
};

void TabObject::draw(libwpg::WPGPaintInterface *const painter, const KNTransformation &)
{
  painter->startTextSpan(WPXPropertyList());
  painter->insertText(WPXString("\t"));
  painter->endTextSpan();
}

}

namespace
{

class LineBreakObject : public KNObject
{
private:
  virtual void draw(libwpg::WPGPaintInterface *painter, const KNTransformation &tr);
};

void LineBreakObject::draw(libwpg::WPGPaintInterface *const painter, const KNTransformation &)
{
  painter->endTextLine();
  // TODO: this should restart with the same props as the previous line,
  // i.e., it needs access to the paragraph style
  painter->startTextLine(WPXPropertyList());
}

}

namespace
{

class TextObject : public KNObject
{
public:
  TextObject(const KNStylePtr_t &layoutStyle, const KNText::ParagraphList_t &paragraphs);

private:
  virtual void draw(libwpg::WPGPaintInterface *painter, const KNTransformation &tr);

private:
  const KNStylePtr_t m_layoutStyle;
  const KNText::ParagraphList_t m_paragraphs;
};

TextObject::TextObject(const KNStylePtr_t &layoutStyle, const KNText::ParagraphList_t &paragraphs)
  : m_layoutStyle(layoutStyle)
  , m_paragraphs(paragraphs)
{
}

void TextObject::draw(libwpg::WPGPaintInterface *const painter, const KNTransformation &tr)
{
  WPXPropertyList props;
  // TODO: fill properties

  painter->startTextObject(props, WPXPropertyListVector());

  for(KNText::ParagraphList_t::const_iterator it = m_paragraphs.begin(); m_paragraphs.end() != it; ++it)
  {
    painter->startTextLine(WPXPropertyList());
    for (KNObjectList_t::const_iterator objIt = (*it)->objects.begin(); (*it)->objects.end() != objIt; ++objIt)
      (*objIt)->draw(painter, tr);
    painter->endTextLine();
  }

  painter->endTextObject();
}

}

KNText::KNText()
  : m_layoutStyle()
  , m_paragraphs()
  , m_currentParagraph()
  , m_lineBreaks(0)
{
}

void KNText::setLayoutStyle(const KNStylePtr_t &style)
{
  m_layoutStyle = style;
}

void KNText::openParagraph(const KNStylePtr_t &style)
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

void KNText::insertText(const std::string &text, const KNStylePtr_t &style)
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

void KNText::insertDeferredLineBreaks()
{
  assert(bool(m_currentParagraph));

  if (0 < m_lineBreaks)
  {
    const KNObjectPtr_t object(new LineBreakObject());
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
