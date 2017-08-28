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
#include <memory>
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include <librevenge/librevenge.h>

#include "IWORKDocumentInterface.h"
#include "IWORKLanguageManager.h"
#include "IWORKPath.h"
#include "IWORKProperties.h"
#include "IWORKTextRecorder.h"
#include "IWORKTypes.h"


using librevenge::RVNGPropertyList;
using librevenge::RVNGPropertyListVector;

using std::string;

namespace libetonyek
{

namespace
{

void fillSectionPropList(const IWORKStyleStack &style, RVNGPropertyList &props)
{
  using namespace property;

  if (style.has<Columns>())
  {
    const IWORKColumns &columns = style.get<Columns>();

    if (columns.m_columns.size() > 1)
    {
      RVNGPropertyListVector vec;

      for (IWORKColumns::Columns_t::const_iterator it = columns.m_columns.begin(); it != columns.m_columns.end(); ++it)
      {
        RVNGPropertyList columnProps;
        // TODO: need to have real width of the section to be able to compute indents.
        columnProps.insert("style:rel-width", it->m_width, librevenge::RVNG_PERCENT);
        vec.append(columnProps);
      }
      props.insert("style:columns", vec);

      props.insert("text:dont-balance-text-columns", columns.m_equal ? "false" : "true");
    }
  }

  if (style.has<LayoutMargins>())
  {
    const IWORKPadding &padding = style.get<LayoutMargins>();

    if (padding.m_left)
      props.insert("fo:margin-left", pt2in(get(padding.m_left)));
    if (padding.m_right)
      props.insert("fo:margin-right", pt2in(get(padding.m_right)));
    if (padding.m_bottom)
      props.insert("librevenge:margin-bottom", pt2in(get(padding.m_bottom)));
  }
}
}

void IWORKText::fillCharPropList(const IWORKStyleStack &style, const IWORKLanguageManager &langManager, librevenge::RVNGPropertyList &props)
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
    langManager.writeProperties(style.get<Language>(), props);
}

namespace
{
void fillParaPropList(const IWORKStyleStack &styleStack, RVNGPropertyList &props)
{
  using namespace property;

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
      props.insert("fo:line-height", spacing.m_amount, librevenge::RVNG_PERCENT);
    else
      props.insert("fo:line-height", pt2in(spacing.m_amount));
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
  if (styleStack.has<Hyphenate>())
    props.insert("fo:hyphenate", styleStack.get<Hyphenate>());

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

  if (styleStack.has<ParagraphBorderType>() && styleStack.has<ParagraphStroke>())
  {
    const IWORKStroke &stroke = styleStack.get<ParagraphStroke>();
    switch (styleStack.get<ParagraphBorderType>())
    {
    case IWORK_BORDER_TYPE_TOP :
      writeBorder(stroke, "fo:border-top", props);
      break;
    case IWORK_BORDER_TYPE_BOTTOM :
      writeBorder(stroke, "fo:border-bottom", props);
      break;
    case IWORK_BORDER_TYPE_TOP_AND_BOTTOM :
      writeBorder(stroke, "fo:border-top", props);
      writeBorder(stroke, "fo:border-bottom", props);
      break;
    case IWORK_BORDER_TYPE_ALL :
      writeBorder(stroke, "fo:border", props);
      break;
    default :
      break;
    }
  }

  if (styleStack.has<PageBreakBefore>())
    props.insert("fo:break-before", "page");
}

struct FillListLabelProps : public boost::static_visitor<bool>
{
public:
  FillListLabelProps(const IWORKListStyle_t &listStyle, const IWORKListStyle_t::const_iterator &level, const IWORKListLabelGeometry *const geometry, const IWORKListLabelTypeInfo_t &typeInfo, RVNGPropertyList &props)
    : m_listStyle(listStyle)
    , m_current(level)
    , m_geometry(geometry)
    , m_typeInfo(typeInfo)
    , m_props(&props)
  {
  }

  bool operator()(const bool) const
  {
    m_props->insert("style:num-format", "");
    return false;
  }

