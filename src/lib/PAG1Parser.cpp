/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAG1Parser.h"

#include <functional>

#include <boost/optional.hpp>

#include "IWORKChainedTokenizer.h"
#include "IWORKCalcEngineContext.h"
#include "IWORKDiscardContext.h"
#include "IWORKGroupElement.h"
#include "IWORKHeaderFooterContext.h"
#include "IWORKImageElement.h"
#include "IWORKLineElement.h"
#include "IWORKMediaElement.h"
#include "IWORKMetadataElement.h"
#include "IWORKNumberConverter.h"
#include "IWORKNumberElement.h"
#include "IWORKStyleRefContext.h"
#include "IWORKStylesContext.h"
#include "IWORKStylesheetBase.h"
#include "IWORKTabularInfoElement.h"
#include "IWORKToken.h"
#include "PAG1AnnotationContext.h"
#include "PAG1Dictionary.h"
#include "PAG1ShapeContext.h"
#include "PAG1StyleContext.h"
#include "PAG1TextStorageElement.h"
#include "PAG1Token.h"
#include "PAG1XMLContextBase.h"
#include "PAGCollector.h"
#include "PAGTypes.h"
#include "libetonyek_xml.h"

using boost::none;
using boost::optional;

using namespace std::placeholders;

using std::string;

namespace libetonyek
{

namespace
{

unsigned getVersion(const int token)
{
  switch (token)
  {
  case PAG1Token::VERSION_STR_4 :
    return 4;
  default:
    break;
  }

  return 0;
}

}

namespace
{

namespace
{

class AnnotationsElement : public PAG1XMLElementContextBase
{
public:
  explicit AnnotationsElement(PAG1ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

AnnotationsElement::AnnotationsElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t AnnotationsElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::annotation))
    return std::make_shared<PAG1AnnotationContext>(getState(),
                                                   std::bind(&PAGCollector::collectAnnotation, std::ref(getCollector()), _1));
  return IWORKXMLContextPtr_t();
}

}

class FootersElement : public PAG1XMLElementContextBase
{
public:
  explicit FootersElement(PAG1ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

FootersElement::FootersElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t FootersElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::footer))
    return std::make_shared<IWORKHeaderFooterContext>(getState(),
                                                      std::bind(&IWORKCollector::collectFooter, std::ref(getCollector()), _1));
  return IWORKXMLContextPtr_t();
}

}

namespace
{
class GroupElement : public PAG1XMLContextBase<IWORKGroupElement>
{
public:
  GroupElement(PAG1ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

GroupElement::GroupElement(PAG1ParserState &state)
  : PAG1XMLContextBase<IWORKGroupElement>(state)
{
}

IWORKXMLContextPtr_t GroupElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::drawable_shape :
    PAG1XMLContextBase<IWORKGroupElement>::ensureClosed();
    return std::make_shared<PAG1ShapeContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::group :
    PAG1XMLContextBase<IWORKGroupElement>::ensureClosed();
    return std::make_shared<GroupElement>(getState());
  default:
    break;
  }

  return PAG1XMLContextBase<IWORKGroupElement>::element(name);
}

}

namespace
{

class HeadersElement : public PAG1XMLElementContextBase
{
public:
  explicit HeadersElement(PAG1ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

HeadersElement::HeadersElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t HeadersElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::header))
    return std::make_shared<IWORKHeaderFooterContext>(getState(),
                                                      std::bind(&IWORKCollector::collectHeader, std::ref(getCollector()), _1));
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class StylesContext : public PAG1XMLContextBase<IWORKStylesContext>
{
public:
  StylesContext(PAG1ParserState &state, bool anonymous);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

StylesContext::StylesContext(PAG1ParserState &state, const bool anonymous)
  : PAG1XMLContextBase<IWORKStylesContext>(state, anonymous)
{
}

IWORKXMLContextPtr_t StylesContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::sectionstyle :
    // TODO: setting of the default parent would also be a good candidate for leaveElement(),
    // if we ever add this, as it seems to be limited to a few style types.
    return std::make_shared<PAG1StyleContext>(getState(), &getState().getDictionary().m_sectionStyles, "section-style-default");
  case IWORKToken::NS_URI_SF | IWORKToken::sectionstyle_ref :
    return std::make_shared<IWORKStyleRefContext>(getState(), getState().getDictionary().m_sectionStyles);
  default:
    break;
  }

