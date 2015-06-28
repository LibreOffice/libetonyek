/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKCHARTINFOELEMENT_H_INCLUDED
#define IWORKCHARTINFOELEMENT_H_INCLUDED

#include "IWORKXMLContextBase.h"

#include <IWORKChart.h>

namespace libetonyek
{

class IWORKChartInfoElement : public IWORKXMLElementContextBase
{

public:
  explicit IWORKChartInfoElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

public:
  IWORKChart m_chart;
};

}

#endif // IWORKCHARTINFOELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
