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
#include "IWORKTable.h"
#include "IWORKText.h"
#include "PAGProperties.h"

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

PAGCollector::Section::Section()
  : m_style()
  , m_width()
  , m_height()
  , m_horizontalMargin()
  , m_verticalMargin()
{
}

void PAGCollector::Section::clear()
{
  m_style.reset();
  m_width.reset();
  m_height.reset();
  m_horizontalMargin.reset();
  m_verticalMargin.reset();
}

PAGCollector::PAGCollector(IWORKDocumentInterface *const document)
  : IWORKCollector(document)
  , m_currentSection()
  , m_firstPageSpan(true)
  , m_pageGroups()
  , m_page(0)
  , m_attachmentPosition()
  , m_inAttachments(false)
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

void PAGCollector::openSection(const std::string &style, const double width, const double height, const double horizontalMargin, const double verticalMargin)
{
  m_currentSection.m_width = width;
  m_currentSection.m_height = height;
  m_currentSection.m_horizontalMargin = horizontalMargin;
  m_currentSection.m_verticalMargin = verticalMargin;

  if (!m_stylesheetStack.empty())
  {
    const IWORKStyleMap_t::iterator it = m_stylesheetStack.top()->m_styles.find(style);
    if (it != m_stylesheetStack.top()->m_styles.end())
    {
      m_currentSection.m_style = it->second;
    }
    else
    {
      ETONYEK_DEBUG_MSG(("style '%s' not found\n", style.c_str()));
    }
  }
  else
  {
    ETONYEK_DEBUG_MSG(("no stylesheet is available\n"));
  }
}

void PAGCollector::closeSection()
{
  flushPageSpan();
}

void PAGCollector::openAttachments()
{
  assert(!m_inAttachments);
  m_inAttachments = true;
}

void PAGCollector::closeAttachments()
{
  assert(m_inAttachments);
  m_inAttachments = false;
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
    ETONYEK_DEBUG_MSG(("Page group for page %u already exists\n", m_page));
  }
  getOutputManager().pop();
}

void PAGCollector::drawTable()
{
  assert(bool(m_currentTable));
  assert(!m_levelStack.empty());

  librevenge::RVNGPropertyList props;

  // TODO: I am not sure this is the default for Pages...
  props.insert("table:align", "center");

  const IWORKGeometryPtr_t geometry(m_levelStack.top().m_geometry);
  if (geometry)
  {
    const glm::dvec3 dim(m_levelStack.top().m_trafo * glm::dvec3(geometry->m_naturalSize.m_width, 0, 0));
    props.insert("style:width", pt2in(dim[0]));
  }

  m_currentTable->draw(props, m_outputManager.getCurrent(), true);
}

void PAGCollector::drawMedia(
  const double x, const double y, const double w, const double h,
  const std::string &mimetype, const librevenge::RVNGBinaryData &data)
{
  RVNGPropertyList frameProps;
  if (m_inAttachments)
  {
    frameProps.insert("text:anchor-type", "char");
  }
  else
  {
    frameProps.insert("text:anchor-type", "page");
    frameProps.insert("text:anchor-page-number", m_page);
  }
  if (m_attachmentPosition)
  {
    frameProps.insert("svg:x", pt2in(get(m_attachmentPosition).m_x));
    frameProps.insert("svg:y", pt2in(get(m_attachmentPosition).m_y));
  }
  else
  {
    frameProps.insert("svg:x", pt2in(x));
    frameProps.insert("svg:y", pt2in(y));
  }
  frameProps.insert("svg:width", pt2in(w));
  frameProps.insert("svg:height", pt2in(h));

  RVNGPropertyList binaryObjectProps;
  binaryObjectProps.insert("librevenge:mime-type", mimetype.c_str());
  binaryObjectProps.insert("office:binary-data", data);

  getOutputManager().getCurrent().addOpenFrame(frameProps);
  getOutputManager().getCurrent().addInsertBinaryObject(binaryObjectProps);
  getOutputManager().getCurrent().addCloseFrame();
}

void PAGCollector::fillShapeProperties(librevenge::RVNGPropertyList &props)
{
  props.insert("text:anchor-type", "page");
  props.insert("text:anchor-page-number", m_page);
}

void PAGCollector::drawTextBox(const IWORKTextPtr_t &text, const glm::dmat3 &trafo, const IWORKGeometryPtr_t &boundingBox)
{
  (void) text;
  (void) trafo;
  (void) boundingBox;
  // FIXME: Text content is never parsed as empty. And the text boxes
  // half cover the shapes in the output.
#if 0
  if (bool(text) && !text->empty())
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

    fillShapeProperties(props);

    IWORKOutputElements &elements = m_outputManager.getCurrent();
    elements.addOpenFrame(props);
    elements.addStartTextObject(RVNGPropertyList());
    text->draw(elements);
    elements.addEndTextObject();
    elements.addCloseFrame();
  }
#endif
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

  if (m_currentSection.m_width)
    props.insert("fo:page-width", get(m_currentSection.m_width));
  if (m_currentSection.m_height)
    props.insert("fo:page-height", get(m_currentSection.m_height));
  if (m_currentSection.m_horizontalMargin)
  {
    props.insert("fo:margin-left", get(m_currentSection.m_horizontalMargin));
    props.insert("fo:margin-right", get(m_currentSection.m_horizontalMargin));
  }
  if (m_currentSection.m_verticalMargin)
  {
    props.insert("fo:margin-top", get(m_currentSection.m_verticalMargin));
    props.insert("fo:margin-bottom", get(m_currentSection.m_verticalMargin));
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
    if (m_currentSection.m_style)
    {
      writeHeadersFooters(m_document, m_currentSection.m_style, m_headers, pickHeader,
                          &IWORKDocumentInterface::openHeader, &IWORKDocumentInterface::closeHeader);
      writeHeadersFooters(m_document, m_currentSection.m_style, m_footers, pickFooter,
                          &IWORKDocumentInterface::openFooter, &IWORKDocumentInterface::closeFooter);
    }
    text.write(m_document);
    m_document->closePageSpan();
  }

  m_currentSection.clear();
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
