/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAGTypes.h"

namespace libetonyek
{

PAGPublicationInfo::PAGPublicationInfo()
  : m_creationDate()
  , m_footnoteGap()
  , m_footnoteKind(PAG_FOOTNOTE_KIND_FOOTNOTE)
{
}

PAGPageMaster::PAGPageMaster()
  : m_header()
  , m_footer()
{
}

PAGFootnoteState::PAGFootnoteState()
  : m_firstTextAfterMark(false)
  , m_pending(false)
  , m_mark()
  , m_footnotes()
  , m_nextFootnote(m_footnotes.end())
{
}

PAGAttachment::PAGAttachment()
  : m_id(0)
  , m_block(false)
{
}

PAGAttachment::PAGAttachment(const IWORKOutputID_t id, const bool block)
  : m_id(id)
  , m_block(block)
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
