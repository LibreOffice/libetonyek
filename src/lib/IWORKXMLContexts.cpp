/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKXMLContexts.h"

#include <string>

#include <boost/lexical_cast.hpp>

#include "libetonyek_utils.h"
#include "libetonyek_xml.h"
#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKParser.h"
#include "IWORKPropertyMap.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

using boost::lexical_cast;
using boost::optional;

using std::string;

namespace
{

class BrContext : public IWORKXMLEmptyContextBase
{
public:
  explicit BrContext(IWORKXMLParserState &state);

private:
  virtual void endOfElement();
};

BrContext::BrContext(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

void BrContext::endOfElement()
{
  getCollector()->collectLineBreak();
}

}

namespace
{

class TabContext : public IWORKXMLEmptyContextBase
{
public:
  explicit TabContext(IWORKXMLParserState &state);

private:
  virtual void endOfElement();
};

TabContext::TabContext(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

void TabContext::endOfElement()
{
  getCollector()->collectTab();
}

}

namespace
{

class SpanContext : public IWORKXMLElementContextBase
{
public:
  explicit SpanContext(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void text(const char *value);

private:
  optional<ID_t> m_style;
};

SpanContext::SpanContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void SpanContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::style :
    m_style = value;
    break;
  }
}

IWORKXMLContextPtr_t SpanContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::br :
  case IWORKToken::NS_URI_SF | IWORKToken::crbr :
  case IWORKToken::NS_URI_SF | IWORKToken::intratopicbr :
  case IWORKToken::NS_URI_SF | IWORKToken::lnbr :
    return makeContext<BrContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tab :
    return makeContext<TabContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void SpanContext::text(const char *const value)
{
  IWORKStylePtr_t style;
  if (m_style)
  {
    const IWORKStyleMap_t::const_iterator it = getState().getDictionary().m_characterStyles.find(get(m_style));
    if (getState().getDictionary().m_characterStyles.end() != it)
      style = it->second;
  }
  getCollector()->collectText(style, value);
}

}

namespace
{

class LinkContext : public IWORKXMLMixedContextBase
{
public:
  explicit LinkContext(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void text(const char *value);
};

LinkContext::LinkContext(IWORKXMLParserState &state)
  : IWORKXMLMixedContextBase(state)
{
}

void LinkContext::attribute(int, const char *)
{
}

IWORKXMLContextPtr_t LinkContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::br :
    return makeContext<BrContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::span :
    return makeContext<SpanContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void LinkContext::text(const char *const value)
{
  getCollector()->collectText(IWORKStylePtr_t(), value);
}

}

namespace
{

class PContext : public IWORKXMLMixedContextBase
{
public:
  explicit PContext(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void endOfAttributes();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
  virtual void text(const char *value);

private:
  IWORKStylePtr_t m_style;
};

PContext::PContext(IWORKXMLParserState &state)
  : IWORKXMLMixedContextBase(state)
{
}

void PContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::style :
  {
    const IWORKStyleMap_t::const_iterator it = getState().getDictionary().m_paragraphStyles.find(value);
    if (getState().getDictionary().m_paragraphStyles.end() != it)
      m_style = it->second;
    break;
  }
  }
}

void PContext::endOfAttributes()
{
  getCollector()->startParagraph(m_style);
}

IWORKXMLContextPtr_t PContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::br :
  case IWORKToken::NS_URI_SF | IWORKToken::crbr :
  case IWORKToken::NS_URI_SF | IWORKToken::intratopicbr :
  case IWORKToken::NS_URI_SF | IWORKToken::lnbr :
    return makeContext<BrContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::span :
    return makeContext<SpanContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tab :
    return makeContext<TabContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::link :
    return makeContext<LinkContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void PContext::text(const char *const value)
{
  getCollector()->collectText(m_style, value);
}

void PContext::endOfElement()
{
  getCollector()->endParagraph();
}

}

namespace
{

class LayoutContext : public IWORKXMLElementContextBase
{
public:
  explicit LayoutContext(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
};

LayoutContext::LayoutContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void LayoutContext::attribute(const int name, const char *)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::style) == name)
    getCollector()->collectLayoutStyle(IWORKStylePtr_t(), false);
}

IWORKXMLContextPtr_t LayoutContext::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::p) == name)
    return makeContext<PContext>(getState());

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class DataContext : public IWORKXMLEmptyContextBase
{
public:
  DataContext(IWORKXMLParserState &state, IWORKDataPtr_t &data);

private:
  virtual void attribute(int name, const char *value);
  virtual void endOfElement();

private:
  IWORKDataPtr_t &m_data;
  optional<string> m_displayName;
  RVNGInputStreamPtr_t m_stream;
  optional<unsigned> m_type;
};

