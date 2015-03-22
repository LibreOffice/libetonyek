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

class TabElement : public IWORKXMLEmptyContextBase
{
public:
  explicit TabElement(IWORKXMLParserState &state);

private:
  virtual void endOfElement();
};

TabElement::TabElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

void TabElement::endOfElement()
{
  getCollector()->collectTab();
}

}

namespace
{

class SpanElement : public IWORKXMLElementContextBase
{
public:
  explicit SpanElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void text(const char *value);

private:
  optional<ID_t> m_style;
};

SpanElement::SpanElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void SpanElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::style :
    m_style = value;
    break;
  }
}

IWORKXMLContextPtr_t SpanElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::br :
  case IWORKToken::NS_URI_SF | IWORKToken::crbr :
  case IWORKToken::NS_URI_SF | IWORKToken::intratopicbr :
  case IWORKToken::NS_URI_SF | IWORKToken::lnbr :
    return makeContext<BrContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tab :
    return makeContext<TabElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void SpanElement::text(const char *const value)
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

class LinkElement : public IWORKXMLMixedContextBase
{
public:
  explicit LinkElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void text(const char *value);
};

LinkElement::LinkElement(IWORKXMLParserState &state)
  : IWORKXMLMixedContextBase(state)
{
}

void LinkElement::attribute(int, const char *)
{
}

IWORKXMLContextPtr_t LinkElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::br :
    return makeContext<BrContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::span :
    return makeContext<SpanElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void LinkElement::text(const char *const value)
{
  getCollector()->collectText(IWORKStylePtr_t(), value);
}

}

namespace
{

class PElement : public IWORKXMLMixedContextBase
{
public:
  explicit PElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void endOfAttributes();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
  virtual void text(const char *value);

private:
  IWORKStylePtr_t m_style;
};

PElement::PElement(IWORKXMLParserState &state)
  : IWORKXMLMixedContextBase(state)
{
}

void PElement::attribute(const int name, const char *const value)
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

void PElement::endOfAttributes()
{
  getCollector()->startParagraph(m_style);
}

IWORKXMLContextPtr_t PElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::br :
  case IWORKToken::NS_URI_SF | IWORKToken::crbr :
  case IWORKToken::NS_URI_SF | IWORKToken::intratopicbr :
  case IWORKToken::NS_URI_SF | IWORKToken::lnbr :
    return makeContext<BrContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::span :
    return makeContext<SpanElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tab :
    return makeContext<TabElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::link :
    return makeContext<LinkElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void PElement::text(const char *const value)
{
  getCollector()->collectText(m_style, value);
}

void PElement::endOfElement()
{
  getCollector()->endParagraph();
}

}

namespace
{

class LayoutElement : public IWORKXMLElementContextBase
{
public:
  explicit LayoutElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
};

LayoutElement::LayoutElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void LayoutElement::attribute(const int name, const char *)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::style) == name)
    getCollector()->collectLayoutStyle(IWORKStylePtr_t(), false);
}

IWORKXMLContextPtr_t LayoutElement::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::p) == name)
    return makeContext<PElement>(getState());

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class DataElement : public IWORKXMLEmptyContextBase
{
public:
  DataElement(IWORKXMLParserState &state, IWORKDataPtr_t &data);

private:
  virtual void attribute(int name, const char *value);
  virtual void endOfElement();

private:
  IWORKDataPtr_t &m_data;
  optional<string> m_displayName;
  RVNGInputStreamPtr_t m_stream;
  optional<unsigned> m_type;
};

DataElement::DataElement(IWORKXMLParserState &state, IWORKDataPtr_t &data)
  : IWORKXMLEmptyContextBase(state)
  , m_data(data)
{
}

void DataElement::attribute(const int name, const char *const value)
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

void DataElement::endOfElement()
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

class ImageContext : public IWORKXMLElementContextBase
{
public:
  ImageContext(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content);

protected:
  virtual void endOfElement();

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  IWORKMediaContentPtr_t &m_content;
  IWORKDataPtr_t m_data;
  optional<IWORKSize> m_size;
};

ImageContext::ImageContext(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content)
  : IWORKXMLElementContextBase(state)
  , m_content(content)
  , m_data()
  , m_size()
{
}

