/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKWRAPELEMENT_H_INCLUDED
#define IWORKWRAPELEMENT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKTypes.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKExternalTextWrapElement : public IWORKXMLElementContextBase
{
public:
  IWORKExternalTextWrapElement(IWORKXMLParserState &state, boost::optional<IWORKExternalTextWrap> &wrap);

protected:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKExternalTextWrapElement(const IWORKExternalTextWrapElement &);
  IWORKExternalTextWrapElement &operator=(const IWORKExternalTextWrapElement &);

  boost::optional<IWORKExternalTextWrap> &m_wrap;
};

class IWORKWrapElement : public IWORKXMLElementContextBase
{
public:
  IWORKWrapElement(IWORKXMLParserState &state, boost::optional<IWORKWrap> &wrap);

protected:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;

private:
  IWORKWrapElement(const IWORKWrapElement &);
  IWORKWrapElement &operator=(const IWORKWrapElement &);

  boost::optional<IWORKWrap> &m_wrap;
};

}

#endif // IWORKWRAPELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