DataContext::DataContext(IWORKXMLParserState &state, IWORKDataPtr_t &data)
  : IWORKXMLEmptyContextBase(state)
  , m_data(data)
{
}

void DataContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::displayname :
    m_displayName = value;
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::hfs_type :
    m_type = lexical_cast<unsigned>(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::path :
    m_stream.reset(getState().getParser().getPackage()->getSubStreamByName(value));
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    // TODO: handle
    break;
  default :
    break;
  }
}

void DataContext::endOfElement()
{
  if (bool(m_stream))
  {
    m_data.reset(new IWORKData());
    m_data->m_stream = m_stream;
    m_data->m_displayName = m_displayName;
    m_data->m_type = m_type;

    if (getId())
      getState().getDictionary().m_data[get(getId())] = m_data;
  }
}

}

namespace
{

class ImageContextBase : public IWORKXMLElementContextBase
{
public:
  ImageContextBase(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content);

protected:
  virtual void endOfElement();

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  IWORKMediaContentPtr_t &m_content;
  IWORKDataPtr_t m_data;
  optional<IWORKSize> m_size;
};

ImageContextBase::ImageContextBase(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content)
  : IWORKXMLElementContextBase(state)
  , m_content(content)
  , m_data()
  , m_size()
{
}

IWORKXMLContextPtr_t ImageContextBase::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::data :
    return makeContext<DataContext>(getState(), m_data);
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeContext>(getState(), m_size);
  }

  return IWORKXMLContextPtr_t();
}

void ImageContextBase::endOfElement()
{
  m_content.reset(new IWORKMediaContent());
  m_content->m_size = m_size;
  m_content->m_data = m_data;
}

}

namespace
{

class UnfilteredContext : public ImageContextBase
{
public:
  UnfilteredContext(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content);

private:
  virtual void endOfElement();

private:
  IWORKMediaContentPtr_t &m_content;
};

UnfilteredContext::UnfilteredContext(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content)
  : ImageContextBase(state, content)
  , m_content(content)
{
}

void UnfilteredContext::endOfElement()
{
  ImageContextBase::endOfElement();

  if (bool(m_content) && getId())
    getState().getDictionary().m_unfiltereds[get(getId())] = m_content;
}

}

namespace
{

typedef ImageContextBase FilteredContext;
typedef ImageContextBase LeveledContext;

}

namespace
{

class FilteredImageContext : public IWORKXMLElementContextBase
{
public:
  FilteredImageContext(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  IWORKMediaContentPtr_t &m_content;
  optional<ID_t> m_unfilteredId;
  IWORKMediaContentPtr_t m_unfiltered;
  IWORKMediaContentPtr_t m_filtered;
  IWORKMediaContentPtr_t m_leveled;
};

FilteredImageContext::FilteredImageContext(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content)
  : IWORKXMLElementContextBase(state)
  , m_content(content)
  , m_unfilteredId()
  , m_unfiltered()
  , m_filtered()
  , m_leveled()
{
}

IWORKXMLContextPtr_t FilteredImageContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::unfiltered_ref :
    return makeContext<IWORKRefContext>(getState(), m_unfilteredId);
  case IWORKToken::NS_URI_SF | IWORKToken::unfiltered :
    return makeContext<UnfilteredContext>(getState(), m_unfiltered);
  case IWORKToken::NS_URI_SF | IWORKToken::filtered :
    return makeContext<FilteredContext>(getState(), m_filtered);
  case IWORKToken::NS_URI_SF | IWORKToken::leveled :
    return makeContext<LeveledContext>(getState(), m_leveled);
  }

  return IWORKXMLContextPtr_t();
}

void FilteredImageContext::endOfElement()
{
  if (m_unfilteredId && !m_unfiltered)
  {
    const IWORKMediaContentMap_t::const_iterator it = getState().getDictionary().m_unfiltereds.find(get(m_unfilteredId));
    if (getState().getDictionary().m_unfiltereds.end() != it)
      m_unfiltered = it->second;
  }

  // If a filter is applied to an image, the new image is saved next
  // to the original. So all we need is to pick the right one. We
  // can happily ignore the whole filter-properties section :-)
  // NOTE: Leveled is apparently used to save the result of using
  // the "Enhance" button.
  if (bool(m_filtered))
    m_content = m_filtered;
  else if (bool(m_leveled))
    m_content = m_leveled;
  else
    m_content = m_unfiltered;

  if (bool(m_content) && getId())
    getState().getDictionary().m_filteredImages[get(getId())] = m_content;
}

}

