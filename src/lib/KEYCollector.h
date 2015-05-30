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

#include "IWORKCollector.h"
#include "IWORKPath_fwd.h"
#include "KEYTypes.h"
#include "KEYTypes_fwd.h"

namespace libetonyek
{

class KEYCollector : public IWORKCollector
{
public:
  explicit KEYCollector(IWORKDocumentInterface *document);
  ~KEYCollector();

  // collector functions

  void collectPresentationSize(const IWORKSize &size);

  KEYLayerPtr_t collectLayer();
  void insertLayer(const KEYLayerPtr_t &layer);
  void collectPage();

  KEYPlaceholderPtr_t collectTextPlaceholder(const IWORKStylePtr_t &style, bool title);
  void insertTextPlaceholder(const KEYPlaceholderPtr_t &placeholder);

  void collectNote();

  void collectStickyNote();

  void startSlides();
  void endSlides();
  void startThemes();
  void endThemes();

  void startPage();
  void endPage();
  void startLayer();
  void endLayer();

private:
  virtual void drawTable();

private:
  IWORKSize m_size;

  IWORKOutputElements m_notes;
  IWORKOutputElements m_stickyNotes;

  bool m_pageOpened;
  bool m_layerOpened;
  int m_layerCount;

  bool m_paint;
};

} // namespace libetonyek

#endif // KEYCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
