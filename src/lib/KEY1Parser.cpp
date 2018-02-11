/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/spirit/include/qi.hpp>

#include <glm/glm.hpp>

#include "KEY1Parser.h"

#include "libetonyek_xml.h"

#include "IWORKDiscardContext.h"
#include "IWORKProperties.h"
#include "IWORKRecorder.h"
#include "IWORKText.h"
#include "IWORKTokenizer.h"
#include "KEYCollector.h"
#include "KEY1ContentElement.h"
#include "KEY1Dictionary.h"
#include "KEY1DivElement.h"
#include "KEY1FillElement.h"
#include "KEY1ParserState.h"
#include "KEY1SpanElement.h"
#include "KEY1StringConverter.h"
#include "KEY1StylesContext.h"
#include "KEY1TableElement.h"
#include "KEY1Token.h"
#include "KEY1XMLContextBase.h"

using boost::optional;

namespace libetonyek
{

namespace
{

class MetadataElement : public KEY1XMLElementContextBase
{
public:
  explicit MetadataElement(KEY1ParserState &state);

protected:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  boost::optional<std::string> m_author;
  boost::optional<std::string> m_title;
  boost::optional<std::string> m_keywords;
  boost::optional<std::string> m_comment;
};

MetadataElement::MetadataElement(KEY1ParserState &state)
  : KEY1XMLElementContextBase(state)
  , m_author()
  , m_title()
  , m_keywords()
  , m_comment()
{
}

IWORKXMLContextPtr_t MetadataElement::element(const int name)
{
  switch (name)
  {
  // find application-name, application-version, time-stamp
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void MetadataElement::endOfElement()
{
  IWORKMetadata metadata;

  if (m_author)
    metadata.m_author = get(m_author);
  if (m_title)
    metadata.m_title = get(m_title);
  if (m_keywords)
    metadata.m_keywords = get(m_keywords);
  if (m_comment)
    metadata.m_comment = get(m_comment);

  if (isCollector())
    getCollector().collectMetadata(metadata);
}

}

namespace
{

class CDATAElement : public KEY1XMLElementContextBase
{
public:
  explicit CDATAElement(KEY1ParserState &state, boost::optional<std::string> &description);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void CDATA(const char *value);

private:
  boost::optional<std::string> &m_description;
};

CDATAElement::CDATAElement(KEY1ParserState &state, boost::optional<std::string> &description)
  : KEY1XMLElementContextBase(state)
  , m_description(description)
{
}

void CDATAElement::CDATA(const char *value)
{
  m_description=value;
}


IWORKXMLContextPtr_t CDATAElement::element(const int /*name*/)
{
  ETONYEK_DEBUG_MSG(("CDATAElement::element[KEY1Parser.cpp]: unknown element\n"));

  return IWORKXMLContextPtr_t();
}

}

namespace
{
enum TransitionStyleType
{
  TRANSITION_STYLE_TYPE_INHERITED,
  TRANSITION_STYLE_TYPE_NONE
};

struct TransitionStyle
{
  TransitionStyle()
    : m_type(TRANSITION_STYLE_TYPE_NONE)
    , m_duration()
  {
  }
  TransitionStyleType m_type;
  boost::optional<int> m_duration;
};
}

namespace
{

class TransitionStyleElement : public KEY1XMLElementContextBase
{
public:
  explicit TransitionStyleElement(KEY1ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);

private:
  TransitionStyle m_transitionStyle;
};

TransitionStyleElement::TransitionStyleElement(KEY1ParserState &state)
  : KEY1XMLElementContextBase(state)
  , m_transitionStyle()
{
}

void TransitionStyleElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::duration :
    m_transitionStyle.m_duration=try_int_cast(value);
    break;
  case KEY1Token::type :
    switch (getState().getTokenizer().getId(value))
    {
    case KEY1Token::none :
      m_transitionStyle.m_type=TRANSITION_STYLE_TYPE_NONE;
      break;
    case KEY1Token::inherited :
      m_transitionStyle.m_type=TRANSITION_STYLE_TYPE_INHERITED;
      break;
    default :
      ETONYEK_DEBUG_MSG(("TransitionStyleElement::attribute[KEY1Parser.cpp]: unexpected type %s\n", value));
      break;
    }
    break;
  default :
    ETONYEK_DEBUG_MSG(("TransitionStyleElement::attribute[KEY1Parser.cpp]: unexpected attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t TransitionStyleElement::element(const int /*name*/)
{
  ETONYEK_DEBUG_MSG(("TransitionStyleElement::element[KEY1Parser.cpp]: unknown element\n"));
  return IWORKXMLContextPtr_t();
}
}

namespace
{
struct BulletStyle
{
  BulletStyle()
    : m_relative(true)
    , m_size()
  {
  }
  bool m_relative;
  boost::optional<double> m_size;
};
}

namespace
{

class BulletCharacterStyleElement : public KEY1XMLElementContextBase
{
public:
  explicit BulletCharacterStyleElement(KEY1ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);

private:
  BulletStyle m_bulletStyle;
};

BulletCharacterStyleElement::BulletCharacterStyleElement(KEY1ParserState &state)
  : KEY1XMLElementContextBase(state)
  , m_bulletStyle()
{
}

void BulletCharacterStyleElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::size_technique :
    switch (getState().getTokenizer().getId(value))
    {
    case KEY1Token::relative :
      m_bulletStyle.m_relative=true;
      break;
    default :
      ETONYEK_DEBUG_MSG(("BulletCharacterStyleElement::attribute[KEY1Parser.cpp]: unexpected size technique %s\n", value));
      break;
    }
    break;
  case KEY1Token::size :
    m_bulletStyle.m_size=try_double_cast(value);
    break;
  default :
    ETONYEK_DEBUG_MSG(("BulletCharacterStyleElement::attribute[KEY1Parser.cpp]: unexpected attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t BulletCharacterStyleElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::bullet_characters | KEY1Token::NS_URI_KEY : // README
    break;
  default :
    ETONYEK_DEBUG_MSG(("BulletCharacterStyleElement::element[KEY1Parser.cpp]: unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}
}

namespace
{

class BulletElement : public KEY1XMLElementContextBase
{
public:
  explicit BulletElement(KEY1ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

  void ensureOpened();

private:
  KEY1Bullet m_bullet;
  bool m_opened;
};

BulletElement::BulletElement(KEY1ParserState &state)
  : KEY1XMLElementContextBase(state)
  , m_bullet()
  , m_opened(false)
{
}

void BulletElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::level :
    m_bullet.m_level=try_int_cast(value);
    break;
  case KEY1Token::marker_type :
    switch (getState().getTokenizer().getId(value))
    {
    case KEY1Token::none :
      m_bullet.m_type=KEY1_BULLETTYPE_NONE;
      break;
    case KEY1Token::character :
      m_bullet.m_type=KEY1_BULLETTYPE_CHARACTER;
      break;
    case KEY1Token::image :
      m_bullet.m_type=KEY1_BULLETTYPE_IMAGE;
      break;
    case KEY1Token::inherited :
      m_bullet.m_type=KEY1_BULLETTYPE_INHERITED;
      break;
    case KEY1Token::sequence :
      m_bullet.m_type=KEY1_BULLETTYPE_SEQUENCE;
      break;
    default :
      ETONYEK_DEBUG_MSG(("BulletElement::attribute[KEY1Parser.cpp]: unexpected type %s\n", value));
      break;
    }
    break;
  case KEY1Token::spacing :
    m_bullet.m_spacing=try_double_cast(value);
    break;
  default :
    ETONYEK_DEBUG_MSG(("BulletElement::attribute[KEY1Parser.cpp]: unexpected attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t BulletElement::element(const int name)
{
  ensureOpened();
  switch (name)
  {
  case KEY1Token::character_bullet_style | KEY1Token::NS_URI_KEY :
    return makeContext<BulletCharacterStyleElement>(getState());
  case KEY1Token::image_bullet_style | KEY1Token::NS_URI_KEY : // README
  case KEY1Token::sequence_bullet_style | KEY1Token::NS_URI_KEY : // README
    break;
  case KEY1Token::content | KEY1Token::NS_URI_KEY :
    return makeContext<KEY1ContentElement>(getState());
  default :
    ETONYEK_DEBUG_MSG(("BulletElement::element[KEY1Parser.cpp]: unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void BulletElement::endOfElement()
{
  if (!m_opened) return;
  getState().closeBullet();
  m_opened=false;
}

void BulletElement::ensureOpened()
{
  if (m_opened) return;
  getState().openBullet(m_bullet);
  m_opened=true;
}

}

namespace
{

class BulletsElement : public KEY1XMLElementContextBase
{
public:
  explicit BulletsElement(KEY1ParserState &state, bool prototype);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  bool m_prototype;
};

BulletsElement::BulletsElement(KEY1ParserState &state, bool prototype)
  : KEY1XMLElementContextBase(state)
  , m_prototype(prototype)
{
}

void BulletsElement::startOfElement()
{
  getState().pushIsPrototype(m_prototype);
  getState().openBullets();
}

IWORKXMLContextPtr_t BulletsElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::bullet | KEY1Token::NS_URI_KEY :
    return makeContext<BulletElement>(getState());
  default :
    ETONYEK_DEBUG_MSG(("BulletsElement::element[KEY1Parser.cpp]: unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void BulletsElement::endOfElement()
{
  getState().closeBullets();
  getState().popIsPrototype();
}

}

namespace
{

class BasicShapeElement : public KEY1XMLElementContextBase
{
public:
  BasicShapeElement(KEY1ParserState &state);

protected:
  virtual void attribute(int name, const char *value);
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

  IWORKStylePtr_t getStyle() const
  {
    return m_style;
  }
private:
  IWORKStylePtr_t m_style;
  boost::optional<IWORKColor> m_strokeColor;
  boost::optional<double> m_strokeWidth;
  boost::optional<double> m_opacity;
};

BasicShapeElement::BasicShapeElement(KEY1ParserState &state)
  : KEY1XMLElementContextBase(state)
  , m_strokeColor()
  , m_strokeWidth()
  , m_opacity()
{
}

void BasicShapeElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::id :
    setId(value);
    break;
  case KEY1Token::opacity :
    m_opacity=try_double_cast(value);
    break;
  case KEY1Token::stroke_color :
    m_strokeColor=KEY1StringConverter<IWORKColor>::convert(value);
    break;
  case KEY1Token::stroke_width :
    m_strokeWidth=try_double_cast(value);
    break;
  default :
    ETONYEK_DEBUG_MSG(("BasicShapeElement::attribute[KEY1Parser.cpp]: unexpected attribute\n"));
    break;
  }
}

void BasicShapeElement::startOfElement()
{
}

IWORKXMLContextPtr_t BasicShapeElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::styles | KEY1Token::NS_URI_KEY :
    return makeContext<KEY1StylesContext>(getState(), m_style, IWORKStylePtr_t());
  default :
    ETONYEK_DEBUG_MSG(("BasicShapeElement::element[KEY1Parser.cpp]: unknown element\n"));
    break;
  }
  return IWORKXMLContextPtr_t();
}

void BasicShapeElement::endOfElement()
{
  if (getState().m_isPrototype&& (!m_strokeColor && !m_strokeWidth && !m_opacity))
    return;
  if (!m_style)
    m_style.reset(new IWORKStyle(IWORKPropertyMap(), boost::none, IWORKStylePtr_t()));
  if (m_opacity)
    m_style->getPropertyMap().put<property::Opacity>(get(m_opacity));
  if (m_strokeColor || m_strokeWidth)
  {
    IWORKStroke stroke;
    if (m_style->has<property::Stroke>())
      stroke=m_style->get<property::Stroke>();
    else
      stroke.m_pattern.m_type = IWORK_STROKE_TYPE_SOLID;
    if (m_strokeColor) stroke.m_color=get(m_strokeColor);
    if (m_strokeWidth) stroke.m_width=get(m_strokeWidth);
    m_style->getPropertyMap().put<property::Stroke>(stroke);
  }
}

}

namespace
{

class ImageElement : public BasicShapeElement
{
public:
  explicit ImageElement(KEY1ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void startOfElement();
  virtual void endOfElement();
private:
  boost::optional<std::string> m_displayName;
  boost::optional<std::string> m_imageName;
  bool m_lockAspectRatio;
  bool m_locked;
  boost::optional<IWORKSize> m_naturalSize;
  boost::optional<glm::dmat3> m_transformation;
};

ImageElement::ImageElement(KEY1ParserState &state)
  : BasicShapeElement(state)
  , m_displayName()
  , m_imageName()
  , m_lockAspectRatio(false)
  , m_locked(false)
  , m_naturalSize()
  , m_transformation()
{
}

void ImageElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::display_name :
    m_displayName=value;
    break;
  case KEY1Token::image_data :
    m_imageName=value;
    break;
  case KEY1Token::locked :
    m_locked = bool_cast(value);
    break;
  case KEY1Token::lock_aspect_ratio :
    m_lockAspectRatio = bool_cast(value);
    break;
  case KEY1Token::natural_size :
    m_naturalSize = KEY1StringConverter<IWORKSize>::convert(value);
    break;
  case KEY1Token::transformation :
    m_transformation=KEY1StringConverter<glm::dmat3>::convert(value);
    break;
  case KEY1Token::byte_size : // the image size
    break;
  default :
    BasicShapeElement::attribute(name,value);
    break;
  }
}

void ImageElement::startOfElement()
{
  BasicShapeElement::startOfElement();
  if (!getState().m_isPrototype&& isCollector())
    getCollector().startLevel();
}

void ImageElement::endOfElement()
{
  BasicShapeElement::endOfElement();
  IWORKMediaContentPtr_t content;

  if (m_imageName)
  {
    content.reset(new IWORKMediaContent());
    content->m_data.reset(new IWORKData);
    content->m_data->m_stream.reset(getState().getParser().getPackage()->getSubStreamByName(get(m_imageName).c_str()));
    content->m_size=m_naturalSize;
  }
  IWORKGeometryPtr_t geometry;
  if (m_transformation && m_naturalSize)
  {
    geometry.reset(new IWORKGeometry);
    glm::dvec3 pos=get(m_transformation)*glm::dvec3(0,0,1);
    geometry->m_position=IWORKPosition(pos[0],pos[1]);
    glm::dvec3 dim=get(m_transformation)*glm::dvec3(get(m_naturalSize).m_width,get(m_naturalSize).m_height,0);
    geometry->m_size=IWORKSize(dim[0],dim[1]);
  }
  if (getId() && content)
    getState().getDictionary().m_images[get(getId())]=content;

  IWORKStylePtr_t style=getStyle();
  getState().getDictionary().storeImageStyle(style, getState().m_isPrototype);
  if (!getState().m_isPrototype&& isCollector())
  {
    if (geometry && content)
    {
      getCollector().collectGeometry(geometry);
      getCollector().setGraphicStyle(style);
      getCollector().collectImage(content, IWORKGeometryPtr_t(), boost::none, m_locked);
    }
    getCollector().endLevel();
  }
}
}

namespace
{

class LineElement : public BasicShapeElement
{
public:
  explicit LineElement(KEY1ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void startOfElement();
  virtual void endOfElement();
private:
  boost::optional<IWORKPosition> m_head;
  boost::optional<IWORKPosition> m_tail;
};

LineElement::LineElement(KEY1ParserState &state)
  : BasicShapeElement(state)
  , m_head()
  , m_tail()
{
}

void LineElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::head :
    m_head=KEY1StringConverter<IWORKPosition>::convert(value);
    break;
  case KEY1Token::tail :
    m_tail=KEY1StringConverter<IWORKPosition>::convert(value);
    break;
  default :
    BasicShapeElement::attribute(name,value);
    break;
  }
}

void LineElement::startOfElement()
{
  BasicShapeElement::startOfElement();
  if (!getState().m_isPrototype&& isCollector())
    getCollector().startLevel();
}

void LineElement::endOfElement()
{
  BasicShapeElement::endOfElement();
  IWORKLinePtr_t line(new IWORKLine());
  if (m_tail)
  {
    line->m_x1 = get(m_tail).m_x;
    line->m_y1 = get(m_tail).m_y;
  }
  if (m_head)
  {
    line->m_x2 = get(m_head).m_x;
    line->m_y2 = get(m_head).m_y;
  }
  IWORKStylePtr_t style=getStyle();
  getState().getDictionary().storeLineStyle(style, getState().m_isPrototype);
  if (!getState().m_isPrototype&& isCollector())
  {
    getCollector().setGraphicStyle(style);
    getCollector().collectLine(line);
    getCollector().endLevel();
  }
}
}

namespace
{

class ShapeElement : public BasicShapeElement
{
public:
  explicit ShapeElement(KEY1ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void startOfElement();
  virtual void endOfElement();
private:
  IWORKPathPtr_t m_path;
  boost::optional<glm::dmat3> m_transformation;
};

ShapeElement::ShapeElement(KEY1ParserState &state)
  : BasicShapeElement(state)
  , m_path()
  , m_transformation()
{
}

void ShapeElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::path :
    try
    {
      m_path.reset(new IWORKPath(value));
    }
    catch (const IWORKPath::InvalidException &)
    {
      ETONYEK_DEBUG_MSG(("ShapeElement::attribute[KEY1Parser.cpp]: '%s' is not a valid path\n", value));
    }
    break;
  case KEY1Token::transformation :
    m_transformation=KEY1StringConverter<glm::dmat3>::convert(value);
    break;
  default :
    BasicShapeElement::attribute(name, value);
    break;
  }
}

void ShapeElement::startOfElement()
{
  BasicShapeElement::startOfElement();
  if (!getState().m_isPrototype&& isCollector())
    getCollector().startLevel();
}

void ShapeElement::endOfElement()
{
  BasicShapeElement::endOfElement();
  IWORKStylePtr_t style=getStyle();
  getState().getDictionary().storeShapeStyle(style, getState().m_isPrototype);
  if (!getState().m_isPrototype&& isCollector())
  {
    if (m_path)
    {
      getCollector().setGraphicStyle(style);
      if (m_transformation)
        *m_path *= get(m_transformation);
      getCollector().collectBezier(m_path);
      getCollector().collectShape();
    }
    getCollector().endLevel();
  }
}
}

namespace
{
class TextAttributesElement : public KEY1XMLElementContextBase
{
public:
  TextAttributesElement(KEY1ParserState &state, IWORKStylePtr_t &spanStyle, IWORKStylePtr_t &divStyle);

protected:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  KEY1DivStyle m_divStyle;
  KEY1SpanStyle m_spanStyle;

  IWORKStylePtr_t &m_divStylePtr;
  IWORKStylePtr_t &m_spanStylePtr;
};

TextAttributesElement::TextAttributesElement(KEY1ParserState &state, IWORKStylePtr_t &spanStyle, IWORKStylePtr_t &divStyle)
  : KEY1XMLElementContextBase(state)
  , m_divStyle(state, IWORKStylePtr_t())
  , m_spanStyle(state, IWORKStylePtr_t())
  , m_divStylePtr(divStyle)
  , m_spanStylePtr(spanStyle)
{
}

void TextAttributesElement::attribute(const int name, const char *const value)
{
  if (m_divStyle.readAttribute(name, value) ||
      m_spanStyle.readAttribute(name, value))
    return;
  switch (name)
  {
  case KEY1Token::id :
    setId(value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("TextAttributesElement::attribute[KEY1Parser.cpp] attribute with value=%s\n", value));
  }
}

IWORKXMLContextPtr_t TextAttributesElement::element(const int name)
{
  switch (name)
  {
  default:
    ETONYEK_DEBUG_MSG(("TextAttributesElement::element[KEY1Parser.cpp]: unknown element\n"));
  }
  return IWORKXMLContextPtr_t();
}

void TextAttributesElement::endOfElement()
{
  m_divStylePtr = m_divStyle.getStyle();
  m_spanStylePtr = m_spanStyle.getStyle();
}

}

namespace
{
class PlaceholderElement : public BasicShapeElement
{
public:
  explicit PlaceholderElement(KEY1ParserState &state);

  KEYPlaceholderPtr_t getPlaceholder();

protected:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);

private:
  boost::optional<IWORKPosition> m_location;
  boost::optional<IWORKSize> m_size;
  IWORKStylePtr_t m_paragraphStyle;
  IWORKStylePtr_t m_spanStyle;
  boost::optional<IWORKVerticalAlignment> m_verticalAlignment;
  boost::optional<bool> m_visible;
};

PlaceholderElement::PlaceholderElement(KEY1ParserState &state)
  : BasicShapeElement(state)
  , m_location()
  , m_size()
  , m_paragraphStyle()
  , m_spanStyle()
  , m_verticalAlignment()
  , m_visible()
{
}

void PlaceholderElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::location :
    m_location=KEY1StringConverter<IWORKPosition>::convert(value);
    break;
  case KEY1Token::size :
    m_size=KEY1StringConverter<IWORKSize>::convert(value);
    break;
  case KEY1Token::vertical_alignment :
    switch (getState().getTokenizer().getId(value))
    {
    case KEY1Token::bottom :
      m_verticalAlignment = IWORK_VERTICAL_ALIGNMENT_BOTTOM;
      break;
    case KEY1Token::middle :
      m_verticalAlignment = IWORK_VERTICAL_ALIGNMENT_MIDDLE;
      break;
    case KEY1Token::top :
      m_verticalAlignment = IWORK_VERTICAL_ALIGNMENT_TOP;
      break;
    case KEY1Token::tracks_master :
      break;
    default :
      ETONYEK_DEBUG_MSG(("PlaceHolderElement::attribute[KEY1Parser.cpp]: unexpected vertical align %s\n", value));
      break;
    }
    break;
  case KEY1Token::visibility :
    switch (getState().getTokenizer().getId(value))
    {
    case KEY1Token::hidden :
      m_visible = false;
      break;
    case KEY1Token::visible :
      m_visible = true;
      break;
    case KEY1Token::tracks_master :
      break;
    default :
      ETONYEK_DEBUG_MSG(("PlaceHolderElement::attribute[KEY1Parser.cpp]: unexpected visibily align %s\n", value));
      break;
    }
    break;
  default :
    BasicShapeElement::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t PlaceholderElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::text_attributes | KEY1Token::NS_URI_KEY :
    return makeContext<TextAttributesElement>(getState(), m_spanStyle, m_paragraphStyle);
  default:
    break;
  }
  return BasicShapeElement::element(name);
}

KEYPlaceholderPtr_t PlaceholderElement::getPlaceholder()
{
  KEYPlaceholderPtr_t res(new KEYPlaceholder);
  res->m_style = getStyle();
  res->m_paragraphStyle = m_paragraphStyle;
  res->m_spanStyle = m_spanStyle;
  if (m_location && m_size)
  {
    IWORKGeometryPtr_t geometry(new IWORKGeometry);
    geometry->m_position=get(m_location);
    geometry->m_naturalSize=geometry->m_size=get(m_size);
    res->m_geometry=geometry;
  }
  res->m_visible=m_visible;

  return res;
}

}

namespace
{
class BodyElement : public PlaceholderElement
{
public:
  explicit BodyElement(KEY1ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void endOfElement();
private:
  std::deque<double> m_indents;
};

BodyElement::BodyElement(KEY1ParserState &state)
  : PlaceholderElement(state)
{
}

void BodyElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::bullet_indentation :
  {
    std::string val(value);
    std::string::const_iterator it(val.begin()), end(val.end());

    namespace ascii = boost::spirit::ascii;
    namespace qi =  boost::spirit::qi;
    if (qi::phrase_parse(it, end, qi::double_ >> *(qi::double_), ascii::space, m_indents) && it==end)
      ;
    else
    {
      ETONYEK_DEBUG_MSG(("BodyElement::attribute[KEY1Parser.cpp]: unexpected bullet indentation %s\n", value));
    }
    break;
  }
  default :
    PlaceholderElement::attribute(name, value);
  }
}

void BodyElement::endOfElement()
{
  KEYPlaceholderPtr_t placeHolder=getPlaceholder();
  if (!placeHolder) return;
  placeHolder->m_bulletIndentations=m_indents;
  getState().getDictionary().storeBodyPlaceholder(placeHolder);
}
}

namespace
{
class PageNumberElement : public PlaceholderElement
{
public:
  explicit PageNumberElement(KEY1ParserState &state);

private:
  virtual void endOfElement();
private:
};

PageNumberElement::PageNumberElement(KEY1ParserState &state)
  : PlaceholderElement(state)
{
}

void PageNumberElement::endOfElement()
{
  getState().getDictionary().storePageNumberPlaceholder(getPlaceholder());
}
}

namespace
{
class TitleElement : public PlaceholderElement
{
public:
  explicit TitleElement(KEY1ParserState &state);

private:
  virtual void endOfElement();
private:
};

TitleElement::TitleElement(KEY1ParserState &state)
  : PlaceholderElement(state)
{
}

void TitleElement::endOfElement()
{
  getState().getDictionary().storeTitlePlaceholder(getPlaceholder());
}
}

namespace
{

class TextboxElement : public BasicShapeElement
{
public:
  explicit TextboxElement(KEY1ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
private:
  boost::optional<IWORKSize> m_size;
  boost::optional<glm::dmat3> m_transformation;
  boost::optional<bool> m_growHorizontally;
};

TextboxElement::TextboxElement(KEY1ParserState &state)
  : BasicShapeElement(state)
  , m_size()
  , m_transformation()
{
}

void TextboxElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::grow_horizontally :
    m_growHorizontally = try_bool_cast(value);
    break;
  case KEY1Token::size :
    m_size=KEY1StringConverter<IWORKSize>::convert(value);
    break;
  case KEY1Token::transformation :
    m_transformation=KEY1StringConverter<glm::dmat3>::convert(value);
    break;
  default :
    BasicShapeElement::attribute(name, value);
    break;
  }
}

void TextboxElement::startOfElement()
{
  BasicShapeElement::startOfElement();
  if (!getState().m_isPrototype&& isCollector())
  {
    getCollector().startLevel();
    assert(!getState().m_currentText);
    getState().m_currentText = getCollector().createText(getState().m_langManager);
  }
}

IWORKXMLContextPtr_t TextboxElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::content | KEY1Token::NS_URI_KEY :
    return makeContext<KEY1ContentElement>(getState());
  default:
    break;
  }
  return BasicShapeElement::element(name);
}

void TextboxElement::endOfElement()
{
  BasicShapeElement::endOfElement();
  IWORKStylePtr_t style=getStyle();
  getState().getDictionary().storeTextboxStyle(style, getState().m_isPrototype);
  if (!getState().m_isPrototype && isCollector())
  {
    getCollector().setGraphicStyle(style);
    if (m_transformation && m_size)
    {
      IWORKGeometryPtr_t geometry(new IWORKGeometry);
      glm::dvec3 pos=get(m_transformation)*glm::dvec3(0,0,1);
      geometry->m_position=IWORKPosition(pos[0],pos[1]);
      glm::dvec3 dim=get(m_transformation)*glm::dvec3(get(m_size).m_width,get(m_size).m_height,0);
      geometry->m_naturalSize=geometry->m_size=IWORKSize(dim[0],dim[1]);
      getCollector().collectGeometry(geometry);
    }
    IWORKPathPtr_t path=makeRoundedRectanglePath(/*m_size ? get(m_size) : */IWORKSize(1,1), 0);
    getCollector().collectBezier(path);
    if (bool(getState().m_currentText) && !getState().m_currentText->empty())
      getCollector().collectText(getState().m_currentText);
    getState().m_currentText.reset();
    getCollector().collectShape();

    getCollector().endLevel();
  }
}
}

namespace
{

class PluginDataElement : public KEY1XMLElementContextBase
{
public:
  PluginDataElement(KEY1ParserState &state, boost::optional<IWORKSize> &size, bool prototype);

protected:
  virtual void attribute(int name, const char *value);
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  bool m_prototype;
  boost::optional<IWORKSize> &m_size;
};

PluginDataElement::PluginDataElement(KEY1ParserState &state, boost::optional<IWORKSize> &size, bool prototype)
  : KEY1XMLElementContextBase(state)
  , m_prototype(prototype)
  , m_size(size)
{
}

void PluginDataElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  default:
    ETONYEK_DEBUG_MSG(("PluginDataElement::attribute[KEY1Parser.cpp]: unknown attribute with value=%s\n", value));
  }
}

void PluginDataElement::startOfElement()
{
  getState().pushIsPrototype(m_prototype);
}

IWORKXMLContextPtr_t PluginDataElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::array  | KEY1Token::NS_URI_KEY : // with list of number
  case KEY1Token::chart_prototype  | KEY1Token::NS_URI_KEY :
  case KEY1Token::color  | KEY1Token::NS_URI_KEY :
  case KEY1Token::content  | KEY1Token::NS_URI_KEY :
  case KEY1Token::dict  | KEY1Token::NS_URI_KEY : // with child size, rect, ...
  case KEY1Token::dash_style  | KEY1Token::NS_URI_KEY :
  case KEY1Token::fill_style  | KEY1Token::NS_URI_KEY :
  case KEY1Token::number  | KEY1Token::NS_URI_KEY :
  case KEY1Token::reference  | KEY1Token::NS_URI_KEY : // style reference
  case KEY1Token::string  | KEY1Token::NS_URI_KEY : // with value dictionary, root
    break;
  case KEY1Token::table  | KEY1Token::NS_URI_KEY :
    return makeContext<KEY1TableElement>(getState(), m_size);
  default:
    ETONYEK_DEBUG_MSG(("PluginDataElement::element[KEY1Parser.cpp]: unknown element\n"));
  }
  return IWORKXMLContextPtr_t();
}

void PluginDataElement::endOfElement()
{
  getState().popIsPrototype();
}

}