  bool operator()(const std::string &bullet) const
  {
    m_props->insert("text:bullet-char", bullet.c_str());
    if (m_geometry)
      m_props->insert("text:bullet-relative-size", m_geometry->m_scale, librevenge::RVNG_PERCENT);
    return false;
  }

  bool operator()(const IWORKTextLabel &label) const
  {
    m_props->insert("style:num-letter-sync", "false");
    fillSurrounding(label.m_format.m_prefix, "style:num-prefix", true);
    fillSurrounding(label.m_format.m_suffix, "style:num-suffix", false);
    switch (label.m_format.m_format)
    {
    case IWORK_LABEL_NUM_FORMAT_NUMERIC :
      m_props->insert("style:num-format", "1");
      break;
    case IWORK_LABEL_NUM_FORMAT_ALPHA :
      m_props->insert("style:num-format", "A");
      break;
    case IWORK_LABEL_NUM_FORMAT_ALPHA_LOWERCASE :
      m_props->insert("style:num-format", "a");
      break;
    case IWORK_LABEL_NUM_FORMAT_ROMAN :
      m_props->insert("style:num-format", "I");
      break;
    case IWORK_LABEL_NUM_FORMAT_ROMAN_LOWERCASE :
      m_props->insert("style:num-format", "i");
      break;
    }
    m_props->insert("text:display-levels", boost::apply_visitor(GetDisplayLevels(m_listStyle, m_current, 1), m_typeInfo));
    return true;
  }

  bool operator()(const IWORKMediaContentPtr_t &image) const
  {
    // TODO: handle
    (void) image;
    return false;
  }

private:
  void fillSurrounding(const IWORKLabelNumFormatSurrounding surrounding, const char *const name, bool prefix) const
  {
    switch (surrounding)
    {
    case IWORK_LABEL_NUM_FORMAT_SURROUNDING_NONE :
      break;
    case IWORK_LABEL_NUM_FORMAT_SURROUNDING_PARENTHESIS :
      if (prefix)
        m_props->insert(name, "(");
      else
        m_props->insert(name, ")");
      break;
    case IWORK_LABEL_NUM_FORMAT_SURROUNDING_DOT :
      m_props->insert(name, ".");
      break;
    }
  }

  struct GetDisplayLevels : public boost::static_visitor<int>
  {
    GetDisplayLevels(const IWORKListStyle_t &listStyle, const IWORKListStyle_t::const_iterator &current, const int initial = 1)
      : m_listStyle(listStyle)
      , m_current(current)
      , m_initial(initial)
    {
    }

    int operator()(const bool) const
    {
      return m_initial;
    }

    int operator()(const std::string &) const
    {
      return m_initial;
    }

    int operator()(const IWORKTextLabel &) const
    {
      if (m_current == m_listStyle.begin())
        return m_initial;
      IWORKListStyle_t::const_iterator prev(m_current);
      --prev;
      if (prev->first != m_current->first - 1) // missing level spec
        return m_initial;
      if (!prev->second)
        return m_initial;
      if (!prev->second->has<property::ListLabelTypeInfo>())
        return m_initial;
      return boost::apply_visitor(GetDisplayLevels(m_listStyle, prev, m_initial + 1), prev->second->get<property::ListLabelTypeInfo>());
    }

    int operator()(const IWORKMediaContentPtr_t &) const
    {
      return m_initial;
    }

