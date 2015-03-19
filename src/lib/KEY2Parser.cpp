/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY2Parser.h"

#include <cassert>
#include <utility>

#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include "libetonyek_utils.h"
#include "libetonyek_xml.h"
#include "IWORKPath.h"
#include "IWORKStyle.h"
#include "IWORKTabularInfoContext.h"
#include "IWORKToken.h"
#include "IWORKTypes.h"
#include "IWORKXMLContexts.h"
#include "IWORKXMLReader.h"
#include "KEY2ParserState.h"
#include "KEY2StyleContext.h"
#include "KEY2Token.h"
#include "KEY2XMLContextBase.h"
#include "KEYCollector.h"
#include "KEYDictionary.h"
#include "KEYTypes.h"

using boost::get_optional_value_or;
using boost::lexical_cast;
using boost::numeric_cast;
using boost::optional;

using std::pair;
using std::string;

namespace libetonyek
{

namespace
{

unsigned getVersion(const int token)
{
  switch (token)
  {
  case KEY2Token::VERSION_STR_2 :
    return 2;
  case KEY2Token::VERSION_STR_3 :
    return 3;
  case KEY2Token::VERSION_STR_4 :
    return 4;
  case KEY2Token::VERSION_STR_5 :
    return 5;
  }

  return 0;
}

}

namespace
{

class RefContext : public KEY2XMLEmptyContextBase
{
public:
  RefContext(KEY2ParserState &state, optional<ID_t> &ref);

private:
  virtual void endOfElement();

private:
  optional<ID_t> &m_ref;
};

RefContext::RefContext(KEY2ParserState &state, optional<ID_t> &ref)
  : KEY2XMLEmptyContextBase(state)
  , m_ref(ref)
{
}

void RefContext::endOfElement()
{
  m_ref = getRef();
}

}

namespace
{

class MetadataContext : public KEY2XMLElementContextBase
{
public:
  explicit MetadataContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

MetadataContext::MetadataContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t MetadataContext::element(int)
{
  // TODO: parse
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class StylesContext : public KEY2XMLElementContextBase
{
public:
  StylesContext(KEY2ParserState &state, bool anonymous);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  const bool m_anonymous;
};

StylesContext::StylesContext(KEY2ParserState &state, const bool anonymous)
  : KEY2XMLElementContextBase(state)
  , m_anonymous(anonymous)
{
}

IWORKXMLContextPtr_t StylesContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::cell_style :
  case IWORKToken::NS_URI_SF | IWORKToken::characterstyle :
  case IWORKToken::NS_URI_SF | IWORKToken::connection_style :
  case IWORKToken::NS_URI_SF | IWORKToken::graphic_style :
  case IWORKToken::NS_URI_SF | IWORKToken::headline_style :
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle :
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle :
  case IWORKToken::NS_URI_SF | IWORKToken::placeholder_style :
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle :
  case IWORKToken::NS_URI_SF | IWORKToken::slide_style :
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_style :
  case IWORKToken::NS_URI_SF | IWORKToken::vector_style :
    return makeContext<KEY2StyleContext>(getState(), name);

  case IWORKToken::NS_URI_SF | IWORKToken::cell_style_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::characterstyle_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::vector_style_ref :
    break;
    return makeContext<KEY2StyleRefContext>(getState(), name, false, m_anonymous);
  }

  return IWORKXMLContextPtr_t();
}

void StylesContext::endOfElement()
{
}

}

namespace
{

class StylesheetContext : public KEY2XMLElementContextBase
{
public:
  explicit StylesheetContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<ID_t> m_parent;
};

StylesheetContext::StylesheetContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t StylesheetContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::styles :
    return makeContext<StylesContext>(getState(), false);
  case IWORKToken::NS_URI_SF | IWORKToken::anon_styles :
    return makeContext<StylesContext>(getState(), false);
  case IWORKToken::NS_URI_SF | IWORKToken::parent_ref :
    return makeContext<RefContext>(getState(), m_parent);
  }

  return IWORKXMLContextPtr_t();
}

void StylesheetContext::endOfElement()
{
  IWORKStylesheetPtr_t parent;

  if (m_parent)
  {
    assert(getId() != m_parent);

    const IWORKStylesheetMap_t::const_iterator it = getState().getDictionary().m_stylesheets.find(get(m_parent));
    if (getState().getDictionary().m_stylesheets.end() != it)
      parent = it->second;
  }

  const IWORKStylesheetPtr_t stylesheet = getCollector()->collectStylesheet(parent);
  if (bool(stylesheet) && getId())
    getState().getDictionary().m_stylesheets[get(getId())] = stylesheet;
}

}

