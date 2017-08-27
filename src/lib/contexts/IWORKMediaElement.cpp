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

#include "IWORKCollector.h"
#include "IWORKDataElement.h"
#include "IWORKDictionary.h"
#include "IWORKFilteredImageElement.h"
#include "IWORKGeometryElement.h"
#include "IWORKParser.h"
#include "IWORKRefContext.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

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
  case IWORKToken::NS_URI_SF | IWORKToken::filtered_image :
    return makeContext<IWORKFilteredImageElement>(getState(), m_content);
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
  IWORKXMLContextPtr_t element(int name) override;

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
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

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
  if (isCollector())
    getCollector().startLevel();
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
  if (isCollector())
  {
    getCollector().collectMedia(m_content);
    getCollector().endLevel();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
