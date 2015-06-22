/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PAGTYPES_H_INCLUDED
#define PAGTYPES_H_INCLUDED

#include <string>

#include <boost/optional.hpp>
#include <boost/unordered_map.hpp>

#include "IWORKTypes_fwd.h"
#include "PAGEnum.h"

namespace libetonyek
{

struct PAGPublicationInfo
{
  PAGPublicationInfo();

  boost::optional<std::string> m_creationDate;
  boost::optional<double> m_footnoteGap;
  PAGFootnoteKind m_footnoteKind;
};

struct PAGPageMaster
{
  PAGPageMaster();

  std::string m_header;
  std::string m_footer;
};

struct PAGFootnoteState
{
  PAGFootnoteState();

  bool m_firstTextAfterMark;
  bool m_pending;
  std::string m_mark;
};

struct PAGAttachment
{
  PAGAttachment();
  PAGAttachment(IWORKOutputID_t id, bool block);

  IWORKOutputID_t m_id;
  bool m_block;
};

typedef boost::unordered_map<ID_t, PAGAttachment> PAGAttachmentMap_t;

}

#endif //  PAGTYPES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
