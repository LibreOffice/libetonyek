/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY2COLLECTOR_H_INCLUDED
#define KEY2COLLECTOR_H_INCLUDED

#include "KEYCollector.h"

namespace libetonyek
{
class IWORKLanguageManager;

class KEY2Collector : public KEYCollector
{
public:
  explicit KEY2Collector(IWORKDocumentInterface *document);

public:
  std::shared_ptr<IWORKTable> createTable(const IWORKTableNameMapPtr_t &tableNameMap, const IWORKLanguageManager &langManager) const override;
  std::shared_ptr<IWORKText> createText(const IWORKLanguageManager &langManager, bool discardEmptyContent = false) const override;
};

} // namespace libetonyek

#endif // KEY2COLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
