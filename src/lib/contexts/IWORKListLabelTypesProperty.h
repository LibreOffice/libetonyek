/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKLISTLABELTYPESPROPERTY_H_INCLUDED
#define IWORKLISTLABELTYPESPROPERTY_H_INCLUDED

#include <deque>

#include "IWORKTypes.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKListLabelTypesProperty : public IWORKXMLElementContextBase
{
public:
  IWORKListLabelTypesProperty(IWORKXMLParserState &state, std::deque<IWORKListLabelTypeInfo_t> &elements);

private:
  IWORKXMLContextPtr_t element(int name) override;

private:
  std::deque<IWORKListLabelTypeInfo_t> &m_elements;
};

}

#endif // IWORKLISTLABELTYPESPROPERTY_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
