/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKMETADATAELEMENT_H_INCLUDED
#define IWORKMETADATAELEMENT_H_INCLUDED

#include "IWORKTypes.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKMetadataElement : public IWORKXMLElementContextBase
{
public:
  explicit IWORKMetadataElement(IWORKXMLParserState &state);

protected:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

protected:
  IWORKMetadata m_metadata;

private:
  boost::optional<std::string> m_author;
  boost::optional<std::string> m_title;
  boost::optional<std::string> m_keywords;
  boost::optional<std::string> m_comment;
};

}

#endif // IWORKMETADATAELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
