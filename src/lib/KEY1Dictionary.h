/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY1DICTIONARY_H_INCLUDED
#define KEY1DICTIONARY_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKDictionary.h"
#include "KEYTypes.h"

namespace libetonyek
{

/** A dictionary of all referenceable entities in the presentation.
  *
  * In other words, all entities that might be included through a -ref
  * element in the format. This is purely a file format concept; all
  * that matters when an entity is inserted by a reference is that there
  * has been an entity of the same type and with the same content
  * inserted previously (or at least that is how I understand it. I
  * might be wrong about it). That means we need to know all entities in
  * the whole presentation to resolve references.
  *
  * @note The dictionary contains (among other things) all master
  * styles, therefore we do not handle them separately. (We might do
  * that in the future, to ensure correctness of the format.)
  */
struct KEY1Dictionary : public IWORKDictionary
{

  KEY1Dictionary();
  void pushStylesContext();
  void collectStylesContext(const std::string &contextName);
  void linkStylesContext(const boost::optional<std::string> &master);
  void popStylesContext();

  void storeImageStyle(IWORKStylePtr_t style, bool definition);
  void storeLineStyle(IWORKStylePtr_t style, bool definition);
  void storeShapeStyle(IWORKStylePtr_t style, bool definition);
  void storeTextboxStyle(IWORKStylePtr_t style, bool definition);

  KEYPlaceholderPtr_t getBodyPlaceholder();
  KEYPlaceholderPtr_t getPageNumberPlaceholder();
  KEYPlaceholderPtr_t getTitlePlaceholder();

  void storeBodyPlaceholder(KEYPlaceholderPtr_t placeHolder);
  void storePageNumberPlaceholder(KEYPlaceholderPtr_t placeHolder);
  void storeTitlePlaceholder(KEYPlaceholderPtr_t placeHolder);

  KEYSlideMap_t m_masterSlides;
  std::deque<KEYSlidePtr_t> m_slides;

protected:
  struct StyleContext
  {
    StyleContext();

    IWORKStylePtr_t m_style;

    std::deque<IWORKStylePtr_t> m_styles;

    /** store a style */
    void storeStyle(IWORKStylePtr_t style, bool definition);
    /** link all style to their parent */
    void link(const StyleContext &parentContext);
  };
  struct StylesContext
  {
    StylesContext();

    StyleContext m_imageContext;
    StyleContext m_lineContext;
    StyleContext m_shapeContext;
    StyleContext m_textboxContext;

    KEYPlaceholderPtr_t m_bodyPlaceholder;
    KEYPlaceholderPtr_t m_pageNumberPlaceholder;
    KEYPlaceholderPtr_t m_titlePlaceholder;

    /** link all style to their parent */
    void link(const StylesContext &parentContext);

  protected:
    static void link(KEYPlaceholderPtr_t &placeholder, const KEYPlaceholderPtr_t &parentHolder);
  };
  typedef std::unordered_map<ID_t, StylesContext> StylesContextMap_t;
  StylesContext &getCurrentStylesContext();

  std::deque<StylesContext> m_styleContexts;
  StylesContextMap_t m_styleContextsMap;
};

}

#endif //  KEY1DICTIONARY_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