  private:
    const IWORKListStyle_t &m_listStyle;
    const IWORKListStyle_t::const_iterator m_current;
    const int m_initial;
  };

private:
  const IWORKListStyle_t &m_listStyle;
  const IWORKListStyle_t::const_iterator m_current;
  const IWORKListLabelGeometry *const m_geometry;
  const IWORKListLabelTypeInfo_t &m_typeInfo;
  RVNGPropertyList *const m_props;
};

bool fillListPropList(const unsigned level, const IWORKStyleStack &style, RVNGPropertyList &props)
{
  assert(level != 0);

  bool isOrdered = true;

  props.insert("librevenge:level", int(level));

  using namespace property;

  if (style.has<ListLevelStyles>())
  {
    const IWORKListStyle_t &listStyle = style.get<ListLevelStyles>();
    const IWORKListStyle_t::const_iterator levelIt = listStyle.find(level - 1);
    if ((levelIt != listStyle.end()) && bool(levelIt->second))
    {
      const IWORKStylePtr_t &levelStyle = levelIt->second;
      const IWORKListLabelGeometry *geometry = nullptr;

      if (levelStyle->has<ListLabelGeometry>())
      {
        geometry = &levelStyle->get<ListLabelGeometry>();
        // TODO: process
      }

      if (levelStyle->has<ListLabelTypeInfo>())
      {
        const IWORKListLabelTypeInfo_t &typeInfo = levelStyle->get<ListLabelTypeInfo>();
        isOrdered = boost::apply_visitor(FillListLabelProps(listStyle, levelIt, geometry, typeInfo, props), typeInfo);
      }
      else
      {
        // TODO: move this to parsers?
        props.insert("text:bullet-char", "\u2022");
        props.insert("text:bullet-relative-size", 100, librevenge::RVNG_PERCENT);
      }

      props.insert("style:vertical-pos", "center");
      props.insert("text:list-level-position-and-space-mode", "label-width-and-position");

      if (levelStyle->has<ListLabelIndent>())
        props.insert("text:space-before", levelStyle->get<ListLabelIndent>(), librevenge::RVNG_POINT);

      if (style.has<FontSize>() && levelStyle->has<ListTextIndent>())
        props.insert("text:min-label-width", levelStyle->get<ListTextIndent>() * style.get<FontSize>(), librevenge::RVNG_POINT);
    }
  }

  return isOrdered;
}

}

void IWORKText::draw(IWORKOutputElements &elements)
{
  assert(!m_recorder);

  elements.append(m_elements);
}

IWORKText::IWORKText(const IWORKLanguageManager &langManager, const bool discardEmptyContent)
  : m_langManager(langManager)
  , m_layoutStyleStack()
  , m_paraStyleStack()
  , m_elements()
  , m_layoutStyle()
  , m_inSection(false)
  , m_sectionProps()
  , m_checkedSection(false)
  , m_listStyle()
  , m_listLevel(0)
  , m_inListLevel(0)
  , m_isOrderedStack()
  , m_paraStyle()
  , m_inPara(false)
    // FIXME: This will work fine when encountering real empty text block, i.e., with a single
    // empty paragraph. But it will cause a loss of a leading empty paragraph otherwise. It is
    // good enough for now, though.
  , m_ignoreEmptyPara(discardEmptyContent)
  , m_spanStyle()
  , m_spanStyleChanged(false)
  , m_inSpan(false)
  , m_oldSpanStyle()
  , m_recorder()
{
}

IWORKText::~IWORKText()
{
  assert(m_isOrderedStack.empty());
}

void IWORKText::setRecorder(const std::shared_ptr<IWORKTextRecorder> &recorder)
{
  m_recorder = recorder;
}

const std::shared_ptr<IWORKTextRecorder> &IWORKText::getRecorder() const
{
  return m_recorder;
}

void IWORKText::pushBaseLayoutStyle(const IWORKStylePtr_t &style)
{
  if (bool(m_recorder))
  {
    m_recorder->pushBaseLayoutStyle(style);
    return;
  }

  m_layoutStyleStack.push(style);
}

void IWORKText::pushBaseParagraphStyle(const IWORKStylePtr_t &style)
{
  if (bool(m_recorder))
  {
    m_recorder->pushBaseParagraphStyle(style);
    return;
  }

  m_paraStyleStack.push(style);
}

void IWORKText::setLayoutStyle(const IWORKStylePtr_t &style)
{
  if (bool(m_recorder))
  {
    m_recorder->setLayoutStyle(style);
    return;
  }

  m_layoutStyle = style;
  m_checkedSection = false;
  m_sectionProps.clear();
}

void IWORKText::flushLayout()
{
  if (bool(m_recorder))
  {
    m_recorder->flushLayout();
    return;
  }

  if (m_inSection)
    closeSection();
}

void IWORKText::openSection()
{
  assert(!m_inSection);
  assert(!m_inPara);
  assert(!m_sectionProps.empty());

  handleListLevelChange(0);

  m_elements.addOpenSection(m_sectionProps);
  m_inSection = true;
}

