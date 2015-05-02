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
#include "IWORKProperties.h"
#include "IWORKTransformation.h"
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

  // TODO: handle baseline shift as well. It does interact with sub/superscript.
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
      props.insert("fo:padding-top", styleStack.get<SpaceBefore>());
    if (styleStack.has<SpaceAfter>())
      props.insert("fo:padding-bottom", styleStack.get<SpaceAfter>());

    if (styleStack.has<Tabs>())
    {
      const IWORKTabStops_t &tabStops = styleStack.get<Tabs>();
      for (IWORKTabStops_t::const_iterator it = tabStops.begin(); tabStops.end() != it; ++it)
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

void IWORKText::draw(IWORKOutputElements &elements)
{
  elements.append(m_elements);
}

void IWORKText::draw(const IWORKTransformation &trafo, const IWORKGeometryPtr_t &boundingBox, IWORKOutputElements &elements)
{
  librevenge::RVNGPropertyList props;

  double x = 0;
  double y = 0;
  trafo(x, y);
  props.insert("svg:x", pt2in(x));
  props.insert("svg:y", pt2in(y));

  if (bool(boundingBox))
  {
    double w = boundingBox->m_naturalSize.m_width;
    double h = boundingBox->m_naturalSize.m_height;
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

  elements.addStartTextObject(props);
  draw(elements);
  elements.addEndTextObject();

}

IWORKText::IWORKText()
  : m_styleStack()
  , m_elements()
  , m_spanOpened(false)
{
}

void IWORKText::openParagraph(const IWORKStylePtr_t &style)
{
  const librevenge::RVNGPropertyList paraProps(makeParaPropList(style, m_styleStack));
  m_elements.addOpenParagraph(paraProps);
  m_styleStack.push();
  m_styleStack.set(style);
}

void IWORKText::closeParagraph()
{
  if (m_spanOpened)
    closeSpan();

  m_elements.addCloseParagraph();
  m_styleStack.pop();
}

void IWORKText::openSpan(const IWORKStylePtr_t &style)
{
  if (m_spanOpened) // implicitly opened span
    closeSpan();

  const librevenge::RVNGPropertyList props(makeCharPropList(style, m_styleStack));
  m_elements.addOpenSpan(props);
  m_spanOpened = true;
}

void IWORKText::closeSpan()
{
  m_elements.addCloseSpan();
  m_spanOpened = false;
}

void IWORKText::insertText(const std::string &text)
{
  if (!m_spanOpened)
    openSpan(IWORKStylePtr_t());
  m_elements.addInsertText(librevenge::RVNGString(text.c_str()));
}

void IWORKText::insertTab()
{
  if (!m_spanOpened)
    openSpan(IWORKStylePtr_t());
  m_elements.addInsertTab();
}

void IWORKText::insertLineBreak()
{
  if (!m_spanOpened)
    openSpan(IWORKStylePtr_t());
  m_elements.addInsertLineBreak();
}

bool IWORKText::empty() const
{
  return m_elements.empty();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
