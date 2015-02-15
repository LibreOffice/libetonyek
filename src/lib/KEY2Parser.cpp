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
#include "IWORKTabularInfoContext.h"
#include "IWORKToken.h"
#include "IWORKTypes.h"
#include "IWORKXMLContextBase.h"
#include "IWORKXMLContexts.h"
#include "IWORKXMLReader.h"
#include "KEY2ParserUtils.h"
#include "KEY2StyleContext.h"
#include "KEY2Token.h"
#include "KEYCollector.h"
#include "KEYStyle.h"
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

class RefContext : public IWORKXMLEmptyContextBase
{
public:
  RefContext(IWORKXMLParserState &state, optional<ID_t> &ref);

private:
  virtual void endOfElement();

private:
  optional<ID_t> &m_ref;
};

RefContext::RefContext(IWORKXMLParserState &state, optional<ID_t> &ref)
  : IWORKXMLEmptyContextBase(state)
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

class MetadataContext : public IWORKXMLElementContextBase
{
public:
  explicit MetadataContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

MetadataContext::MetadataContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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

class StylesContext : public IWORKXMLElementContextBase
{
public:
  StylesContext(IWORKXMLParserState &state, bool anonymous);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  const bool m_anonymous;
};

StylesContext::StylesContext(IWORKXMLParserState &state, const bool anonymous)
  : IWORKXMLElementContextBase(state)
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

class StylesheetContext : public IWORKXMLElementContextBase
{
public:
  explicit StylesheetContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<ID_t> m_parent;
};

StylesheetContext::StylesheetContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
  getCollector()->collectStylesheet(getId(), m_parent);
}

}

namespace
{

class ProxyMasterLayerContext : public IWORKXMLElementContextBase
{
public:
  explicit ProxyMasterLayerContext(IWORKXMLParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<ID_t> m_ref;
};

ProxyMasterLayerContext::ProxyMasterLayerContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_ref()
{
}

void ProxyMasterLayerContext::startOfElement()
{
  getCollector()->startLayer();
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
  getCollector()->collectLayer(m_ref, true);
  getCollector()->endLayer();
}

}

namespace
{

class DataContext : public IWORKXMLEmptyContextBase
{
public:
  explicit DataContext(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void endOfElement();

private:
  optional<string> m_displayName;
  RVNGInputStreamPtr_t m_stream;
  optional<unsigned> m_type;
};

DataContext::DataContext(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
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
  {
    ETONYEK_DEBUG_XML_TODO_ATTRIBUTE(attr);
    break;
  }
  default :
    break;
  }
}

void DataContext::endOfElement()
{
  getCollector()->collectData(getId(), m_stream, m_displayName, m_type, false);
}

}

namespace
{

class FilteredContext : public IWORKXMLElementContextBase
{
public:
  explicit FilteredContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKSize> m_size;
};

FilteredContext::FilteredContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
  getCollector()->collectFiltered(getId(), m_size);
}

}

namespace
{

class LeveledContext : public IWORKXMLElementContextBase
{
public:
  explicit LeveledContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

LeveledContext::LeveledContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t LeveledContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::data :
    return makeContext<DataContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    ETONYEK_DEBUG_XML_TODO_ELEMENT(element);
    break;
  }

  return IWORKXMLContextPtr_t();
}

void LeveledContext::endOfElement()
{
  getCollector()->collectLeveled(getId(), optional<IWORKSize>());
}

}

namespace
{

class UnfilteredContext : public IWORKXMLElementContextBase
{
public:
  explicit UnfilteredContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKSize> m_size;
};

UnfilteredContext::UnfilteredContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
  getCollector()->collectUnfiltered(getId(), m_size, false);
}

}

namespace
{

class FilteredImageContext : public IWORKXMLElementContextBase
{
public:
  explicit FilteredImageContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<ID_t> m_unfilteredId;
};

FilteredImageContext::FilteredImageContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
    getCollector()->collectUnfiltered(m_unfilteredId, optional<IWORKSize>(), true);
  getCollector()->collectFilteredImage(getId(), false);
}

}

