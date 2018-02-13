/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKMediaElement.h"

#include <boost/optional.hpp>
#include <memory>

#include "libetonyek_xml.h"

#include "IWORKBinaryElement.h"
#include "IWORKCollector.h"
#include "IWORKDataElement.h"
#include "IWORKDictionary.h"
#include "IWORKFilteredImageElement.h"
#include "IWORKGeometryElement.h"
#include "IWORKParser.h"
#include "IWORKRefContext.h"
#include "IWORKSizeElement.h"
#include "IWORKStyleContainer.h"
#include "IWORKToken.h"
#include "IWORKWrapElement.h"
#include "IWORKXMLParserState.h"

#include "PAG1Token.h"

namespace libetonyek
{

using boost::optional;

using std::string;

namespace
{

class ImageMediaElement : public IWORKXMLElementContextBase
{
public:
  ImageMediaElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content);

private:
  IWORKXMLContextPtr_t element(int name) override;

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
  case IWORKToken::NS_URI_SF | IWORKToken::alpha_mask_path : // README
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::filtered_image :
    return makeContext<IWORKFilteredImageElement>(getState(), m_content);
  case IWORKToken::NS_URI_SF | IWORKToken::traced_path : // README
    break;
  default:
    ETONYEK_DEBUG_MSG(("ImageMediaElement::element[IWORKMediaElement.cpp]: unknown element %d\n", name));
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
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

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
    return makeContext<IWORKDataElement>(getState(), m_data);
  case IWORKToken::NS_URI_SF | IWORKToken::data_ref :
    return makeContext<IWORKRefContext>(getState(), m_dataRef);
  default:
    ETONYEK_DEBUG_MSG(("OtherDatasElement::element[IWORKMediaElement.cpp]: unknown element %d\n", name));
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
    else
    {
      ETONYEK_DEBUG_MSG(("OtherDatasElement::element[IWORKMediaElement.cpp]: can not find %s\n", get(m_dataRef).c_str()));
    }
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
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKDataPtr_t &m_data;
  IWORKDataPtr_t m_otherData;
  optional<ID_t> m_mainMovieRef;
};

SelfContainedMovieElement::SelfContainedMovieElement(IWORKXMLParserState &state, IWORKDataPtr_t &data)
  : IWORKXMLElementContextBase(state)
  , m_data(data)
  , m_otherData()
  , m_mainMovieRef()
{
}

IWORKXMLContextPtr_t SelfContainedMovieElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::main_movie :
    return makeContext<IWORKDataElement>(getState(), m_data);
  case IWORKToken::NS_URI_SF | IWORKToken::main_movie_ref :
    return makeContext<IWORKRefContext>(getState(), m_mainMovieRef);
  case IWORKToken::NS_URI_SF | IWORKToken::other_datas :
    return makeContext<OtherDatasElement>(getState(), m_otherData);
  default:
    ETONYEK_DEBUG_MSG(("SelfContainedMovieElement::element[IWORKMediaElement.cpp]: unknown element %d\n", name));
  }

  return IWORKXMLContextPtr_t();
}

void SelfContainedMovieElement::endOfElement()
{
  if (m_data)
    return;
  if (m_mainMovieRef)
  {
    const IWORKDataMap_t::const_iterator it = getState().getDictionary().m_data.find(get(m_mainMovieRef));
    if (getState().getDictionary().m_data.end() != it)
    {
      m_data = it->second;
      return;
    }
    else
    {
      ETONYEK_DEBUG_MSG(("SelfContainedMovieElement::element[IWORKMediaElement.cpp]: can not find %s\n", get(m_mainMovieRef).c_str()));
    }
  }
  m_data=m_otherData;
}
}

namespace
{

class MovieMediaElement : public IWORKXMLElementContextBase
{
public:
  MovieMediaElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKMediaContentPtr_t &m_content;
  IWORKDataPtr_t m_data;
  IWORKMediaContentPtr_t m_audioOnlyImage;
  IWORKMediaContentPtr_t m_posterImage;
  boost::optional<ID_t> m_audioOnlyImageRef;
};

MovieMediaElement::MovieMediaElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content)
  : IWORKXMLElementContextBase(state)
  , m_content(content)
  , m_data()
  , m_audioOnlyImage()
  , m_posterImage()
  , m_audioOnlyImageRef()
{
}

IWORKXMLContextPtr_t MovieMediaElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::audio_only_image :
    return makeContext<IWORKBinaryElement>(getState(), m_audioOnlyImage);
  case IWORKToken::NS_URI_SF | IWORKToken::audio_only_image_ref :
    return makeContext<IWORKRefContext>(getState(), m_audioOnlyImageRef);
  case IWORKToken::NS_URI_SF | IWORKToken::poster_image :
    return makeContext<IWORKBinaryElement>(getState(), m_posterImage);
  case IWORKToken::NS_URI_SF | IWORKToken::self_contained_movie :
    return makeContext<SelfContainedMovieElement>(getState(), m_data);
  default:
    ETONYEK_DEBUG_MSG(("MovieMediaElement::element[IWORKMediaElement.cpp]: unknown element %d\n", name));
  }

  return IWORKXMLContextPtr_t();
}

