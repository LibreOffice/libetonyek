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

namespace libetonyek
{

class IWORKDocumentInterface;

class PAGMetadata;
class PAGPublicationInfo;

class PAGCollector : public IWORKCollector
{
  struct Section
  {
    Section();

    void clear();

    boost::optional<double> m_width;
    boost::optional<double> m_height;
    boost::optional<double> m_horizontalMargin;
    boost::optional<double> m_verticalMargin;
  };

public:
  explicit PAGCollector(IWORKDocumentInterface *document);

  // collector functions

  void collectPublicationInfo(const PAGPublicationInfo &pubInfo);
  void collectMetadata(const IWORKMetadata &metadata);

  void collectTextBody();
  void collectAttachment(const IWORKOutputID_t &id);

  // helper functions
  void startDocument();
  void endDocument();

  void openSection(double width, double height, double horizontalMargin, double verticalMargin);
  void closeSection();

private:
  virtual void drawTable();

private:
  Section m_currentSection;
};

} // namespace libetonyek

#endif //  PAGCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
