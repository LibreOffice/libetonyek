/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKPropertyMapElement.h"

#include <memory>

#include "libetonyek_xml.h"
#include "IWORKCollector.h"
#include "IWORKColorElement.h"
#include "IWORKCoreImageFilterDescriptorElement.h"
#include "IWORKDictionary.h"
#include "IWORKDirectCollector.h"
#include "IWORKFillElement.h"
#include "IWORKFormatElement.h"
#include "IWORKGeometryElement.h"
#include "IWORKLineElement.h"
#include "IWORKLineEndElement.h"
#include "IWORKListLabelGeometriesProperty.h"
#include "IWORKListLabelIndentsProperty.h"
#include "IWORKListLabelTypesProperty.h"
#include "IWORKListTextIndentsProperty.h"
#include "IWORKMutableArrayElement.h"
#include "IWORKNumericPropertyContext.h"
#include "IWORKProperties.h"
#include "IWORKPropertyContext.h"
#include "IWORKPropertyHandler.h"
#include "IWORKPropertyMap.h"
#include "IWORKPtrPropertyContext.h"
#include "IWORKRefContext.h"
#include "IWORKStringElement.h"
#include "IWORKStrokeContext.h"
#include "IWORKStyleContainer.h"
#include "IWORKStyleContext.h"
#include "IWORKStyleRefContext.h"
#include "IWORKTabsElement.h"
#include "IWORKToken.h"
#include "IWORKTokenizer.h"
#include "IWORKValueContext.h"
#include "IWORKWrapElement.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

using boost::none;
using boost::optional;

using std::deque;
using std::make_shared;
using std::string;

namespace
{
template<typename Property, typename Context>
class RedirectPropertyContext : public IWORKPropertyContextBase
{
public:
  RedirectPropertyContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
    : IWORKPropertyContextBase(state, propMap)
    , m_context(new Context(state, m_value))
    , m_value()
  {
  }

  virtual void attribute(int name, const char *value)
  {
    m_context->attribute(name,value);
  }
  virtual void startOfElement()
  {
    m_context->startOfElement();
  }
  virtual IWORKXMLContextPtr_t element(int name)
  {
    return m_context->element(name);
  }
  virtual void endOfElement()
  {
    m_context->endOfElement();
    if (m_value) m_propMap.put<Property>(get(m_value));
  }

private:

  std::shared_ptr<IWORKXMLContext> m_context;
  boost::optional<typename IWORKPropertyInfo<Property>::ValueType > m_value;
};

template<typename Property, typename Context, int TokenId, int RefTokenId>
class RefPropertyContext : public IWORKPropertyContextBase
{
  typedef typename IWORKPropertyInfo<Property>::ValueType RedirectType;
  typedef std::unordered_map<ID_t, RedirectType> RedirectMap_t;
public:
  RefPropertyContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap, RedirectMap_t &dataMap);

protected:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

protected:
  RedirectMap_t &m_dataMap;
  boost::optional<RedirectType> m_data;
  optional<ID_t> m_ref;
};

template<typename Property, typename Context, int TokenId, int RefTokenId>
RefPropertyContext<Property, Context, TokenId, RefTokenId>::RefPropertyContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap, RedirectMap_t &dataMap)
  : IWORKPropertyContextBase(state, propMap)
  , m_dataMap(dataMap)
  , m_data()
  , m_ref()
{
}

template<typename Property, typename Context, int TokenId, int RefTokenId>
IWORKXMLContextPtr_t RefPropertyContext<Property, Context, TokenId, RefTokenId>::element(const int name)
{
  switch (name)
  {
  case TokenId :
    return makeContext<Context>(getState(), m_data);
  case RefTokenId :
    return makeContext<IWORKRefContext>(getState(), m_ref);
  case IWORKToken::NS_URI_SF | IWORKToken::null:
    return IWORKXMLContextPtr_t();
  default:
    break;
  }
  if (name==(IWORKToken::NS_URI_SF | IWORKToken::frame))
  {
    // REMOVEME in graphicStyle sf:stroke can contain some frame element instead of stroke
    static bool first=true;
    if (first)
    {
      first=false;
      ETONYEK_DEBUG_MSG(("RefPropertyContext<...>::element: Oops, find some frame elements\n"));
    }
    return IWORKXMLContextPtr_t();
  }
  ETONYEK_DEBUG_MSG(("RefPropertyContext<...>::element: unknown element %d\n", name));
  return IWORKXMLContextPtr_t();
}

template<typename Property, typename Context, int TokenId, int RefTokenId>
void RefPropertyContext<Property, Context, TokenId, RefTokenId>::endOfElement()
{
  if (m_ref)
  {
    typename RedirectMap_t::const_iterator it = m_dataMap.find(get(m_ref));
    if (m_dataMap.end() != it)
      m_propMap.put<Property>(it->second);
    else if (!get(m_ref).empty())
    {
      ETONYEK_DEBUG_MSG(("RefPropertyContext<...>::endOfElement: unknown data %s\n", get(m_ref).c_str()));
    }
  }
  else if (m_data)
  {
    m_propMap.put<Property>(get(m_data));
  }
}

}

namespace
{
template<typename Property, int TokenId, int RefTokenId, int TokenId2=0, int RefTokenId2=0>
class StylePropertyContext : public IWORKStyleContainer<TokenId, RefTokenId, TokenId2, RefTokenId2>
{
public:
  StylePropertyContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap, IWORKStyleMap_t &styleMap);
  StylePropertyContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap, IWORKStyleMap_t &styleMap, IWORKStyleMap_t &styleMap2);

private:
  void endOfElement() override;

  IWORKStylePtr_t m_style;
  IWORKPropertyMap &m_propertyMap;
};

template<typename Property, int TokenId, int RefTokenId, int TokenId2, int RefTokenId2>
StylePropertyContext<Property, TokenId, RefTokenId, TokenId2, RefTokenId2>::StylePropertyContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap, IWORKStyleMap_t &styleMap)
  : IWORKStyleContainer<TokenId, RefTokenId, TokenId2, RefTokenId2>(state, m_style, styleMap)
  , m_style()
  , m_propertyMap(propMap)
{
}

