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
#include <string>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>

#include <librevenge/librevenge.h>

#include "IWORKDocumentInterface.h"
#include "IWORKPath.h"
#include "IWORKProperties.h"
#include "IWORKTypes.h"

using boost::optional;

using std::string;

namespace libetonyek
{

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

void fillCharPropList(librevenge::RVNGPropertyList &props, const IWORKStyleStack &style)
{
  using namespace property;

  if (style.has<Italic>() && style.get<Italic>())
    props.insert("fo:font-style", "italic");
  if (style.has<Bold>() && style.get<Bold>())
    props.insert("fo:font-weight", "bold");
  if (style.has<Underline>() && style.get<Underline>())
    props.insert("style:text-underline-type", "single");
  if (style.has<Strikethru>() && style.get<Strikethru>())
    props.insert("style:text-line-through-type", "single");
  if (style.has<Outline>() && style.get<Outline>())
    props.insert("style:text-outline", true);
  if (style.has<Tracking>())
    props.insert("fo:letter-spacing", 1 + style.get<Tracking>(), librevenge::RVNG_PERCENT);

  if (style.has<Baseline>())
  {
    switch (style.get<Baseline>())
    {
    case IWORK_BASELINE_SUB :
      props.insert("style:text-position", "sub");
      break;
    case IWORK_BASELINE_SUPER :
      props.insert("style:text-position", "super");
      break;
    default :
      break;
    }
  }
  else if (style.has<BaselineShift>())
  {
    props.insert("style:text-position", style.get<BaselineShift>(), librevenge::RVNG_PERCENT);
  }

  if (style.has<Capitalization>())
  {
    switch (style.get<Capitalization>())
    {
    case IWORK_CAPITALIZATION_ALL_CAPS :
      props.insert("fo:text-transform", "uppercase");
      break;
    case IWORK_CAPITALIZATION_SMALL_CAPS :
      props.insert("fo:font-variant", "small-caps");
      break;
    case IWORK_CAPITALIZATION_TITLE :
      props.insert("fo:text-transform", "capitalize");
      break;
    default :
      break;
    }
  }

  if (style.has<FontName>())
    props.insert("style:font-name", librevenge::RVNGString(style.get<FontName>().c_str()));

  if (style.has<FontSize>())
    props.insert("fo:font-size", pt2in(style.get<FontSize>()));

  if (style.has<FontColor>())
    props.insert("fo:color", makeColor(style.get<FontColor>()));
  if (style.has<TextBackground>())
    props.insert("fo:background-color", makeColor(style.get<TextBackground>()));

  if (style.has<Language>())
  {
    const string &lang = style.get<Language>();

    std::vector<string> components;
    components.reserve(2);
    boost::split(components, lang, boost::is_any_of("_"));

    if (2 != components.size())
    {
      ETONYEK_DEBUG_MSG(("irregular lang specifier '%s'", lang.c_str()));
    }

    if ((1 <= components.size()) && (2 <= components[0].size()) && (3 >= components[0].size()) && boost::all(components[0], boost::is_lower()))
      props.insert("fo:language", components[0].c_str());
    if ((2 <= components.size()) && (2 == components[1].size()) && boost::all(components[1], boost::is_upper()))
      props.insert("fo:country", components[1].c_str());
  }
}

librevenge::RVNGPropertyList makeCharPropList(const IWORKStylePtr_t &style, const IWORKStyleStack &context)
{
  librevenge::RVNGPropertyList props;

  IWORKStyleStack styleStack(context);
  styleStack.push();
  styleStack.set(style);

  fillCharPropList(props, styleStack);

  return props;
}

librevenge::RVNGPropertyList makeParaPropList(const IWORKStylePtr_t &style, const IWORKStyleStack &context)
{
  using namespace property;

  librevenge::RVNGPropertyList props;

  if (bool(style))
  {
    IWORKStyleStack styleStack(context);
    styleStack.push();
    styleStack.set(style);

    if (styleStack.has<Alignment>())
    {
      const IWORKAlignment &alignment(styleStack.get<Alignment>());
      switch (alignment)
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

    if (styleStack.has<LineSpacing>())
    {
      const IWORKLineSpacing &spacing = styleStack.get<LineSpacing>();
      if (spacing.m_relative)
        props.insert("fo:line-height", spacing.m_value, librevenge::RVNG_PERCENT);
      else
        props.insert("fo:line-height", pt2in(spacing.m_value));
    }

    if (styleStack.has<ParagraphFill>())
      props.insert("fo:background-color", makeColor(styleStack.get<ParagraphFill>()));

    if (styleStack.has<LeftIndent>())
      props.insert("fo:padding-left", pt2in(styleStack.get<LeftIndent>()));
    if (styleStack.has<RightIndent>())
      props.insert("fo:padding-right", pt2in(styleStack.get<RightIndent>()));
    if (styleStack.has<FirstLineIndent>())
      props.insert("fo:text-indent", pt2in(styleStack.get<FirstLineIndent>()));

    if (styleStack.has<SpaceBefore>())
      props.insert("fo:padding-top", pt2in(styleStack.get<SpaceBefore>()));
    if (styleStack.has<SpaceAfter>())
      props.insert("fo:padding-bottom", pt2in(styleStack.get<SpaceAfter>()));

    if (styleStack.has<KeepLinesTogether>() && styleStack.get<KeepLinesTogether>())
      props.insert("fo:keep-together", "always");
    if (styleStack.has<KeepWithNext>() && styleStack.get<KeepWithNext>())
      props.insert("fo:keep-with-next", "always");
    // Orphans and widows are covered by a single setting. The number of lines is not adjustable.
    const bool enableWidows = styleStack.has<WidowControl>() && !styleStack.get<WidowControl>();
    props.insert("orphans", enableWidows ? "0" : "2");
    props.insert("widows", enableWidows ? "0" : "2");

    if (styleStack.has<Tabs>())
    {
      librevenge::RVNGPropertyListVector tabs;

      const IWORKTabStops_t &tabStops = styleStack.get<Tabs>();
      for (IWORKTabStops_t::const_iterator it = tabStops.begin(); tabStops.end() != it; ++it)
      {
        librevenge::RVNGPropertyList tab;
        tab.insert("style:position", pt2in(it->m_pos));
        tab.insert("style:type", "left");
        tabs.append(tab);
      }

      props.insert("librevenge:tab-stops", tabs);
    }

    if (styleStack.has<ParagraphBorderType>())
    {
      librevenge::RVNGString border;

      if (styleStack.has<ParagraphStroke>())
      {
        const IWORKStroke &stroke = styleStack.get<ParagraphStroke>();
        border.sprintf("%fpt", stroke.m_width);

        // The format can represent arbitrary patterns, but we have to
        // fit them to the limited number of options ODF allows...
        if (stroke.m_pattern.size() >= 2)
        {
          const double x = stroke.m_pattern[0];
          const double y = stroke.m_pattern[1];
          if (((x / y) < 0.01) || ((y / x) < 0.01)) // arbitrarily picked constant
            border.append(" dotted");
          else
            border.append(" dashed");
        }
        else
        {
          border.append(" solid");
        }

        border.append(" ");
        border.append(makeColor(stroke.m_color));
      }

      switch (styleStack.get<ParagraphBorderType>())
      {
      case IWORK_BORDER_TYPE_TOP :
        props.insert("fo:border-top", border);
        break;
      case IWORK_BORDER_TYPE_BOTTOM :
        props.insert("fo:border-bottom", border);
        break;
      case IWORK_BORDER_TYPE_TOP_AND_BOTTOM :
        props.insert("fo:border-top", border);
        props.insert("fo:border-bottom", border);
        break;
      case IWORK_BORDER_TYPE_ALL :
        props.insert("fo:border", border);
        break;
      default :
        break;
      }
    }

    if (styleStack.has<PageBreakBefore>())
      props.insert("fo:break-before", "page");
  }

  return props;
}

}

void IWORKText::draw(IWORKOutputElements &elements)
{
  elements.append(m_elements);
}

void IWORKText::draw(const glm::dmat3 &trafo, const IWORKGeometryPtr_t &boundingBox, IWORKOutputElements &elements)
{
  librevenge::RVNGPropertyList props;

  glm::dvec3 vec = trafo * glm::dvec3(0, 0, 1);

  props.insert("svg:x", pt2in(vec[0]));
  props.insert("svg:y", pt2in(vec[1]));

  if (bool(boundingBox))
  {
    double w = boundingBox->m_naturalSize.m_width;
    double h = boundingBox->m_naturalSize.m_height;
    vec = trafo * glm::dvec3(w, h, 0);

    props.insert("svg:width", pt2in(vec[0]));
    props.insert("svg:height", pt2in(vec[1]));
  }

  IWORKPath path;
  path.appendMoveTo(0, 0);
  path.appendLineTo(0, 1);
  path.appendLineTo(1, 1);
  path.appendLineTo(1, 0);
  path.appendClose();
  path *= trafo;

  props.insert("svg:d", path.toWPG());

  elements.addStartTextObject(props);
  draw(elements);
  elements.addEndTextObject();

}

IWORKText::IWORKText()
  : m_styleStack()
  , m_elements()
  , m_currentParaStyle()
  , m_paraOpened(false)
  , m_ignoreEmptyPara(false)
  , m_currentSpanStyle()
  , m_spanOpened(false)
  , m_pendingSpanClose(false)
  , m_inSpan(false)
{
}

void IWORKText::openParagraph(const IWORKStylePtr_t &style)
{
  assert(!m_paraOpened);

  // A paragraph might have to be closed an then opened again, if there
  // is a block content in it. That is why we only open them when needed.
  m_currentParaStyle = style;
}

void IWORKText::closeParagraph()
{
  if (!m_paraOpened && !m_ignoreEmptyPara)
    doOpenPara(); // empty paragraphs are allowed, contrary to empty spans
  if (m_paraOpened)
    doClosePara();
  m_ignoreEmptyPara = false;
}

void IWORKText::openSpan(const IWORKStylePtr_t &style)
{
  if (m_inSpan) // there was an implicit span
    m_pendingSpanClose = true;
  m_currentSpanStyle = style;
}

void IWORKText::closeSpan()
{
  m_currentSpanStyle.reset();
  m_pendingSpanClose = true;
}

void IWORKText::openLink(const std::string &url)
{
  if (!m_paraOpened)
    doOpenPara();
  if (m_spanOpened)
    doCloseSpan(); // A link is always outside of a span

  librevenge::RVNGPropertyList props;
  props.insert("xlink:type", "simple");
  props.insert("xlink:href", url.c_str());
  m_elements.addOpenLink(props);
}

void IWORKText::closeLink()
{
  if (m_spanOpened)
    doCloseSpan();

  m_elements.addCloseLink();
}

void IWORKText::insertText(const std::string &text)
{
  flushSpan();
  m_elements.addInsertText(librevenge::RVNGString(text.c_str()));
  m_inSpan = true;
}

void IWORKText::insertTab()
{
  flushSpan();
  m_elements.addInsertTab();
  m_inSpan = true;
}

void IWORKText::insertLineBreak()
{
  flushSpan();
  m_elements.addInsertLineBreak();
  m_inSpan = true;
}

void IWORKText::insertInlineContent(const IWORKOutputElements &elements)
{
  flushSpan();
  m_elements.append(elements);
  m_inSpan = true;
}

void IWORKText::insertBlockContent(const IWORKOutputElements &elements)
{
  if (m_paraOpened)
    doClosePara();
  m_elements.append(elements);
  m_ignoreEmptyPara = true;
}

bool IWORKText::empty() const
{
  return m_elements.empty();
}

void IWORKText::doOpenPara()
{
  assert(!m_paraOpened);

  const librevenge::RVNGPropertyList paraProps(makeParaPropList(m_currentParaStyle, m_styleStack));
  m_elements.addOpenParagraph(paraProps);
  m_paraOpened = true;
  m_styleStack.push();
  m_styleStack.set(m_currentParaStyle);
}

void IWORKText::doClosePara()
{
  assert(m_paraOpened);

  if (m_spanOpened)
    doCloseSpan();

  m_elements.addCloseParagraph();
  m_paraOpened = false;
  m_styleStack.pop();
}

void IWORKText::doOpenSpan()
{
  assert(!m_pendingSpanClose);

  if (!m_paraOpened)
    doOpenPara();
  assert(m_paraOpened);

  const librevenge::RVNGPropertyList props(makeCharPropList(m_currentSpanStyle, m_styleStack));
  m_elements.addOpenSpan(props);
  m_spanOpened = true;
}

void IWORKText::doCloseSpan()
{
  assert(m_paraOpened);

  if (m_spanOpened)
  {
    m_elements.addCloseSpan();
    m_spanOpened = false;
  }
  m_pendingSpanClose = false;
  m_inSpan = false;
}

void IWORKText::flushSpan()
{
  if (m_pendingSpanClose)
    doCloseSpan();
  if (!m_spanOpened)
    doOpenSpan();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
