/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKXMLParserState.h"

#include <memory>

#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKParser.h"
#include "IWORKTable.h"
#include "IWORKText.h"
#include "IWORKTypes.h"

namespace libetonyek
{

IWORKXMLParserState::IWORKXMLParserState(IWORKParser &parser, IWORKCollector &collector, IWORKDictionary &dict)
  : m_tableData()
  , m_stylesheet()
  , m_enableCollector(true)
  , m_formatNameMap()
  , m_tableNameMap(std::make_shared<IWORKTableNameMap_t>())
  , m_langManager()
  , m_currentTable()
  , m_currentText()
  , m_parser(parser)
  , m_collector(collector)
  , m_dict(dict)
{
}

IWORKParser &IWORKXMLParserState::getParser()
{
  return m_parser;
}

IWORKDictionary &IWORKXMLParserState::getDictionary()
{
  return m_dict;
}

IWORKCollector &IWORKXMLParserState::getCollector() const
{
  return m_collector;
}

const IWORKTokenizer &IWORKXMLParserState::getTokenizer() const
{
  return m_parser.getTokenizer();
}

IWORKStylePtr_t IWORKXMLParserState::getStyleByName(const char *const name, const IWORKStyleMap_t &mainMap, bool mustExist) const
{
  if (!name)
  {
    ETONYEK_DEBUG_MSG(("IWORKXMLParserState::getStyleByName: called without name\n"));
    return IWORKStylePtr_t();
  }
  const IWORKStyleMap_t::const_iterator it = mainMap.find(name);
  if (mainMap.end() != it)
    return it->second;
  if (m_stylesheet && m_stylesheet->m_styles.find(name)!=m_stylesheet->m_styles.end())
    return m_stylesheet->m_styles.find(name)->second;
  if (mustExist)
  {
    ETONYEK_DEBUG_MSG(("IWORKXMLParserState::getStyleByName: unknown style %s\n", name));
  }
  return IWORKStylePtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