template<typename Property, int TokenId, int RefTokenId, int TokenId2, int RefTokenId2>
StylePropertyContext<Property, TokenId, RefTokenId, TokenId2, RefTokenId2>::StylePropertyContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap, IWORKStyleMap_t &styleMap, IWORKStyleMap_t &styleMap2)
  : IWORKStyleContainer<TokenId, RefTokenId, TokenId2, RefTokenId2>(state, m_style, styleMap, styleMap2)
  , m_style()
  , m_propertyMap(propMap)
{
}

template<typename Property, int TokenId, int RefTokenId, int TokenId2, int RefTokenId2>
void StylePropertyContext<Property, TokenId, RefTokenId, TokenId2, RefTokenId2>::endOfElement()
{
  IWORKStyleContainer<TokenId, RefTokenId, TokenId2, RefTokenId2>::endOfElement();
  m_propertyMap.template put<Property>(m_style);
}

}

namespace
{

class TabsProperty : public IWORKPropertyContextBase
{
public:
  TabsProperty(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKTabStops_t m_tabs;
  optional<ID_t> m_ref;
};

TabsProperty::TabsProperty(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKPropertyContextBase(state, propMap)
  , m_tabs()
  , m_ref()
{
}

IWORKXMLContextPtr_t TabsProperty::element(const int name)
{
  m_default = false;

  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::tabs :
    return makeContext<IWORKTabsElement>(getState(), m_tabs);
  case IWORKToken::NS_URI_SF | IWORKToken::tabs_ref :
    return makeContext<IWORKRefContext>(getState(), m_ref);
  default:
    ETONYEK_DEBUG_MSG(("TabsProperty::element[IWORKPropertyMapElement.cpp]: find unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

void TabsProperty::endOfElement()
{
  if (!m_tabs.empty())
  {
    m_propMap.put<property::Tabs>(m_tabs);
  }
  else if (m_ref)
  {
    IWORKTabStopsMap_t::const_iterator it = getState().getDictionary().m_tabs.find(get(m_ref));
    if (getState().getDictionary().m_tabs.end() != it)
      m_propMap.put<property::Tabs>(it->second);
    else if (!get(m_ref).empty())
    {
      ETONYEK_DEBUG_MSG(("TabsProperty::endOfElement[IWORKPropertyMapElement.cpp]: unknown tabs %s\n", get(m_ref).c_str()));
    }
  }
  else if (m_default)
  {
    m_propMap.clear<property::Tabs>();
  }
}

}

namespace
{

class LinespacingElement : public IWORKXMLEmptyContextBase
{
public:
  LinespacingElement(IWORKXMLParserState &state, optional<IWORKLineSpacing> &value);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;

private:
  optional<IWORKLineSpacing> &m_value;
  optional<double> m_amount;
  optional<bool> m_relative;
};

LinespacingElement::LinespacingElement(IWORKXMLParserState &state, optional<IWORKLineSpacing> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
  , m_amount()
  , m_relative()
{
}

void LinespacingElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::amt :
    m_amount = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::mode :
    m_relative = IWORKToken::relative == getToken(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("Linespacing::attribute[IWORKPropertyMapElement.cpp]: find unknown attribute\n"));
  }
}

void LinespacingElement::endOfElement()
{
  if (m_amount)
  {
    m_value = IWORKLineSpacing(get(m_amount), get_optional_value_or(m_relative, false));
    if (getId())
      getState().getDictionary().m_lineSpacings.insert(IWORKLineSpacingMap_t::value_type(get(getId()),get(m_value)));
  }
}

}

namespace
{

class ColumnElement : public IWORKXMLEmptyContextBase
{
public:
  ColumnElement(IWORKXMLParserState &state, IWORKColumns::Column &value);

private:
  void attribute(int name, const char *value) override;

private:
  IWORKColumns::Column &m_value;
};

ColumnElement::ColumnElement(IWORKXMLParserState &state, IWORKColumns::Column &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
{
}

void ColumnElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::spacing :
    m_value.m_spacing = get_optional_value_or(try_double_cast(value), 0);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::width :
    m_value.m_width = get_optional_value_or(try_double_cast(value), 0);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::index : // column index, probably save to ignore
    break;
  default:
    ETONYEK_DEBUG_MSG(("ColumnElement::attribute[IWORKPropertyMapElement.cpp]: find unknown attribute\n"));
    break;
  }
}
}

namespace
{

class ColumnsElement : public IWORKXMLElementContextBase
{
public:
  ColumnsElement(IWORKXMLParserState &state, optional<IWORKColumns> &value);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  optional<IWORKColumns> &m_value;
};

ColumnsElement::ColumnsElement(IWORKXMLParserState &state, optional<IWORKColumns> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
{
  m_value=IWORKColumns();
}

void ColumnsElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::equal_columns :
    get(m_value).m_equal = get_optional_value_or(try_bool_cast(value), false);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("ColumnsElement::attribute[IWORKPropertyMapElement.cpp]: find unknown attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t ColumnsElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::column :
    get(m_value).m_columns.push_back(IWORKColumns::Column());
    return makeContext<ColumnElement>(getState(), get(m_value).m_columns.back());
  default:
    ETONYEK_DEBUG_MSG(("ColumnsElement::element[IWORKPropertyMapElement.cpp]: find unknown element\n"));
  }
  return IWORKXMLContextPtr_t();
}

void ColumnsElement::endOfElement()
{
  if (getId())
    getState().getDictionary().m_columnSets.insert(IWORKColumnsMap_t::value_type(get(getId()),get(m_value)));
}

}

namespace
{

class PaddingElement : public IWORKXMLEmptyContextBase
{
public:
  PaddingElement(IWORKXMLParserState &state, optional<IWORKPadding> &value);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;

private:
  optional<IWORKPadding> &m_value;
};

PaddingElement::PaddingElement(IWORKXMLParserState &state, optional<IWORKPadding> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
{
  m_value=IWORKPadding();
}

void PaddingElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::bottom :
    get(m_value).m_bottom = try_double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::left :
    get(m_value).m_left = try_double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::right :
    get(m_value).m_right = try_double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::top :
    get(m_value).m_top = try_double_cast(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("PaddingElement::attribute[IWORKPropertyMapElement.cpp]: find unknown attribute\n"));
    break;
  }
}

void PaddingElement::endOfElement()
{
  if (getId())
    getState().getDictionary().m_paddings.insert(IWORKPaddingMap_t::value_type(get(getId()),get(m_value)));
}

}

namespace
{

class LanguageElement : public IWORKPropertyContextBase
{
public:
  LanguageElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  optional<string> m_lang;
};

LanguageElement::LanguageElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKPropertyContextBase(state, propMap)
  , m_lang()
{
}

IWORKXMLContextPtr_t LanguageElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::string:
    return makeContext<IWORKStringElement>(getState(), m_lang);
  case IWORKToken::NS_URI_SF | IWORKToken::null:
    break;
  default:
    ETONYEK_DEBUG_MSG(("LanguageElement::element[IWORKPropertyMapElement.cpp]: find unknown element\n"));
    break;
  }
  return IWORKXMLContextPtr_t();
}

void LanguageElement::endOfElement()
{
  if (m_lang)
  {
    if (IWORKToken::__multilingual != getToken(get(m_lang).c_str()))
    {
      const string &tag = getState().m_langManager.addLocale(get(m_lang));
      if (tag.empty())
        m_propMap.clear<property::Language>();
      else
        m_propMap.put<property::Language>(tag);
    }
  }
  else if (m_default)
  {
    m_propMap.clear<property::Language>();
  }
}

}

