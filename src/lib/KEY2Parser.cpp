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
#include "IWORKGeometryElement.h"
#include "IWORKMediaElement.h"
#include "IWORKPath.h"
#include "IWORKPositionElement.h"
#include "IWORKRefContext.h"
#include "IWORKStyle.h"
#include "IWORKSizeElement.h"
#include "IWORKTabularInfoElement.h"
#include "IWORKTextBodyElement.h"
#include "IWORKToken.h"
#include "IWORKTypes.h"
#include "KEY2ParserState.h"
#include "KEY2StyleContext.h"
#include "KEY2StyleRefContext.h"
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

class MetadataElement : public KEY2XMLElementContextBase
{
public:
  explicit MetadataElement(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

MetadataElement::MetadataElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t MetadataElement::element(int)
{
  // TODO: parse
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class StylesElement : public KEY2XMLElementContextBase
{
public:
  StylesElement(KEY2ParserState &state, bool anonymous);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  const bool m_anonymous;
};

StylesElement::StylesElement(KEY2ParserState &state, const bool anonymous)
  : KEY2XMLElementContextBase(state)
  , m_anonymous(anonymous)
{
}

IWORKXMLContextPtr_t StylesElement::element(const int name)
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

void StylesElement::endOfElement()
{
}

}

namespace
{

class StylesheetElement : public KEY2XMLElementContextBase
{
public:
  explicit StylesheetElement(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<ID_t> m_parent;
};

StylesheetElement::StylesheetElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t StylesheetElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::styles :
    return makeContext<StylesElement>(getState(), false);
  case IWORKToken::NS_URI_SF | IWORKToken::anon_styles :
    return makeContext<StylesElement>(getState(), false);
  case IWORKToken::NS_URI_SF | IWORKToken::parent_ref :
    return makeContext<IWORKRefContext>(getState(), m_parent);
  }

  return IWORKXMLContextPtr_t();
}

void StylesheetElement::endOfElement()
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

class ProxyMasterLayerElement : public KEY2XMLElementContextBase
{
public:
  explicit ProxyMasterLayerElement(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<ID_t> m_ref;
};

ProxyMasterLayerElement::ProxyMasterLayerElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_ref()
{
}

IWORKXMLContextPtr_t ProxyMasterLayerElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layer_ref :
    return makeContext<IWORKRefContext>(getState(), m_ref);
  }

  return IWORKXMLContextPtr_t();
}

void ProxyMasterLayerElement::endOfElement()
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

class PointElement : public KEY2XMLEmptyContextBase
{
public:
  PointElement(KEY2ParserState &state, pair<optional<double>, optional<double> > &point);

private:
  virtual void attribute(int name, const char *value);

private:
  pair<optional<double>, optional<double> > &m_point;
};

PointElement::PointElement(KEY2ParserState &state, pair<optional<double>, optional<double> > &point)
  : KEY2XMLEmptyContextBase(state)
  , m_point(point)
{
}

void PointElement::attribute(const int name, const char *const value)
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

class ConnectionPathElement : public KEY2XMLElementContextBase
{
public:
  explicit ConnectionPathElement(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKSize> m_size;
  pair<optional<double>, optional<double> > m_point;
};

ConnectionPathElement::ConnectionPathElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t ConnectionPathElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::point :
    return makeContext<PointElement>(getState(), m_point);
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeElement>(getState(), m_size);
  }

  return IWORKXMLContextPtr_t();
}

void ConnectionPathElement::endOfElement()
{
  if (m_size)
    getCollector()->collectConnectionPath(get(m_size), get_optional_value_or(m_point.first, 0), get_optional_value_or(m_point.second, 0));
}

}

namespace
{

class PointPathElement : public KEY2XMLElementContextBase
{
public:
  explicit PointPathElement(KEY2ParserState &state);

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

PointPathElement::PointPathElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_star(false)
  , m_doubleArrow(false) // right arrow is the default (by my decree .-)
{
}

void PointPathElement::attribute(const int name, const char *const value)
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

IWORKXMLContextPtr_t PointPathElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::point :
    return makeContext<PointElement>(getState(), m_point);
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeElement>(getState(), m_size);
  }

