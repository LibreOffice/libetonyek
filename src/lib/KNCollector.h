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

#include <boost/optional.hpp>

#include "libkeynote_utils.h"
#include "KNPath_fwd.h"
#include "KNTypes_fwd.h"

namespace libwpg
{

class WPGPaintInterface;

} // namespace libwpg

namespace libkeynote
{

class KNDefaults;
class KNPropertyMap;
struct KNSize;

class KNCollector
{
public:
  virtual ~KNCollector() = 0;

  // collector functions

  virtual void collectCellStyle(const boost::optional<ID_t> &id,
                                const boost::optional<KNPropertyMap> &props,
                                const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                bool ref, bool anonymous) = 0;
  virtual void collectCharacterStyle(const boost::optional<ID_t> &id,
                                     const boost::optional<KNPropertyMap> &props,
                                     const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                     bool ref, bool anonymous) = 0;
  virtual void collectConnectionStyle(const boost::optional<ID_t> &id,
                                      const boost::optional<KNPropertyMap> &props,
                                      const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                      bool ref, bool anonymous) = 0;
  virtual void collectGraphicStyle(const boost::optional<ID_t> &id,
                                   const boost::optional<KNPropertyMap> &props,
                                   const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                   bool ref, bool anonymous) = 0;
  virtual void collectLayoutStyle(const boost::optional<ID_t> &id,
                                  const boost::optional<KNPropertyMap> &props,
                                  const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                  bool ref, bool anonymous) = 0;
  virtual void collectListStyle(const boost::optional<ID_t> &id,
                                const boost::optional<KNPropertyMap> &props,
                                const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                bool ref, bool anonymous) = 0;
  virtual void collectParagraphStyle(const boost::optional<ID_t> &id,
                                     const boost::optional<KNPropertyMap> &props,
                                     const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                     bool ref, bool anonymous) = 0;
  virtual void collectPlaceholderStyle(const boost::optional<ID_t> &id,
                                       const boost::optional<KNPropertyMap> &props,
                                       const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                       bool ref, bool anonymous) = 0;
  virtual void collectSlideStyle(const boost::optional<ID_t> &id,
                                 const boost::optional<KNPropertyMap> &props,
                                 const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                 bool ref, bool anonymous) = 0;
  virtual void collectTabularStyle(const boost::optional<ID_t> &id,
                                   const boost::optional<KNPropertyMap> &props,
                                   const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                   bool ref, bool anonymous) = 0;
  virtual void collectVectorStyle(const boost::optional<ID_t> &id,
                                  const boost::optional<KNPropertyMap> &props,
                                  const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent,
                                  bool ref, bool anonymous) = 0;

  virtual void collectGeometry(boost::optional<ID_t> &id,
                               boost::optional<KNSize> &naturalSize, boost::optional<KNPosition> &position,
                               boost::optional<double> &angle,
                               boost::optional<double> &shearXAngle, boost::optional<double> &shearYAngle,
                               boost::optional<bool> &horizontalFlip, boost::optional<bool> &verticalFlip,
                               boost::optional<bool> &aspectRatioLocked, boost::optional<bool> &sizesLocked) = 0;

  virtual void collectBezier(const boost::optional<ID_t> &id, const KNPathPtr_t &path, bool ref) = 0;
  virtual void collectGroup(const boost::optional<ID_t> &id, const KNGroupPtr_t &group) = 0;
  virtual void collectImage(const boost::optional<ID_t> &id, const KNImagePtr_t &image) = 0;
  virtual void collectLine(const boost::optional<ID_t> &id, const KNLinePtr_t &line) = 0;
  virtual void collectShape(const boost::optional<ID_t> &id) = 0;

  virtual void collectBezierPath(const boost::optional<ID_t> &id) = 0;
  virtual void collectPolygonPath(const boost::optional<ID_t> &id, const KNSize &size, unsigned edges) = 0;
  virtual void collectRoundedRectanglePath(const boost::optional<ID_t> &id, const KNSize &size, double radius) = 0;
  virtual void collectArrowPath(const boost::optional<ID_t> &id, const KNSize &size, double headWidth, double stemRelYPos, bool doubleSided) = 0;
  virtual void collectStarPath(const boost::optional<ID_t> &id, const KNSize &size, unsigned points, double innerRadius) = 0;
  virtual void collectConnectionPath(const boost::optional<ID_t> &id, const KNSize &size, double middleX, double middleY) = 0;
  virtual void collectCalloutPath(const boost::optional<ID_t> &id, const KNSize &size, double radius, double tailSize, double tailX, double tailY, bool quoteBubble) = 0;

  virtual void collectData(const boost::optional<ID_t> &id, const WPXInputStreamPtr_t &stream, const boost::optional<std::string> &displayName, const boost::optional<unsigned> &type, bool ref) = 0;
  virtual void collectUnfiltered(const boost::optional<ID_t> &id, const boost::optional<KNSize> &size, bool ref) = 0;
  virtual void collectFilteredImage(const boost::optional<ID_t> &id, bool ref) = 0;
  virtual void collectMedia(const boost::optional<ID_t> &id) = 0;

  virtual void collectPresentation(const boost::optional<KNSize> &size) = 0;

  virtual void collectLayer(const boost::optional<ID_t> &id, bool ref) = 0;
  virtual void collectPage(const boost::optional<ID_t> &id) = 0;
  virtual void collectStylesheet(const boost::optional<ID_t> &id, const boost::optional<ID_t> &parent) = 0;

  virtual void collectText(const boost::optional<ID_t> &style, const std::string &text) = 0;
  virtual void collectTab() = 0;
  virtual void collectLineBreak() = 0;

  virtual void collectTextPlaceholder(const boost::optional<ID_t> &id, bool title, bool ref) = 0;

  virtual void startPage() = 0;
  virtual void endPage() = 0;
  virtual void startLayer() = 0;
  virtual void endLayer() = 0;
  virtual void startGroup() = 0;
  virtual void endGroup() = 0;

  virtual void startParagraph(const boost::optional<ID_t> &style) = 0;
  virtual void endParagraph() = 0;
  virtual void startTextLayout(const boost::optional<ID_t> &style) = 0;
  virtual void endTextLayout() = 0;

  // helper functions

  virtual void startSlides() = 0;
  virtual void endSlides() = 0;
  virtual void startThemes() = 0;
  virtual void endThemes() = 0;

  virtual void startLevel() = 0;
  virtual void endLevel() = 0;
};

} // namespace libkeynote

#endif // KNCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