void IWORKText::closeSection()
{
  assert(m_inSection);

  if (m_inPara)
    closePara();
  handleListLevelChange(0);

  m_elements.addCloseSection();
  m_inSection = false;
}

void IWORKText::setListStyle(const IWORKStylePtr_t &style)
{
  if (bool(m_recorder))
  {
    m_recorder->setListStyle(style);
    return;
  }

  m_listStyle = style;
}

void IWORKText::setListLevel(const unsigned level)
{
  if (bool(m_recorder))
  {
    m_recorder->setListLevel(level);
    return;
  }

  m_listLevel = level;
}

void IWORKText::flushList()
{
  if (bool(m_recorder))
  {
    m_recorder->flushList();
    return;
  }

  handleListLevelChange(m_listLevel);
}

void IWORKText::setParagraphStyle(const IWORKStylePtr_t &style)
{
  if (bool(m_recorder))
  {
    m_recorder->setParagraphStyle(style);
    return;
  }

  m_paraStyle = style;
}

void IWORKText::flushParagraph()
{
  if (bool(m_recorder))
  {
    m_recorder->flushParagraph();
    return;
  }

  if (!m_inPara && !m_ignoreEmptyPara)
    openPara(); // empty paragraphs are allowed, contrary to empty spans
  if (m_inSpan)
    closeSpan();
  if (m_inPara)
    closePara();
  m_ignoreEmptyPara = false;
}

void IWORKText::setSpanStyle(const IWORKStylePtr_t &style)
{
  if (bool(m_recorder))
  {
    m_recorder->setSpanStyle(style);
    return;
  }

  m_spanStyleChanged |= m_spanStyle != style;
  m_spanStyle = style;
}

void IWORKText::setLanguage(const IWORKStylePtr_t &style)
{
  if (bool(m_recorder))
  {
    m_recorder->setLanguage(style);
    return;
  }

  m_spanStyleChanged |= m_langStyle != style;
  m_langStyle = style;
}

void IWORKText::flushSpan()
{
  if (bool(m_recorder))
  {
    m_recorder->flushSpan();
    return;
  }

  if (m_inSpan)
    closeSpan();
}

void IWORKText::openLink(const std::string &url)
{
  if (bool(m_recorder))
  {
    m_recorder->openLink(url);
    return;
  }

  if (!m_inPara)
    openPara();
  if (m_inSpan)
  {
    m_oldSpanStyle = m_spanStyle;
    closeSpan(); // A link is always outside of a span
  }

  // TODO: handle link overflowing to next paragraph
  librevenge::RVNGPropertyList props;
  props.insert("xlink:type", "simple");
  props.insert("xlink:href", url.c_str());
  m_elements.addOpenLink(props);
}

void IWORKText::closeLink()
{
  if (bool(m_recorder))
  {
    m_recorder->closeLink();
    return;
  }

  if (m_inSpan)
    closeSpan();
  m_spanStyle = m_oldSpanStyle;
  m_oldSpanStyle.reset();

  m_elements.addCloseLink();
}

void IWORKText::insertText(const std::string &text)
{
  if (bool(m_recorder))
  {
    m_recorder->insertText(text);
    return;
  }

  if (m_inSpan && m_spanStyleChanged)
    closeSpan();
  if (!m_inSpan)
    openSpan();
  m_elements.addInsertText(librevenge::RVNGString(text.c_str()));
}

void IWORKText::insertTab()
{
  if (bool(m_recorder))
  {
    m_recorder->insertTab();
    return;
  }

  if (m_inSpan && m_spanStyleChanged)
    closeSpan();
  if (!m_inSpan)
    openSpan();
  m_elements.addInsertTab();
}

void IWORKText::insertSpace()
{
  if (bool(m_recorder))
  {
    m_recorder->insertSpace();
    return;
  }

  if (m_inSpan && m_spanStyleChanged)
    closeSpan();
  if (!m_inSpan)
    openSpan();
  m_elements.addInsertSpace();
}

