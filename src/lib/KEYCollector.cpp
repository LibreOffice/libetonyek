/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYCollector.h"

#include <algorithm>
#include <functional>

#include <glm/glm.hpp>

#include "libetonyek_utils.h"
#include "IWORKDocumentInterface.h"
#include "IWORKOutputElements.h"
#include "IWORKPath.h"
#include "IWORKProperties.h"
#include "IWORKRecorder.h"
#include "IWORKTable.h"
#include "IWORKText.h"

namespace libetonyek
{

using librevenge::RVNGPropertyList;

using namespace std::placeholders;

KEYCollector::KEYCollector(IWORKDocumentInterface *const document)
  : IWORKCollector(document)
  , m_paint(false)
  , m_size()
  , m_slides()
  , m_notes()
  , m_stickyNotes()
  , m_pageOpened(false)
  , m_layerOpened(false)
  , m_layerCount(0)
{
  assert(!m_paint);
}

KEYCollector::~KEYCollector()
{
  assert(!m_paint);
}

void KEYCollector::collectPresentationSize(const IWORKSize &size)
{
  m_size = size;
}

KEYLayerPtr_t KEYCollector::collectLayer()
{
  assert(m_layerOpened);

  KEYLayerPtr_t layer(new KEYLayer());

  layer->m_outputId = getOutputManager().save();

  return layer;
}

void KEYCollector::insertLayer(const KEYLayerPtr_t &layer)
{
  assert(!m_layerOpened);

  if (bool(layer))
  {
    if (m_paint)
    {
      assert(!m_slides.empty());

      ++m_layerCount;

      librevenge::RVNGPropertyList props;
      props.insert("svg:id", m_layerCount);

      m_slides.back().m_content.addStartLayer(props);
      if (layer->m_outputId)
        m_slides.back().m_content.append(getOutputManager().get(get(layer->m_outputId)));
      m_slides.back().m_content.addEndLayer();
    }
  }
  else
  {
    ETONYEK_DEBUG_MSG(("no layer\n"));
  }
}

void KEYCollector::collectPage()
{
  assert(m_pageOpened);

  if (m_paint)
  {
    if (!m_notes.empty())
    {
      m_slides.back().m_content.addStartNotes(librevenge::RVNGPropertyList());
      m_slides.back().m_content.append(m_notes);
      m_slides.back().m_content.addEndNotes();
    }
    m_slides.back().m_content.append(m_stickyNotes);
  }
}

KEYPlaceholderPtr_t KEYCollector::collectTextPlaceholder(const IWORKStylePtr_t &style, const bool title)
{
  KEYPlaceholderPtr_t placeholder(new KEYPlaceholder());
  placeholder->m_title = title;
  placeholder->m_style = style;
  if (bool(placeholder->m_style))
  {
    m_styleStack.push();
    m_styleStack.set(placeholder->m_style);

    if (m_styleStack.has<property::Geometry>())
      placeholder->m_geometry = m_styleStack.get<property::Geometry>();

    m_styleStack.pop();
  }
  placeholder->m_text = m_currentText;
  m_currentText.reset();

  return placeholder;
}

void KEYCollector::insertTextPlaceholder(const KEYPlaceholderPtr_t &placeholder)
{
  if (bool(placeholder))
  {
    glm::dmat3 trafo;
    if (bool(placeholder->m_geometry))
      trafo = makeTransformation(*placeholder->m_geometry);
    trafo *= m_levelStack.top().m_trafo;

    if (bool(placeholder) && bool(placeholder->m_style) && bool(placeholder->m_text))
      drawTextBox(placeholder->m_text, trafo, placeholder->m_geometry);
  }
  else
  {
    ETONYEK_DEBUG_MSG(("no text placeholder found\n"));
  }
}

void KEYCollector::collectNote()
{
  if (bool(m_currentText))
  {
    m_currentText->draw(m_notes);
    m_currentText.reset();
  }
}

void KEYCollector::collectStickyNote()
{
  assert(!m_levelStack.empty());

  librevenge::RVNGPropertyList props;

  if (bool(m_levelStack.top().m_geometry))
  {
    props.insert("svg:x", pt2in(m_levelStack.top().m_geometry->m_position.m_x));
    props.insert("svg:y", pt2in(m_levelStack.top().m_geometry->m_position.m_y));
    props.insert("svg:width", pt2in(m_levelStack.top().m_geometry->m_naturalSize.m_width));
    props.insert("svg:height", pt2in(m_levelStack.top().m_geometry->m_naturalSize.m_height));
  }

  if (bool(m_currentText))
  {
    m_stickyNotes.addOpenComment(props);
    m_currentText->draw(m_stickyNotes);
    m_currentText.reset();
    m_stickyNotes.addCloseComment();
  }

  m_levelStack.top().m_geometry.reset();
  m_currentPath.reset();
}

void KEYCollector::setSlideStyle(const IWORKStylePtr_t &style)
{
  if (m_pageOpened)
  {
    assert(!m_slides.empty());
    m_slides.back().m_style = style;
  }
}

void KEYCollector::startDocument()
{
  IWORKCollector::startDocument();
}

void KEYCollector::endDocument()
{
  RVNGPropertyList metadata;
  fillMetadata(metadata);
  m_document->setDocumentMetaData(metadata);

  std::for_each(m_slides.begin(), m_slides.end(), std::bind(&KEYCollector::writeSlide, this, _1));

  IWORKCollector::endDocument();
}

void KEYCollector::startSlides()
{
  m_paint = true;
}

void KEYCollector::endSlides()
{
  m_paint = false;
}

void KEYCollector::startThemes()
{
}

void KEYCollector::endThemes()
{
}

void KEYCollector::startPage()
{
  assert(!m_pageOpened);
  assert(!m_layerOpened);
  assert(m_notes.empty());
  assert(m_stickyNotes.empty());

  startLevel();

  if (m_paint)
    m_slides.push_back(Slide());
  m_pageOpened = true;
}

void KEYCollector::endPage()
{
  assert(m_pageOpened);

  const std::shared_ptr<IWORKRecorder> recorder(m_recorder);
  m_recorder.reset();
  if (recorder)
    recorder->replay(*this);

  endLevel();

  m_notes.clear();
  m_stickyNotes.clear();

  m_pageOpened = false;
}

void KEYCollector::startLayer()
{
  assert(m_pageOpened);
  assert(!m_layerOpened);

  getOutputManager().push();
  m_layerOpened = true;

  startLevel();
}

void KEYCollector::endLayer()
{
  assert(m_pageOpened);
  assert(m_layerOpened);

  endLevel();
  getOutputManager().pop();

  m_layerOpened = false;
}

void KEYCollector::drawTable()
{
  assert(bool(m_currentTable));
  assert(!m_levelStack.empty());

  librevenge::RVNGPropertyList tableProps;

  tableProps.insert("table:align", "center");

  glm::dvec3 vec = m_levelStack.top().m_trafo * glm::dvec3(0, 0, 1);

  tableProps.insert("svg:x", pt2in(vec[0]));
  tableProps.insert("svg:y", pt2in(vec[1]));

  const IWORKGeometryPtr_t geometry(m_levelStack.top().m_geometry);
  if (geometry)
  {
    double w = geometry->m_naturalSize.m_width;
    double h = geometry->m_naturalSize.m_height;

    vec = m_levelStack.top().m_trafo * glm::dvec3(w, h, 0);

    tableProps.insert("svg:width", pt2in(vec[0]));
    tableProps.insert("svg:height", pt2in(vec[1]));
  }

  m_currentTable->draw(tableProps, m_outputManager.getCurrent(), true);
}

void KEYCollector::drawMedia(
  const double x, const double y,
  const librevenge::RVNGPropertyList &data)
{
  librevenge::RVNGPropertyList props(data);

  props.insert("svg:x", pt2in(x));
  props.insert("svg:y", pt2in(y));

  getOutputManager().getCurrent().addDrawGraphicObject(props);
}

void KEYCollector::fillShapeProperties(librevenge::RVNGPropertyList &)
{
}

void KEYCollector::drawTextBox(const IWORKTextPtr_t &text, const glm::dmat3 &trafo, const IWORKGeometryPtr_t &boundingBox)
{
  if (bool(text))
  {
    librevenge::RVNGPropertyList props;

    glm::dvec3 vec = trafo * glm::dvec3(0, 0, 1);

    props.insert("svg:x", pt2in(vec[0]));
    props.insert("svg:y", pt2in(vec[1]));

    if (bool(boundingBox))
    {
      double w = boundingBox->m_naturalSize.m_width;
      double h = boundingBox->m_naturalSize.m_height;
      vec = trafo * glm::dvec3(w, h, 0);

      props.insert("svg:width", pt2in(vec[0]));
      props.insert("svg:height", pt2in(vec[1]));
    }

    IWORKPath path;
    path.appendMoveTo(0, 0);
    path.appendLineTo(0, 1);
    path.appendLineTo(1, 1);
    path.appendLineTo(1, 0);
    path.appendClose();
    path *= trafo;

    librevenge::RVNGPropertyListVector d;
    path.write(d);
    props.insert("svg:d", d);

    IWORKOutputElements &elements = m_outputManager.getCurrent();
    elements.addStartTextObject(props);
    text->draw(elements);
    elements.addEndTextObject();
  }
}

void KEYCollector::writeSlide(const Slide &slide)
{
  librevenge::RVNGPropertyList props;
  props.insert("svg:width", pt2in(m_size.m_width));
  props.insert("svg:height", pt2in(m_size.m_height));

  if (bool(slide.m_style) && slide.m_style->has<property::Fill>())
    writeFill(slide.m_style->get<property::Fill>(), props);

  m_document->startSlide(props);
  slide.m_content.write(m_document);
  m_document->endSlide();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