IWORKXMLContextPtr_t ImageContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::data :
    return makeContext<DataElement>(getState(), m_data);
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeElement>(getState(), m_size);
  }

  return IWORKXMLContextPtr_t();
}

void ImageContext::endOfElement()
{
  m_content.reset(new IWORKMediaContent());
  m_content->m_size = m_size;
  m_content->m_data = m_data;
}

}

namespace
{

class UnfilteredElement : public ImageContext
{
public:
  UnfilteredElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content);

private:
  virtual void endOfElement();

private:
  IWORKMediaContentPtr_t &m_content;
};

UnfilteredElement::UnfilteredElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content)
  : ImageContext(state, content)
  , m_content(content)
{
}

void UnfilteredElement::endOfElement()
{
  ImageContext::endOfElement();

  if (bool(m_content) && getId())
    getState().getDictionary().m_unfiltereds[get(getId())] = m_content;
}

}

namespace
{

typedef ImageContext FilteredElement;
typedef ImageContext LeveledElement;

}

namespace
{

class FilteredImageElement : public IWORKXMLElementContextBase
{
public:
  FilteredImageElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content);

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

FilteredImageElement::FilteredImageElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content)
  : IWORKXMLElementContextBase(state)
  , m_content(content)
  , m_unfilteredId()
  , m_unfiltered()
  , m_filtered()
  , m_leveled()
{
}

IWORKXMLContextPtr_t FilteredImageElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::unfiltered_ref :
    return makeContext<IWORKRefContext>(getState(), m_unfilteredId);
  case IWORKToken::NS_URI_SF | IWORKToken::unfiltered :
    return makeContext<UnfilteredElement>(getState(), m_unfiltered);
  case IWORKToken::NS_URI_SF | IWORKToken::filtered :
    return makeContext<FilteredElement>(getState(), m_filtered);
  case IWORKToken::NS_URI_SF | IWORKToken::leveled :
    return makeContext<LeveledElement>(getState(), m_leveled);
  }

  return IWORKXMLContextPtr_t();
}

void FilteredImageElement::endOfElement()
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

class ImageMediaElement : public IWORKXMLElementContextBase
{
public:
  ImageMediaElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  IWORKMediaContentPtr_t &m_content;
};

ImageMediaElement::ImageMediaElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content)
  : IWORKXMLElementContextBase(state)
  , m_content(content)
{
}

IWORKXMLContextPtr_t ImageMediaElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::filtered_image :
    return makeContext<FilteredImageElement>(getState(), m_content);
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class OtherDatasElement : public IWORKXMLElementContextBase
{
public:
  OtherDatasElement(IWORKXMLParserState &state, IWORKDataPtr_t &data);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  IWORKDataPtr_t &m_data;
  optional<ID_t> m_dataRef;
};

OtherDatasElement::OtherDatasElement(IWORKXMLParserState &state, IWORKDataPtr_t &data)
  : IWORKXMLElementContextBase(state)
  , m_data(data)
  , m_dataRef()
{
}

IWORKXMLContextPtr_t OtherDatasElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::data :
    return makeContext<DataElement>(getState(), m_data);
  case IWORKToken::NS_URI_SF | IWORKToken::data_ref :
    return makeContext<IWORKRefContext>(getState(), m_dataRef);
  }

  return IWORKXMLContextPtr_t();
}

void OtherDatasElement::endOfElement()
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

class SelfContainedMovieElement : public IWORKXMLElementContextBase
{
public:
  SelfContainedMovieElement(IWORKXMLParserState &state, IWORKDataPtr_t &data);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  IWORKDataPtr_t &m_data;
};

SelfContainedMovieElement::SelfContainedMovieElement(IWORKXMLParserState &state, IWORKDataPtr_t &data)
  : IWORKXMLElementContextBase(state)
  , m_data(data)
{
}

IWORKXMLContextPtr_t SelfContainedMovieElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::other_datas :
    return makeContext<OtherDatasElement>(getState(), m_data);
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class MovieMediaElement : public IWORKXMLElementContextBase
{
public:
  MovieMediaElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  IWORKMediaContentPtr_t &m_content;
  IWORKDataPtr_t m_data;
};

MovieMediaElement::MovieMediaElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content)
  : IWORKXMLElementContextBase(state)
  , m_content(content)
  , m_data()
{
}

