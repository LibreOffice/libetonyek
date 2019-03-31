/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef NUM3PARSER_H_INCLUDED
#define NUM3PARSER_H_INCLUDED

#include "IWAParser.h"

namespace libetonyek
{

class NUMCollector;

class NUM3Parser : public IWAParser
{
public:
  NUM3Parser(const RVNGInputStreamPtr_t &fragments, const RVNGInputStreamPtr_t &package, NUMCollector &collector);

private:
  bool parseDocument() override;
  bool parseShapePlacement(const IWAMessage &msg, IWORKGeometryPtr_t &geometry) override;
  bool parseStickyNote(const IWAMessage &msg) override;

  bool parseSheet(unsigned id);

private:
  NUMCollector &m_collector;
};

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