namespace
{

class ImageMediaContext : public IWORKXMLElementContextBase
{
public:
  ImageMediaContext(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  IWORKMediaContentPtr_t &m_content;
};

ImageMediaContext::ImageMediaContext(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content)
  : IWORKXMLElementContextBase(state)
  , m_content(content)
{
}

IWORKXMLContextPtr_t ImageMediaContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::filtered_image :
    return makeContext<FilteredImageContext>(getState(), m_content);
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class OtherDatasContext : public IWORKXMLElementContextBase
{
public:
  OtherDatasContext(IWORKXMLParserState &state, IWORKDataPtr_t &data);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  IWORKDataPtr_t &m_data;
  optional<ID_t> m_dataRef;
};

OtherDatasContext::OtherDatasContext(IWORKXMLParserState &state, IWORKDataPtr_t &data)
  : IWORKXMLElementContextBase(state)
  , m_data(data)
  , m_dataRef()
{
}

IWORKXMLContextPtr_t OtherDatasContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::data :
    return makeContext<DataContext>(getState(), m_data);
  case IWORKToken::NS_URI_SF | IWORKToken::data_ref :
    return makeContext<IWORKRefContext>(getState(), m_dataRef);
  }

  return IWORKXMLContextPtr_t();
}

void OtherDatasContext::endOfElement()
{
  if (m_dataRef && !m_data)
  {
    const IWORKDataMap_t::const_iterator it = getState().getDictionary().m_data.find(get(m_dataRef));
    if (getState().getDictionary().m_data.end() != it)
      m_data = it->second;
  }
}

}

namespace
{

class SelfContainedMovieContext : public IWORKXMLElementContextBase
{
public:
  SelfContainedMovieContext(IWORKXMLParserState &state, IWORKDataPtr_t &data);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  IWORKDataPtr_t &m_data;
};

SelfContainedMovieContext::SelfContainedMovieContext(IWORKXMLParserState &state, IWORKDataPtr_t &data)
  : IWORKXMLElementContextBase(state)
  , m_data(data)
{
}

IWORKXMLContextPtr_t SelfContainedMovieContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::other_datas :
    return makeContext<OtherDatasContext>(getState(), m_data);
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class MovieMediaContext : public IWORKXMLElementContextBase
{
public:
  MovieMediaContext(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  IWORKMediaContentPtr_t &m_content;
  IWORKDataPtr_t m_data;
};

MovieMediaContext::MovieMediaContext(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content)
  : IWORKXMLElementContextBase(state)
  , m_content(content)
  , m_data()
{
}

IWORKXMLContextPtr_t MovieMediaContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::self_contained_movie :
    return makeContext<SelfContainedMovieContext>(getState(), m_data);
  }

  return IWORKXMLContextPtr_t();
}

void MovieMediaContext::endOfElement()
{
  m_content.reset(new IWORKMediaContent());
  m_content->m_data = m_data;
}

}

namespace
{

class ContentContext : public IWORKXMLElementContextBase
{
public:
  ContentContext(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  IWORKMediaContentPtr_t &m_content;
};

ContentContext::ContentContext(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content)
  : IWORKXMLElementContextBase(state)
  , m_content(content)
{
}

IWORKXMLContextPtr_t ContentContext::element(const int name)
{
  if (bool(m_content))
  {
    ETONYEK_DEBUG_MSG(("sf:content containing multiple content elements\n"));
  }

  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::image_media :
    return makeContext<ImageMediaContext>(getState(), m_content);
  case IWORKToken::NS_URI_SF | IWORKToken::movie_media :
    return makeContext<MovieMediaContext>(getState(), m_content);
  }

  return IWORKXMLContextPtr_t();
}

}

IWORKColorContext::IWORKColorContext(IWORKXMLParserState &state, boost::optional<IWORKColor> &color)
  : IWORKXMLEmptyContextBase(state)
  , m_color(color)
  , m_r(0)
  , m_g(0)
  , m_b(0)
  , m_a(0)
{
}

void IWORKColorContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  // TODO: check xsi:type too
  case IWORKToken::NS_URI_SFA | IWORKToken::a :
    m_a = lexical_cast<double>(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::b :
    m_b = lexical_cast<double>(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::g :
    m_g = lexical_cast<double>(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::r :
    m_r = lexical_cast<double>(value);
    break;
  }
}

void IWORKColorContext::endOfElement()
{
  m_color = IWORKColor(m_r, m_g, m_b, m_a);
}

IWORKGeometryContext::IWORKGeometryContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_geometry(0)
{
}

IWORKGeometryContext::IWORKGeometryContext(IWORKXMLParserState &state, IWORKGeometryPtr_t &geometry)
  : IWORKXMLElementContextBase(state)
  , m_geometry(&geometry)
{
}

void IWORKGeometryContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::angle :
    m_angle = deg2rad(lexical_cast<double>(value));
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::aspectRatioLocked :
    m_aspectRatioLocked = bool_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::horizontalFlip :
    m_horizontalFlip = bool_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::shearXAngle :
    m_shearXAngle = deg2rad(lexical_cast<double>(value));
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::shearYAngle :
    m_shearYAngle = deg2rad(lexical_cast<double>(value));
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::sizesLocked :
    m_sizesLocked = bool_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::verticalFlip :
    m_verticalFlip = bool_cast(value);
    break;
  default :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t IWORKGeometryContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::naturalSize :
    return makeContext<IWORKSizeContext>(getState(), m_naturalSize);
  case IWORKToken::NS_URI_SF | IWORKToken::position :
    return makeContext<IWORKPositionContext>(getState(), m_pos);
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeContext>(getState(), m_size);
  }

  return IWORKXMLContextPtr_t();
}

void IWORKGeometryContext::endOfElement()
{
  IWORKGeometryPtr_t geometry(new IWORKGeometry());
  geometry->m_naturalSize = get(m_naturalSize);
  geometry->m_size = bool(m_size) ? get(m_size) : get(m_naturalSize);
  geometry->m_position = get(m_pos);
  geometry->m_angle = m_angle;
  geometry->m_shearXAngle = m_shearXAngle;
  geometry->m_shearYAngle = m_shearYAngle;
  geometry->m_horizontalFlip = m_horizontalFlip;
  geometry->m_verticalFlip = m_verticalFlip;
  geometry->m_aspectRatioLocked = m_aspectRatioLocked;
  geometry->m_sizesLocked = m_sizesLocked;

  if (m_geometry)
    *m_geometry = geometry;
  else
    getCollector()->collectGeometry(geometry);
}

IWORKPositionContext::IWORKPositionContext(IWORKXMLParserState &state, optional<IWORKPosition> &position)
  : IWORKXMLEmptyContextBase(state)
  , m_position(position)
{
}

void IWORKPositionContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::x :
    m_x = lexical_cast<double>(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::y :
    m_y = lexical_cast<double>(value);
    break;
  }
}

void IWORKPositionContext::endOfElement()
{
  if (m_x && m_y)
    m_position = IWORKPosition(get(m_x), get(m_y));
}

IWORKSizeContext::IWORKSizeContext(IWORKXMLParserState &state, boost::optional<IWORKSize> &size)
  : IWORKXMLEmptyContextBase(state)
  , m_size(size)
  , m_width()
  , m_height()
{
}

void IWORKSizeContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::w :
    m_width = lexical_cast<double>(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::h :
    m_height = lexical_cast<double>(value);
    break;
  }
}

void IWORKSizeContext::endOfElement()
{
  if (m_width && m_height)
    m_size = IWORKSize(get(m_width), get(m_height));
}

IWORKTextBodyContext::IWORKTextBodyContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_layout(false)
  , m_para(false)
{
}

IWORKXMLContextPtr_t IWORKTextBodyContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layout :
    if (m_layout || m_para)
    {
      ETONYEK_DEBUG_MSG(("layout following another element, not allowed, skipping\n"));
    }
    else
    {
      m_layout = true;
      return makeContext<LayoutContext>(getState());
    }
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::p :
    if (m_layout)
    {
      ETONYEK_DEBUG_MSG(("paragraph following layout, not allowed, skipping\n"));
    }
    else if (m_para)
    {
      return makeContext<PContext>(getState());
    }
    else
    {
      m_para = true;
      return makeContext<PContext>(getState());
    }
    break;
  }

  return IWORKXMLContextPtr_t();
}

IWORKMediaContext::IWORKMediaContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void IWORKMediaContext::startOfElement()
{
  getCollector()->startLevel();
}

IWORKXMLContextPtr_t IWORKMediaContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::content :
    return makeContext<ContentContext>(getState(), m_content);
  }

  return IWORKXMLContextPtr_t();
}

void IWORKMediaContext::endOfElement()
{
  getCollector()->collectMedia(m_content);
  getCollector()->endLevel();
}

IWORKRefContext::IWORKRefContext(IWORKXMLParserState &state, optional<ID_t> &ref)
  : IWORKXMLEmptyContextBase(state)
  , m_ref(ref)
{
}

void IWORKRefContext::endOfElement()
{
  m_ref = getRef();
}


}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
