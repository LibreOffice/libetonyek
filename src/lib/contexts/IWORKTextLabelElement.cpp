/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTextLabelElement.h"

#include <boost/spirit/include/qi_attr.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/qi_parse_attr.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/qi_symbols.hpp>

#include "IWORKDictionary.h"
#include "IWORKToken.h"
#include "IWORKTokenizer.h"
#include "IWORKXMLParserState.h"
#include "libetonyek_xml.h"

namespace libetonyek
{

using boost::optional;

using std::string;

namespace
{

void parseFormat(const string &format, IWORKTextLabel &label)
{
  namespace qi = boost::spirit::qi;
  using qi::attr;
  using qi::lit;

  optional<IWORKLabelNumFormatSurrounding> prefix;
  IWORKLabelNumFormatSurrounding suffix;
  optional<bool> tiered;

  qi::symbols<char, IWORKLabelNumFormatSurrounding> prefixes, suffixes;
  suffixes.add(")", IWORK_LABEL_NUM_FORMAT_SURROUNDING_PARENTHESIS)
  (".", IWORK_LABEL_NUM_FORMAT_SURROUNDING_DOT)
  ;
  prefixes.add("(", IWORK_LABEL_NUM_FORMAT_SURROUNDING_PARENTHESIS);

  string::const_iterator it = format.begin();
  const bool r = qi::parse(it, format.end(),
                           (-(lit("%P") >> attr(true)) >> -prefixes >> "%L" >> suffixes),
                           tiered, prefix, suffix)
                 ;
  if (r && (it == format.end()))
  {
    if (prefix)
      label.m_format.m_prefix = get(prefix);
    label.m_format.m_suffix = suffix;
    if (tiered)
      label.m_tiered = get(tiered);
  }
  else
  {
    ETONYEK_DEBUG_MSG(("parsing of label format '%s' failed\n", format.c_str()));
  }
}

}

IWORKTextLabelElement::IWORKTextLabelElement(IWORKXMLParserState &state, boost::optional<IWORKListLabelTypeInfo_t> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
  , m_bullet(false)
  , m_type()
  , m_format()
  , m_first()
{
}

void IWORKTextLabelElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::first :
    m_first = try_int_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format :
    m_format = value;
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::type :
    switch (getState().getTokenizer().getId(value))
    {
    case IWORKToken::bullet :
      m_bullet = true;
      break;
    case IWORKToken::decimal :
      m_type = IWORK_LABEL_NUM_FORMAT_NUMERIC;
      break;
    case IWORKToken::lower_alpha :
      m_type = IWORK_LABEL_NUM_FORMAT_ALPHA_LOWERCASE;
      break;
    case IWORKToken::lower_roman :
      m_type = IWORK_LABEL_NUM_FORMAT_ROMAN_LOWERCASE;
      break;
    case IWORKToken::upper_alpha :
      m_type = IWORK_LABEL_NUM_FORMAT_ALPHA;
      break;
    case IWORKToken::upper_roman :
      m_type = IWORK_LABEL_NUM_FORMAT_ROMAN;
      break;
    default :
      ETONYEK_DEBUG_MSG(("IWORKTextLabelElement::attribute: unknown label type \"%s\"\n", value));
      break;
    }
    break;
  default:
    ETONYEK_DEBUG_MSG(("IWORKTextLabelElement::attribute: find some unknown attribute\n"));
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  }
}

void IWORKTextLabelElement::endOfElement()
{
  if (!m_type && !m_bullet)
  {
    ETONYEK_DEBUG_MSG(("IWORKTextLabelElement::endOfElement: no label type found\n"));
  }
  if (m_type)
  {
    IWORKTextLabel label;
    if (m_first)
      label.m_first = get(m_first);
    label.m_format.m_format = get(m_type);
    parseFormat(m_format, label);
    m_value = label;
  }
  else if (m_bullet && !m_format.empty())
  {
    m_value = m_format;
  }
  if (getId())
    getState().getDictionary().m_textLabels[get(getId())]=get_optional_value_or(m_value, IWORKListLabelTypeInfo_t());
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
