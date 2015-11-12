/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PAGCOLLECTOR_H_INCLUDED
#define PAGCOLLECTOR_H_INCLUDED

#include <map>

#include "IWORKCollector.h"
#include "PAGTypes.h"

namespace libetonyek
{

class IWORKDocumentInterface;

class PAGCollector : public IWORKCollector
{
  struct Section
  {
    Section();

    void clear();

    IWORKStylePtr_t m_style;
    boost::optional<double> m_width;
    boost::optional<double> m_height;
    boost::optional<double> m_horizontalMargin;
    boost::optional<double> m_verticalMargin;
  };

  typedef std::map<unsigned, IWORKOutputID_t> PageGroupsMap_t;

public:
  explicit PAGCollector(IWORKDocumentInterface *document);

  // collector functions

  void collectPublicationInfo(const PAGPublicationInfo &pubInfo);

  void collectTextBody();

  void collectAttachment(const IWORKOutputID_t &id, bool block);
  void collectAttachmentPosition(const IWORKPosition &position);

  void openPageGroup(const boost::optional<int> &page);
  void closePageGroup();

  // helper functions

  void openSection(const std::string &style, double width, double height, double horizontalMargin, double verticalMargin);
  void closeSection();

  void openAttachments();
  void closeAttachments();

  PAGFootnoteKind getFootnoteKind() const;

private:
  virtual void drawTable();
  virtual void drawMedia(double x, double y, double w, double h, const std::string &mimetype, const librevenge::RVNGBinaryData &data);
  virtual void fillShapeProperties(librevenge::RVNGPropertyList &props);
  virtual void drawTextBox(const IWORKTextPtr_t &text, const glm::dmat3 &trafo, const IWORKGeometryPtr_t &boundingBox);

  void flushPageSpan(bool writeEmpty = true);
  void writePageGroupsObjects();

private:
  Section m_currentSection;
  bool m_firstPageSpan;

  PAGPublicationInfo m_pubInfo;

  PageGroupsMap_t m_pageGroups;
  int m_page;

  // FIXME: This is a clumsy workaround.
  boost::optional<IWORKPosition> m_attachmentPosition;
  bool m_inAttachments;
};

} // namespace libetonyek

#endif //  PAGCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
