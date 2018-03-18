/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKFIELDELEMENT_H_INCLUDED
#define IWORKFIELDELEMENT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKEnum.h"

#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKFieldElement : public IWORKXMLMixedContextBase
{
public:
  IWORKFieldElement(IWORKXMLParserState &state, IWORKFieldType type);

protected:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void text(const char *value) override;
  void endOfElement() override;

private:
  IWORKFieldType m_type;
  boost::optional<unsigned> m_value;
  boost::optional<std::string> m_val;
};

}

#endif // IWORKFIELDELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
