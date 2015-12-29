/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKLISTLABELGEOMETRIESPROPERTY_H_INCLUDED
#define IWORKLISTLABELGEOMETRIESPROPERTY_H_INCLUDED

#include <deque>

#include "IWORKTypes.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKListLabelGeometriesProperty : public IWORKXMLElementContextBase
{
public:
  IWORKListLabelGeometriesProperty(IWORKXMLParserState &state, std::deque<IWORKListLabelGeometry> &elements);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  std::deque<IWORKListLabelGeometry> &m_elements;
};

}

#endif // IWORKLISTLABELGEOMETRIESPROPERTY_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
