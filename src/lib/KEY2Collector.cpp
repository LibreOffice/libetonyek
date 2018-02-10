/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY2Collector.h"

#include <memory>

#include "IWORKRecorder.h"
#include "IWORKTable.h"
#include "IWORKTableRecorder.h"
#include "IWORKText.h"
#include "IWORKTextRecorder.h"

namespace libetonyek
{

KEY2Collector::KEY2Collector(IWORKDocumentInterface *const document)
  : KEYCollector(document)
{
}

std::shared_ptr<IWORKTable> KEY2Collector::createTable(const IWORKTableNameMapPtr_t &tableNameMap, const IWORKLanguageManager &langManager) const
{
  const std::shared_ptr<IWORKTable> table(IWORKCollector::createTable(tableNameMap, langManager));
  if (m_paint)
    table->setRecorder(std::make_shared<IWORKTableRecorder>());
  return table;
}

std::shared_ptr<IWORKText> KEY2Collector::createText(const IWORKLanguageManager &langManager, const bool discardEmptyContent, bool allowListInsertion) const
{
  const std::shared_ptr<IWORKText> text(IWORKCollector::createText(langManager, discardEmptyContent, allowListInsertion));
  if (m_paint)
    text->setRecorder(std::make_shared<IWORKTextRecorder>());
  return text;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
