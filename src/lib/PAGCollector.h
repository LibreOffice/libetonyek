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

public:
  explicit PAGCollector(IWORKDocumentInterface *document);

  // collector functions

  void collectPublicationInfo(const PAGPublicationInfo &pubInfo);

  void collectTextBody();
  void collectAttachment(const IWORKOutputID_t &id);

  void collectFootnote();
  void insertFootnote();

  // helper functions

  void flushFootnote();

  void openSection(const std::string &style, double width, double height, double horizontalMargin, double verticalMargin);
  void closeSection();

private:
  virtual void drawTable();
  void flushPageSpan(bool writeEmpty = true);

private:
  Section m_currentSection;
  bool m_firstPageSpan;

  std::deque<IWORKOutputElements> m_footnotes;
  std::deque<IWORKOutputElements>::const_iterator m_nextFootnote;
  bool m_pendingFootnote;

  PAGPublicationInfo m_pubInfo;
};

} // namespace libetonyek

#endif //  PAGCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