  return PAG1XMLContextBase<IWORKStylesContext>::element(name);
}

}

namespace
{

class StylesheetElement : public PAG1XMLContextBase<IWORKStylesheetBase>
{
public:
  explicit StylesheetElement(PAG1ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

StylesheetElement::StylesheetElement(PAG1ParserState &state)
  : PAG1XMLContextBase<IWORKStylesheetBase>(state)
{
}

IWORKXMLContextPtr_t StylesheetElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::anon_styles :
    return std::make_shared<StylesContext>(getState(), true);
  case IWORKToken::NS_URI_SF | IWORKToken::styles :
    return std::make_shared<StylesContext>(getState(), false);
  default:
    break;
  }

  return PAG1XMLContextBase<IWORKStylesheetBase>::element(name);
}

}

namespace
{

class PrototypeElement : public PAG1XMLElementContextBase
{
public:
  explicit PrototypeElement(PAG1ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

PrototypeElement::PrototypeElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t PrototypeElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::stylesheet))
    return std::make_shared<StylesheetElement>(getState());
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class SectionPrototypesElement : public PAG1XMLElementContextBase
{
public:
  explicit SectionPrototypesElement(PAG1ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

SectionPrototypesElement::SectionPrototypesElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t SectionPrototypesElement::element(const int name)
{
  if (name == (PAG1Token::NS_URI_SL | PAG1Token::prototype))
    return std::make_shared<PrototypeElement>(getState());
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class DateElement : public PAG1XMLEmptyContextBase
{
public:
  DateElement(PAG1ParserState &state, optional<string> &value);

private:
  void attribute(int name, const char *value) override;

private:
  optional<string> &m_value;
};

DateElement::DateElement(PAG1ParserState &state, optional<string> &value)
  : PAG1XMLEmptyContextBase(state)
  , m_value(value)
{
}

void DateElement::attribute(const int name, const char *const value)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::val))
    m_value = value;
}

}

namespace
{

class SLCreationDatePropertyElement : public PAG1XMLElementContextBase
{
public:
  SLCreationDatePropertyElement(PAG1ParserState &state, optional<string> &value);

private:
  IWORKXMLContextPtr_t element(int name) override;

private:
  optional<string> &m_value;
};

SLCreationDatePropertyElement::SLCreationDatePropertyElement(PAG1ParserState &state, optional<string> &value)
  : PAG1XMLElementContextBase(state)
  , m_value(value)
{
}

IWORKXMLContextPtr_t SLCreationDatePropertyElement::element(const int name)
{
  if (name == (PAG1Token::NS_URI_SL | PAG1Token::SLCreationDateProperty))
    return std::make_shared<DateElement>(getState(), m_value);
  return IWORKXMLContextPtr_t();
}

}

namespace
{

template<class T, class C, int I>
class DocumentPropertyContext : public PAG1XMLElementContextBase
{
public:
  DocumentPropertyContext(PAG1ParserState &state, optional<T> &value);

private:
  IWORKXMLContextPtr_t element(int name) override;

private:
  optional<T> &m_value;
};

template<class T, class C, int I>
DocumentPropertyContext<T, C, I>::DocumentPropertyContext(PAG1ParserState &state, optional<T> &value)
  : PAG1XMLElementContextBase(state)
  , m_value(value)
{
}

template<class T, class C, int I>
IWORKXMLContextPtr_t DocumentPropertyContext<T, C, I>::element(const int name)
{
  if (name == I)
    return std::make_shared<C>(getState(), m_value);
  return IWORKXMLContextPtr_t();
}

}

IWORK_DECLARE_NUMBER_CONVERTER(PAGFootnoteKind);

