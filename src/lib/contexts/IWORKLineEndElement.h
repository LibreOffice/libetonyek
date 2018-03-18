/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKLINEENDELEMENT_H_INCLUDED
#define IWORKLINEENDELEMENT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKTypes.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKLineEndElement : public IWORKXMLElementContextBase
{
public:
  explicit IWORKLineEndElement(IWORKXMLParserState &state, boost::optional<IWORKMarker> &value);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;

  boost::optional<IWORKMarker> &m_value;
  boost::optional<ID_t> m_id;
};

}

#endif // IWORKLINEENDELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
