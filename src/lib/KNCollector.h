/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNCOLLECTOR_H_INCLUDED
#define KNCOLLECTOR_H_INCLUDED

#include <string>

#include <boost/unordered_map.hpp>

#include "KNStyle.h"

namespace libwpg
{

class WPGPaintInterface;

} // namespace libwpg

namespace libkeynote
{

class KNCollector
{
  typedef boost::unordered_map<std::string, KNStyle> StyleMap_t;

  struct StyleSheet
  {
    StyleMap_t characterStyles;
    StyleMap_t graphicStyles;
    StyleMap_t headlineStyles;
    StyleMap_t layoutStyles;
    StyleMap_t paragraphStyles;

    StyleSheet();
  };

public:
  explicit KNCollector(::libwpg::WPGPaintInterface *painter);
  virtual ~KNCollector();

  // collector functions

  void collectCharacterStyle(const std::string &id, const KNStyle &style);
  void collectGraphicStyle(const std::string &id, const KNStyle &style);
  void collectHeadlineStyle(const std::string &id, const KNStyle &style);
  void collectLayoutStyle(const std::string &id, const KNStyle &style);
  void collectParagraphStyle(const std::string &id, const KNStyle &style);

  void collectMasterStyles();

private:
  KNCollector(const KNCollector &);
  KNCollector &operator=(const KNCollector &);

  // helper functions

  void resolveStyle(KNStyle &style);

private:
  libwpg::WPGPaintInterface *m_painter;
  StyleSheet m_masterStyles;
  StyleSheet m_currentStyles;
};

} // namespace libkeynote

#endif // KNCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
