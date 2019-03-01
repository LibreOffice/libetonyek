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
#include "IWORKTable.h"
#include "IWORKText.h"

namespace libetonyek
{

NUMCollector::NUMCollector(IWORKDocumentInterface *const document)
  : IWORKCollector(document)
  , m_layerOpened(false)
  , m_tableElementLists()
{
}

void NUMCollector::startDocument()
{
  librevenge::RVNGPropertyList calcSettings;
  calcSettings.insert("librevenge:type","table:calculation-settings");
  calcSettings.insert("table:use-wildcards",true);
  librevenge::RVNGPropertyListVector pVect;
  pVect.append(calcSettings);
  librevenge::RVNGPropertyList props;
  props.insert("librevenge:childs", pVect);
  IWORKCollector::startDocument(props);
}

void NUMCollector::endDocument()
{
  librevenge::RVNGPropertyList metadata;
  fillMetadata(metadata);
  m_document->setDocumentMetaData(metadata);
  getOutputManager().getCurrent().write(m_document);

  IWORKCollector::endDocument();
}

void NUMCollector::startLayer()
{
  if (m_layerOpened)
  {
    ETONYEK_DEBUG_MSG(("NUMCollector::startLayer: oops a layer is already open\n"));
    endLayer();
  }
  getOutputManager().push();
  m_layerOpened = true;

  startLevel();
}

void NUMCollector::endLayer()
{
  if (!m_layerOpened)
  {
    ETONYEK_DEBUG_MSG(("NUMCollector::endLayer: no open layer\n"));
    return;
  }

  endLevel();
  auto shapeElements=getOutputManager().getCurrent();
  getOutputManager().pop();

  for (auto const &tableElt : m_tableElementLists)
  {
    if (tableElt.empty()) continue;
    if (!shapeElements.empty())
    {
      auto finalTableElt(tableElt);
      finalTableElt.addShapesInSpreadsheet(shapeElements);
      getOutputManager().getCurrent().append(finalTableElt);
      shapeElements.clear();
    }
    else
      getOutputManager().getCurrent().append(tableElt);
  }
  m_tableElementLists.clear();
  m_layerOpened = false;
}

void NUMCollector::drawTable()
{
  assert(bool(m_currentTable));
  m_tableElementLists.push_back(IWORKOutputElements());
  librevenge::RVNGPropertyList props;
  m_currentTable->draw(props, m_tableElementLists.back(), false);
}

void NUMCollector::drawMedia(
  const double x, const double y,
  const librevenge::RVNGPropertyList &data)
{
  // TODO: implement me
  (void) x;
  (void) y;
  (void) data;
}

void NUMCollector::fillShapeProperties(librevenge::RVNGPropertyList &props)
{
  // TODO: implement me
  (void) props;
}

void NUMCollector::drawTextBox(const IWORKTextPtr_t &text, const glm::dmat3 &trafo, const IWORKGeometryPtr_t &boundingBox, const librevenge::RVNGPropertyList &style)
{
  if (!bool(text) || text->empty())
    return;

  librevenge::RVNGPropertyList props(style);
  if (!style["draw:fill"]) props.insert("draw:fill", "none");
  if (!style["draw:stroke"]) props.insert("draw:stroke", "none");

  glm::dvec3 vec = trafo * glm::dvec3(0, 0, 1);

  props.insert("svg:x", pt2in(vec[0]));
  props.insert("svg:y", pt2in(vec[1]));

  if (bool(boundingBox))
  {
    double w = boundingBox->m_naturalSize.m_width;
    double h = boundingBox->m_naturalSize.m_height;
    vec = trafo * glm::dvec3(w, h, 0);

    if (vec[0]>0)
      props.insert("svg:width", pt2in(vec[0]));
    if (vec[1]>0)
      props.insert("svg:height", pt2in(vec[1]));
  }

  fillShapeProperties(props);

  IWORKOutputElements &elements = m_outputManager.getCurrent();
  elements.addOpenFrame(props);
  elements.addStartTextObject(librevenge::RVNGPropertyList());
  text->draw(elements);
  elements.addEndTextObject();
  elements.addCloseFrame();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
