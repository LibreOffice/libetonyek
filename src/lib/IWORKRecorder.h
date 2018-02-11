/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKRECORDER_H_INCLUDED
#define IWORKRECORDER_H_INCLUDED

#include <memory>
#include <string>

#include <boost/optional.hpp>

#include "IWORKPath_fwd.h"
#include "IWORKStyle.h"
#include "IWORKStylesheet.h"
#include "IWORKTypes_fwd.h"

namespace libetonyek
{

class IWORKCollector;
class IWORKTable;
class IWORKText;
struct IWORKSize;

class IWORKRecorder
{
public:
  IWORKRecorder();

  void replay(IWORKCollector &collector) const;

  // recorder functions

  void collectStyle(const IWORKStylePtr_t &style);

  void setGraphicStyle(const IWORKStylePtr_t &style);

  void collectGeometry(const IWORKGeometryPtr_t &geometry);

  void collectPath(const IWORKPathPtr_t &path);
  void collectImage(const IWORKMediaContentPtr_t &image, const IWORKGeometryPtr_t &cropGeometry, bool locked);
  void collectLine(const IWORKLinePtr_t &line);
  void collectShape(bool locked);

  void collectMedia(const IWORKMediaContentPtr_t &content, const IWORKGeometryPtr_t &cropGeometry);

  void collectStylesheet(const IWORKStylesheetPtr_t &stylesheet);

  void collectTable(const std::shared_ptr<IWORKTable> &table);
  void collectText(const std::shared_ptr<IWORKText> &text);

  void startAttachment();
  void endAttachment();

  void startAttachments();
  void endAttachments();

  void startGroup();
  void endGroup();

  void openGroup();
  void closeGroup();

  void startLevel();
  void endLevel();

  void pushStylesheet(const IWORKStylesheetPtr_t &stylesheet);
  void popStylesheet();

private:
  struct Impl;

private:
  std::shared_ptr<Impl> m_impl;
};

} // namespace libetonyek

#endif // IWORKRECORDER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
