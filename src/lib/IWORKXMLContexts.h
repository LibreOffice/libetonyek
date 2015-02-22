/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKXMLCONTEXTS_H_INCLUDED
#define IWORKXMLCONTEXTS_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKTypes.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKColorContext : public IWORKXMLEmptyContextBase
{
public:
  IWORKColorContext(IWORKXMLParserState &state, boost::optional<IWORKColor> &color);

protected:
  virtual void attribute(int name, const char *value);
  virtual void endOfElement();

private:
  boost::optional<IWORKColor> &m_color;
  double m_r;
  double m_g;
  double m_b;
  double m_a;
};

class IWORKGeometryContext : public IWORKXMLElementContextBase
{
public:
  explicit IWORKGeometryContext(IWORKXMLParserState &state);
  IWORKGeometryContext(IWORKXMLParserState &state, IWORKGeometryPtr_t &geometry);

protected:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
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

class IWORKPositionContext : public IWORKXMLEmptyContextBase
{
public:
  IWORKPositionContext(IWORKXMLParserState &state, boost::optional<IWORKPosition> &position);

protected:
  virtual void attribute(int name, const char *value);
  virtual void endOfElement();

private:
  boost::optional<IWORKPosition> &m_position;
  boost::optional<double> m_x;
  boost::optional<double> m_y;
};

class IWORKSizeContext : public IWORKXMLEmptyContextBase
{
public:
  IWORKSizeContext(IWORKXMLParserState &state, boost::optional<IWORKSize> &size);

protected:
  virtual void attribute(int name, const char *value);
  virtual void endOfElement();

private:
  boost::optional<IWORKSize> &m_size;
  boost::optional<double> m_width;
  boost::optional<double> m_height;
};

class IWORKTextBodyContext : public IWORKXMLElementContextBase
{
public:
  explicit IWORKTextBodyContext(IWORKXMLParserState &state);

protected:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  bool m_layout;
  bool m_para;
};

}

#endif // IWORKXMLCONTEXTS_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