namespace
{

class ProxyMasterLayerContext : public KEY2XMLElementContextBase
{
public:
  explicit ProxyMasterLayerContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<ID_t> m_ref;
};

ProxyMasterLayerContext::ProxyMasterLayerContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_ref()
{
}

IWORKXMLContextPtr_t ProxyMasterLayerContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layer_ref :
    return makeContext<RefContext>(getState(), m_ref);
  }

  return IWORKXMLContextPtr_t();
}

void ProxyMasterLayerContext::endOfElement()
{
  if (m_ref)
  {
    const KEYLayerMap_t::const_iterator it = getState().getDictionary().m_layers.find(get(m_ref));
    if (getState().getDictionary().m_layers.end() != it)
      getCollector()->insertLayer(it->second);
  }
}

}

namespace
{

class DataContext : public KEY2XMLEmptyContextBase
{
public:
  explicit DataContext(KEY2ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void endOfElement();

private:
  optional<string> m_displayName;
  RVNGInputStreamPtr_t m_stream;
  optional<unsigned> m_type;
};

DataContext::DataContext(KEY2ParserState &state)
  : KEY2XMLEmptyContextBase(state)
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
  IWORKDataPtr_t data(new IWORKData());
  data->m_stream = m_stream;
  data->m_displayName = m_displayName;
  data->m_type = m_type;

  if (getId())
    getState().getDictionary().m_data[get(getId())] = data;

  getCollector()->collectData(data);
}

}

namespace
{

class FilteredContext : public KEY2XMLElementContextBase
{
public:
  explicit FilteredContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKSize> m_size;
};

FilteredContext::FilteredContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t FilteredContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeContext>(getState(), m_size);
  case IWORKToken::NS_URI_SF | IWORKToken::data :
    return makeContext<DataContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void FilteredContext::endOfElement()
{
  getCollector()->collectFiltered(m_size);
}

}

namespace
{

class LeveledContext : public KEY2XMLElementContextBase
{
public:
  explicit LeveledContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

LeveledContext::LeveledContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t LeveledContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::data :
    return makeContext<DataContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    // TODO: handle
    break;
  }

  return IWORKXMLContextPtr_t();
}

void LeveledContext::endOfElement()
{
  getCollector()->collectLeveled(optional<IWORKSize>());
}

}

namespace
{

class UnfilteredContext : public KEY2XMLElementContextBase
{
public:
  explicit UnfilteredContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKSize> m_size;
};

UnfilteredContext::UnfilteredContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t UnfilteredContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeContext>(getState(), m_size);
  case IWORKToken::NS_URI_SF | IWORKToken::data :
    return makeContext<DataContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void UnfilteredContext::endOfElement()
{
  const IWORKMediaContentPtr_t content = getCollector()->collectUnfiltered(m_size);

  if (bool(content) && getId())
    getState().getDictionary().m_unfiltereds[get(getId())] = content;

  getCollector()->insertUnfiltered(content);
}

}

namespace
{

class FilteredImageContext : public KEY2XMLElementContextBase
{
public:
  explicit FilteredImageContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<ID_t> m_unfilteredId;
};

FilteredImageContext::FilteredImageContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t FilteredImageContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::unfiltered_ref :
    return makeContext<RefContext>(getState(), m_unfilteredId);
  case IWORKToken::NS_URI_SF | IWORKToken::unfiltered :
    return makeContext<UnfilteredContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::filtered :
    return makeContext<FilteredContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::leveled :
    return makeContext<LeveledContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void FilteredImageContext::endOfElement()
{
  if (m_unfilteredId)
  {
    const IWORKMediaContentMap_t::const_iterator it = getState().getDictionary().m_unfiltereds.find(get(m_unfilteredId));
    if (getState().getDictionary().m_unfiltereds.end() != it)
      getCollector()->insertUnfiltered(it->second);
  }

  const IWORKMediaContentPtr_t content = getCollector()->collectFilteredImage();
  if (bool(content) && getId())
    getState().getDictionary().m_filteredImages[get(getId())] = content;
  getCollector()->insertFilteredImage(content);
}

}