void IWORKText::insertLineBreak()
{
  if (bool(m_recorder))
  {
    m_recorder->insertLineBreak();
    return;
  }

  if (m_inSpan && m_spanStyleChanged)
    closeSpan();
  if (!m_inSpan)
    openSpan();
  m_elements.addInsertLineBreak();
}

void IWORKText::insertInlineContent(const IWORKOutputElements &elements)
{
  if (!m_inSpan)
    openSpan();
  m_elements.append(elements);
}

void IWORKText::insertBlockContent(const IWORKOutputElements &elements)
{
  if (m_inPara)
    closePara();
  if (!m_inSection && needsSection())
    openSection();
  m_elements.append(elements);
  m_ignoreEmptyPara = true;
}

bool IWORKText::empty() const
{
  return m_elements.empty();
}

void IWORKText::handleListLevelChange(const unsigned level)
{
  if (level > m_inListLevel)
  {
    RVNGPropertyList paraProps;
    fillParaPropList(paraProps);

    for (; level > m_inListLevel;)
    {
      IWORKStyleStack styleStack(m_paraStyleStack);
      styleStack.push(m_paraStyle);
      styleStack.push(m_listStyle);
      ++m_inListLevel;
      RVNGPropertyList listProps;
      m_isOrderedStack.push(fillListPropList(m_inListLevel, styleStack, listProps));
      if (m_isOrderedStack.top())
        m_elements.addOpenOrderedListLevel(listProps);
      else
        m_elements.addOpenUnorderedListLevel(listProps);
      if (m_inListLevel != level)
        m_elements.addOpenListElement(paraProps);
    }
  }
  if (level < m_inListLevel)
  {
    if (m_inPara)
      closePara();
    for (; level < m_inListLevel; --m_inListLevel)
    {
      assert(!m_isOrderedStack.empty());
      if (m_isOrderedStack.top())
        m_elements.addCloseOrderedListLevel();
      else
        m_elements.addCloseUnorderedListLevel();
      m_isOrderedStack.pop();
    }
  }
}

void IWORKText::openPara()
{
  assert(!m_inPara);

  if (!m_inSection && needsSection())
    openSection();
  handleListLevelChange(m_listLevel);

  librevenge::RVNGPropertyList paraProps;
  fillParaPropList(paraProps);
  if (m_inListLevel > 0)
    m_elements.addOpenListElement(paraProps);
  else
    m_elements.addOpenParagraph(paraProps);
  m_inPara = true;
}

void IWORKText::closePara()
{
  assert(m_inPara);

  if (m_inSpan)
    closeSpan();

  // TODO: This is a temporary hack. The use of list element vs. paragraph needs rework.
  if (m_inListLevel > 0)
    m_elements.addCloseListElement();
  else
    m_elements.addCloseParagraph();
  m_inPara = false;
}

void IWORKText::fillParaPropList(librevenge::RVNGPropertyList &propList)
{
  m_paraStyleStack.push(m_paraStyle);
  libetonyek::fillParaPropList(m_paraStyleStack, propList);
  m_paraStyleStack.pop();
}

void IWORKText::openSpan()
{
  assert(!m_inSpan);

  if (!m_inPara)
    openPara();

  m_paraStyleStack.push(m_paraStyle);
  m_paraStyleStack.push(m_spanStyle);
  m_paraStyleStack.push(m_langStyle);
  librevenge::RVNGPropertyList props;
  fillCharPropList(m_paraStyleStack, m_langManager, props);
  m_paraStyleStack.pop();
  m_paraStyleStack.pop();
  m_paraStyleStack.pop();
  m_elements.addOpenSpan(props);
  m_inSpan = true;
  m_spanStyleChanged = false;
}

void IWORKText::closeSpan()
{
  assert(m_inSpan);

  m_elements.addCloseSpan();
  m_inSpan = false;
}

bool IWORKText::needsSection() const
{
  if (!m_checkedSection)
  {
    IWORKStyleStack styleStack(m_layoutStyleStack);
    styleStack.push(m_layoutStyle);
    fillSectionPropList(styleStack, m_sectionProps);
    m_checkedSection = true;
  }
  return !m_sectionProps.empty();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
