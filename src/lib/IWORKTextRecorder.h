/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTEXTRECORDER_H_INCLUDED
#define IWORKTEXTRECORDER_H_INCLUDED

#include <memory>
#include <string>

#include "IWORKEnum.h"
#include "IWORKStyle_fwd.h"

namespace libetonyek
{

class IWORKText;

class IWORKTextRecorder
{
public:
  IWORKTextRecorder();

  void replay(IWORKText &text) const;

  void pushBaseLayoutStyle(const IWORKStylePtr_t &style);
  void pushBaseParagraphStyle(const IWORKStylePtr_t &style);

  void setLayoutStyle(const IWORKStylePtr_t &style);
  void flushLayout();

  void setListStyle(const IWORKStylePtr_t &style);
  void setListLevel(const unsigned level);
  void flushList();

  void setParagraphStyle(const IWORKStylePtr_t &style);
  void flushParagraph();

  void setSpanStyle(const IWORKStylePtr_t &style);
  void setLanguage(const IWORKStylePtr_t &style);
  void flushSpan();

  void insertField(IWORKFieldType fieldType);
  void openLink(const std::string &url);
  void closeLink();

  void insertText(const std::string &text);
  void insertTab();
  void insertSpace();
  void insertLineBreak();
  void insertPageBreak();

private:
  struct Impl;

private:
  std::shared_ptr<Impl> m_impl;
};

}

#endif // IWORKTEXTRECORDER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