namespace
{

class ImageMediaContext : public IWORKXMLElementContextBase
{
public:
  explicit ImageMediaContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

ImageMediaContext::ImageMediaContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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

class OtherDatasContext : public IWORKXMLElementContextBase
{
public:
  explicit OtherDatasContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<ID_t> m_dataRef;
};

OtherDatasContext::OtherDatasContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
  getCollector()->collectData(m_dataRef, RVNGInputStreamPtr_t(), optional<string>(), optional<unsigned>(), true);
}

}

namespace
{

class SelfContainedMovieContext : public IWORKXMLElementContextBase
{
public:
  explicit SelfContainedMovieContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

SelfContainedMovieContext::SelfContainedMovieContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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

class MovieMediaContext : public IWORKXMLElementContextBase
{
public:
  explicit MovieMediaContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

MovieMediaContext::MovieMediaContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
  getCollector()->collectMovieMedia(optional<ID_t>());
}

}

namespace
{

class ContentContext : public IWORKXMLElementContextBase
{
public:
  explicit ContentContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

ContentContext::ContentContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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

class MediaContext : public IWORKXMLElementContextBase
{
public:
  explicit MediaContext(IWORKXMLParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

MediaContext::MediaContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
  getCollector()->collectMedia(getId());
  getCollector()->endLevel();
}

}

namespace
{

class PointContext : public IWORKXMLEmptyContextBase
{
public:
  PointContext(IWORKXMLParserState &state, pair<optional<double>, optional<double> > &point);

private:
  virtual void attribute(int name, const char *value);

private:
  pair<optional<double>, optional<double> > &m_point;
};

PointContext::PointContext(IWORKXMLParserState &state, pair<optional<double>, optional<double> > &point)
  : IWORKXMLEmptyContextBase(state)
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

class ConnectionPathContext : public IWORKXMLElementContextBase
{
public:
  explicit ConnectionPathContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKSize> m_size;
  pair<optional<double>, optional<double> > m_point;
};

ConnectionPathContext::ConnectionPathContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
    getCollector()->collectConnectionPath(getId(), get(m_size), get_optional_value_or(m_point.first, 0), get_optional_value_or(m_point.second, 0));
}

}

namespace
{

class PointPathContext : public IWORKXMLElementContextBase
{
public:
  explicit PointPathContext(IWORKXMLParserState &state);

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

PointPathContext::PointPathContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
    IWORKXMLElementContextBase::attribute(name, value);
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
    getCollector()->collectStarPath(getId(), size, numeric_cast<unsigned>(get_optional_value_or(m_point.first, 0.0)), get_optional_value_or(m_point.second, 0));
  else
    getCollector()->collectArrowPath(getId(), size, get_optional_value_or(m_point.first, 0), get_optional_value_or(m_point.second, 0), m_doubleArrow);
}

}

namespace
{

class ScalarPathContext : public IWORKXMLElementContextBase
{
public:
  explicit ScalarPathContext(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKSize> m_size;
  bool m_polygon;
  double m_value;
};

ScalarPathContext::ScalarPathContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
    IWORKXMLElementContextBase::attribute(name, value);
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
    getCollector()->collectPolygonPath(getId(), size, numeric_cast<unsigned>(m_value));
  else
    getCollector()->collectRoundedRectanglePath(getId(), size, m_value);
}

}

namespace
{

class BezierContext : public IWORKXMLEmptyContextBase
{
public:
  explicit BezierContext(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void endOfAttributes();

private:
  IWORKPathPtr_t m_path;
};

BezierContext::BezierContext(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
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
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  }
}

void BezierContext::endOfAttributes()
{
  getCollector()->collectBezier(getId(), m_path, false);
}

}

namespace
{

class BezierRefContext : public IWORKXMLEmptyContextBase
{
public:
  explicit BezierRefContext(IWORKXMLParserState &state);

private:
  virtual void endOfElement();
};

BezierRefContext::BezierRefContext(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

void BezierRefContext::endOfElement()
{
  getCollector()->collectBezier(getRef(), IWORKPathPtr_t(), true);
}

}

namespace
{

class BezierPathContext : public IWORKXMLElementContextBase
{
public:
  explicit BezierPathContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

BezierPathContext::BezierPathContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
  getCollector()->collectBezierPath(getId());
}

}

namespace
{

class Callout2PathContext : public IWORKXMLElementContextBase
{
public:
  explicit Callout2PathContext(IWORKXMLParserState &state);

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

Callout2PathContext::Callout2PathContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
    m_tailAtCenter = KEY2ParserUtils::bool_cast(value);
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
    IWORKXMLElementContextBase::attribute(name, value);
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
  getCollector()->collectCalloutPath(getId(), get_optional_value_or(m_size, IWORKSize()), m_cornerRadius, m_tailSize, m_tailPosX, m_tailPosY, m_tailAtCenter);
}

}

namespace
{

class PathContext : public IWORKXMLElementContextBase
{
public:
  explicit PathContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

PathContext::PathContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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

class TextStorageContext : public IWORKXMLElementContextBase
{
public:
  explicit TextStorageContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

TextStorageContext::TextStorageContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
class TextContext : public IWORKXMLElementContextBase
{
public:
  explicit TextContext(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
};

TextContext::TextContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void TextContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle :
  {
    optional<IWORKPropertyMap> dummyProps;
    optional<string> dummyIdent;
    optional<ID_t> id(value);
    getCollector()->collectLayoutStyle(id, dummyProps, dummyIdent, dummyIdent, true, false);
    break;
  }
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

class ShapeContext : public IWORKXMLElementContextBase
{
public:
  explicit ShapeContext(IWORKXMLParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

ShapeContext::ShapeContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void ShapeContext::startOfElement()
{
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
  getCollector()->collectShape(getId());
  getCollector()->endText();
}

}

namespace
{

class ImageContext : public IWORKXMLElementContextBase
{
public:
  explicit ImageContext(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  KEYImagePtr_t m_image;
};

ImageContext::ImageContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_image(new KEYImage())
{
}

void ImageContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::locked :
    m_image->locked = KEY2ParserUtils::bool_cast(value);
    break;
  default :
    IWORKXMLElementContextBase::attribute(name, value);
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
  getCollector()->collectImage(getId(), m_image);
}

}

namespace
{

class LineContext : public IWORKXMLElementContextBase
{
public:
  explicit LineContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKPosition> m_head;
  optional<IWORKPosition> m_tail;
};

LineContext::LineContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
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
  KEYLinePtr_t line(new KEYLine());
  if (m_head)
  {
    line->x1 = get(m_head).x;
    line->y1 = get(m_head).y;
  }
  if (m_tail)
  {
    line->x2 = get(m_tail).x;
    line->y2 = get(m_tail).y;
  }
  getCollector()->collectLine(getId(), line);
}

}

namespace
{

class GroupContext : public IWORKXMLElementContextBase
{
public:
  explicit GroupContext(IWORKXMLParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

GroupContext::GroupContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
  KEYGroupPtr_t group(new KEYGroup());
  getCollector()->collectGroup(getId(), group);
  getCollector()->endGroup();
  getCollector()->endLevel();
}

}

namespace
{

class PlaceholderRefContext : public IWORKXMLEmptyContextBase
{
public:
  PlaceholderRefContext(IWORKXMLParserState &state, bool title);

private:
  virtual void endOfElement();

private:
  const bool m_title;
};

PlaceholderRefContext::PlaceholderRefContext(IWORKXMLParserState &state, const bool title)
  : IWORKXMLEmptyContextBase(state)
  , m_title(title)
{
}

void PlaceholderRefContext::endOfElement()
{
  getCollector()->collectTextPlaceholder(getRef(), m_title, true);
}

}

namespace
{

class ConnectionLineContext : public IWORKXMLElementContextBase
{
public:
  explicit ConnectionLineContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

ConnectionLineContext::ConnectionLineContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
  getCollector()->collectShape(getId());
}

}

namespace
{

class StickyNoteContext : public IWORKXMLElementContextBase
{
public:
  explicit StickyNoteContext(IWORKXMLParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

StickyNoteContext::StickyNoteContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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

class DrawablesContext : public IWORKXMLElementContextBase
{
public:
  explicit DrawablesContext(IWORKXMLParserState &state);

private:
  virtual void startOfElement();
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

DrawablesContext::DrawablesContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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

class LayerContext : public IWORKXMLElementContextBase
{
public:
  explicit LayerContext(IWORKXMLParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

LayerContext::LayerContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
  getCollector()->collectLayer(getId(), false);
  getCollector()->endLayer();
}

}

namespace
{

class LayersContext : public IWORKXMLElementContextBase
{
public:
  explicit LayersContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

LayersContext::LayersContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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

class PageContext : public IWORKXMLElementContextBase
{
public:
  explicit PageContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  // TODO: use size
  optional<IWORKSize> m_size;
};

PageContext::PageContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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

class StyleContext : public IWORKXMLElementContextBase
{
public:
  explicit StyleContext(IWORKXMLParserState &state, optional<ID_t> &ref);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  optional<ID_t> &m_ref;
};

StyleContext::StyleContext(IWORKXMLParserState &state, optional<ID_t> &ref)
  : IWORKXMLElementContextBase(state)
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

class PlaceholderContext : public IWORKXMLElementContextBase
{
public:
  PlaceholderContext(IWORKXMLParserState &state, bool title);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  const bool m_title;
  optional<ID_t> m_ref;
};

PlaceholderContext::PlaceholderContext(IWORKXMLParserState &state, const bool title)
  : IWORKXMLElementContextBase(state)
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
  if (m_ref)
  {
    const optional<string> none;
    getCollector()->collectPlaceholderStyle(get(m_ref), optional<IWORKPropertyMap>(), none, none, true, false);
  }
  getCollector()->collectTextPlaceholder(getId(), m_title, false);
  getCollector()->endText();
}

}

namespace
{

class MasterSlideContext : public IWORKXMLElementContextBase
{
public:
  explicit MasterSlideContext(IWORKXMLParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

MasterSlideContext::MasterSlideContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
  getCollector()->collectPage(getId());
  getCollector()->endPage();
}

}

namespace
{

class MasterSlidesContext : public IWORKXMLElementContextBase
{
public:
  explicit MasterSlidesContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

MasterSlidesContext::MasterSlidesContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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

class ThemeContext : public IWORKXMLElementContextBase
{
public:
  explicit ThemeContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  // TODO: use size
  optional<IWORKSize> m_size;
};

ThemeContext::ThemeContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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

class ThemeListContext : public IWORKXMLElementContextBase
{
public:
  explicit ThemeListContext(IWORKXMLParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

ThemeListContext::ThemeListContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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

class NotesContext : public IWORKXMLElementContextBase
{
public:
  explicit NotesContext(IWORKXMLParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

NotesContext::NotesContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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

class StickyNotesContext : public IWORKXMLElementContextBase
{
public:
  explicit StickyNotesContext(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

StickyNotesContext::StickyNotesContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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

class SlideContext : public IWORKXMLElementContextBase
{
public:
  explicit SlideContext(IWORKXMLParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

SlideContext::SlideContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
  getCollector()->collectPage(getId());
  getCollector()->endPage();
}

}

namespace
{

class SlideListContext : public IWORKXMLElementContextBase
{
public:
  explicit SlideListContext(IWORKXMLParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

SlideListContext::SlideListContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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

class PresentationContext : public IWORKXMLElementContextBase
{
public:
  explicit PresentationContext(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKSize> m_size;
};

PresentationContext::PresentationContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_size()
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
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::metadata :
    return makeContext<MetadataContext>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::theme_list :
    return makeContext<ThemeListContext>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::slide_list :
    return makeContext<SlideListContext>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::size :
    return makeContext<IWORKSizeContext>(getState(), m_size);
  }

  return IWORKXMLContextPtr_t();
}

void PresentationContext::endOfElement()
{
  getCollector()->collectPresentation(m_size);
}

}

KEY2Parser::KEY2Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, KEYCollector *const collector, const KEYDefaults &defaults)
  : KEYParser(input, package, collector, defaults)
  , m_version(0)
{
}

KEY2Parser::~KEY2Parser()
{
}

IWORKXMLContextPtr_t KEY2Parser::makeDocumentContext(IWORKXMLParserState &state) const
{
  return makeContext<PresentationContext>(state);
}

IWORKXMLReader::TokenizerFunction_t KEY2Parser::getTokenizer() const
{
  return IWORKXMLReader::ChainedTokenizer(KEY2Tokenizer(), IWORKTokenizer());
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