void MovieMediaElement::endOfElement()
{
  if (m_data)
  {
    m_content = std::make_shared<IWORKMediaContent>();
    m_content->m_data = m_data;
    return;
  }
  if (m_posterImage)
    m_content = m_posterImage;
  else if (m_audioOnlyImage)
    m_content = m_audioOnlyImage;
  else if (m_audioOnlyImageRef)
  {
    const IWORKMediaContentMap_t::const_iterator it = getState().getDictionary().m_binaries.find(get(m_audioOnlyImageRef));
    if (getState().getDictionary().m_binaries.end() != it)
      m_content = it->second;
    else
    {
      ETONYEK_DEBUG_MSG(("MovieMediaElement::endOfElement[IWORKMediaElement.cpp]: can not find image %s\n", get(m_audioOnlyImageRef).c_str()));
    }
  }
}

}

namespace
{

class ContentElement : public IWORKXMLElementContextBase
{
public:
  ContentElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content);

private:
  IWORKXMLContextPtr_t element(int name) override;

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
  default:
    ETONYEK_DEBUG_MSG(("ContentElement::element[IWORKMediaElement.cpp]: unknown element %d\n", name));
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{
typedef IWORKStyleContainer<IWORKToken::NS_URI_SF | IWORKToken::graphic_style, IWORKToken::NS_URI_SF | IWORKToken::graphic_style_ref> GraphicStyleContext;
}

IWORKMediaElement::IWORKMediaElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_content()
  , m_movieData()
  , m_audioOnlyImage()
  , m_posterImage()
  , m_audioOnlyImageRef()
  , m_style()
  , m_cropGeometry()
  , m_order()
  , m_wrap()
  , m_placeholderSize()
{
}

void IWORKMediaElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case PAG1Token::order | PAG1Token::NS_URI_SL :
    m_order=try_int_cast(value);
    break;
  default:
    IWORKXMLElementContextBase::attribute(name, value);
  }
}

void IWORKMediaElement::startOfElement()
{
  if (isCollector())
    getCollector().startLevel();
}

IWORKXMLContextPtr_t IWORKMediaElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::audio_only_image :
    return makeContext<IWORKBinaryElement>(getState(), m_audioOnlyImage);
  case IWORKToken::NS_URI_SF | IWORKToken::audio_only_image_ref :
    return makeContext<IWORKRefContext>(getState(), m_audioOnlyImageRef);
  case IWORKToken::NS_URI_SF | IWORKToken::content :
    return makeContext<ContentElement>(getState(), m_content);
  case IWORKToken::NS_URI_SF | IWORKToken::crop_geometry :
    return makeContext<IWORKGeometryElement>(getState(), m_cropGeometry);
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::masking_shape_path_source :
  {
    static bool first=true;
    if (first)
    {
      ETONYEK_DEBUG_MSG(("IWORKMediaElement::element: find some masking shape's paths\n"));
      first=false;
    }
    break;
  }
  case IWORKToken::NS_URI_SF | IWORKToken::placeholder_size : // USEME
    return makeContext<IWORKSizeElement>(getState(),m_placeholderSize);
  case IWORKToken::NS_URI_SF | IWORKToken::poster_image :
    return makeContext<IWORKBinaryElement>(getState(), m_posterImage);
  case IWORKToken::NS_URI_SF | IWORKToken::self_contained_movie :
    return makeContext<SelfContainedMovieElement>(getState(), m_movieData);
  case IWORKToken::NS_URI_SF | IWORKToken::style : // USEME
    return makeContext<GraphicStyleContext>(getState(), m_style, getState().getDictionary().m_graphicStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::wrap : // USEME
    return makeContext<IWORKWrapElement>(getState(), m_wrap);
  default:
    ETONYEK_DEBUG_MSG(("IWORKMediaElement::element: find some unknown elements\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void IWORKMediaElement::endOfElement()
{
  if (!isCollector())
    return;
  if (!m_content)
  {
    if (m_movieData)
    {
      m_content = std::make_shared<IWORKMediaContent>();
      m_content->m_data = m_movieData;
    }
    else if (m_posterImage)
      m_content = m_posterImage;
    else if (m_audioOnlyImage)
      m_content = m_audioOnlyImage;
    else if (m_audioOnlyImageRef)
    {
      const IWORKMediaContentMap_t::const_iterator it = getState().getDictionary().m_binaries.find(get(m_audioOnlyImageRef));
      if (getState().getDictionary().m_binaries.end() != it)
        m_content = it->second;
      else
      {
        ETONYEK_DEBUG_MSG(("IWORKMediaElement::endOfElement: can not find image %s\n", get(m_audioOnlyImageRef).c_str()));
      }
    }
  }
  getCollector().collectMedia(m_content, m_cropGeometry, m_order);
  getCollector().endLevel();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
