/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef NUMCOLLECTOR_H_INCLUDED
#define NUMCOLLECTOR_H_INCLUDED

#include "IWORKCollector.h"

namespace libetonyek
{

class IWORKDocumentInterface;

class NUMCollector : public IWORKCollector
{
public:
  explicit NUMCollector(IWORKDocumentInterface *document);

  // collector functions

  // helper functions
  void startDocument();
  void endDocument();

private:
  void drawTable() override;
  void drawMedia(double x, double y, const librevenge::RVNGPropertyList &data) override;
  void fillShapeProperties(librevenge::RVNGPropertyList &props) override;
  bool createFrameStylesForTextBox() const override
  {
    return true;
  }
  void drawTextBox(const IWORKTextPtr_t &text, const glm::dmat3 &trafo, const IWORKGeometryPtr_t &boundingBox, const librevenge::RVNGPropertyList &style) override;
};

} // namespace libetonyek

#endif //  NUMCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
