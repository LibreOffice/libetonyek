/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKGEOMETRYELEMENT_H_INCLUDED
#define IWORKGEOMETRYELEMENT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKTypes.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKGeometryElement : public IWORKXMLElementContextBase
{
public:
  explicit IWORKGeometryElement(IWORKXMLParserState &state);
  IWORKGeometryElement(IWORKXMLParserState &state, IWORKGeometryPtr_t &geometry);

protected:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKGeometryElement(const IWORKGeometryElement &);
  IWORKGeometryElement &operator=(const IWORKGeometryElement &);

  IWORKGeometryPtr_t *const m_geometry;
  boost::optional<IWORKSize> m_naturalSize;
  boost::optional<IWORKSize> m_size;
  boost::optional<IWORKPosition> m_pos;
  boost::optional<double> m_angle;
  boost::optional<double> m_shearXAngle;
  boost::optional<double> m_shearYAngle;
  boost::optional<bool> m_aspectRatioLocked;
  boost::optional<bool> m_sizesLocked;
  boost::optional<bool> m_horizontalFlip;
  boost::optional<bool> m_verticalFlip;
};

}

#endif // IWORKGEOMETRYELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