  return IWORKXMLContextPtr_t();
}

void PointPathElement::endOfElement()
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

class ScalarPathElement : public KEY2XMLElementContextBase
{
public:
  explicit ScalarPathElement(KEY2ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKSize> m_size;
  bool m_polygon;
  double m_value;
};

ScalarPathElement::ScalarPathElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_size()
  , m_polygon(false)
  , m_value(0)
{
}

void ScalarPathElement::attribute(const int name, const char *const value)
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

IWORKXMLContextPtr_t ScalarPathElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeElement>(getState(), m_size);
  }

  return IWORKXMLContextPtr_t();
}

void ScalarPathElement::endOfElement()
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

class BezierElement : public KEY2XMLEmptyContextBase
{
public:
  explicit BezierElement(KEY2ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void endOfElement();

private:
  IWORKPathPtr_t m_path;
};

BezierElement::BezierElement(KEY2ParserState &state)
  : KEY2XMLEmptyContextBase(state)
  , m_path()
{
}

void BezierElement::attribute(const int name, const char *const value)
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

void BezierElement::endOfElement()
{
  if (getId())
    getState().getDictionary().m_beziers[get(getId())] = m_path;
  getCollector()->collectBezier(m_path);
}

}

namespace
{

class BezierRefElement : public KEY2XMLEmptyContextBase
{
public:
  explicit BezierRefElement(KEY2ParserState &state);

private:
  virtual void endOfElement();
};

BezierRefElement::BezierRefElement(KEY2ParserState &state)
  : KEY2XMLEmptyContextBase(state)
{
}

void BezierRefElement::endOfElement()
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

class BezierPathElement : public KEY2XMLElementContextBase
{
public:
  explicit BezierPathElement(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

BezierPathElement::BezierPathElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t BezierPathElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::bezier :
    return makeContext<BezierElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::bezier_ref :
    return makeContext<BezierRefElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void BezierPathElement::endOfElement()
{
  getCollector()->collectBezierPath();
}

}

namespace
{

class Callout2PathElement : public KEY2XMLElementContextBase
{
public:
  explicit Callout2PathElement(KEY2ParserState &state);

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

Callout2PathElement::Callout2PathElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_size()
  , m_cornerRadius(0)
  , m_tailAtCenter(false)
  , m_tailPosX(0)
  , m_tailPosY(0)
  , m_tailSize(0)
{
}

void Callout2PathElement::attribute(const int name, const char *const value)
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

IWORKXMLContextPtr_t Callout2PathElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeElement>(getState(), m_size);
  }

