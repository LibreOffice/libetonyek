/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KNParser.h"
#include "KNCollector.h"
#include "KNDefaults.h"
#include "KNXMLReader.h"

namespace libkeynote
{

KNParser::KNParser(const WPXInputStreamPtr_t &input, KNCollector *const collector, const KNDefaults &defaults)
  : m_input(input)
  , m_collector(collector)
  , m_defaults(defaults)
{
}

KNParser::~KNParser()
{
}

bool KNParser::parse()
{
  KNXMLReader reader(m_input.get(), getTokenizer());
  return processXmlDocument(reader);
}

KNCollector *KNParser::getCollector() const
{
  return m_collector;
}

const KNDefaults &KNParser::getDefaults() const
{
  return m_defaults;
}

bool KNParser::processXmlDocument(const KNXMLReader &reader) try
{
  processXmlNode(reader);
  return true;
}
catch (...)
{
  return false;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