optional<PAGFootnoteKind> IWORKNumberConverter<PAGFootnoteKind>::convert(const char *const value)
{
  const optional<int> kind(try_int_cast(value));
  if (kind)
  {
    switch (get(kind))
    {
    case 0 :
      return PAG_FOOTNOTE_KIND_FOOTNOTE;
    case 1 :
      return PAG_FOOTNOTE_KIND_ENDNOTE;
    case 2 :
      return PAG_FOOTNOTE_KIND_SECTION_ENDNOTE;
    default:
      break;
    }
  }

  return none;
}

namespace
{

typedef DocumentPropertyContext<double, IWORKNumberElement<double>, PAG1Token::NS_URI_SL | PAG1Token::number> KSFWPFootnoteGapPropertyElement;
typedef DocumentPropertyContext<PAGFootnoteKind, IWORKNumberElement<PAGFootnoteKind>, PAG1Token::NS_URI_SL | PAG1Token::number> KSFWPFootnoteKindPropertyElement;

}

namespace
{

class PublicationInfoElement : public PAG1XMLElementContextBase
{
public:
  explicit PublicationInfoElement(PAG1ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  PAGPublicationInfo m_pubInfo;
  optional<PAGFootnoteKind> m_footnoteKind;
};

PublicationInfoElement::PublicationInfoElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
  , m_pubInfo()
  , m_footnoteKind()
{
}

IWORKXMLContextPtr_t PublicationInfoElement::element(const int name)
{
  switch (name)
  {
  case PAG1Token::NS_URI_SL | PAG1Token::kSFWPFootnoteGapProperty :
    return std::make_shared<KSFWPFootnoteGapPropertyElement>(getState(), m_pubInfo.m_footnoteGap);
  case PAG1Token::NS_URI_SL | PAG1Token::kSFWPFootnoteKindProperty :
    return std::make_shared<KSFWPFootnoteKindPropertyElement>(getState(), m_footnoteKind);
  case PAG1Token::NS_URI_SL | PAG1Token::SLCreationDateProperty :
    return std::make_shared<SLCreationDatePropertyElement>(getState(), m_pubInfo.m_creationDate);
  default:
  {
    static bool first=true;
    if (first)
    {
      first=false;
      ETONYEK_DEBUG_MSG(("PublicationInfoElement::element[PAG1Parser.cpp]: find some unknown elements\n"));
    }
    break;
  }
  }
  return IWORKXMLContextPtr_t();
}

void PublicationInfoElement::endOfElement()
{
  if (isCollector())
  {
    if (m_footnoteKind)
      m_pubInfo.m_footnoteKind = get(m_footnoteKind);
    getCollector().collectPublicationInfo(m_pubInfo);
  }
}

}

namespace
{

class PageMarginsElement : public PAG1XMLElementContextBase
{
public:
  explicit PageMarginsElement(PAG1ParserState &state, IWORKPrintInfo &printInfo);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(const int name) override;

private:
  IWORKPrintInfo &m_printInfo;
};

PageMarginsElement::PageMarginsElement(PAG1ParserState &state, IWORKPrintInfo &printInfo)
  : PAG1XMLElementContextBase(state)
  , m_printInfo(printInfo)
{
}

void PageMarginsElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::ID | IWORKToken::NS_URI_SFA :
    PAG1XMLElementContextBase::attribute(name,value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::bottom:
    m_printInfo.m_marginBottom=try_double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::left:
    m_printInfo.m_marginLeft=try_double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::right:
    m_printInfo.m_marginRight=try_double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::top:
    m_printInfo.m_marginTop=try_double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::footer:
    m_printInfo.m_footerHeight=try_double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::header:
    m_printInfo.m_headerHeight=try_double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::facing_pages: // a bool
    break;
  default:
    ETONYEK_DEBUG_MSG(("PageMarginsElement::attribute[PAG1Parser.cpp]: find unknown attribute\n"));
    break;
  }
}

#ifndef DEBUG
IWORKXMLContextPtr_t PageMarginsElement::element(const int /*name*/)
#else
IWORKXMLContextPtr_t PageMarginsElement::element(const int name)
#endif
{
  ETONYEK_DEBUG_MSG(("PageMarginsElement::element[PAG1Parser.cpp]: find unknown element %d\n", name));
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class SLPrintInfoElement : public PAG1XMLElementContextBase
{
public:
  explicit SLPrintInfoElement(PAG1ParserState &state);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKPrintInfo m_printInfo;
};

SLPrintInfoElement::SLPrintInfoElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
  , m_printInfo()
{
}

void SLPrintInfoElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::ID | IWORKToken::NS_URI_SFA :
    PAG1XMLElementContextBase::attribute(name,value);
    break;
  case PAG1Token::NS_URI_SL | PAG1Token::page_height:
    m_printInfo.m_height=try_double_cast(value);
    break;
  case PAG1Token::NS_URI_SL | PAG1Token::page_scale:
    break;
  case PAG1Token::NS_URI_SL | PAG1Token::page_width:
    m_printInfo.m_width=try_double_cast(value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("SLPrintInfoElement::attribute[PAG1Parser.cpp]: find unknown attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t SLPrintInfoElement::element(const int name)
{
  switch (name)
  {
  case PAG1Token::NS_URI_SL | PAG1Token::print_info:
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::page_margins:
    return std::make_shared<PageMarginsElement>(getState(), m_printInfo);
  default:
    ETONYEK_DEBUG_MSG(("SLPrintInfoElement::element[PAG1Parser.cpp]: find unknown element\n"));
    break;
  }
  return IWORKXMLContextPtr_t();
}

void SLPrintInfoElement::endOfElement()
{
  if (isCollector())
    getCollector().setPageDimensions(m_printInfo);
}

}

namespace
{

class PageGroupElement : public PAG1XMLElementContextBase
{
public:
  explicit PageGroupElement(PAG1ParserState &state);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  void open();

private:
  optional<int> m_page;
  optional<int> m_rpage;
  bool m_opened;
};

PageGroupElement::PageGroupElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
  , m_page()
  , m_rpage()
  , m_opened(false)
{
}

void PageGroupElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  // TODO: what is the exact relation of sl:page and sl:rpage attrs?
  case PAG1Token::NS_URI_SL | PAG1Token::page :
    m_page = try_int_cast(value);
    break;
  case PAG1Token::NS_URI_SL | PAG1Token::rpage :
    m_rpage = try_int_cast(value);
    break;
  default:
    break;
  }
}

IWORKXMLContextPtr_t PageGroupElement::element(const int name)
{
  if (!m_opened)
    open();

  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::drawable_shape :
    return std::make_shared<PAG1ShapeContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::group :
    return std::make_shared<GroupElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::line :
    return std::make_shared<IWORKLineElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::image :
    return std::make_shared<IWORKImageElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::media :
    return std::make_shared<IWORKMediaElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_info :
    return std::make_shared<IWORKTabularInfoElement>(getState());
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void PageGroupElement::endOfElement()
{
  if (isCollector())
  {
    if (!m_opened || !m_page) // ignore empty group
      return;
    getCollector().closePageGroup();
  }
}

void PageGroupElement::open()
{
  if (isCollector())
  {
    if (!m_page && m_rpage)
      m_page = get(m_rpage) + 1;
    if (m_page)
      getCollector().openPageGroup(m_page);
  }
  m_opened = true;
}

}

namespace
{

class DrawablesElement : public PAG1XMLElementContextBase
{
public:
  explicit DrawablesElement(PAG1ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

DrawablesElement::DrawablesElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t DrawablesElement::element(const int name)
{
  switch (name)
  {
  case PAG1Token::NS_URI_SL | PAG1Token::page_group :
    return std::make_shared<PageGroupElement>(getState());
  // see also sl:master-groups which contains sl:section-drawables
  default:
    break;
  }
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class DocumentElement : public PAG1XMLElementContextBase
{
public:
  explicit DocumentElement(PAG1ParserState &state);

private:
  void startOfElement() override;
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  optional<IWORKSize> m_size;
};

DocumentElement::DocumentElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
  , m_size()
{
}

void DocumentElement::startOfElement()
{
  if (isCollector())
    getCollector().startDocument();
}

void DocumentElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case PAG1Token::NS_URI_SL | PAG1Token::version :
  {
    const unsigned version = getVersion(getToken(value));
    if (0 == version)
    {
      ETONYEK_DEBUG_MSG(("DocumentElement::attribute[PAG1Parser.cpp]: unknown version %s\n", value));
    }
  }
  break;
  default:
    break;
  }
}

IWORKXMLContextPtr_t DocumentElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::annotations :
    return std::make_shared<AnnotationsElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::calc_engine :
    return std::make_shared<IWORKCalcEngineContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::headers :
    return std::make_shared<HeadersElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::footers :
    return std::make_shared<FootersElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::metadata :
    return std::make_shared<IWORKMetadataElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::text_storage :
    return std::make_shared<PAG1TextStorageElement>(getState());
  case PAG1Token::NS_URI_SL | PAG1Token::drawables :
    return std::make_shared<DrawablesElement>(getState());
  case PAG1Token::NS_URI_SL | PAG1Token::publication_info :
    return std::make_shared<PublicationInfoElement>(getState());
  case PAG1Token::NS_URI_SL | PAG1Token::section_prototypes :
    return std::make_shared<SectionPrototypesElement>(getState());
  case PAG1Token::NS_URI_SL | PAG1Token::slprint_info :
    return std::make_shared<SLPrintInfoElement>(getState());
  case PAG1Token::NS_URI_SL | PAG1Token::stylesheet :
    return std::make_shared<StylesheetElement>(getState());
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void DocumentElement::endOfElement()
{
  if (isCollector())
    getCollector().endDocument();
}

}

namespace
{

class XMLDocument : public PAG1XMLElementContextBase
{
public:
  explicit XMLDocument(PAG1ParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

XMLDocument::XMLDocument(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t XMLDocument::element(const int name)
{
  switch (name)
  {
  case PAG1Token::NS_URI_SL | PAG1Token::document :
    return std::make_shared<DocumentElement>(m_state);
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class DiscardContext : public PAG1XMLContextBase<IWORKDiscardContext>
{
public:
  explicit DiscardContext(PAG1ParserState &state);

  IWORKXMLContextPtr_t element(int name) override;
};

DiscardContext::DiscardContext(PAG1ParserState &state)
  : PAG1XMLContextBase<IWORKDiscardContext>(state)
{
}

IWORKXMLContextPtr_t DiscardContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::sectionstyle :
    return std::make_shared<PAG1StyleContext>(getState(), &getState().getDictionary().m_sectionStyles, "section-style-default");
  case IWORKToken::NS_URI_SF | IWORKToken::stylesheet :
  case PAG1Token::NS_URI_SL | PAG1Token::stylesheet :
    return std::make_shared<StylesheetElement>(getState());
  default:
    break;
  }

  return PAG1XMLContextBase<IWORKDiscardContext>::element(name);
}

}

PAG1Parser::PAG1Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, PAGCollector &collector, PAG1Dictionary *const dict)
  : IWORKParser(input, package)
  , m_state(*this, collector, *dict)
{
}

PAG1Parser::~PAG1Parser()
{
}

IWORKXMLContextPtr_t PAG1Parser::createDocumentContext()
{
  return std::make_shared<XMLDocument>(m_state);
}

IWORKXMLContextPtr_t PAG1Parser::createDiscardContext()
{
  return std::make_shared<DiscardContext>(m_state);
}

const IWORKTokenizer &PAG1Parser::getTokenizer() const
{
  static IWORKChainedTokenizer tokenizer(PAG1Token::getTokenizer(), IWORKToken::getTokenizer());
  return tokenizer;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