IWORKXMLContextPtr_t MovieMediaElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::self_contained_movie :
    return makeContext<SelfContainedMovieElement>(getState(), m_data);
  }

  return IWORKXMLContextPtr_t();
}

void MovieMediaElement::endOfElement()
{
  m_content.reset(new IWORKMediaContent());
  m_content->m_data = m_data;
}

}

namespace
{

class ContentElement : public IWORKXMLElementContextBase
{
public:
  ContentElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  IWORKMediaContentPtr_t &m_content;
};

ContentElement::ContentElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content)
  : IWORKXMLElementContextBase(state)
  , m_content(content)
{
}

IWORKXMLContextPtr_t ContentElement::element(const int name)
{
  if (bool(m_content))
  {
    ETONYEK_DEBUG_MSG(("sf:content containing multiple content elements\n"));
  }

  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::image_media :
    return makeContext<ImageMediaElement>(getState(), m_content);
  case IWORKToken::NS_URI_SF | IWORKToken::movie_media :
    return makeContext<MovieMediaElement>(getState(), m_content);
  }

  return IWORKXMLContextPtr_t();
}

}

IWORKColorElement::IWORKColorElement(IWORKXMLParserState &state, boost::optional<IWORKColor> &color)
  : IWORKXMLEmptyContextBase(state)
  , m_color(color)
  , m_r(0)
  , m_g(0)
  , m_b(0)
  , m_a(0)
{
}

void IWORKColorElement::attribute(const int name, const char *const value)
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

void IWORKColorElement::endOfElement()
{
  m_color = IWORKColor(m_r, m_g, m_b, m_a);
}

IWORKGeometryElement::IWORKGeometryElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_geometry(0)
{
}

IWORKGeometryElement::IWORKGeometryElement(IWORKXMLParserState &state, IWORKGeometryPtr_t &geometry)
  : IWORKXMLElementContextBase(state)
  , m_geometry(&geometry)
{
}

void IWORKGeometryElement::attribute(const int name, const char *const value)
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

IWORKXMLContextPtr_t IWORKGeometryElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::naturalSize :
    return makeContext<IWORKSizeElement>(getState(), m_naturalSize);
  case IWORKToken::NS_URI_SF | IWORKToken::position :
    return makeContext<IWORKPositionElement>(getState(), m_pos);
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeElement>(getState(), m_size);
  }

  return IWORKXMLContextPtr_t();
}

void IWORKGeometryElement::endOfElement()
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

IWORKPositionElement::IWORKPositionElement(IWORKXMLParserState &state, optional<IWORKPosition> &position)
  : IWORKXMLEmptyContextBase(state)
  , m_position(position)
{
}

void IWORKPositionElement::attribute(const int name, const char *const value)
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

void IWORKPositionElement::endOfElement()
{
  if (m_x && m_y)
    m_position = IWORKPosition(get(m_x), get(m_y));
}

IWORKSizeElement::IWORKSizeElement(IWORKXMLParserState &state, boost::optional<IWORKSize> &size)
  : IWORKXMLEmptyContextBase(state)
  , m_size(size)
  , m_width()
  , m_height()
{
}

void IWORKSizeElement::attribute(const int name, const char *const value)
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

void IWORKSizeElement::endOfElement()
{
  if (m_width && m_height)
    m_size = IWORKSize(get(m_width), get(m_height));
}

IWORKTextBodyElement::IWORKTextBodyElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_layout(false)
  , m_para(false)
{
}

IWORKXMLContextPtr_t IWORKTextBodyElement::element(const int name)
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
      return makeContext<LayoutElement>(getState());
    }
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::p :
    if (m_layout)
    {
      ETONYEK_DEBUG_MSG(("paragraph following layout, not allowed, skipping\n"));
    }
    else if (m_para)
    {
      return makeContext<PElement>(getState());
    }
    else
    {
      m_para = true;
      return makeContext<PElement>(getState());
    }
    break;
  }

  return IWORKXMLContextPtr_t();
}

IWORKMediaElement::IWORKMediaElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void IWORKMediaElement::startOfElement()
{
  getCollector()->startLevel();
}

IWORKXMLContextPtr_t IWORKMediaElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::content :
    return makeContext<ContentElement>(getState(), m_content);
  }

  return IWORKXMLContextPtr_t();
}

void IWORKMediaElement::endOfElement()
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
