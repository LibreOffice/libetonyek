/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAGCollector.h"

#include <cassert>
#include <memory>

#include "IWORKDocumentInterface.h"
#include "IWORKOutputElements.h"
#include "IWORKPath.h"
#include "IWORKTable.h"
#include "IWORKText.h"
#include "PAGProperties.h"
#include "libetonyek_utils.h"

namespace libetonyek
{

using librevenge::RVNGPropertyList;

using std::string;

namespace
{

typedef void (IWORKDocumentInterface::*OpenFunction)(const RVNGPropertyList &);
typedef void (IWORKDocumentInterface::*CloseFunction)();
typedef const std::string &(*PickFunction)(const PAGPageMaster &);

const std::string &pickHeader(const PAGPageMaster &pageMaster)
{
  return pageMaster.m_header;
}

const std::string &pickFooter(const PAGPageMaster &pageMaster)
{
  return pageMaster.m_footer;
}

void writeHeaderFooter(
  IWORKDocumentInterface *const document, const IWORKHeaderFooterMap_t &hfMap,
  const string &name, const string &occurrence,
  const OpenFunction open, const CloseFunction close)
{
  assert(document);
  if (name.empty())
    return;

  const IWORKHeaderFooterMap_t::const_iterator it = hfMap.find(name);
  if ((it != hfMap.end()) && !it->second.empty())
  {
    RVNGPropertyList props;
    props.insert("librevenge:occurrence", occurrence.c_str());
    (document->*open)(props);
    it->second.write(document);
    (document->*close)();
  }
}

void writeHeadersFooters(
  IWORKDocumentInterface *const document, const IWORKStylePtr_t &style, const IWORKHeaderFooterMap_t &hfMap,
  const PickFunction pick, const OpenFunction open, const CloseFunction close)
{
  assert(bool(style));

  using namespace property;
  const string odd((style->has<OddPageMaster>()) ? pick(style->get<OddPageMaster>()) : "");
  const string even((style->has<EvenPageMaster>()) ? pick(style->get<EvenPageMaster>()) : "");
  const string first((style->has<FirstPageMaster>()) ? pick(style->get<FirstPageMaster>()) : "");

  if (odd == even)
  {
    writeHeaderFooter(document, hfMap, odd, "both", open, close);
  }
  else
  {
    writeHeaderFooter(document, hfMap, odd, "odd", open, close);
    writeHeaderFooter(document, hfMap, even, "even", open, close);
  }
  writeHeaderFooter(document, hfMap, first, "first", open, close);
}

}

PAGCollector::PAGCollector(IWORKDocumentInterface *const document)
  : IWORKCollector(document)
  , m_pageDimensions()
  , m_currentSectionStyle()
  , m_firstPageSpan(true)
  , m_pubInfo()
  , m_pageGroups()
  , m_page(0)
  , m_attachmentPosition()
  , m_annotations()
{
}

void PAGCollector::collectAnnotation(const std::string &name)
{
  IWORKOutputElements &elements = m_annotations[name];
  if (!elements.empty())
  {
    ETONYEK_DEBUG_MSG(("PAGCollector::collectAnnotation '%s' already exists, overwriting\n", name.c_str()));
    elements.clear();
  }
  if (bool(m_currentText))
  {
    librevenge::RVNGPropertyList propList;
    elements.addOpenComment(propList);
    m_currentText->draw(elements);
    elements.addCloseComment();
    m_currentText.reset();
  }
  else
  {
    ETONYEK_DEBUG_MSG(("PAGCollector::sendAnnotation: called without text\n"));
  }
}

void PAGCollector::collectPublicationInfo(const PAGPublicationInfo &pubInfo)
{
  m_pubInfo = pubInfo;
}

void PAGCollector::collectTextBody()
{
  // It seems that this is never used, as Pages always inserts all text
  // into a section. But better safe than sorry.
  flushPageSpan(false);
}

void PAGCollector::collectAttachmentPosition(const IWORKPosition &position)
{
  m_attachmentPosition = position;
}

void PAGCollector::setPageDimensions(const IWORKPrintInfo &dimensions)
{
  m_pageDimensions=dimensions;
}

void PAGCollector::openSection(const std::string &style)
{
  if (!m_stylesheetStack.empty())
  {
    const IWORKStyleMap_t::iterator it = m_stylesheetStack.top()->m_styles.find(style);
    if (it != m_stylesheetStack.top()->m_styles.end())
    {
      m_currentSectionStyle = it->second;
    }
    else
    {
      ETONYEK_DEBUG_MSG(("PAGCollector::openSection: style '%s' not found\n", style.c_str()));
    }
  }
  else
  {
    ETONYEK_DEBUG_MSG(("PAGCollector::openSection: no stylesheet is available\n"));
  }
}

void PAGCollector::closeSection()
{
  flushPageSpan();
}

void PAGCollector::sendAnnotation(const std::string &name)
{
  if (m_annotations.find(name)==m_annotations.end())
  {
    ETONYEK_DEBUG_MSG(("PAGCollector::sendAnnotation can not find annotation'%s'\n", name.c_str()));
    m_currentText.reset();
    return;
  }
  if (bool(m_currentText))
  {
    m_currentText->insertInlineContent(m_annotations.find(name)->second);
    m_currentText.reset();
  }
  else
  {
    ETONYEK_DEBUG_MSG(("PAGCollector::sendAnnotation: called without text\n"));
  }
}

void PAGCollector::openPageGroup(const boost::optional<int> &page)
{
  getOutputManager().push();
  if (page)
    m_page = get(page);
  else
    ++m_page;
}

void PAGCollector::closePageGroup()
{
  typedef std::pair<PageGroupsMap_t::const_iterator, bool> Result_t;
  const IWORKOutputID_t id = getOutputManager().save();
  const Result_t result = m_pageGroups.insert(PageGroupsMap_t::value_type(m_page, id));
  if (!result.second)
  {
    ETONYEK_DEBUG_MSG(("PAGCollector::closePageGroup: Page group for page %u already exists\n", m_page));
  }
  getOutputManager().pop();
}

void PAGCollector::drawTable()
{
  assert(bool(m_currentTable));
  assert(!m_levelStack.empty());

  RVNGPropertyList frameProps;
  librevenge::RVNGPropertyList props;

  // TODO: I am not sure this is the default for Pages...
  props.insert("table:align", "center");

  const IWORKGeometryPtr_t geometry(m_levelStack.top().m_geometry);
  if (m_inAttachments)
  {
    if (geometry)
    {
      const glm::dvec3 dim(m_levelStack.top().m_trafo * glm::dvec3(geometry->m_naturalSize.m_width, 0, 0));
      props.insert("style:width", pt2in(dim[0]));
    }
  }
  else
  {
    fillShapeProperties(frameProps);

    const glm::dmat3 trafo = m_levelStack.top().m_trafo;
    const glm::dvec3 pos(trafo * glm::dvec3(0, 0, 1));
    frameProps.insert("svg:x", pos[0], librevenge::RVNG_POINT);
    frameProps.insert("svg:y", pos[1], librevenge::RVNG_POINT);
    if (geometry)
    {
      const glm::dvec3 dim(trafo * glm::dvec3(geometry->m_naturalSize.m_width, geometry->m_naturalSize.m_height, 0));
      frameProps.insert("svg:width", pt2in(dim[0]), librevenge::RVNG_INCH);
      frameProps.insert("svg:height", pt2in(dim[1]), librevenge::RVNG_INCH);
    }
  }

  if (m_inAttachments)
    m_currentTable->draw(props, m_outputManager.getCurrent(), true);
  else
  {
    /* in Oasis v1.2, we can add the table directly in a frame, but
       LibreOffice does not display it, so ...*/
    getOutputManager().getCurrent().addOpenFrame(frameProps);
    getOutputManager().getCurrent().addStartTextObject(RVNGPropertyList());
    m_currentTable->draw(props, m_outputManager.getCurrent(), true);
    getOutputManager().getCurrent().addEndTextObject();
    getOutputManager().getCurrent().addCloseFrame();
  }
}

void PAGCollector::drawShape(const IWORKShapePtr_t &shape)
{
  if (!bool(shape) || !bool(shape->m_path))
  {
    ETONYEK_DEBUG_MSG(("PAGCollector::drawShape: can not find the shape\n"));
    return;
  }
  const glm::dmat3 trafo = m_levelStack.top().m_trafo;
  IWORKOutputElements &elements = m_outputManager.getCurrent();

  const IWORKPath path = m_inAttachment ? *shape->m_path : (*shape->m_path * trafo);
  bool isRectangle=path.isRectangle();
  bool hasText=bool(shape->m_text) && !shape->m_text->empty();
  bool createOnlyTextbox=!m_inAttachment && hasText && isRectangle;
  librevenge::RVNGPropertyList styleProps;

  if (bool(shape->m_style))
    fillGraphicProps(shape->m_style, styleProps, true, createOnlyTextbox);
  if (shape->m_locked) // CHECKME: maybe also content
    styleProps.insert("style:protect", "position size");
  if (createOnlyTextbox)
  {
    // we can create a basic textbox
    drawTextBox(shape->m_text, trafo, shape->m_geometry, styleProps);
    return;
  }

  librevenge::RVNGPropertyList shapeProps;
  librevenge::RVNGPropertyListVector vec;
  path.write(vec);
  shapeProps.insert("svg:d", vec);

  if (m_inAttachment)
  {
    shapeProps.insert("text:anchor-type", "as-char");
    shapeProps.insert("style:vertical-pos", "bottom");
    shapeProps.insert("style:vertical-rel", "text");
    shapeProps.insert("style:run-through", "foreground");
    shapeProps.insert("style:wrap","run-through");
  }
  else
    fillShapeProperties(shapeProps);

  elements.addSetStyle(styleProps);
  elements.addDrawPath(shapeProps);

  if (!m_inAttachment && hasText)
    drawTextBox(shape->m_text, trafo, shape->m_geometry, librevenge::RVNGPropertyList());
  else if (hasText)
  {
    ETONYEK_DEBUG_MSG(("PAGCollector::drawShape: sorry sending text in a attachment is not implemented\n"));
  }
}

void PAGCollector::drawMedia(const double x, const double y, const librevenge::RVNGPropertyList &data)
{
  if (!data["office:binary-data"] || !data["librevenge:mime-type"])
  {
    ETONYEK_DEBUG_MSG(("PAGCollector::drawMedia: oops can not find the picture\n"));
  }
  RVNGPropertyList frameProps(data);
  if (m_inAttachments)
  {
    frameProps.insert("text:anchor-type", "as-char");
    frameProps.insert("style:vertical-pos", "bottom");
    frameProps.insert("style:vertical-rel", "text");
  }
  else
    fillShapeProperties(frameProps);
  if (m_inAttachments && m_attachmentPosition)
  {
    frameProps.insert("svg:x", pt2in(get(m_attachmentPosition).m_x));
    frameProps.insert("svg:y", pt2in(get(m_attachmentPosition).m_y));
  }
  else
  {
    frameProps.insert("svg:x", pt2in(x));
    frameProps.insert("svg:y", pt2in(y));
  }
  frameProps.remove("librevenge:mime-type");
  frameProps.remove("office:binary-data");

  RVNGPropertyList binaryObjectProps;
  binaryObjectProps.insert("librevenge:mime-type", data["librevenge:mime-type"]->clone());
  binaryObjectProps.insert("office:binary-data", data["office:binary-data"]->clone());

  getOutputManager().getCurrent().addOpenFrame(frameProps);
  getOutputManager().getCurrent().addInsertBinaryObject(binaryObjectProps);
  getOutputManager().getCurrent().addCloseFrame();
}

void PAGCollector::fillShapeProperties(librevenge::RVNGPropertyList &props)
{
  props.insert("text:anchor-type", "page");
  props.insert("text:anchor-page-number", m_page);
  props.insert("style:vertical-pos", "from-top");
  props.insert("style:vertical-rel", "page");
}

void PAGCollector::drawTextBox(const IWORKTextPtr_t &text, const glm::dmat3 &trafo, const IWORKGeometryPtr_t &boundingBox, const librevenge::RVNGPropertyList &style)
{
  if (!bool(text) || text->empty())
    return;

  librevenge::RVNGPropertyList props(style);

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

  fillShapeProperties(props);

  IWORKOutputElements &elements = m_outputManager.getCurrent();
  elements.addOpenFrame(props);
  elements.addStartTextObject(librevenge::RVNGPropertyList());
  text->draw(elements);
  elements.addEndTextObject();
  elements.addCloseFrame();
}

void PAGCollector::flushPageSpan(const bool writeEmpty)
{
  if (m_firstPageSpan)
  {
    RVNGPropertyList metadata;
    fillMetadata(metadata);
    m_document->setDocumentMetaData(metadata);
    writePageGroupsObjects();
    m_firstPageSpan = false;
  }

  librevenge::RVNGPropertyList props;

  if (m_pageDimensions)
  {
    IWORKPrintInfo const &page=get(m_pageDimensions);
    if (page.m_width)
      props.insert("fo:page-width", get(page.m_width), librevenge::RVNG_POINT);
    if (page.m_height)
      props.insert("fo:page-height", get(page.m_height), librevenge::RVNG_POINT);
    if (page.m_marginBottom)
      props.insert("fo:margin-bottom", get(page.m_marginBottom), librevenge::RVNG_POINT);
    if (page.m_marginLeft)
      props.insert("fo:margin-left", get(page.m_marginLeft), librevenge::RVNG_POINT);
    if (page.m_marginRight)
      props.insert("fo:margin-right", get(page.m_marginRight), librevenge::RVNG_POINT);
    if (page.m_marginTop)
      props.insert("fo:margin-top", get(page.m_marginTop), librevenge::RVNG_POINT);
    //TODO set also the header/footer height here
  }
  IWORKOutputElements text;

  if (bool(m_currentText))
  {
    m_currentText->draw(text);
    m_currentText.reset();
  }

  if (!text.empty() || writeEmpty)
  {
    m_document->openPageSpan(props);
    if (m_currentSectionStyle)
    {
      writeHeadersFooters(m_document, m_currentSectionStyle, m_headers, pickHeader,
                          &IWORKDocumentInterface::openHeader, &IWORKDocumentInterface::closeHeader);
      writeHeadersFooters(m_document, m_currentSectionStyle, m_footers, pickFooter,
                          &IWORKDocumentInterface::openFooter, &IWORKDocumentInterface::closeFooter);
    }
    text.write(m_document);
    m_document->closePageSpan();
  }

  m_currentSectionStyle.reset();
}

void PAGCollector::writePageGroupsObjects()
{
  for (PageGroupsMap_t::const_iterator it = m_pageGroups.begin(); it != m_pageGroups.end(); ++it)
    getOutputManager().get(it->second).write(m_document);
}

PAGFootnoteKind PAGCollector::getFootnoteKind() const
{
  return m_pubInfo.m_footnoteKind;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
