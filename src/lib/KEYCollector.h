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
  virtual ~KEYCollector();

  // collector functions

  void collectPresentationSize(const IWORKSize &size);

  KEYLayerPtr_t collectLayer();
  void insertLayer(const KEYLayerPtr_t &layer);
  void collectPage();

  KEYPlaceholderPtr_t collectTextPlaceholder(const IWORKStylePtr_t &style, bool title);
  void insertTextPlaceholder(const KEYPlaceholderPtr_t &placeholder);

  void collectNote();

  void collectStickyNote();

  void setSlideStyle(const IWORKStylePtr_t &style);

  // helper functions

  void startDocument();
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
  bool m_paint;

private:
  struct Slide
  {
    Slide()
      : m_content()
      , m_style()
    {
    }
    IWORKOutputElements m_content;
    IWORKStylePtr_t m_style;
  };

private:
  void drawTable() override;
  void drawMedia(double x, double y, double w, double h, const std::string &mimetype, const librevenge::RVNGBinaryData &data) override;
  void fillShapeProperties(librevenge::RVNGPropertyList &props) override;
  void drawTextBox(const IWORKTextPtr_t &text, const glm::dmat3 &trafo, const IWORKGeometryPtr_t &boundingBox) override;

  void writeSlide(const Slide &slide);

private:
  IWORKSize m_size;

  std::deque<Slide> m_slides;
  IWORKOutputElements m_notes;
  IWORKOutputElements m_stickyNotes;

  bool m_pageOpened;
  bool m_layerOpened;
  int m_layerCount;
};

} // namespace libetonyek

#endif // KEYCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
