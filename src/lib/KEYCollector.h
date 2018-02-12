/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYCOLLECTOR_H_INCLUDED
#define KEYCOLLECTOR_H_INCLUDED

#include <deque>

#include "IWORKCollector.h"
#include "IWORKPath_fwd.h"
#include "IWORKStyle_fwd.h"
#include "KEYTypes.h"
#include "KEYTypes_fwd.h"

namespace libetonyek
{

class KEYCollector : public IWORKCollector
{
public:
  explicit KEYCollector(IWORKDocumentInterface *document);
  ~KEYCollector() override;

  // collector functions

  void collectPresentationSize(const IWORKSize &size);

  KEYLayerPtr_t collectLayer();
  void insertLayer(const KEYLayerPtr_t &layer);
  KEYSlidePtr_t collectSlide();

  KEYPlaceholderPtr_t collectTextPlaceholder(const IWORKStylePtr_t &style, bool title);
  void insertTextPlaceholder(const KEYPlaceholderPtr_t &placeholder);

  void collectNote();

  void collectStickyNote();

  void setSlideStyle(const IWORKStylePtr_t &style);

  // helper functions

  void startDocument();
  void sendSlides(const std::deque<KEYSlidePtr_t> &slides);
  void endDocument();

  void startSlides();
  void endSlides();
  void startThemes();
  void endThemes();

  void startPage();
  void endPage();
  void startLayer();
  void endLayer();

protected:
  bool m_inSlides;

private:
  void insertSlide(const KEYSlidePtr_t &slide, bool isMaster, const boost::optional<std::string> &pageName=boost::none);
  void drawTable() override;
  void drawMedia(double x, double y, const librevenge::RVNGPropertyList &data) override;
  void fillShapeProperties(librevenge::RVNGPropertyList &props) override;
  bool createFrameStylesForTextBox() const override
  {
    return false;
  }
  void drawTextBox(const IWORKTextPtr_t &text, const glm::dmat3 &trafo, const IWORKGeometryPtr_t &boundingBox, const librevenge::RVNGPropertyList &style) override;

private:
  IWORKSize m_size;

  KEYSlidePtr_t m_currentSlide;
  IWORKOutputElements m_notes;
  IWORKOutputElements m_stickyNotes;

  bool m_pageOpened;
  bool m_layerOpened;
  int m_layerCount;
};

} // namespace libetonyek

#endif // KEYCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