namespace
{

class PluginElement : public BasicShapeElement
{
public:
  explicit PluginElement(KEY1ParserState &state);

protected:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  boost::optional<std::string> m_key;
  boost::optional<glm::dmat3> m_transformation;
  boost::optional<IWORKSize> m_size;
};

PluginElement::PluginElement(KEY1ParserState &state)
  : BasicShapeElement(state)
  , m_key()
  , m_transformation()
  , m_size()
{
}

void PluginElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::key :
    m_key = value;
    break;
  case KEY1Token::transformation :
    m_transformation=KEY1StringConverter<glm::dmat3>::convert(value);
    break;
  default :
    BasicShapeElement::attribute(name,value);
    break;
  }
}

IWORKXMLContextPtr_t PluginElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::plugin_data  | KEY1Token::NS_URI_KEY :
    return makeContext<PluginDataElement>(getState(), m_size, false);
  case KEY1Token::prototype_data  | KEY1Token::NS_URI_KEY :
    return makeContext<PluginDataElement>(getState(), m_size, true);
  default :
    return BasicShapeElement::element(name);
  }
  return IWORKXMLContextPtr_t();
}

void PluginElement::endOfElement()
{
  if (!isCollector())
    return;
  IWORKStylePtr_t style=getStyle();
  // CHANGEME: check the plugin-data has created a table, if so, send it
  if (getState().m_currentTable)
  {
    getCollector().setGraphicStyle(style);
    if (m_transformation && m_size)
    {
      IWORKGeometryPtr_t geometry(new IWORKGeometry);
      glm::dvec3 pos=get(m_transformation)*glm::dvec3(0,0,1);
      geometry->m_position=IWORKPosition(pos[0],pos[1]);
      glm::dvec3 dim=get(m_transformation)*glm::dvec3(get(m_size).m_width,get(m_size).m_height,0);
      geometry->m_naturalSize=geometry->m_size=IWORKSize(dim[0],dim[1]);
      getCollector().collectGeometry(geometry);
    }
    getCollector().collectTable(getState().m_currentTable);
    getState().m_currentTable.reset();
  }
}

}