  return IWORKXMLContextPtr_t();
}

void Callout2PathElement::endOfElement()
{
  getCollector()->collectCalloutPath(get_optional_value_or(m_size, IWORKSize()), m_cornerRadius, m_tailSize, m_tailPosX, m_tailPosY, m_tailAtCenter);
}

}

namespace
{

class PathElement : public KEY2XMLElementContextBase
{
public:
  explicit PathElement(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

PathElement::PathElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t PathElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::bezier_path :
  case IWORKToken::NS_URI_SF | IWORKToken::editable_bezier_path :
    return makeContext<BezierPathElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::callout2_path :
    return makeContext<Callout2PathElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::connection_path :
    return makeContext<ConnectionPathElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::point_path :
    return makeContext<PointPathElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::scalar_path :
    return makeContext<ScalarPathElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class TextStorageElement : public KEY2XMLElementContextBase
{
public:
  explicit TextStorageElement(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

TextStorageElement::TextStorageElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t TextStorageElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::text_body :
    return makeContext<IWORKTextBodyElement>(getState());
    break;
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

// NOTE: isn't it wonderful that there are two text elements in two
// different namespaces, but with the same schema?
class TextElement : public KEY2XMLElementContextBase
{
public:
  explicit TextElement(KEY2ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
};

TextElement::TextElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void TextElement::attribute(const int name, const char *)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle :
    getCollector()->collectStyle(IWORKStylePtr_t(), false);
    // TODO: this call is in the wrong place
    getCollector()->setLayoutStyle(IWORKStylePtr_t());
    break;
  }
}

IWORKXMLContextPtr_t TextElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::text_storage :
    return makeContext<TextStorageElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class ShapeElement : public KEY2XMLElementContextBase
{
public:
  explicit ShapeElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

ShapeElement::ShapeElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void ShapeElement::startOfElement()
{
  getCollector()->startLevel();
  getCollector()->startText(true);
}

IWORKXMLContextPtr_t ShapeElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::path :
    return makeContext<PathElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::text :
    return makeContext<TextElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void ShapeElement::endOfElement()
{
  getCollector()->collectShape();
  getCollector()->endText();
  getCollector()->endLevel();
}

}

namespace
{

class ImageElement : public KEY2XMLElementContextBase
{
public:
  explicit ImageElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  IWORKImagePtr_t m_image;
};

ImageElement::ImageElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_image(new IWORKImage())
{
}

void ImageElement::startOfElement()
{
  getCollector()->startLevel();
}

void ImageElement::attribute(const int name, const char *const value)
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

IWORKXMLContextPtr_t ImageElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void ImageElement::endOfElement()
{
  if (getId())
    getState().getDictionary().m_images[get(getId())] = m_image;
  getCollector()->collectImage(m_image);
  getCollector()->endLevel();
}

}

namespace
{

class LineElement : public KEY2XMLElementContextBase
{
public:
  explicit LineElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKPosition> m_head;
  optional<IWORKPosition> m_tail;
};

LineElement::LineElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void LineElement::startOfElement()
{
  getCollector()->startLevel();
}

IWORKXMLContextPtr_t LineElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::head :
    return makeContext<IWORKPositionElement>(getState(), m_head);
  case IWORKToken::NS_URI_SF | IWORKToken::tail :
    return makeContext<IWORKPositionElement>(getState(), m_tail);
  }

  return IWORKXMLContextPtr_t();
}

void LineElement::endOfElement()
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

class GroupElement : public KEY2XMLElementContextBase
{
public:
  explicit GroupElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

GroupElement::GroupElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void GroupElement::startOfElement()
{
  getCollector()->startLevel();
  getCollector()->startGroup();
}

IWORKXMLContextPtr_t GroupElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::group :
    return makeContext<GroupElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::image :
    return makeContext<ImageElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::line :
    return makeContext<LineElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::media :
    return makeContext<IWORKMediaElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::shape :
    return makeContext<ShapeElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void GroupElement::endOfElement()
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

class ConnectionLineElement : public KEY2XMLElementContextBase
{
public:
  explicit ConnectionLineElement(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

ConnectionLineElement::ConnectionLineElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t ConnectionLineElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::path :
    return makeContext<PathElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void ConnectionLineElement::endOfElement()
{
  getCollector()->collectShape();
}

}

namespace
{

class StickyNoteElement : public KEY2XMLElementContextBase
{
public:
  explicit StickyNoteElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

StickyNoteElement::StickyNoteElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void StickyNoteElement::startOfElement()
{
  getCollector()->startText(false);
  getCollector()->startLevel();
}

IWORKXMLContextPtr_t StickyNoteElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::text :
    return makeContext<TextElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void StickyNoteElement::endOfElement()
{
  getCollector()->collectStickyNote();

  getCollector()->endLevel();
  getCollector()->endText();
}

}

namespace
{

class DrawablesElement : public KEY2XMLElementContextBase
{
public:
  explicit DrawablesElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

DrawablesElement::DrawablesElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void DrawablesElement::startOfElement()
{
  getCollector()->startLevel();
}

void DrawablesElement::attribute(int, const char *)
{
}

IWORKXMLContextPtr_t DrawablesElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::body_placeholder_ref :
    return makeContext<PlaceholderRefContext>(getState(), false);
  case IWORKToken::NS_URI_SF | IWORKToken::connection_line :
    return makeContext<ConnectionLineElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::group :
    return makeContext<GroupElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::image :
    return makeContext<ImageElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::line :
    return makeContext<LineElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::media :
    return makeContext<IWORKMediaElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::shape :
    return makeContext<ShapeElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::sticky_note :
    return makeContext<StickyNoteElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_info :
    return makeContext<IWORKTabularInfoElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::title_placeholder_ref :
    return makeContext<PlaceholderRefContext>(getState(), true);
  case KEY2Token::NS_URI_KEY | KEY2Token::sticky_note :
    return makeContext<StickyNoteElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void DrawablesElement::endOfElement()
{
  getCollector()->endLevel();
}

}

namespace
{

class LayerElement : public KEY2XMLElementContextBase
{
public:
  explicit LayerElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

LayerElement::LayerElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void LayerElement::startOfElement()
{
  getCollector()->startLayer();
}

IWORKXMLContextPtr_t LayerElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::drawables :
    return makeContext<DrawablesElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void LayerElement::endOfElement()
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

class LayersElement : public KEY2XMLElementContextBase
{
public:
  explicit LayersElement(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

LayersElement::LayersElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t LayersElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layer :
    return makeContext<LayerElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::proxy_master_layer :
    return makeContext<ProxyMasterLayerElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class PageElement : public KEY2XMLElementContextBase
{
public:
  explicit PageElement(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  // TODO: use size
  optional<IWORKSize> m_size;
};

PageElement::PageElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t PageElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeElement>(getState(), m_size);
  case IWORKToken::NS_URI_SF | IWORKToken::layers :
    return makeContext<LayersElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void PageElement::endOfElement()
{
}

}

namespace
{

class StyleElement : public KEY2XMLElementContextBase
{
public:
  explicit StyleElement(KEY2ParserState &state, optional<ID_t> &ref);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  optional<ID_t> &m_ref;
};

StyleElement::StyleElement(KEY2ParserState &state, optional<ID_t> &ref)
  : KEY2XMLElementContextBase(state)
  , m_ref(ref)
{
}

IWORKXMLContextPtr_t StyleElement::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::placeholder_style_ref) == name)
    return makeContext<IWORKRefContext>(getState(), m_ref);

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
    return makeContext<StyleElement>(getState(), m_ref);
  case KEY2Token::NS_URI_KEY | KEY2Token::text :
    return makeContext<TextElement>(getState());
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

class MasterSlideElement : public KEY2XMLElementContextBase
{
public:
  explicit MasterSlideElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

MasterSlideElement::MasterSlideElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void MasterSlideElement::startOfElement()
{
  getCollector()->startPage();
}

IWORKXMLContextPtr_t MasterSlideElement::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::page :
    return makeContext<PageElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::stylesheet :
    return makeContext<StylesheetElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::title_placeholder :
    return makeContext<PlaceholderContext>(getState(), true);
  case KEY2Token::NS_URI_KEY | KEY2Token::body_placeholder :
    return makeContext<PlaceholderContext>(getState(), false);
  case KEY2Token::NS_URI_KEY | KEY2Token::sticky_notes :
    return makeContext<StickyNoteElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void MasterSlideElement::endOfElement()
{
  getCollector()->collectPage();
  getCollector()->endPage();
}

}

namespace
{

class MasterSlidesElement : public KEY2XMLElementContextBase
{
public:
  explicit MasterSlidesElement(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

MasterSlidesElement::MasterSlidesElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t MasterSlidesElement::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::master_slide :
    return makeContext<MasterSlideElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class ThemeElement : public KEY2XMLElementContextBase
{
public:
  explicit ThemeElement(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  // TODO: use size
  optional<IWORKSize> m_size;
};

ThemeElement::ThemeElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t ThemeElement::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | IWORKToken::size :
    return makeContext<IWORKSizeElement>(getState(), m_size);
  case KEY2Token::NS_URI_KEY | KEY2Token::stylesheet :
    return makeContext<StylesheetElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::master_slides :
    return makeContext<MasterSlidesElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class ThemeListElement : public KEY2XMLElementContextBase
{
public:
  explicit ThemeListElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

ThemeListElement::ThemeListElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void ThemeListElement::startOfElement()
{
  getCollector()->startThemes();
}

IWORKXMLContextPtr_t ThemeListElement::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::theme :
    return makeContext<ThemeElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void ThemeListElement::endOfElement()
{
  getCollector()->endThemes();
}

}

namespace
{

class NotesElement : public KEY2XMLElementContextBase
{
public:
  explicit NotesElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

NotesElement::NotesElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void NotesElement::startOfElement()
{
  getCollector()->startText(false);
}

IWORKXMLContextPtr_t NotesElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::text_storage | IWORKToken::NS_URI_SF :
    return makeContext<TextStorageElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void NotesElement::endOfElement()
{
  getCollector()->collectNote();
  getCollector()->endText();
}

}

namespace
{

class StickyNotesElement : public KEY2XMLElementContextBase
{
public:
  explicit StickyNotesElement(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

StickyNotesElement::StickyNotesElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t StickyNotesElement::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::sticky_note :
    return makeContext<StickyNoteElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class SlideElement : public KEY2XMLElementContextBase
{
public:
  explicit SlideElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

SlideElement::SlideElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void SlideElement::startOfElement()
{
  getCollector()->startPage();
}

IWORKXMLContextPtr_t SlideElement::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::notes :
    return makeContext<NotesElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::page :
    return makeContext<PageElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::stylesheet :
    return makeContext<StylesheetElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::title_placeholder :
    return makeContext<PlaceholderContext>(getState(), true);
  case KEY2Token::NS_URI_KEY | KEY2Token::body_placeholder :
    return makeContext<PlaceholderContext>(getState(), false);
  case KEY2Token::NS_URI_KEY | KEY2Token::sticky_notes :
    return makeContext<StickyNotesElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void SlideElement::endOfElement()
{
  getCollector()->collectPage();
  getCollector()->endPage();
}

}

namespace
{

class SlideListElement : public KEY2XMLElementContextBase
{
public:
  explicit SlideListElement(KEY2ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

SlideListElement::SlideListElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

void SlideListElement::startOfElement()
{
  getCollector()->startSlides();
}

IWORKXMLContextPtr_t SlideListElement::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::slide :
    return makeContext<SlideElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void SlideListElement::endOfElement()
{
  getCollector()->endSlides();
}

}

namespace
{

class PresentationElement : public KEY2XMLElementContextBase
{
public:
  explicit PresentationElement(KEY2ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);

private:
  optional<IWORKSize> m_size;
  bool m_pendingSize;
};

PresentationElement::PresentationElement(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
  , m_size()
  , m_pendingSize(false)
{
}

void PresentationElement::attribute(const int name, const char *const value)
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

IWORKXMLContextPtr_t PresentationElement::element(const int name)
{
  if (m_pendingSize)
  {
    if (m_size)
      getCollector()->collectPresentationSize(get(m_size));
    m_pendingSize = false;
  }

  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::metadata :
    return makeContext<MetadataElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::theme_list :
    return makeContext<ThemeListElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::slide_list :
    return makeContext<SlideListElement>(getState());
  case KEY2Token::NS_URI_KEY | KEY2Token::size :
    m_pendingSize = true;
    return makeContext<IWORKSizeElement>(getState(), m_size);
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class XMLDocument : public KEY2XMLElementContextBase
{
public:
  explicit XMLDocument(KEY2ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

XMLDocument::XMLDocument(KEY2ParserState &state)
  : KEY2XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t XMLDocument::element(const int name)
{
  switch (name)
  {
  case KEY2Token::NS_URI_KEY | KEY2Token::presentation :
    return makeContext<PresentationElement>(m_state);
  }

  return IWORKXMLContextPtr_t();
}

}

KEY2Parser::KEY2Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, KEYCollector *const collector, KEYDictionary &dict)
  : IWORKParser(input, package)
  , m_state(*this, collector, dict, getTokenizer())
  , m_version(0)
{
}

KEY2Parser::~KEY2Parser()
{
}

IWORKXMLContextPtr_t KEY2Parser::createDocumentContext()
{
  return makeContext<XMLDocument>(m_state);
}

TokenizerFunction_t KEY2Parser::getTokenizer() const
{
  return ChainedTokenizer(KEY2Tokenizer(), IWORKTokenizer());
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
