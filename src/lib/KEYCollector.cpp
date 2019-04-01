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
#include <memory>
#include <set>

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
  , m_inSlides(false)
  , m_size()
  , m_currentSlide()
  , m_notes()
  , m_stickyNotes()
  , m_pageOpened(false)
  , m_layerOpened(false)
  , m_layerCount(0)
{
  assert(!m_inSlides);
}

KEYCollector::~KEYCollector()
{
  assert(!m_inSlides);
}

void KEYCollector::collectPresentationSize(const IWORKSize &size)
{
  m_size = size;
}

KEYLayerPtr_t KEYCollector::collectLayer()
{
  assert(m_layerOpened);

  const std::shared_ptr<IWORKRecorder> recorder(m_recorder);
  m_recorder.reset();
  if (recorder)
    recorder->replay(*this);

  KEYLayerPtr_t layer(new KEYLayer());

  layer->m_outputId = getOutputManager().save();

  return layer;
}

void KEYCollector::insertLayer(const KEYLayerPtr_t &layer)
{
  assert(!m_layerOpened);

  if (bool(layer))
  {
    if (m_currentSlide)
    {

      ++m_layerCount;

      librevenge::RVNGPropertyList props;
      props.insert("svg:id", m_layerCount);

      m_currentSlide->m_content.addStartLayer(props);
      if (layer->m_outputId)
        m_currentSlide->m_content.append(getOutputManager().get(get(layer->m_outputId)));
      m_currentSlide->m_content.addEndLayer();
    }
  }
  else
  {
    ETONYEK_DEBUG_MSG(("no layer\n"));
  }
}

KEYSlidePtr_t KEYCollector::collectSlide()
{
  assert(m_pageOpened);

  if (m_currentSlide)
  {
    if (!m_notes.empty())
    {
      m_currentSlide->m_content.addStartNotes(librevenge::RVNGPropertyList());
      m_currentSlide->m_content.append(m_notes);
      m_currentSlide->m_content.addEndNotes();
    }
    m_currentSlide->m_content.append(m_stickyNotes);
    return m_currentSlide;
  }

  return KEYSlidePtr_t();
}

void KEYCollector::insertSlide(const KEYSlidePtr_t &slide, bool isMaster, const boost::optional<std::string> &pageName)
{
  if (!slide)
  {
    ETONYEK_DEBUG_MSG(("KEYCollector::insertSlide: called with no slide\n"));
    return;
  }
  librevenge::RVNGPropertyList props;
  props.insert("svg:width", pt2in(m_size.m_width));
  props.insert("svg:height", pt2in(m_size.m_height));
  if (pageName) props.insert("librevenge:master-page-name", get(pageName).c_str());
  if (bool(slide->m_style) && slide->m_style->has<property::Fill>())
    writeFill(slide->m_style->get<property::Fill>(), props);

  if (isMaster)
    m_document->startMasterSlide(props);
  else
    m_document->startSlide(props);
  slide->m_content.write(m_document);
  if (isMaster)
    m_document->endMasterSlide();
  else
    m_document->endSlide();
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
    glm::dmat3 trafo(1);
    if (bool(placeholder->m_geometry))
      trafo = makeTransformation(*placeholder->m_geometry);
    trafo *= m_levelStack.top().m_trafo;

    if (bool(placeholder) && bool(placeholder->m_style) && bool(placeholder->m_text))
    {
      librevenge::RVNGPropertyList props;
      fillLayoutProps(placeholder->m_style, props);
      drawTextBox(placeholder->m_text, trafo, placeholder->m_geometry, props);
    }
  }
  else
  {
    ETONYEK_DEBUG_MSG(("KEYCollector::insertTextPlaceholder: no text placeholder found\n"));
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
    assert(m_currentSlide);
    m_currentSlide->m_style = style;
  }
}

void KEYCollector::startDocument()
{
  IWORKCollector::startDocument(librevenge::RVNGPropertyList());
}

void KEYCollector::sendSlides(const std::deque<KEYSlidePtr_t> &slides)
{
  RVNGPropertyList metadata;
  fillMetadata(metadata);
  m_document->setDocumentMetaData(metadata);

  std::map<KEYSlide const *, std::string> masterToNameMap;
  std::set<std::string> nameSet;
  unsigned nameId=0;
  for (auto slide : slides)
  {
    if (!slide) continue;
    boost::optional<std::string> name;
    if (slide->m_masterSlide)
    {
      if (masterToNameMap.find(slide->m_masterSlide.get())==masterToNameMap.end())
      {
        if (slide->m_masterSlide->m_name && nameSet.find(get(slide->m_masterSlide->m_name))==nameSet.end())
          name=get(slide->m_masterSlide->m_name);
        else
        {
          // ok try to find an unused name
          do
          {
            std::stringstream s;
            if (slide->m_masterSlide->m_name)
              s << get(slide->m_masterSlide->m_name) << nameId++;
            else
              s << "MasterSlide" << nameId++;
            name=s.str();
          }
          while (nameSet.find(get(name))!=nameSet.end());
        }
        nameSet.insert(get(name));
        masterToNameMap[slide->m_masterSlide.get()]=get(name);
        insertSlide(slide->m_masterSlide, true, name);
      }
      else
        name=masterToNameMap.find(slide->m_masterSlide.get())->second;
    }
    insertSlide(slide, false, name);
  }

}

void KEYCollector::endDocument()
{
  IWORKCollector::endDocument();
}

void KEYCollector::startSlides()
{
  m_inSlides = true;
}

void KEYCollector::endSlides()
{
  m_inSlides = false;
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

  assert(m_inSlides && !m_currentSlide);
  m_currentSlide = std::make_shared<KEYSlide>();
  m_pageOpened = true;
}

void KEYCollector::endPage()
{
  assert(m_pageOpened);

  endLevel();

  m_notes.clear();
  m_stickyNotes.clear();

  m_currentSlide.reset();
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

  if (bool(m_currentTable->getStyle()))
    fillWrapProps(m_currentTable->getStyle(), tableProps, m_currentTable->getOrder());

  m_currentTable->draw(tableProps, m_outputManager.getCurrent(), true);
}

void KEYCollector::drawMedia(
  const double x, const double y,
  const librevenge::RVNGPropertyList &data)
{
  librevenge::RVNGPropertyList style(data);
  if (style["office:binary-data"])
    style.remove("office:binary-data");
  getOutputManager().getCurrent().addSetStyle(style);

  librevenge::RVNGPropertyList props(data);
  props.insert("svg:x", pt2in(x));
  props.insert("svg:y", pt2in(y));
  getOutputManager().getCurrent().addDrawGraphicObject(props);
}

void KEYCollector::fillShapeProperties(librevenge::RVNGPropertyList &)
{
}

void KEYCollector::drawTextBox(const IWORKTextPtr_t &text, const glm::dmat3 &trafo, const IWORKGeometryPtr_t &boundingBox, const librevenge::RVNGPropertyList &style)
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

  IWORKOutputElements &elements = m_outputManager.getCurrent();
  elements.addStartTextObject(props);
  text->draw(elements);
  elements.addEndTextObject();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