namespace
{
/* a date time format is either store as a real date-format or a simple string.

    note: This is probably similar for duration */
template <typename Property>
class PropertyDateTimeFormatElement : public RefPropertyContext<Property, IWORKDateTimeFormatElement, IWORKToken::NS_URI_SF | IWORKToken::date_format, IWORKToken::NS_URI_SF | IWORKToken::date_format_ref>
{
  typedef RefPropertyContext<Property, IWORKDateTimeFormatElement, IWORKToken::NS_URI_SF | IWORKToken::date_format, IWORKToken::NS_URI_SF | IWORKToken::date_format_ref> Parent;
  typedef std::unordered_map<ID_t, IWORKDateTimeFormat> RedirectMap_t;

public:
  PropertyDateTimeFormatElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap, RedirectMap_t &dataMap)
    : Parent(state, propMap, dataMap)
    , m_string()
  {
  }
private:
  virtual IWORKXMLContextPtr_t element(int name)
  {
    if (name==(IWORKToken::NS_URI_SF | IWORKToken::string))
      return makeContext<IWORKStringElement>(Parent::getState(), m_string);
    else
      return Parent::element(name);
  }
  virtual void endOfElement()
  {
    if (m_string)
    {
      m_data=IWORKDateTimeFormat();
      get(m_data).m_format=get(m_string);
    }
    Parent::endOfElement();
  }
  using Parent::m_data;
  optional<std::string> m_string;
};
}

