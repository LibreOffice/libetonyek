/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKLISTLABELTYPEINFOELEMENT_H_INCLUDED
#define IWORKLISTLABELTYPEINFOELEMENT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKTypes.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKListLabelTypeinfoElement : public IWORKXMLElementContextBase
{
public:
  IWORKListLabelTypeinfoElement(IWORKXMLParserState &state, boost::optional<IWORKListLabelTypeInfo_t> &value);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  boost::optional<IWORKListLabelTypeInfo_t> &m_value;
  boost::optional<IWORKListLabelTypeInfo_t> m_text;
  boost::optional<ID_t> m_textRef;
  IWORKMediaContentPtr_t m_image;
  boost::optional<ID_t> m_imageRef;
  bool m_isText;
  bool m_isImage;
};

}

#endif // IWORKLISTLABELTYPEINFOELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