namespace
{

class ImageMediaContext : public KEY2XMLElementContextBase
{
public:
  explicit ImageMediaContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

ImageMediaContext::ImageMediaContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t ImageMediaContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::filtered_image :
    return makeContext<FilteredImageContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class OtherDatasContext : public KEY2XMLElementContextBase
{
public:
  explicit OtherDatasContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<ID_t> m_dataRef;
};

OtherDatasContext::OtherDatasContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_dataRef()
{
}

IWORKXMLContextPtr_t OtherDatasContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::data :
    return makeContext<DataContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::data_ref :
    return makeContext<RefContext>(getState(), m_dataRef);
  }

  return IWORKXMLContextPtr_t();
}

void OtherDatasContext::endOfElement()
{
  IWORKDataPtr_t data;
  if (m_dataRef)
  {
    const IWORKDataMap_t::const_iterator it = getState().getDictionary().m_data.find(get(m_dataRef));
    if (getState().getDictionary().m_data.end() != it)
      data = it->second;
  }

  getCollector()->collectData(data);
}

}

namespace
{

class SelfContainedMovieContext : public KEY2XMLElementContextBase
{
public:
  explicit SelfContainedMovieContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

SelfContainedMovieContext::SelfContainedMovieContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t SelfContainedMovieContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::other_datas :
    return makeContext<OtherDatasContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class MovieMediaContext : public KEY2XMLElementContextBase
{
public:
  explicit MovieMediaContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

MovieMediaContext::MovieMediaContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t MovieMediaContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::self_contained_movie :
    return makeContext<SelfContainedMovieContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void MovieMediaContext::endOfElement()
{
  getCollector()->collectMovieMedia();
}

}

namespace
{

class ContentContext : public KEY2XMLElementContextBase
{
public:
  explicit ContentContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

ContentContext::ContentContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t ContentContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::image_media :
    return makeContext<ImageMediaContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::movie_media :
    return makeContext<MovieMediaContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class MediaContext : public KEY2XMLElementContextBase
{
public:
  explicit MediaContext(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

MediaContext::MediaContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void MediaContext::startOfElement()
{
  getCollector()->startLevel();
}

IWORKXMLContextPtr_t MediaContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::content :
    return makeContext<ContentContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void MediaContext::endOfElement()
{
  getCollector()->collectMedia();
  getCollector()->endLevel();
}

}

namespace
{

class PointContext : public KEY2XMLEmptyContextBase
{
public:
  PointContext(KEY2ParserState &state, pair<optional<double>, optional<double> > &point);

private:
  virtual void attribute(int name, const char *value);

private:
  pair<optional<double>, optional<double> > &m_point;
};

PointContext::PointContext(KEY2ParserState &state, pair<optional<double>, optional<double> > &point)
  : KEY2XMLEmptyContextBase(state)
  , m_point(point)
{
}

void PointContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::x :
    m_point.first = lexical_cast<double>(value);
  case IWORKToken::NS_URI_SFA | IWORKToken::y :
    m_point.second = lexical_cast<double>(value);
  }
}

}

namespace
{

class ConnectionPathContext : public KEY2XMLElementContextBase
{
public:
  explicit ConnectionPathContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKSize> m_size;
  pair<optional<double>, optional<double> > m_point;
};

ConnectionPathContext::ConnectionPathContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t ConnectionPathContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::point :
    return makeContext<PointContext>(getState(), m_point);
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeContext>(getState(), m_size);
  }

  return IWORKXMLContextPtr_t();
}

void ConnectionPathContext::endOfElement()
{
  if (m_size)
    getCollector()->collectConnectionPath(get(m_size), get_optional_value_or(m_point.first, 0), get_optional_value_or(m_point.second, 0));
}

}

namespace
{

class PointPathContext : public KEY2XMLElementContextBase
{
public:
  explicit PointPathContext(KEY2ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  bool m_star;
  bool m_doubleArrow;
  optional<IWORKSize> m_size;
  pair<optional<double>, optional<double> > m_point;
};

PointPathContext::PointPathContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_star(false)
  , m_doubleArrow(false) // right arrow is the default (by my decree .-)
{
}

void PointPathContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::type :
  {
    switch (getToken(value))
    {
    case KEY2Token::double_ :
      m_doubleArrow = true;
      break;
    case KEY2Token::right :
      break;
    case KEY2Token::star :
      m_star = true;
      break;
    default :
      ETONYEK_DEBUG_MSG(("unknown point path type: %s\n", value));
      break;
    }
  }
  default :
    KEY2XMLElementContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t PointPathContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::point :
    return makeContext<PointContext>(getState(), m_point);
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeContext>(getState(), m_size);
  }