namespace
{

class OverridesElement : public IWORKXMLElementContextBase
{
public:
  OverridesElement(IWORKXMLParserState &state, IWORKShadow &value);

private:
  typedef IWORKValueContext<double, IWORKNumberElement<double>, IWORKToken::NS_URI_SF | IWORKToken::number> NumberProperty;
  typedef IWORKValueContext<IWORKColor, IWORKColorElement, IWORKToken::NS_URI_SF | IWORKToken::color> ColorProperty;

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKShadow &m_value;
};

OverridesElement::OverridesElement(IWORKXMLParserState &state, IWORKShadow &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
{
}

void OverridesElement::attribute(int name, const char *value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("OverridesElement::attribute[IWORKPropertyMapElement.cpp]: find unknown attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t OverridesElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::inputAngle :
    return makeContext<NumberProperty>(getState(), m_value.m_angle);
  case IWORKToken::NS_URI_SF | IWORKToken::inputColor :
    return makeContext<ColorProperty>(getState(), m_value.m_color);
  case IWORKToken::NS_URI_SF | IWORKToken::inputDistance :
    return makeContext<NumberProperty>(getState(), m_value.m_offset);
  case IWORKToken::NS_URI_SF | IWORKToken::inputGlossiness :
    return makeContext<NumberProperty>(getState(), m_value.m_glossiness);
  case IWORKToken::NS_URI_SF | IWORKToken::inputOpacity :
    return makeContext<NumberProperty>(getState(), m_value.m_opacity);
  case IWORKToken::NS_URI_SF | IWORKToken::inputRadius :
    return makeContext<NumberProperty>(getState(), m_value.m_radius);
  default:
    ETONYEK_DEBUG_MSG(("OverridesElement::element[IWORKPropertyMapElement.cpp]: find unknown element\n"));
  }
  return IWORKXMLContextPtr_t();
}

void OverridesElement::endOfElement()
{
  if (getId())
    getState().getDictionary().m_shadows.insert(IWORKShadowMap_t::value_type(get(getId()),m_value));
}
}

namespace
{

class CoreImageFilterInfoElement : public IWORKXMLElementContextBase
{
public:
  CoreImageFilterInfoElement(IWORKXMLParserState &state, deque<IWORKShadow> &elements);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  deque<IWORKShadow> &m_elements;
  bool m_isShadow;
  optional<ID_t> m_descriptorRef;
  optional<ID_t> m_overridesRef;
  IWORKShadow m_value;
};

CoreImageFilterInfoElement::CoreImageFilterInfoElement(IWORKXMLParserState &state, deque<IWORKShadow> &elements)
  : IWORKXMLElementContextBase(state)
  , m_elements(elements)
  , m_isShadow(false)
  , m_descriptorRef()
  , m_overridesRef()
  , m_value()
{
}

void CoreImageFilterInfoElement::attribute(int name, const char *value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  case IWORKToken::filter_visibility: // USEME? a bool
    break;
  default:
    ETONYEK_DEBUG_MSG(("CoreImageFilterInfoElement::attribute[IWORKPropertyMapElement.cpp]: find unknown attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t CoreImageFilterInfoElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::core_image_filter_descriptor :
    return makeContext<IWORKCoreImageFilterDescriptorElement>(getState(), m_isShadow);
  case IWORKToken::NS_URI_SF | IWORKToken::core_image_filter_descriptor_ref :
    return makeContext<IWORKRefContext>(getState(), m_descriptorRef);
  case IWORKToken::NS_URI_SF | IWORKToken::overrides :
    return makeContext<OverridesElement>(getState(), m_value);
  case IWORKToken::NS_URI_SF | IWORKToken::overrides_ref :
    return makeContext<IWORKRefContext>(getState(), m_overridesRef);
  default:
    ETONYEK_DEBUG_MSG(("CoreImageFilterInfoElement::element[IWORKPropertyMapElement.cpp]: find unknown element\n"));
  }
  return IWORKXMLContextPtr_t();
}

void CoreImageFilterInfoElement::endOfElement()
{
  if (m_descriptorRef)
  {
    const IWORKFilterDescriptorMap_t::const_iterator it = getState().getDictionary().m_filterDescriptors.find(get(m_descriptorRef));
    if (it != getState().getDictionary().m_filterDescriptors.end())
      m_isShadow = it->second.m_isShadow;
    else if (!get(m_descriptorRef).empty())
    {
      ETONYEK_DEBUG_MSG(("CoreImageFilterInfoElement::endOfElement[IWORKPropertyMapElement.cpp]: unknown descriptor %s\n", get(m_descriptorRef).c_str()));
    }
  }
  if (m_overridesRef)
  {
    const IWORKShadowMap_t::const_iterator it = getState().getDictionary().m_shadows.find(get(m_overridesRef));
    if (it != getState().getDictionary().m_shadows.end())
      m_value = it->second;
    else if (!get(m_overridesRef).empty())
    {
      ETONYEK_DEBUG_MSG(("CoreImageFilterInfoElement::endOfElement[IWORKPropertyMapElement.cpp]: unknown overrides %s\n", get(m_overridesRef).c_str()));
    }
  }
  if (m_isShadow)
  {
    m_elements.push_back(m_value);
    if (getId())
      getState().getDictionary().m_coreImageFilterInfos.insert(IWORKShadowMap_t::value_type(get(getId()),m_value));
  }
}

}

namespace
{

class FiltersElement : public IWORKXMLElementContextBase
{
  typedef IWORKMutableArrayElement<IWORKShadow, CoreImageFilterInfoElement, IWORKDirectCollector, IWORKToken::NS_URI_SF | IWORKToken::core_image_filter_info, IWORKToken::NS_URI_SF | IWORKToken::core_image_filter_info_ref> MutableArrayElement;

public:
  FiltersElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKPropertyMap &m_propMap;
  deque<IWORKShadow> m_elements;
  optional<ID_t> m_ref;
  IWORKStylePtr_t m_layout;
};

FiltersElement::FiltersElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKXMLElementContextBase(state)
  , m_propMap(propMap)
  , m_elements()
  , m_ref()
  , m_layout()
{
}

typedef IWORKStyleContainer<IWORKToken::NS_URI_SF | IWORKToken::layoutstyle, IWORKToken::NS_URI_SF | IWORKToken::layoutstyle_ref> FiltersLayoutStyle;

IWORKXMLContextPtr_t FiltersElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::mutable_array:
  case IWORKToken::NS_URI_SF | IWORKToken::array:
    return makeContext<MutableArrayElement>(getState(), getState().getDictionary().m_filters, getState().getDictionary().m_coreImageFilterInfos, m_elements);
  case IWORKToken::NS_URI_SF | IWORKToken::mutable_array_ref:
  case IWORKToken::NS_URI_SF | IWORKToken::array_ref:
    return makeContext<IWORKRefContext>(getState(), m_ref);
  case IWORKToken::NS_URI_SF | IWORKToken::layoutStyle: // useme
    return makeContext<FiltersLayoutStyle>(getState(), m_layout, getState().getDictionary().m_layoutStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::null:
    break;
  default:
    ETONYEK_DEBUG_MSG(("FiltersElement::element[FiltersElements.cpp]: find unknown element\n"));
    break;
  }
  return IWORKXMLContextPtr_t();
}

void FiltersElement::endOfElement()
{
  if (m_ref)
  {
    const IWORKFiltersMap_t::const_iterator it = getState().getDictionary().m_filters.find(get(m_ref));
    if (it != getState().getDictionary().m_filters.end())
      m_elements = it->second;
    else if (!get(m_ref).empty())
    {
      ETONYEK_DEBUG_MSG(("FiltersElement::endOfElement[IWORKPropertyMapElement.cpp]: unknown overrides %s\n", get(m_ref).c_str()));
    }
  }
  if (m_elements.empty())
    m_propMap.clear<property::Shadow>();
  else
    m_propMap.put<property::Shadow>(m_elements.back());
}

}

namespace
{

typedef RefPropertyContext<property::Columns, ColumnsElement, IWORKToken::NS_URI_SF | IWORKToken::columns, IWORKToken::NS_URI_SF | IWORKToken::columns_ref> ColumnsPropertyElement;
typedef IWORKPropertyContext<property::ExternalTextWrap, IWORKExternalTextWrapElement, IWORKToken::NS_URI_SF | IWORKToken::external_text_wrap> ExternalTextWrapElement;
typedef RedirectPropertyContext<property::Fill, IWORKFillElement> FillPropertyElement;
typedef IWORKPropertyContext<property::FontColor, IWORKColorElement, IWORKToken::NS_URI_SF | IWORKToken::color> FontColorElement;
typedef IWORKPropertyContext<property::FontName, IWORKStringElement, IWORKToken::NS_URI_SF | IWORKToken::string> FontNameElement;
typedef IWORKPropertyContext<property::HeadLineEnd, IWORKLineEndElement, IWORKToken::NS_URI_SF | IWORKToken::line_end> HeadLineEndElement;
typedef RefPropertyContext<property::LayoutMargins, PaddingElement, IWORKToken::NS_URI_SF | IWORKToken::padding, IWORKToken::NS_URI_SF | IWORKToken::padding_ref> LayoutMarginsElement;
typedef RefPropertyContext<property::LineSpacing, LinespacingElement, IWORKToken::NS_URI_SF | IWORKToken::linespacing, IWORKToken::NS_URI_SF | IWORKToken::linespacing_ref> LineSpacingElement;
typedef RefPropertyContext<property::Padding, PaddingElement, IWORKToken::NS_URI_SF | IWORKToken::padding, IWORKToken::NS_URI_SF | IWORKToken::padding_ref> PaddingContext;
typedef IWORKPropertyContext<property::ParagraphFill, IWORKColorElement, IWORKToken::NS_URI_SF | IWORKToken::color> ParagraphFillElement;
typedef RedirectPropertyContext<property::ParagraphStroke, IWORKStrokeContext> ParagraphStrokeElement;
typedef IWORKPropertyContext<property::TailLineEnd, IWORKLineEndElement, IWORKToken::NS_URI_SF | IWORKToken::line_end> TailLineEndElement;
typedef PropertyDateTimeFormatElement<property::SFTCellStylePropertyDateTimeFormat> SFTCellStylePropertyDateTimeFormatElement;
typedef RefPropertyContext<property::SFTCellStylePropertyDurationFormat, IWORKDurationFormatElement, IWORKToken::NS_URI_SF | IWORKToken::duration_format, IWORKToken::NS_URI_SF | IWORKToken::duration_format_ref> SFTCellStylePropertyDurationFormatElement;
typedef RefPropertyContext<property::SFTCellStylePropertyNumberFormat, IWORKNumberFormatElement, IWORKToken::NS_URI_SF | IWORKToken::number_format, IWORKToken::NS_URI_SF | IWORKToken::number_format_ref> SFTCellStylePropertyNumberFormatElement;
typedef RedirectPropertyContext<property::SFTStrokeProperty, IWORKStrokeContext> SFTStrokePropertyElement;
typedef RedirectPropertyContext<property::Stroke, IWORKStrokeContext> StrokePropertyElement;
typedef IWORKPropertyContext<property::TextBackground, IWORKColorElement, IWORKToken::NS_URI_SF | IWORKToken::color> TextBackgroundElement;

typedef IWORKPtrPropertyContext<property::Geometry, IWORKGeometryElement, IWORKToken::NS_URI_SF | IWORKToken::geometry> GeometryElement;

typedef StylePropertyContext<property::FollowingLayoutStyle, IWORKToken::NS_URI_SF | IWORKToken::layoutstyle, IWORKToken::NS_URI_SF | IWORKToken::layoutstyle_ref> FollowingLayoutStyleElement;
typedef StylePropertyContext<property::FollowingParagraphStyle, IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle, IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle_ref> FollowingParagraphStyleElement;
typedef StylePropertyContext<property::LabelCharacterStyle, IWORKToken::NS_URI_SF | IWORKToken::characterstyle, IWORKToken::NS_URI_SF | IWORKToken::characterstyle_ref> LabelCharacterStyleElement;
typedef StylePropertyContext<property::LayoutParagraphStyle, IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle, IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle_ref> LayoutParagraphStyleElement;
typedef StylePropertyContext<property::LayoutStyle, IWORKToken::NS_URI_SF | IWORKToken::layoutstyle, IWORKToken::NS_URI_SF | IWORKToken::layoutstyle_ref> LayoutStyleElement;
typedef StylePropertyContext<property::ListStyle, IWORKToken::NS_URI_SF | IWORKToken::liststyle, IWORKToken::NS_URI_SF | IWORKToken::liststyle_ref> ListStyleElement;
// in Pages, followed by a tocstyle, in Number by a paragraph-style ?
typedef StylePropertyContext<property::TocStyle, IWORKToken::NS_URI_SF | IWORKToken::tocstyle, IWORKToken::NS_URI_SF | IWORKToken::tocstyle_ref
, IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle, IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle_ref> TocStyleElement;
typedef StylePropertyContext<property::SFSeries, IWORKToken::NS_URI_SF | IWORKToken::chart_series_style, IWORKToken::NS_URI_SF | IWORKToken::chart_series_style_ref> SFSeriesElement;
typedef RedirectPropertyContext<property::SFC2DAreaFillProperty, IWORKFillElement> SFC2DAreaFillPropertyElement;
typedef RedirectPropertyContext<property::SFC2DColumnFillProperty, IWORKFillElement> SFC2DColumnFillPropertyElement;
typedef RedirectPropertyContext<property::SFC2DMixedColumnFillProperty, IWORKFillElement> SFC2DMixedColumnFillPropertyElement;
typedef RedirectPropertyContext<property::SFC2DPieFillProperty, IWORKFillElement> SFC2DPieFillPropertyElement;
typedef RedirectPropertyContext<property::SFC3DAreaFillProperty, IWORKFillElement> SFC3DAreaFillPropertyElement;
typedef RedirectPropertyContext<property::SFC3DColumnFillProperty, IWORKFillElement> SFC3DColumnFillPropertyElement;
typedef RedirectPropertyContext<property::SFC3DPieFillProperty, IWORKFillElement> SFC3DPieFillPropertyElement;
typedef RedirectPropertyContext<property::SFTableCellStylePropertyFill, IWORKFillElement> SFTableCellStylePropertyFillElement;
typedef StylePropertyContext<property::SFTableStylePropertyCellStyle, IWORKToken::NS_URI_SF | IWORKToken::table_cell_style, IWORKToken::NS_URI_SF | IWORKToken::table_cell_style_ref> SFTableStylePropertyCellStyleElement;
typedef StylePropertyContext<property::SFTableStylePropertyHeaderColumnCellStyle, IWORKToken::NS_URI_SF | IWORKToken::table_cell_style, IWORKToken::NS_URI_SF | IWORKToken::table_cell_style_ref> SFTableStylePropertyHeaderColumnCellStyleElement;
typedef StylePropertyContext<property::SFTableStylePropertyHeaderRowCellStyle, IWORKToken::NS_URI_SF | IWORKToken::table_cell_style, IWORKToken::NS_URI_SF | IWORKToken::table_cell_style_ref> SFTableStylePropertyHeaderRowCellStyleElement;
typedef StylePropertyContext<property::SFTCellStylePropertyLayoutStyle, IWORKToken::NS_URI_SF | IWORKToken::layoutstyle, IWORKToken::NS_URI_SF | IWORKToken::layoutstyle_ref> SFTCellStylePropertyLayoutStylePropertyElement;
typedef StylePropertyContext<property::SFTCellStylePropertyParagraphStyle, IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle, IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle_ref> SFTCellStylePropertyParagraphStylePropertyElement;
typedef StylePropertyContext<property::SFTDefaultBodyCellStyleProperty, IWORKToken::NS_URI_SF | IWORKToken::cell_style, IWORKToken::NS_URI_SF | IWORKToken::cell_style_ref> SFTDefaultBodyCellStylePropertyElement;
typedef StylePropertyContext<property::SFTDefaultBodyVectorStyleProperty, IWORKToken::NS_URI_SF | IWORKToken::vector_style, IWORKToken::NS_URI_SF | IWORKToken::vector_style_ref> SFTDefaultBodyVectorStylePropertyElement;
typedef StylePropertyContext<property::SFTDefaultBorderVectorStyleProperty, IWORKToken::NS_URI_SF | IWORKToken::vector_style, IWORKToken::NS_URI_SF | IWORKToken::vector_style_ref> SFTDefaultBorderVectorStylePropertyElement;
typedef StylePropertyContext<property::SFTDefaultFooterBodyVectorStyleProperty, IWORKToken::NS_URI_SF | IWORKToken::vector_style, IWORKToken::NS_URI_SF | IWORKToken::vector_style_ref> SFTDefaultFooterBodyVectorStylePropertyElement;
typedef StylePropertyContext<property::SFTDefaultFooterRowCellStyleProperty, IWORKToken::NS_URI_SF | IWORKToken::cell_style, IWORKToken::NS_URI_SF | IWORKToken::cell_style_ref> SFTDefaultFooterRowCellStylePropertyElement;
typedef StylePropertyContext<property::SFTDefaultFooterSeparatorVectorStyleProperty, IWORKToken::NS_URI_SF | IWORKToken::vector_style, IWORKToken::NS_URI_SF | IWORKToken::vector_style_ref> SFTDefaultFooterSeparatorVectorStylePropertyElement;
typedef StylePropertyContext<property::SFTDefaultGroupingLevelVectorStyleProperty, IWORKToken::NS_URI_SF | IWORKToken::vector_style, IWORKToken::NS_URI_SF | IWORKToken::vector_style_ref> SFTDefaultGroupingLevelVectorStylePropertyElement;
typedef StylePropertyContext<property::SFTDefaultGroupingRowCellStyleProperty, IWORKToken::NS_URI_SF | IWORKToken::cell_style, IWORKToken::NS_URI_SF | IWORKToken::cell_style_ref> SFTDefaultGroupingRowCellStylePropertyElement;
typedef StylePropertyContext<property::SFTDefaultHeaderBodyVectorStyleProperty, IWORKToken::NS_URI_SF | IWORKToken::vector_style, IWORKToken::NS_URI_SF | IWORKToken::vector_style_ref> SFTDefaultHeaderBodyVectorStylePropertyElement;
typedef StylePropertyContext<property::SFTDefaultHeaderColumnCellStyleProperty, IWORKToken::NS_URI_SF | IWORKToken::cell_style, IWORKToken::NS_URI_SF | IWORKToken::cell_style_ref> SFTDefaultHeaderColumnCellStylePropertyElement;
typedef StylePropertyContext<property::SFTDefaultHeaderRowCellStyleProperty, IWORKToken::NS_URI_SF | IWORKToken::cell_style, IWORKToken::NS_URI_SF | IWORKToken::cell_style_ref> SFTDefaultHeaderRowCellStylePropertyElement;
typedef StylePropertyContext<property::SFTDefaultHeaderSeparatorVectorStyleProperty, IWORKToken::NS_URI_SF | IWORKToken::vector_style, IWORKToken::NS_URI_SF | IWORKToken::vector_style_ref> SFTDefaultHeaderSeparatorVectorStylePropertyElement;
typedef StylePropertyContext<property::SFTTableNameStylePropertyLayoutStyle, IWORKToken::NS_URI_SF | IWORKToken::layoutstyle, IWORKToken::NS_URI_SF | IWORKToken::layoutstyle_ref> SFTTableNameStylePropertyLayoutStyleElement;
typedef StylePropertyContext<property::SFTTableNameStylePropertyParagraphStyle, IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle, IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle_ref> SFTTableNameStylePropertyParagraphStyleElement;

typedef IWORKNumericPropertyContext<property::Alignment> AlignmentElement;
typedef IWORKNumericPropertyContext<property::Baseline> SuperscriptElement;
typedef IWORKNumericPropertyContext<property::BaselineShift> BaselineShiftElement;
typedef IWORKNumericPropertyContext<property::Bold> BoldElement;
typedef IWORKNumericPropertyContext<property::Capitalization> CapitalizationElement;
typedef IWORKNumericPropertyContext<property::FirstLineIndent> FirstLineIndentElement;
typedef IWORKNumericPropertyContext<property::FontSize> FontSizeElement;
typedef IWORKNumericPropertyContext<property::Italic> ItalicElement;
typedef IWORKNumericPropertyContext<property::KeepLinesTogether> KeepLinesTogetherElement;
typedef IWORKNumericPropertyContext<property::KeepWithNext> KeepWithNextElement;
typedef IWORKNumericPropertyContext<property::LeftIndent> LeftIndentElement;
typedef IWORKNumericPropertyContext<property::Opacity> OpacityElement;
typedef IWORKNumericPropertyContext<property::Outline> OutlineElement;
typedef IWORKNumericPropertyContext<property::PageBreakBefore> PageBreakBeforeElement;
typedef IWORKNumericPropertyContext<property::ParagraphBorderType> ParagraphBorderTypeElement;
typedef IWORKNumericPropertyContext<property::RightIndent> RightIndentElement;
typedef IWORKNumericPropertyContext<property::SFTHeaderColumnRepeatsProperty> SFTHeaderColumnRepeatsPropertyElement;
typedef IWORKNumericPropertyContext<property::SFTHeaderRowRepeatsProperty> SFTHeaderRowRepeatsPropertyElement;
typedef IWORKNumericPropertyContext<property::SFTTableBandedRowsProperty> SFTTableBandedRowsPropertyElement;
typedef IWORKNumericPropertyContext<property::SpaceAfter> SpaceAfterElement;
typedef IWORKNumericPropertyContext<property::SpaceBefore> SpaceBeforeElement;
typedef IWORKNumericPropertyContext<property::Strikethru> StrikethruElement;
typedef IWORKNumericPropertyContext<property::Tracking> TrackingElement;
typedef IWORKNumericPropertyContext<property::Underline> UnderlineElement;
typedef IWORKNumericPropertyContext<property::VerticalAlignment> VerticalAlignmentElement;
typedef IWORKNumericPropertyContext<property::WidowControl> WidowControlElement;

}

IWORKPropertyMapElement::IWORKPropertyMapElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKXMLElementContextBase(state)
  , m_propMap(&propMap)
  , m_propHandler(nullptr)
{
}

IWORKPropertyMapElement::IWORKPropertyMapElement(IWORKXMLParserState &state, IWORKPropertyHandler &propHandler)
  : IWORKXMLElementContextBase(state)
  , m_propMap(nullptr)
  , m_propHandler(&propHandler)
{
}

IWORKXMLContextPtr_t IWORKPropertyMapElement::element(const int name)
{
  if (m_propHandler)
    return m_propHandler->handle(name);

  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::alignment :
    return makeContext<AlignmentElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::baselineShift :
    return makeContext<BaselineShiftElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::bold :
    return makeContext<BoldElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::capitalization :
    return makeContext<CapitalizationElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::columns :
    return makeContext<ColumnsPropertyElement>(getState(), *m_propMap, getState().getDictionary().m_columnSets);
  case IWORKToken::NS_URI_SF | IWORKToken::externalTextWrap:
    return makeContext<ExternalTextWrapElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::fill :
    return makeContext<FillPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::filters :
    return makeContext<FiltersElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::firstLineIndent :
    return makeContext<FirstLineIndentElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::followingLayoutStyle :
    return makeContext<FollowingLayoutStyleElement>(getState(), *m_propMap, getState().getDictionary().m_layoutStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::followingParagraphStyle :
    return makeContext<FollowingParagraphStyleElement>(getState(), *m_propMap, getState().getDictionary().m_paragraphStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::fontColor :
    return makeContext<FontColorElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::fontName :
    return makeContext<FontNameElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::fontSize :
    return makeContext<FontSizeElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<GeometryElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::headLineEnd :
    return makeContext<HeadLineEndElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::italic :
    return makeContext<ItalicElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::keepLinesTogether :
    return makeContext<KeepLinesTogetherElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::keepWithNext :
    return makeContext<KeepWithNextElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::labelCharacterStyle1 :
  case IWORKToken::NS_URI_SF | IWORKToken::labelCharacterStyle2 :
  case IWORKToken::NS_URI_SF | IWORKToken::labelCharacterStyle3 :
  case IWORKToken::NS_URI_SF | IWORKToken::labelCharacterStyle4 :
  case IWORKToken::NS_URI_SF | IWORKToken::labelCharacterStyle5 :
  case IWORKToken::NS_URI_SF | IWORKToken::labelCharacterStyle6 :
  case IWORKToken::NS_URI_SF | IWORKToken::labelCharacterStyle7 :
  case IWORKToken::NS_URI_SF | IWORKToken::labelCharacterStyle8 :
  case IWORKToken::NS_URI_SF | IWORKToken::labelCharacterStyle9 :
    // CHANGE: this must be used to retrieve some Wingdings bullet character
    return makeContext<LabelCharacterStyleElement>(getState(), *m_propMap, getState().getDictionary().m_characterStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::language :
    return makeContext<LanguageElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::layoutMargins :
    return makeContext<LayoutMarginsElement>(getState(), *m_propMap, getState().getDictionary().m_paddings);
  case IWORKToken::NS_URI_SF | IWORKToken::layoutParagraphStyle :
    return makeContext<LayoutParagraphStyleElement>(getState(), *m_propMap, getState().getDictionary().m_paragraphStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::layoutStyle :
    return makeContext<LayoutStyleElement>(getState(), *m_propMap, getState().getDictionary().m_layoutStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::leftIndent :
    return makeContext<LeftIndentElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::lineSpacing :
    return makeContext<LineSpacingElement>(getState(), *m_propMap, getState().getDictionary().m_lineSpacings);
  case IWORKToken::NS_URI_SF | IWORKToken::listLabelGeometries :
    return makeContext<IWORKListLabelGeometriesProperty>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::listLabelIndents :
    return makeContext<IWORKListLabelIndentsProperty>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::listLabelTypes :
    return makeContext<IWORKListLabelTypesProperty>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::listStyle :
    return makeContext<ListStyleElement>(getState(), *m_propMap, getState().getDictionary().m_listStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::listTextIndents :
    return makeContext<IWORKListTextIndentsProperty>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::opacity :
    return makeContext<OpacityElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::outline :
    return makeContext<OutlineElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::padding :
    return makeContext<PaddingContext>(getState(), *m_propMap, getState().getDictionary().m_paddings);
  case IWORKToken::NS_URI_SF | IWORKToken::pageBreakBefore :
    return makeContext<PageBreakBeforeElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphBorderType :
    return makeContext<ParagraphBorderTypeElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphFill :
    return makeContext<ParagraphFillElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphStroke :
    return makeContext<ParagraphStrokeElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::rightIndent :
    return makeContext<RightIndentElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::tailLineEnd :
    return makeContext<TailLineEndElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::Series_0 :
  case IWORKToken::NS_URI_SF | IWORKToken::Series_1 :
  case IWORKToken::NS_URI_SF | IWORKToken::Series_2 :
  case IWORKToken::NS_URI_SF | IWORKToken::Series_3 :
  case IWORKToken::NS_URI_SF | IWORKToken::Series_4 :
  case IWORKToken::NS_URI_SF | IWORKToken::Series_5 :
  case IWORKToken::NS_URI_SF | IWORKToken::Series_6 :
  case IWORKToken::NS_URI_SF | IWORKToken::Series_7 :
    // CHANGEME
    return makeContext<SFSeriesElement>(getState(), *m_propMap, getState().getDictionary().m_chartSeriesStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFC2DAreaFillProperty :
    return makeContext<SFC2DAreaFillPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFC2DColumnFillProperty :
    return makeContext<SFC2DColumnFillPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFC2DMixedColumnFillProperty :
    return makeContext<SFC2DMixedColumnFillPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFC2DPieFillProperty :
    return makeContext<SFC2DPieFillPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFC3DAreaFillProperty :
    return makeContext<SFC3DAreaFillPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFC3DColumnFillProperty :
    return makeContext<SFC3DColumnFillPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFC3DPieFillProperty :
    return makeContext<SFC2DPieFillPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableCellStylePropertyFill :
    return makeContext<SFTableCellStylePropertyFillElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyCellStyle :
    return makeContext<SFTableStylePropertyCellStyleElement>(getState(), *m_propMap, getState().getDictionary().m_tableCellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderColumnCellStyle :
    return makeContext<SFTableStylePropertyHeaderColumnCellStyleElement>(getState(), *m_propMap, getState().getDictionary().m_tableCellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderRowCellStyle :
    return makeContext<SFTableStylePropertyHeaderRowCellStyleElement>(getState(), *m_propMap, getState().getDictionary().m_tableCellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyNumberFormat :
    return makeContext<SFTCellStylePropertyNumberFormatElement>(getState(), *m_propMap, getState().getDictionary().m_numberFormats);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyDateTimeFormat :
    return makeContext<SFTCellStylePropertyDateTimeFormatElement>(getState(), *m_propMap, getState().getDictionary().m_dateTimeFormats);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyDurationFormat :
    return makeContext<SFTCellStylePropertyDurationFormatElement>(getState(), *m_propMap, getState().getDictionary().m_durationFormats);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyLayoutStyle :
    return makeContext<SFTCellStylePropertyLayoutStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_layoutStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyParagraphStyle :
    return makeContext<SFTCellStylePropertyParagraphStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_paragraphStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultBodyCellStyleProperty :
    return makeContext<SFTDefaultBodyCellStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultBodyVectorStyleProperty :
    return makeContext<SFTDefaultBodyVectorStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_vectorStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultBorderVectorStyleProperty :
    return makeContext<SFTDefaultBorderVectorStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_vectorStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultFooterBodyVectorStyleProperty :
    return makeContext<SFTDefaultFooterBodyVectorStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_vectorStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultFooterRowCellStyleProperty :
    return makeContext<SFTDefaultFooterRowCellStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultFooterSeparatorVectorStyleProperty :
    return makeContext<SFTDefaultFooterSeparatorVectorStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_vectorStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingLevel0VectorStyleProperty:
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingLevel1VectorStyleProperty:
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingLevel2VectorStyleProperty:
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingLevel3VectorStyleProperty:
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingLevel4VectorStyleProperty:
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingLevel5VectorStyleProperty:
    return makeContext<SFTDefaultGroupingLevelVectorStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_vectorStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingRowCell0StyleProperty :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingRowCell1StyleProperty :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingRowCell2StyleProperty :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingRowCell3StyleProperty :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingRowCell4StyleProperty :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingRowCell5StyleProperty :
    return makeContext<SFTDefaultGroupingRowCellStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultHeaderBodyVectorStyleProperty :
    return makeContext<SFTDefaultHeaderBodyVectorStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_vectorStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultHeaderColumnCellStyleProperty :
    return makeContext<SFTDefaultHeaderColumnCellStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultHeaderRowCellStyleProperty :
    return makeContext<SFTDefaultHeaderRowCellStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultHeaderSeparatorVectorStyleProperty :
    return makeContext<SFTDefaultHeaderSeparatorVectorStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_vectorStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTTableNameStylePropertyLayoutStyle :
    return makeContext<SFTTableNameStylePropertyLayoutStyleElement>(getState(), *m_propMap, getState().getDictionary().m_layoutStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTTableNameStylePropertyParagraphStyle :
    return makeContext<SFTTableNameStylePropertyParagraphStyleElement>(getState(), *m_propMap, getState().getDictionary().m_paragraphStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTHeaderColumnRepeatsProperty :
    return makeContext<SFTHeaderColumnRepeatsPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTHeaderRowRepeatsProperty :
    return makeContext<SFTHeaderRowRepeatsPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTStrokeProperty :
    return makeContext<SFTStrokePropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTTableBandedRowsProperty :
    return makeContext<SFTTableBandedRowsPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::spaceAfter :
    return makeContext<SpaceAfterElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::spaceBefore :
    return makeContext<SpaceBeforeElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::strikethru :
    return makeContext<StrikethruElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::stroke :
    return makeContext<StrokePropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::superscript :
    return makeContext<SuperscriptElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::tabs :
    return makeContext<TabsProperty>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::textBackground :
    return makeContext<TextBackgroundElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::tocStyle :
    return makeContext<TocStyleElement>(getState(), *m_propMap, getState().getDictionary().m_tocStyles, getState().getDictionary().m_paragraphStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::tracking :
    return makeContext<TrackingElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::underline :
    return makeContext<UnderlineElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::verticalAlignment :
    return makeContext<VerticalAlignmentElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::widowControl :
    return makeContext<WidowControlElement>(getState(), *m_propMap);
  }

  return IWORKXMLContextPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
