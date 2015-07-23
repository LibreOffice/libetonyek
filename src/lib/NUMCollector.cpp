/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "NUMCollector.h"

#include "IWORKDocumentInterface.h"

namespace libetonyek
{

NUMCollector::NUMCollector(IWORKDocumentInterface *const document)
  : IWORKCollector(document)
{
}

void NUMCollector::startDocument()
{
  IWORKCollector::startDocument();
}

void NUMCollector::endDocument()
{
  librevenge::RVNGPropertyList metadata;
  fillMetadata(metadata);
  m_document->setDocumentMetaData(metadata);
  getOutputManager().getCurrent().write(m_document);

  IWORKCollector::endDocument();
}

void NUMCollector::drawTable()
{
  librevenge::RVNGPropertyList props;
  m_currentTable.draw(props, m_outputManager.getCurrent());
}

void NUMCollector::drawMedia(
  const double x, const double y, const double w, const double h,
  const std::string &mimetype, const librevenge::RVNGBinaryData &data)
{
  librevenge::RVNGPropertyList props;

  props.insert("librevenge:mime-type", mimetype.c_str());
  props.insert("office:binary-data", data);
  props.insert("svg:x", pt2in(x));
  props.insert("svg:y", pt2in(y));
  props.insert("svg:width", pt2in(w));
  props.insert("svg:height", pt2in(h));

  IWORKOutputElements &elements = getOutputManager().getCurrent();
  elements.addStartGraphic(props);
  elements.addEndGraphic();
  m_currentTable.insertObject(elements);
}

void NUMCollector::fillShapeProperties(librevenge::RVNGPropertyList &props)
{
  // TODO: implement me
  (void) props;
}

void NUMCollector::drawTextBox(const IWORKTextPtr_t &text, const glm::dmat3 &trafo, const IWORKGeometryPtr_t &boundingBox)
{
  // TODO: implement me
  (void) text;
  (void) trafo;
  (void) boundingBox;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