namespace
{

class DrawablesElement : public KEY1XMLElementContextBase
{
public:
  explicit DrawablesElement(KEY1ParserState &state, bool prototype);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  bool m_prototype;
};

DrawablesElement::DrawablesElement(KEY1ParserState &state, bool prototype)
  : KEY1XMLElementContextBase(state)
  , m_prototype(prototype)
{
}

void DrawablesElement::startOfElement()
{
  getState().pushIsPrototype(m_prototype);
  if (isCollector())
    getCollector().startLevel();
}

IWORKXMLContextPtr_t DrawablesElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::body | KEY1Token::NS_URI_KEY :
    return makeContext<BodyElement>(getState());
  case KEY1Token::image | KEY1Token::NS_URI_KEY :
    return makeContext<ImageElement>(getState());
  case KEY1Token::line | KEY1Token::NS_URI_KEY :
    return makeContext<LineElement>(getState());
  case KEY1Token::page_number | KEY1Token::NS_URI_KEY :
    return makeContext<PageNumberElement>(getState());
  case KEY1Token::plugin | KEY1Token::NS_URI_KEY :
    return makeContext<PluginElement>(getState());
  case KEY1Token::shape | KEY1Token::NS_URI_KEY :
    return makeContext<ShapeElement>(getState());
  case KEY1Token::textbox | KEY1Token::NS_URI_KEY :
    return makeContext<TextboxElement>(getState());
  case KEY1Token::title | KEY1Token::NS_URI_KEY :
    return makeContext<TitleElement>(getState());
  default :
    ETONYEK_DEBUG_MSG(("DrawablesElement::element[KEY1Parser.cpp]: unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void DrawablesElement::endOfElement()
{
  if (isCollector())
    getCollector().endLevel();
  getState().popIsPrototype();
}

}

namespace
{

class PluginsElement : public KEY1XMLElementContextBase
{
public:
  explicit PluginsElement(KEY1ParserState &state, bool prototype);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  bool m_prototype;
};

PluginsElement::PluginsElement(KEY1ParserState &state, bool prototype)
  : KEY1XMLElementContextBase(state)
  , m_prototype(prototype)
{
}

void PluginsElement::startOfElement()
{
  getState().pushIsPrototype(m_prototype);
}

IWORKXMLContextPtr_t PluginsElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::prototype_plugin | KEY1Token::NS_URI_KEY :
    return makeContext<PluginElement>(getState());
  default :
    ETONYEK_DEBUG_MSG(("PluginsElement::element[KEY1Parser.cpp]: unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void PluginsElement::endOfElement()
{
  getState().popIsPrototype();
}
}

namespace
{

class SlideElement : public KEY1XMLElementContextBase
{
public:
  explicit SlideElement(KEY1ParserState &state, bool isMasterSlide);

private:
  virtual void attribute(int name, const char *value);
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  bool m_isMasterSlide;
  optional<ID_t> m_styleRef;
  optional<ID_t> m_masterRef;

  boost::optional<std::string> m_name; // master name
  boost::optional<IWORKFill> m_background;

  // we need to record the layer as the main style are not read
  std::shared_ptr<IWORKRecorder> m_recorder;
  boost::optional<std::string> m_notes;
};

SlideElement::SlideElement(KEY1ParserState &state, bool isMasterSlide)
  : KEY1XMLElementContextBase(state)
  , m_isMasterSlide(isMasterSlide)
  , m_masterRef()
  , m_name()
  , m_background()
  , m_recorder()
  , m_notes()
{
}


void SlideElement::attribute(int name, const char *value)
{
  switch (name)
  {
  case KEY1Token::name :
    m_name=value;
    break;
  case KEY1Token::id :
    setId(value);
    break;
  case KEY1Token::master_slide_id :
    m_masterRef=value;
    break;
  case KEY1Token::floating_content : // when exist with value=true, probably meaning has floating content
    break;
  default :
    ETONYEK_DEBUG_MSG(("SlideElement::attribute[KEY1Parser.cpp]: unknown attribute\n"));
    break;
  }
}

void SlideElement::startOfElement()
{
  getState().pushIsMasterSlide(m_isMasterSlide);
  getState().getDictionary().pushStylesContext();
  if (isCollector())
  {
    getCollector().startPage();
    getCollector().startLayer();
    m_recorder.reset(new IWORKRecorder());
    getCollector().setRecorder(m_recorder);
  }
}

IWORKXMLContextPtr_t SlideElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::bullets | KEY1Token::NS_URI_KEY :
    return makeContext<BulletsElement>(getState(), false);
  case KEY1Token::drawables | KEY1Token::NS_URI_KEY :
    return makeContext<DrawablesElement>(getState(), false);
  case KEY1Token::guides | KEY1Token::NS_URI_KEY : // list of guide, safe to ignore?
    break;
  case KEY1Token::notes | KEY1Token::NS_URI_KEY :
    return makeContext<CDATAElement>(getState(), m_notes);
  case KEY1Token::prototype_bullets | KEY1Token::NS_URI_KEY :
    return makeContext<BulletsElement>(getState(), true);
  case KEY1Token::prototype_drawables | KEY1Token::NS_URI_KEY :
    return makeContext<DrawablesElement>(getState(), true);
  case KEY1Token::prototype_plugins | KEY1Token::NS_URI_KEY :
    return makeContext<PluginsElement>(getState(), true);
  case KEY1Token::background_fill_style | KEY1Token::NS_URI_KEY :
    return makeContext<KEY1FillElement>(getState(), m_background);
  case KEY1Token::transition_style | KEY1Token::NS_URI_KEY :
    return makeContext<TransitionStyleElement>(getState());
  case KEY1Token::thumbnails | KEY1Token::NS_URI_KEY : // ok to ignore
    break;
  default :
    ETONYEK_DEBUG_MSG(("SlideElement::element[KEY1Parser.cpp]: unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void SlideElement::endOfElement()
{
  getState().getDictionary().linkStylesContext(m_masterRef);
  if (m_isMasterSlide && getId()) getState().getDictionary().collectStylesContext(get(getId()));
  if (isCollector())
  {
    if (m_background)
    {
      IWORKPropertyMap props;
      props.put<property::Fill>(get(m_background));
      IWORKStylePtr_t style(new IWORKStyle(props, boost::none, boost::none));

      getCollector().setSlideStyle(style);
    }
    if (!m_isMasterSlide)
    {
      for (int i=0; i<2; ++i)
      {
        KEYPlaceholderPtr_t placeholder=i==0 ? getState().getDictionary().getTitlePlaceholder() :
                                        getState().getDictionary().getBodyPlaceholder();
        if (!placeholder || (placeholder->m_visible && !get(placeholder->m_visible)))
          continue;
        getCollector().insertTextPlaceholder(placeholder);
      }
    }
    const KEYLayerPtr_t layer(getCollector().collectLayer());
    getCollector().endLayer();
    if (bool(layer))
      getCollector().insertLayer(layer);
    if (m_notes)
    {
      assert(!getState().m_currentText);
      getState().m_currentText = getCollector().createText(getState().m_langManager);
      getState().m_currentText->insertText(get(m_notes).c_str());
      getCollector().collectText(getState().m_currentText);
      getState().m_currentText.reset();
      getCollector().collectNote();
    }
    KEYSlidePtr_t slide=getCollector().collectSlide();
    getCollector().endPage();
    if (!slide)
      return;
    slide->m_name=m_name;
    if (m_masterRef && m_isMasterSlide)
    {
      ETONYEK_DEBUG_MSG(("SlideElement::endOfElement[KEY1Parser.cpp]: find a master slide with masterRef\n"));
    }
    else if (m_masterRef)
    {
      const KEYSlideMap_t::const_iterator it = getState().getDictionary().m_masterSlides.find(get(m_masterRef));
      if (it != getState().getDictionary().m_masterSlides.end())
        slide->m_masterSlide=it->second;
      else
      {
        ETONYEK_DEBUG_MSG(("SlideElement::endOfElement[KEY1Parser.cpp]: unknown master %s\n", get(m_masterRef).c_str()));
      }
    }
    if (!m_isMasterSlide)
      getState().getDictionary().m_slides.push_back(slide);
    else if (getId())
      getState().getDictionary().m_masterSlides[get(getId())]=slide;
    else
    {
      ETONYEK_DEBUG_MSG(("SlideElement::endOfElement[KEY1Parser.cpp]: can not find a master id\n"));
    }
  }
  getState().getDictionary().popStylesContext();
  getState().popIsMasterSlide();
}

}

namespace
{

class SlideListElement : public KEY1XMLElementContextBase
{
public:
  explicit SlideListElement(KEY1ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

SlideListElement::SlideListElement(KEY1ParserState &state)
  : KEY1XMLElementContextBase(state)
{
}

void SlideListElement::startOfElement()
{
  if (isCollector())
    getCollector().startSlides();
}

IWORKXMLContextPtr_t SlideListElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::slide | KEY1Token::NS_URI_KEY :
    return makeContext<SlideElement>(getState(), false);
  default :
    ETONYEK_DEBUG_MSG(("SlideListElement::element[KEY1Parser.cpp]: unexpected element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void SlideListElement::endOfElement()
{
  if (isCollector())
    getCollector().endSlides();
}

}

namespace
{

class MasterSlidesElement : public KEY1XMLElementContextBase
{
public:
  explicit MasterSlidesElement(KEY1ParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

MasterSlidesElement::MasterSlidesElement(KEY1ParserState &state)
  : KEY1XMLElementContextBase(state)
{
}

void MasterSlidesElement::startOfElement()
{
  if (isCollector())
    getCollector().startSlides();
}

IWORKXMLContextPtr_t MasterSlidesElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::master_slide | KEY1Token::NS_URI_KEY :
    return makeContext<SlideElement>(getState(), true);
  default :
    ETONYEK_DEBUG_MSG(("MasterSlidesElement::element[KEY1Parser.cpp]: unexpected element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void MasterSlidesElement::endOfElement()
{
  if (isCollector())
    getCollector().endSlides();
}
}

namespace
{
class ThemeElement : public KEY1XMLElementContextBase
{
public:
  explicit ThemeElement(KEY1ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);

private:
  boost::optional<std::string> m_description;
};

ThemeElement::ThemeElement(KEY1ParserState &state)
  : KEY1XMLElementContextBase(state)
  , m_description()
{
}

void ThemeElement::attribute(int name, const char *value)
{
  switch (name)
  {
  case KEY1Token::slide_size :
  {
    boost::optional<IWORKSize> size=KEY1StringConverter<IWORKSize>::convert(value);
    if (size && isCollector())
      getCollector().collectPresentationSize(get(size));
    break;
  }
  default :
    ETONYEK_DEBUG_MSG(("ThemeElement::attribute[KEY1Parser.cpp]: unexpected attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t ThemeElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::description | KEY1Token::NS_URI_KEY :
    return makeContext<CDATAElement>(getState(), m_description);
  case KEY1Token::prototype_drawables | KEY1Token::NS_URI_KEY :
    return makeContext<DrawablesElement>(getState(), true);
  case KEY1Token::prototype_plugins | KEY1Token::NS_URI_KEY :
    return makeContext<PluginsElement>(getState(), true);
  case KEY1Token::master_slides | KEY1Token::NS_URI_KEY :
    return makeContext<MasterSlidesElement>(getState());
  default :
    ETONYEK_DEBUG_MSG(("ThemeElement::element[KEY1Parser.cpp]: unexpected element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}
}

namespace
{
class PresentationElement : public KEY1XMLElementContextBase
{
public:
  explicit PresentationElement(KEY1ParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
};

PresentationElement::PresentationElement(KEY1ParserState &state)
  : KEY1XMLElementContextBase(state)
{
}

void PresentationElement::startOfElement()
{
  if (isCollector())
  {
    getCollector().startDocument();
    getCollector().setAccumulateTransformTo(false);
  }
}

void PresentationElement::attribute(const int name, const char *const /*value*/)
{
  switch (name)
  {
  case KEY1Token::version : // find 36
    break;
  default :
    // normally only the namespace xmlns:plugin
    break;
  }
}

IWORKXMLContextPtr_t PresentationElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::metadata | KEY1Token::NS_URI_KEY :
    return makeContext<MetadataElement>(getState());
  case KEY1Token::theme | KEY1Token::NS_URI_KEY :
    return makeContext<ThemeElement>(getState());
  case KEY1Token::slide_list | KEY1Token::NS_URI_KEY :
    return makeContext<SlideListElement>(getState());
  case KEY1Token::ui_state | KEY1Token::NS_URI_KEY : // safe to ignore
    break;
  default :
    ETONYEK_DEBUG_MSG(("PresentationElement::element[KEY1Parser.cpp]: unexpected element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void PresentationElement::endOfElement()
{
  if (isCollector())
  {
    getCollector().sendSlides(getState().getDictionary().m_slides);
    getCollector().endDocument();
  }
}

}

namespace
{

class XMLDocument : public KEY1XMLElementContextBase
{
public:
  explicit XMLDocument(KEY1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

XMLDocument::XMLDocument(KEY1ParserState &state)
  : KEY1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t XMLDocument::element(const int name)
{
  switch (name)
  {
  case KEY1Token::presentation | KEY1Token::NS_URI_KEY :
    return makeContext<PresentationElement>(m_state);
  default:
    ETONYEK_DEBUG_MSG(("XMLDocument::element[KEY1Parser.cpp]: unexpected element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class DiscardContext : public KEY1XMLContextBase<IWORKDiscardContext>
{
public:
  explicit DiscardContext(KEY1ParserState &state);
  ~DiscardContext();

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  KEY1ParserState &m_state;
};

DiscardContext::DiscardContext(KEY1ParserState &state)
  : KEY1XMLContextBase<IWORKDiscardContext>(state)
  , m_state(state)
{
}

DiscardContext::~DiscardContext()
{
}

IWORKXMLContextPtr_t DiscardContext::element(const int name)
{
  return KEY1XMLContextBase<IWORKDiscardContext>::element(name);
}

}

KEY1Parser::KEY1Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, KEYCollector &collector, KEY1Dictionary &dict)
  : IWORKParser(input, package)
  , m_state(*this, collector, dict)
  , m_data()
{

}

KEY1Parser::~KEY1Parser()
{
}

IWORKXMLContextPtr_t KEY1Parser::createDocumentContext()
{
  return makeContext<XMLDocument>(m_state);
}

IWORKXMLContextPtr_t KEY1Parser::createDiscardContext()
{
  return makeContext<DiscardContext>(m_state);
}

const IWORKTokenizer &KEY1Parser::getTokenizer() const
{
  return KEY1Token::getTokenizer();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
