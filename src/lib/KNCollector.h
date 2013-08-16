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

namespace libwpg
{

class WPGPaintInterface;

} // namespace libwpg

namespace libkeynote
{

class KNGeometry;
class KNGroup;
class KNImage;
class KNLine;
class KNMedia;
class KNPath;
class KNSize;
class KNStyle;

class KNCollector
{
public:
  virtual ~KNCollector() = 0;

  // collector functions

  virtual void collectCharacterStyle(const std::string &id, const KNStyle &style) = 0;
  virtual void collectGraphicStyle(const std::string &id, const KNStyle &style) = 0;
  virtual void collectHeadlineStyle(const std::string &id, const KNStyle &style) = 0;
  virtual void collectLayoutStyle(const std::string &id, const KNStyle &style) = 0;
  virtual void collectParagraphStyle(const std::string &id, const KNStyle &style) = 0;

  virtual void collectGeometry(const std::string &id, const KNGeometry &geometry) = 0;
  virtual void collectGroup(const std::string &id, const KNGroup &group) = 0;
  virtual void collectImage(const std::string &id, const KNImage &image) = 0;
  virtual void collectLine(const std::string &id, const KNLine &line) = 0;
  virtual void collectMedia(const std::string &id, const KNMedia &media) = 0;
  virtual void collectPath(const std::string &id, const KNPath &path) = 0;

  virtual void collectSize(const KNSize &size) = 0;

  // helper functions

  virtual void startSlides() = 0;
  virtual void endSlides() = 0;
  virtual void startThemes() = 0;
  virtual void endThemes() = 0;
};

} // namespace libkeynote

#endif // KNCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
