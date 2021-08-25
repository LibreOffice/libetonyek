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
#include "IWORKLanguageManager.h"
#include "IWORKProperties.h"
#include "IWORKTable.h"
#include "IWORKText.h"

namespace libetonyek
{

NUMCollector::NUMCollector(IWORKDocumentInterface *const document)
  : IWORKCollector(document)
  , m_workSpaceOpened(false)
  , m_workSpaceName()
  , m_workSpaceCreateGraphic(false)
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

void NUMCollector::startWorkSpace(boost::optional<std::string> const &name)
{
  if (m_workSpaceOpened)
  {
    ETONYEK_DEBUG_MSG(("NUMCollector::startWorkSpace: oops a workSpace is already open\n"));
    endWorkSpace(nullptr);
  }
  getOutputManager().push();
  m_workSpaceOpened = true;
  m_workSpaceName = name;
  m_workSpaceCreateGraphic = false;
  startLevel();
}

void NUMCollector::endWorkSpace(IWORKTableNameMapPtr_t tableNameMap)
{
  if (!m_workSpaceOpened)
  {
    ETONYEK_DEBUG_MSG(("NUMCollector::endWorkSpace: no open workSpace\n"));
    return;
  }

  endLevel();
  auto shapeElements=getOutputManager().getCurrent();
  getOutputManager().pop();

  if (m_tableElementLists.size()>=2)
    m_workSpaceCreateGraphic=true;
  for (auto const &tableElt : m_tableElementLists)
  {
    if (tableElt.empty()) continue;
    if (!shapeElements.empty() && !m_workSpaceCreateGraphic)
    {
      auto finalTableElt(tableElt);
      finalTableElt.addShapesInSpreadsheet(shapeElements);
      getOutputManager().getCurrent().append(finalTableElt);
      shapeElements.clear();
    }
    else
      getOutputManager().getCurrent().append(tableElt);
  }
  if (!shapeElements.empty())
  {
    // ok, we must create a empty spreadsheet
    IWORKLanguageManager langManager;
    IWORKFormatNameMap formatNameMap;
    IWORKTable table(tableNameMap, formatNameMap, langManager);
    if (m_workSpaceName && tableNameMap)
    {
      auto tableName=*m_workSpaceName;
      if (m_tableElementLists.size()>=2) tableName += "_Graphics";
      if (tableNameMap->find(tableName)!=tableNameMap->end())
      {
        ETONYEK_DEBUG_MSG(("NUMCollector::endWorkSpace: a table with name %s already exists\n", tableName.c_str()));
        // let create an unique name
        int id=0;
        while (true)
        {
          std::stringstream s;
          s << tableName << "_" << ++id;
          if (tableNameMap->find(s.str())!=tableNameMap->end()) continue;
          tableName=s.str();
          break;
        }
      }
      (*tableNameMap)[tableName]=tableName;
      table.setName(tableName);
    }
    IWORKOutputElements tableElements;
    librevenge::RVNGPropertyList props;
    table.draw(props, tableElements, false);
    tableElements.addShapesInSpreadsheet(shapeElements);
    getOutputManager().getCurrent().append(tableElements);
  }
  m_tableElementLists.clear();
  m_workSpaceOpened = false;
  m_workSpaceName = boost::none;
  m_workSpaceCreateGraphic = false;
}

void NUMCollector::drawTable()
{
  assert(bool(m_currentTable));
  if (!m_workSpaceCreateGraphic && !m_levelStack.empty())
  {
    // check if the table can be the main sheet
    glm::dvec3 vec = m_levelStack.top().m_trafo * glm::dvec3(0, 0, 1);
    m_workSpaceCreateGraphic = vec[0]>5 || vec[1]>5;
  }

  m_tableElementLists.push_back(IWORKOutputElements());
  librevenge::RVNGPropertyList props;
  m_currentTable->draw(props, m_tableElementLists.back(), false);
}

void NUMCollector::drawMedia(
  const double x, const double y,
  const librevenge::RVNGPropertyList &data)
{
  if (!data["office:binary-data"] || !data["librevenge:mime-type"])
  {
    ETONYEK_DEBUG_MSG(("NUMCollector::drawMedia: oops can not find the picture\n"));
    return;
  }
  librevenge::RVNGPropertyList frameProps(data);
  fillShapeProperties(frameProps);
  frameProps.insert("svg:x", pt2in(x));
  frameProps.insert("svg:y", pt2in(y));
  frameProps.remove("librevenge:mime-type");
  frameProps.remove("office:binary-data");

  librevenge::RVNGPropertyList binaryObjectProps;
  binaryObjectProps.insert("librevenge:mime-type", data["librevenge:mime-type"]->clone());
  binaryObjectProps.insert("office:binary-data", data["office:binary-data"]->clone());

  getOutputManager().getCurrent().addOpenFrame(frameProps);
  getOutputManager().getCurrent().addInsertBinaryObject(binaryObjectProps);
  getOutputManager().getCurrent().addCloseFrame();
}

void NUMCollector::fillShapeProperties(librevenge::RVNGPropertyList &/*props*/)
{
  // TODO: implement me
}

void NUMCollector::collectStickyNote()
{
  assert(!m_levelStack.empty());

  auto style=m_levelStack.top().m_graphicStyle;
  if (!style || !style->has<property::Fill>())
  {
    // force shape background to be yellow if it is unknown
    IWORKPropertyMap props;
    props.put<property::Fill>(IWORKColor(1,1,0,1));
    m_levelStack.top().m_graphicStyle=std::make_shared<IWORKStyle>(props, boost::none, style);
  }
  collectShape();
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
