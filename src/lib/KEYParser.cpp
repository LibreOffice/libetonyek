/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYParser.h"
#include "KEYCollector.h"
#include "KEYDefaults.h"
#include "KEYXMLReader.h"

namespace libetonyek
{

KEYParser::KEYParser(const WPXInputStreamPtr_t &input, KEYCollector *const collector, const KEYDefaults &defaults)
  : m_input(input)
  , m_collector(collector)
  , m_defaults(defaults)
{
}

KEYParser::~KEYParser()
{
}

bool KEYParser::parse()
{
  KEYXMLReader reader(m_input.get(), getTokenizer());
  return processXmlDocument(reader);
}

KEYCollector *KEYParser::getCollector() const
{
  return m_collector;
}

const KEYDefaults &KEYParser::getDefaults() const
{
  return m_defaults;
}

bool KEYParser::processXmlDocument(const KEYXMLReader &reader) try
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