  return IWORKXMLContextPtr_t();
}

void PointPathContext::endOfElement()
{
  IWORKSize size;
  if (m_size)
    size = get(m_size);

  if (m_star)
    getCollector()->collectStarPath(size, numeric_cast<unsigned>(get_optional_value_or(m_point.first, 0.0)), get_optional_value_or(m_point.second, 0));
  else
    getCollector()->collectArrowPath(size, get_optional_value_or(m_point.first, 0), get_optional_value_or(m_point.second, 0), m_doubleArrow);
}

}

namespace
{

class ScalarPathContext : public KEY2XMLElementContextBase
{
public:
  explicit ScalarPathContext(KEY2ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKSize> m_size;
  bool m_polygon;
  double m_value;
};

ScalarPathContext::ScalarPathContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_size()
  , m_polygon(false)
  , m_value(0)
{
}

void ScalarPathContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::scalar :
    m_value = lexical_cast<double>(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::type :
  {
    switch (getToken(value))
    {
    case IWORKToken::_0 :
      break;
    case IWORKToken::_1 :
      m_polygon = true;
      break;
    default :
      ETONYEK_DEBUG_MSG(("unknown scalar path type: %s\n", value));
      break;
    }
    break;
  }
  default :
    KEY2XMLElementContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t ScalarPathContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeContext>(getState(), m_size);
  }

  return IWORKXMLContextPtr_t();
}

void ScalarPathContext::endOfElement()
{
  IWORKSize size;
  if (m_size)
    size = get(m_size);

  if (m_polygon)
    getCollector()->collectPolygonPath(size, numeric_cast<unsigned>(m_value));
  else
    getCollector()->collectRoundedRectanglePath(size, m_value);
}

}

namespace
{

class BezierContext : public KEY2XMLEmptyContextBase
{
public:
  explicit BezierContext(KEY2ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void endOfAttributes();

private:
  IWORKPathPtr_t m_path;
};

BezierContext::BezierContext(KEY2ParserState &state)
  : KEY2XMLEmptyContextBase(state)
  , m_path()
{
}

void BezierContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::path :
    m_path.reset(new IWORKPath(value));
    break;
  default :
    KEY2XMLEmptyContextBase::attribute(name, value);
    break;
  }
}

void BezierContext::endOfAttributes()
{
  if (getId())
    getState().getDictionary().m_beziers[get(getId())] = m_path;
  getCollector()->collectBezier(m_path);
}

}

namespace
{

class BezierRefContext : public KEY2XMLEmptyContextBase
{
public:
  explicit BezierRefContext(KEY2ParserState &state);

private:
  virtual void endOfElement();
};

BezierRefContext::BezierRefContext(KEY2ParserState &state)
  : KEY2XMLEmptyContextBase(state)
{
}

void BezierRefContext::endOfElement()
{
  if (getRef())
  {
    const IWORKPathMap_t::const_iterator it = getState().getDictionary().m_beziers.find(get(getRef()));
    if (getState().getDictionary().m_beziers.end() != it)
      getCollector()->collectBezier(it->second);
  }
}

}

