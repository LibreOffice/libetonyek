/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKMediaElement.h"

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKGeometryElement.h"
#include "IWORKParser.h"
#include "IWORKRefContext.h"
#include "IWORKSizeElement.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

using boost::lexical_cast;
using boost::optional;

using std::string;

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

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
