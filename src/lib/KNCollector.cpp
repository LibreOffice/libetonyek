/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <libwpg/WPGPaintInterface.h>

#include "KNCollector.h"

using std::string;

namespace libkeynote
{

KNCollector::KNCollector(libwpg::WPGPaintInterface *const painter) :
  m_painter(painter)
{
}

KNCollector::~KNCollector()
{
}

void KNCollector::collectCharacterStyle(const string &id, const KNStyle &style)
{
  m_characterStyles[id] = style;
}

void KNCollector::collectGraphicStyle(const string &id, const KNStyle &style)
{
  m_graphicStyles[id] = style;
}

void KNCollector::collectHeadlineStyle(const std::string  &id, const KNStyle &style)
{
  m_headlineStyles[id] = style;
}

void KNCollector::collectLayoutStyle(const string &id, const KNStyle &style)
{
  m_layoutStyles[id] = style;
}

void KNCollector::collectParagraphStyle(const string &id, const KNStyle &style)
{
  m_paragraphStyles[id] = style;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