namespace
{

class BezierPathContext : public KEY2XMLElementContextBase
{
public:
  explicit BezierPathContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

BezierPathContext::BezierPathContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t BezierPathContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::bezier :
    return makeContext<BezierContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::bezier_ref :
    return makeContext<BezierRefContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void BezierPathContext::endOfElement()
{
  getCollector()->collectBezierPath();
}

}

namespace
{

class Callout2PathContext : public KEY2XMLElementContextBase
{
public:
  explicit Callout2PathContext(KEY2ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKSize> m_size;
  double m_cornerRadius;
  bool m_tailAtCenter;
  double m_tailPosX;
  double m_tailPosY;
  double m_tailSize;
};

Callout2PathContext::Callout2PathContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_size()
  , m_cornerRadius(0)
  , m_tailAtCenter(false)
  , m_tailPosX(0)
  , m_tailPosY(0)
  , m_tailSize(0)
{
}

void Callout2PathContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::cornerRadius :
    m_cornerRadius = lexical_cast<double>(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::tailAtCenter :
    m_tailAtCenter = bool_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::tailPositionX :
    m_tailPosX = lexical_cast<double>(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::tailPositionY :
    m_tailPosY = lexical_cast<double>(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::tailSize :
    m_tailSize = lexical_cast<double>(value);
    break;
  default :
    KEY2XMLElementContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t Callout2PathContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeContext>(getState(), m_size);
  }

  return IWORKXMLContextPtr_t();
}

void Callout2PathContext::endOfElement()
{
  getCollector()->collectCalloutPath(get_optional_value_or(m_size, IWORKSize()), m_cornerRadius, m_tailSize, m_tailPosX, m_tailPosY, m_tailAtCenter);
}

}

namespace
{

class PathContext : public KEY2XMLElementContextBase
{
public:
  explicit PathContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

PathContext::PathContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t PathContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::bezier_path :
  case IWORKToken::NS_URI_SF | IWORKToken::editable_bezier_path :
    return makeContext<BezierPathContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::callout2_path :
    return makeContext<Callout2PathContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::connection_path :
    return makeContext<ConnectionPathContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::point_path :
    return makeContext<PointPathContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::scalar_path :
    return makeContext<ScalarPathContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class TextStorageContext : public KEY2XMLElementContextBase
{
public:
  explicit TextStorageContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

TextStorageContext::TextStorageContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t TextStorageContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::text_body :
    return makeContext<IWORKTextBodyContext>(getState());
    break;
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

// NOTE: isn't it wonderful that there are two text elements in two
// different namespaces, but with the same schema?
class TextContext : public KEY2XMLElementContextBase
{
public:
  explicit TextContext(KEY2ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
};

TextContext::TextContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void TextContext::attribute(const int name, const char *)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle :
    getCollector()->collectLayoutStyle(IWORKStylePtr_t(), false);
    break;
  }
}

IWORKXMLContextPtr_t TextContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::text_storage :
    return makeContext<TextStorageContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class ShapeContext : public KEY2XMLElementContextBase
{
public:
  explicit ShapeContext(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

ShapeContext::ShapeContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void ShapeContext::startOfElement()
{
  getCollector()->startLevel();
  getCollector()->startText(true);
}

IWORKXMLContextPtr_t ShapeContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::path :
    return makeContext<PathContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::text :
    return makeContext<TextContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void ShapeContext::endOfElement()
{
  getCollector()->collectShape();
  getCollector()->endText();
  getCollector()->endLevel();
}

}

namespace
{

class ImageContext : public KEY2XMLElementContextBase
{
public:
  explicit ImageContext(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  IWORKImagePtr_t m_image;
};

ImageContext::ImageContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_image(new IWORKImage())
{
}

void ImageContext::startOfElement()
{
  getCollector()->startLevel();
}

void ImageContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::locked :
    m_image->m_locked = bool_cast(value);
    break;
  default :
    KEY2XMLElementContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t ImageContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void ImageContext::endOfElement()
{
  if (getId())
    getState().getDictionary().m_images[get(getId())] = m_image;
  getCollector()->collectImage(m_image);
  getCollector()->endLevel();
}

}

namespace
{

class LineContext : public KEY2XMLElementContextBase
{
public:
  explicit LineContext(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKPosition> m_head;
  optional<IWORKPosition> m_tail;
};

LineContext::LineContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void LineContext::startOfElement()
{
  getCollector()->startLevel();
}

IWORKXMLContextPtr_t LineContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::head :
    return makeContext<IWORKPositionContext>(getState(), m_head);
  case IWORKToken::NS_URI_SF | IWORKToken::tail :
    return makeContext<IWORKPositionContext>(getState(), m_tail);
  }

  return IWORKXMLContextPtr_t();
}

void LineContext::endOfElement()
{
  IWORKLinePtr_t line(new IWORKLine());
  if (m_head)
  {
    line->m_x1 = get(m_head).m_x;
    line->m_y1 = get(m_head).m_y;
  }
  if (m_tail)
  {
    line->m_x2 = get(m_tail).m_x;
    line->m_y2 = get(m_tail).m_y;
  }
  getCollector()->collectLine(line);
  getCollector()->endLevel();
}

}

namespace
{

class GroupContext : public KEY2XMLElementContextBase
{
public:
  explicit GroupContext(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

GroupContext::GroupContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void GroupContext::startOfElement()
{
  getCollector()->startLevel();
  getCollector()->startGroup();
}

IWORKXMLContextPtr_t GroupContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::group :
    return makeContext<GroupContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::image :
    return makeContext<ImageContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::line :
    return makeContext<LineContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::media :
    return makeContext<MediaContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::shape :
    return makeContext<ShapeContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void GroupContext::endOfElement()
{
  getCollector()->endGroup();
  getCollector()->endLevel();
}

}

namespace
{

class PlaceholderRefContext : public KEY2XMLEmptyContextBase
{
public:
  PlaceholderRefContext(KEY2ParserState &state, bool title);

private:
  virtual void endOfElement();

private:
  const bool m_title;
};

PlaceholderRefContext::PlaceholderRefContext(KEY2ParserState &state, const bool title)
  : KEY2XMLEmptyContextBase(state)
  , m_title(title)
{
}

void PlaceholderRefContext::endOfElement()
{
  if (getRef())
  {
    KEYDictionary &dict = getState().getDictionary();
    KEYPlaceholderMap_t &placeholderMap = m_title ? dict.m_titlePlaceholders : dict.m_bodyPlaceholders;
    const KEYPlaceholderMap_t::const_iterator it = placeholderMap.find(get(getRef()));
    if (placeholderMap.end() != it)
      getCollector()->insertTextPlaceholder(it->second);
  }
}

}

namespace
{

class ConnectionLineContext : public KEY2XMLElementContextBase
{
public:
  explicit ConnectionLineContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

ConnectionLineContext::ConnectionLineContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t ConnectionLineContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::path :
    return makeContext<PathContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void ConnectionLineContext::endOfElement()
{
  getCollector()->collectShape();
}

}

namespace
{

class StickyNoteContext : public KEY2XMLElementContextBase
{
public:
  explicit StickyNoteContext(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

StickyNoteContext::StickyNoteContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void StickyNoteContext::startOfElement()
{
  getCollector()->startText(false);
  getCollector()->startLevel();
}

IWORKXMLContextPtr_t StickyNoteContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::text :
    return makeContext<TextContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void StickyNoteContext::endOfElement()
{
  getCollector()->collectStickyNote();

  getCollector()->endLevel();
  getCollector()->endText();
}

}

namespace
{

class DrawablesContext : public KEY2XMLElementContextBase
{
public:
  explicit DrawablesContext(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

DrawablesContext::DrawablesContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void DrawablesContext::startOfElement()
{
  getCollector()->startLevel();
}

void DrawablesContext::attribute(int, const char *)
{
}

IWORKXMLContextPtr_t DrawablesContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::body_placeholder_ref :
    return makeContext<PlaceholderRefContext>(getState(), false);
  case IWORKToken::NS_URI_SF | IWORKToken::connection_line :
    return makeContext<ConnectionLineContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::group :
    return makeContext<GroupContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::image :
    return makeContext<ImageContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::line :
    return makeContext<LineContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::media :
    return makeContext<MediaContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::shape :
    return makeContext<ShapeContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::sticky_note :
    return makeContext<StickyNoteContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_info :
    return makeContext<IWORKTabularInfoContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::title_placeholder_ref :
    return makeContext<PlaceholderRefContext>(getState(), true);
  case KEY2Token::NS_URI_KEY | KEY2Token::sticky_note :
    return makeContext<StickyNoteContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void DrawablesContext::endOfElement()
{
  getCollector()->endLevel();
}

}

namespace
{

class LayerContext : public KEY2XMLElementContextBase
{
public:
  explicit LayerContext(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

LayerContext::LayerContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void LayerContext::startOfElement()
{
  getCollector()->startLayer();
}

IWORKXMLContextPtr_t LayerContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::drawables :
    return makeContext<DrawablesContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void LayerContext::endOfElement()
{
  const KEYLayerPtr_t layer(getCollector()->collectLayer());
  getCollector()->endLayer();
  if (bool(layer))
  {
    if (bool(layer) && getId())
      getState().getDictionary().m_layers[get(getId())] = layer;
    getCollector()->insertLayer(layer);
  }
}

}

namespace
{

class LayersContext : public KEY2XMLElementContextBase
{
public:
  explicit LayersContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

LayersContext::LayersContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t LayersContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layer :
    return makeContext<LayerContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::proxy_master_layer :
    return makeContext<ProxyMasterLayerContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class PageContext : public KEY2XMLElementContextBase
{
public:
  explicit PageContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  // TODO: use size
  optional<IWORKSize> m_size;
};

PageContext::PageContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t PageContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeContext>(getState(), m_size);
  case IWORKToken::NS_URI_SF | IWORKToken::layers :
    return makeContext<LayersContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void PageContext::endOfElement()
{
}

}

namespace
{

class StyleContext : public KEY2XMLElementContextBase
{
public:
  explicit StyleContext(KEY2ParserState &state, optional<ID_t> &ref);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  optional<ID_t> &m_ref;
};

StyleContext::StyleContext(KEY2ParserState &state, optional<ID_t> &ref)
  : KEY2XMLElementContextBase(state)
  , m_ref(ref)
{
}

IWORKXMLContextPtr_t StyleContext::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::placeholder_style_ref) == name)
    return makeContext<RefContext>(getState(), m_ref);

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class PlaceholderContext : public KEY2XMLElementContextBase
{
public:
  PlaceholderContext(KEY2ParserState &state, bool title);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  const bool m_title;
  optional<ID_t> m_ref;
};

PlaceholderContext::PlaceholderContext(KEY2ParserState &state, const bool title)
  : KEY2XMLElementContextBase(state)
  , m_title(title)
  , m_ref()
{
}

void PlaceholderContext::startOfElement()
{
  getCollector()->startText(true);
}

IWORKXMLContextPtr_t PlaceholderContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    // ignore; the real geometry comes from style
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::style :
    return makeContext<StyleContext>(getState(), m_ref);
  case KEY2Token::NS_URI_KEY | KEY2Token::text :
    return makeContext<TextContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void PlaceholderContext::endOfElement()
{
  IWORKStylePtr_t style;
  if (m_ref)
  {
    const IWORKStyleMap_t::const_iterator it = getState().getDictionary().m_placeholderStyles.find(get(m_ref));
    if (getState().getDictionary().m_placeholderStyles.end() != it)
      style = it->second;
  }

  const KEYPlaceholderPtr_t placeholder = getCollector()->collectTextPlaceholder(style, m_title);
  if (bool(placeholder) && getId())
  {
    KEYDictionary &dict = getState().getDictionary();
    KEYPlaceholderMap_t &placeholderMap = m_title ? dict.m_titlePlaceholders : dict.m_bodyPlaceholders;
    placeholderMap[get(getId())] = placeholder;
  }
  getCollector()->endText();
}

}

namespace
{

class MasterSlideContext : public KEY2XMLElementContextBase
{
public:
  explicit MasterSlideContext(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

MasterSlideContext::MasterSlideContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void MasterSlideContext::startOfElement()
{
  getCollector()->startPage();
}

IWORKXMLContextPtr_t MasterSlideContext::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::page :
    return makeContext<PageContext>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::stylesheet :
    return makeContext<StylesheetContext>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::title_placeholder :
    return makeContext<PlaceholderContext>(getState(), true);
  case KEY2Token::NS_URI_KEY | KEY2Token::body_placeholder :
    return makeContext<PlaceholderContext>(getState(), false);
  case KEY2Token::NS_URI_KEY | KEY2Token::sticky_notes :
    return makeContext<StickyNoteContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void MasterSlideContext::endOfElement()
{
  getCollector()->collectPage();
  getCollector()->endPage();
}

}

namespace
{

class MasterSlidesContext : public KEY2XMLElementContextBase
{
public:
  explicit MasterSlidesContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

MasterSlidesContext::MasterSlidesContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t MasterSlidesContext::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::master_slide :
    return makeContext<MasterSlideContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class ThemeContext : public KEY2XMLElementContextBase
{
public:
  explicit ThemeContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  // TODO: use size
  optional<IWORKSize> m_size;
};

ThemeContext::ThemeContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t ThemeContext::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | IWORKToken::size :
    return makeContext<IWORKSizeContext>(getState(), m_size);
  case KEY2Token::NS_URI_KEY | KEY2Token::stylesheet :
    return makeContext<StylesheetContext>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::master_slides :
    return makeContext<MasterSlidesContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class ThemeListContext : public KEY2XMLElementContextBase
{
public:
  explicit ThemeListContext(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

ThemeListContext::ThemeListContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void ThemeListContext::startOfElement()
{
  getCollector()->startThemes();
}

IWORKXMLContextPtr_t ThemeListContext::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::theme :
    return makeContext<ThemeContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void ThemeListContext::endOfElement()
{
  getCollector()->endThemes();
}

}

namespace
{

class NotesContext : public KEY2XMLElementContextBase
{
public:
  explicit NotesContext(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

NotesContext::NotesContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void NotesContext::startOfElement()
{
  getCollector()->startText(false);
}

IWORKXMLContextPtr_t NotesContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::text_storage | IWORKToken::NS_URI_SF :
    return makeContext<TextStorageContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void NotesContext::endOfElement()
{
  getCollector()->collectNote();
  getCollector()->endText();
}

}

namespace
{

class StickyNotesContext : public KEY2XMLElementContextBase
{
public:
  explicit StickyNotesContext(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

StickyNotesContext::StickyNotesContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t StickyNotesContext::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::sticky_note :
    return makeContext<StickyNoteContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class SlideContext : public KEY2XMLElementContextBase
{
public:
  explicit SlideContext(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

SlideContext::SlideContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void SlideContext::startOfElement()
{
  getCollector()->startPage();
}

IWORKXMLContextPtr_t SlideContext::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::notes :
    return makeContext<NotesContext>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::page :
    return makeContext<PageContext>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::stylesheet :
    return makeContext<StylesheetContext>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::title_placeholder :
    return makeContext<PlaceholderContext>(getState(), true);
  case KEY2Token::NS_URI_KEY | KEY2Token::body_placeholder :
    return makeContext<PlaceholderContext>(getState(), false);
  case KEY2Token::NS_URI_KEY | KEY2Token::sticky_notes :
    return makeContext<StickyNotesContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void SlideContext::endOfElement()
{
  getCollector()->collectPage();
  getCollector()->endPage();
}

}

namespace
{

class SlideListContext : public KEY2XMLElementContextBase
{
public:
  explicit SlideListContext(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

SlideListContext::SlideListContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void SlideListContext::startOfElement()
{
  getCollector()->startSlides();
}

IWORKXMLContextPtr_t SlideListContext::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::slide :
    return makeContext<SlideContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void SlideListContext::endOfElement()
{
  getCollector()->endSlides();
}

}

namespace
{

class PresentationContext : public KEY2XMLElementContextBase
{
public:
  explicit PresentationContext(KEY2ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);

private:
  optional<IWORKSize> m_size;
  bool m_pendingSize;
};

PresentationContext::PresentationContext(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_size()
  , m_pendingSize(false)
{
}

void PresentationContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::version :
  {
    const unsigned version = getVersion(getToken(value));
    if (0 == version)
    {
      ETONYEK_DEBUG_MSG(("unknown version %s\n", value));
    }
  }
  break;
  }
}

IWORKXMLContextPtr_t PresentationContext::element(const int name)
{
  if (m_pendingSize)
  {
    getCollector()->collectPresentation(m_size);
    m_pendingSize = false;
  }

  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::metadata :
    return makeContext<MetadataContext>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::theme_list :
    return makeContext<ThemeListContext>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::slide_list :
    return makeContext<SlideListContext>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::size :
    m_pendingSize = true;
    return makeContext<IWORKSizeContext>(getState(), m_size);
  }

  return IWORKXMLContextPtr_t();
}

}

KEY2Parser::KEY2Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, KEYCollector *const collector, KEYDictionary &dict)
  : IWORKParser(input, package, collector)
  , m_state(*this, dict, getTokenizer())
  , m_version(0)
{
}

KEY2Parser::~KEY2Parser()
{
}

IWORKXMLContextPtr_t KEY2Parser::createDocumentContext()
{
  return makeContext<PresentationContext>(m_state);
}

IWORKXMLReader::TokenizerFunction_t KEY2Parser::getTokenizer() const
{
  return IWORKXMLReader::ChainedTokenizer(KEY2Tokenizer(), IWORKTokenizer());
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
