/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY1FILLELEMENT_H_INCLUDED
#define KEY1FILLELEMENT_H_INCLUDED

#include <string>
#include <boost/optional.hpp>

#include "KEY1XMLContextBase.h"

#include "IWORKEnum.h"
#include "IWORKTypes.h"

namespace libetonyek
{
class KEY1FillElement : public KEY1XMLElementContextBase
{
  enum FillType
  {
    FILL_TYPE_COLOR,
    FILL_TYPE_IMAGE,
    FILL_TYPE_NONE
  };
public:
  KEY1FillElement(KEY1ParserState &state, boost::optional<IWORKFill> &fill);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  boost::optional<IWORKFill> &m_fill;
  boost::optional<FillType> m_type;
  boost::optional<IWORKColor> m_color;
  boost::optional<std::string> m_imageName;
  boost::optional<IWORKImageType> m_imageType;
};
}

#endif // KEY1FILLELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
