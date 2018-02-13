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

using boost::optional;

using std::deque;
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

  void attribute(int name, const char *value) override
  {
    m_context->attribute(name,value);
  }
  void startOfElement() override
  {
    m_context->startOfElement();
  }
  IWORKXMLContextPtr_t element(int name) override
  {
    return m_context->element(name);
  }
  void endOfElement() override
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
    return std::make_shared<Context>(getState(), m_data);
  case RefTokenId :
    return std::make_shared<IWORKRefContext>(getState(), m_ref);
  case IWORKToken::NS_URI_SF | IWORKToken::null:
    return IWORKXMLContextPtr_t();
  default:
    break;
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
    return std::make_shared<IWORKTabsElement>(getState(), m_tabs);
  case IWORKToken::NS_URI_SF | IWORKToken::tabs_ref :
    return std::make_shared<IWORKRefContext>(getState(), m_ref);
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
    return std::make_shared<ColumnElement>(getState(), get(m_value).m_columns.back());
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
    return std::make_shared<IWORKStringElement>(getState(), m_lang);
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
  IWORKXMLContextPtr_t element(int name) override
  {
    if (name==(IWORKToken::NS_URI_SF | IWORKToken::string))
      return std::make_shared<IWORKStringElement>(Parent::getState(), m_string);
    else
      return Parent::element(name);
  }
  void endOfElement() override
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
    return std::make_shared<NumberProperty>(getState(), m_value.m_angle);
  case IWORKToken::NS_URI_SF | IWORKToken::inputColor :
    return std::make_shared<ColorProperty>(getState(), m_value.m_color);
  case IWORKToken::NS_URI_SF | IWORKToken::inputDistance :
    return std::make_shared<NumberProperty>(getState(), m_value.m_offset);
  case IWORKToken::NS_URI_SF | IWORKToken::inputGlossiness :
    return std::make_shared<NumberProperty>(getState(), m_value.m_glossiness);
  case IWORKToken::NS_URI_SF | IWORKToken::inputOpacity :
    return std::make_shared<NumberProperty>(getState(), m_value.m_opacity);
  case IWORKToken::NS_URI_SF | IWORKToken::inputRadius :
    return std::make_shared<NumberProperty>(getState(), m_value.m_radius);
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
    return std::make_shared<IWORKCoreImageFilterDescriptorElement>(getState(), m_isShadow);
  case IWORKToken::NS_URI_SF | IWORKToken::core_image_filter_descriptor_ref :
    return std::make_shared<IWORKRefContext>(getState(), m_descriptorRef);
  case IWORKToken::NS_URI_SF | IWORKToken::overrides :
    return std::make_shared<OverridesElement>(getState(), m_value);
  case IWORKToken::NS_URI_SF | IWORKToken::overrides_ref :
    return std::make_shared<IWORKRefContext>(getState(), m_overridesRef);
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
    return std::make_shared<MutableArrayElement>(getState(), getState().getDictionary().m_filters, getState().getDictionary().m_coreImageFilterInfos, m_elements);
  case IWORKToken::NS_URI_SF | IWORKToken::mutable_array_ref:
  case IWORKToken::NS_URI_SF | IWORKToken::array_ref:
    return std::make_shared<IWORKRefContext>(getState(), m_ref);
  case IWORKToken::NS_URI_SF | IWORKToken::layoutStyle: // useme
    return std::make_shared<FiltersLayoutStyle>(getState(), m_layout, getState().getDictionary().m_layoutStyles);
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
typedef RefPropertyContext<property::ExternalTextWrap, IWORKExternalTextWrapElement, IWORKToken::NS_URI_SF | IWORKToken::external_text_wrap, IWORKToken::NS_URI_SF | IWORKToken::external_text_wrap_ref> ExternalTextWrapElement;
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
    return std::make_shared<AlignmentElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::baselineShift :
    return std::make_shared<BaselineShiftElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::bold :
    return std::make_shared<BoldElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::capitalization :
    return std::make_shared<CapitalizationElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::columns :
    return std::make_shared<ColumnsPropertyElement>(getState(), *m_propMap, getState().getDictionary().m_columnSets);
  case IWORKToken::NS_URI_SF | IWORKToken::externalTextWrap:
    return std::make_shared<ExternalTextWrapElement>(getState(), *m_propMap, getState().getDictionary().m_externalTextWraps);
  case IWORKToken::NS_URI_SF | IWORKToken::fill :
    return std::make_shared<FillPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::filters :
    return std::make_shared<FiltersElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::firstLineIndent :
    return std::make_shared<FirstLineIndentElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::followingLayoutStyle :
    return std::make_shared<FollowingLayoutStyleElement>(getState(), *m_propMap, getState().getDictionary().m_layoutStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::followingParagraphStyle :
    return std::make_shared<FollowingParagraphStyleElement>(getState(), *m_propMap, getState().getDictionary().m_paragraphStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::fontColor :
    return std::make_shared<FontColorElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::fontName :
    return std::make_shared<FontNameElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::fontSize :
    return std::make_shared<FontSizeElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return std::make_shared<GeometryElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::headLineEnd :
    return std::make_shared<HeadLineEndElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::italic :
    return std::make_shared<ItalicElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::keepLinesTogether :
    return std::make_shared<KeepLinesTogetherElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::keepWithNext :
    return std::make_shared<KeepWithNextElement>(getState(), *m_propMap);
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
    return std::make_shared<LabelCharacterStyleElement>(getState(), *m_propMap, getState().getDictionary().m_characterStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::language :
    return std::make_shared<LanguageElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::layoutMargins :
    return std::make_shared<LayoutMarginsElement>(getState(), *m_propMap, getState().getDictionary().m_paddings);
  case IWORKToken::NS_URI_SF | IWORKToken::layoutParagraphStyle :
    return std::make_shared<LayoutParagraphStyleElement>(getState(), *m_propMap, getState().getDictionary().m_paragraphStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::layoutStyle :
    return std::make_shared<LayoutStyleElement>(getState(), *m_propMap, getState().getDictionary().m_layoutStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::leftIndent :
    return std::make_shared<LeftIndentElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::lineSpacing :
    return std::make_shared<LineSpacingElement>(getState(), *m_propMap, getState().getDictionary().m_lineSpacings);
  case IWORKToken::NS_URI_SF | IWORKToken::listLabelGeometries :
    return std::make_shared<IWORKListLabelGeometriesProperty>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::listLabelIndents :
    return std::make_shared<IWORKListLabelIndentsProperty>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::listLabelTypes :
    return std::make_shared<IWORKListLabelTypesProperty>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::listStyle :
    return std::make_shared<ListStyleElement>(getState(), *m_propMap, getState().getDictionary().m_listStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::listTextIndents :
    return std::make_shared<IWORKListTextIndentsProperty>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::opacity :
    return std::make_shared<OpacityElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::outline :
    return std::make_shared<OutlineElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::padding :
    return std::make_shared<PaddingContext>(getState(), *m_propMap, getState().getDictionary().m_paddings);
  case IWORKToken::NS_URI_SF | IWORKToken::pageBreakBefore :
    return std::make_shared<PageBreakBeforeElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphBorderType :
    return std::make_shared<ParagraphBorderTypeElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphFill :
    return std::make_shared<ParagraphFillElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphStroke :
    return std::make_shared<ParagraphStrokeElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::rightIndent :
    return std::make_shared<RightIndentElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::tailLineEnd :
    return std::make_shared<TailLineEndElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::Series_0 :
  case IWORKToken::NS_URI_SF | IWORKToken::Series_1 :
  case IWORKToken::NS_URI_SF | IWORKToken::Series_2 :
  case IWORKToken::NS_URI_SF | IWORKToken::Series_3 :
  case IWORKToken::NS_URI_SF | IWORKToken::Series_4 :
  case IWORKToken::NS_URI_SF | IWORKToken::Series_5 :
  case IWORKToken::NS_URI_SF | IWORKToken::Series_6 :
  case IWORKToken::NS_URI_SF | IWORKToken::Series_7 :
    // CHANGEME
    return std::make_shared<SFSeriesElement>(getState(), *m_propMap, getState().getDictionary().m_chartSeriesStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFC2DAreaFillProperty :
    return std::make_shared<SFC2DAreaFillPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFC2DColumnFillProperty :
    return std::make_shared<SFC2DColumnFillPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFC2DMixedColumnFillProperty :
    return std::make_shared<SFC2DMixedColumnFillPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFC2DPieFillProperty :
    return std::make_shared<SFC2DPieFillPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFC3DAreaFillProperty :
    return std::make_shared<SFC3DAreaFillPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFC3DColumnFillProperty :
    return std::make_shared<SFC3DColumnFillPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFC3DPieFillProperty :
    return std::make_shared<SFC2DPieFillPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableCellStylePropertyFill :
    return std::make_shared<SFTableCellStylePropertyFillElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyCellStyle :
    return std::make_shared<SFTableStylePropertyCellStyleElement>(getState(), *m_propMap, getState().getDictionary().m_tableCellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderColumnCellStyle :
    return std::make_shared<SFTableStylePropertyHeaderColumnCellStyleElement>(getState(), *m_propMap, getState().getDictionary().m_tableCellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderRowCellStyle :
    return std::make_shared<SFTableStylePropertyHeaderRowCellStyleElement>(getState(), *m_propMap, getState().getDictionary().m_tableCellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyNumberFormat :
    return std::make_shared<SFTCellStylePropertyNumberFormatElement>(getState(), *m_propMap, getState().getDictionary().m_numberFormats);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyDateTimeFormat :
    return std::make_shared<SFTCellStylePropertyDateTimeFormatElement>(getState(), *m_propMap, getState().getDictionary().m_dateTimeFormats);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyDurationFormat :
    return std::make_shared<SFTCellStylePropertyDurationFormatElement>(getState(), *m_propMap, getState().getDictionary().m_durationFormats);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyLayoutStyle :
    return std::make_shared<SFTCellStylePropertyLayoutStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_layoutStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyParagraphStyle :
    return std::make_shared<SFTCellStylePropertyParagraphStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_paragraphStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultBodyCellStyleProperty :
    return std::make_shared<SFTDefaultBodyCellStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultBodyVectorStyleProperty :
    return std::make_shared<SFTDefaultBodyVectorStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_vectorStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultBorderVectorStyleProperty :
    return std::make_shared<SFTDefaultBorderVectorStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_vectorStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultFooterBodyVectorStyleProperty :
    return std::make_shared<SFTDefaultFooterBodyVectorStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_vectorStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultFooterRowCellStyleProperty :
    return std::make_shared<SFTDefaultFooterRowCellStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultFooterSeparatorVectorStyleProperty :
    return std::make_shared<SFTDefaultFooterSeparatorVectorStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_vectorStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingLevel0VectorStyleProperty:
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingLevel1VectorStyleProperty:
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingLevel2VectorStyleProperty:
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingLevel3VectorStyleProperty:
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingLevel4VectorStyleProperty:
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingLevel5VectorStyleProperty:
    return std::make_shared<SFTDefaultGroupingLevelVectorStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_vectorStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingRowCell0StyleProperty :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingRowCell1StyleProperty :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingRowCell2StyleProperty :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingRowCell3StyleProperty :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingRowCell4StyleProperty :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultGroupingRowCell5StyleProperty :
    return std::make_shared<SFTDefaultGroupingRowCellStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultHeaderBodyVectorStyleProperty :
    return std::make_shared<SFTDefaultHeaderBodyVectorStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_vectorStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultHeaderColumnCellStyleProperty :
    return std::make_shared<SFTDefaultHeaderColumnCellStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultHeaderRowCellStyleProperty :
    return std::make_shared<SFTDefaultHeaderRowCellStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultHeaderSeparatorVectorStyleProperty :
    return std::make_shared<SFTDefaultHeaderSeparatorVectorStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_vectorStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTTableNameStylePropertyLayoutStyle :
    return std::make_shared<SFTTableNameStylePropertyLayoutStyleElement>(getState(), *m_propMap, getState().getDictionary().m_layoutStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTTableNameStylePropertyParagraphStyle :
    return std::make_shared<SFTTableNameStylePropertyParagraphStyleElement>(getState(), *m_propMap, getState().getDictionary().m_paragraphStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTHeaderColumnRepeatsProperty :
    return std::make_shared<SFTHeaderColumnRepeatsPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTHeaderRowRepeatsProperty :
    return std::make_shared<SFTHeaderRowRepeatsPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTStrokeProperty :
    return std::make_shared<SFTStrokePropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTTableBandedRowsProperty :
    return std::make_shared<SFTTableBandedRowsPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::spaceAfter :
    return std::make_shared<SpaceAfterElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::spaceBefore :
    return std::make_shared<SpaceBeforeElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::strikethru :
    return std::make_shared<StrikethruElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::stroke :
    return std::make_shared<StrokePropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::superscript :
    return std::make_shared<SuperscriptElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::tabs :
    return std::make_shared<TabsProperty>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::textBackground :
    return std::make_shared<TextBackgroundElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::tocStyle :
    return std::make_shared<TocStyleElement>(getState(), *m_propMap, getState().getDictionary().m_tocStyles, getState().getDictionary().m_paragraphStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::tracking :
    return std::make_shared<TrackingElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::underline :
    return std::make_shared<UnderlineElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::verticalAlignment :
    return std::make_shared<VerticalAlignmentElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::widowControl :
    return std::make_shared<WidowControlElement>(getState(), *m_propMap);
  default:
    if (name)
    {
      //TODO: print a message even if name is unknown...
      ETONYEK_DEBUG_MSG(("IWORKPropertyMapElement::element: find some unknown element %d\n", int(name)));
    }
  }

  return IWORKXMLContextPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
