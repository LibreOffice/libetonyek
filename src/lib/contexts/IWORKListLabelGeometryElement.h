/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKLISTLABELGEOMETRYELEMENT_H_INCLUDED
#define IWORKLISTLABELGEOMETRYELEMENT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKTypes.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKListLabelGeometryElement : public IWORKXMLEmptyContextBase
{
public:
  IWORKListLabelGeometryElement(IWORKXMLParserState &state, boost::optional<IWORKListLabelGeometry> &value);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;

private:
  boost::optional<IWORKListLabelGeometry> &m_value;
  boost::optional<double> m_scale;
  boost::optional<double> m_offset;
  boost::optional<bool> m_scaleWithText;
};

}

#endif // IWORKLISTLABELGEOMETRYELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
