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

class KNPath;

class KNCollector
{
  typedef boost::unordered_map<std::string, KNStyle> StyleMap_t;
  typedef boost::unordered_map<std::string, KNLayer> LayerMap_t;

  typedef boost::unordered_map<std::string, KNGeometry> GeometryMap_t;
  typedef boost::unordered_map<std::string, KNGroup> GroupMap_t;
  typedef boost::unordered_map<std::string, KNImage> ImageMap_t;
  typedef boost::unordered_map<std::string, KNLine> LineMap_t;
  typedef boost::unordered_map<std::string, KNMedia> MediaMap_t;
  typedef boost::unordered_map<std::string, KNPath> PathMap_t;

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

  void collectGeometry(const std::string &id, const KNGeometry &geometry);
  void collectGroup(const std::string &id, const KNGroup &group);
  void collectImage(const std::string &id, const KNImage &image);
  void collectLine(const std::string &id, const KNLine &line);
  void collectMedia(const std::string &id, const KNMedia &media);
  void collectPath(const std::string &id, const KNPath &path);

private:
  KNCollector(const KNCollector &);
  KNCollector &operator=(const KNCollector &);

  // helper functions

  void resolveStyle(KNStyle &style);

private:
  libwpg::WPGPaintInterface *m_painter;

  StyleSheet m_masterStyles;
  StyleSheet m_currentStyles;

  GeometryMap_t m_currentGeometries;
  GroupMap_t m_currentGroups;
  ImageMap_t m_currentImages;
  LineMap_t m_currentLines;
  MediaMap_t m_currentMedia;
  PathMap_t m_currentPaths;

  LayerMap_t m_masterPages;
  KNLayer m_currentLayer;

  bool m_master;
};

} // namespace libkeynote

#endif // KNCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
