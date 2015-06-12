/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKDATAELEMENT_H_INCLUDED
#define IWORKDATAELEMENT_H_INCLUDED

#include <string>

#include <boost/optional.hpp>

#include "libetonyek_utils.h"
#include "IWORKTypes_fwd.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKDataElement : public IWORKXMLEmptyContextBase
{
public:
  IWORKDataElement(IWORKXMLParserState &state, IWORKDataPtr_t &data);

private:
  virtual void attribute(int name, const char *value);
  virtual void endOfElement();

private:
  IWORKDataPtr_t &m_data;
  boost::optional<std::string> m_displayName;
  RVNGInputStreamPtr_t m_stream;
  boost::optional<std::string> m_mimeType;
};

}

#endif // IWORKDATAELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
