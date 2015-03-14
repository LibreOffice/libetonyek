/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKParser.h"

#include <boost/enable_shared_from_this.hpp>

#include "libetonyek_xml.h"
#include "IWORKXMLContextBase.h"
#include "IWORKXMLParserState.h"
#include "IWORKXMLReader.h"
#include "KEYCollector.h"

namespace libetonyek
{

namespace
{

class DiscardContext : public IWORKXMLContext, public boost::enable_shared_from_this<DiscardContext>
{
private:
  virtual void startOfElement();
  void attribute(int name, const char *value);
  virtual void endOfAttributes();
  IWORKXMLContextPtr_t element(int name);
  void text(const char *value);
  virtual void endOfElement();
};

void DiscardContext::startOfElement()
{
}

void DiscardContext::attribute(int, const char *)
{
}

void DiscardContext::endOfAttributes()
{
}

IWORKXMLContextPtr_t DiscardContext::element(int)
{
  return shared_from_this();
}

void DiscardContext::text(const char *)
{
}

void DiscardContext::endOfElement()
{
}

}

namespace
{

void processAttribute(xmlTextReaderPtr reader, const IWORKXMLContextPtr_t &context, const TokenizerFunction_t &tokenizer)
{

  const int name = tokenizer(reinterpret_cast<const char *>(xmlTextReaderConstLocalName(reader)));
  const int ns = tokenizer(reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(reader)) ? reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(reader)) : "");
  const char *value = reinterpret_cast<const char *>(xmlTextReaderConstValue(reader));
  context->attribute((name | ns), value);
}

IWORKXMLContextPtr_t processNode(xmlTextReaderPtr reader, const IWORKXMLContextPtr_t &context, const TokenizerFunction_t &tokenizer)
{

  IWORKXMLContextPtr_t newContext = context;
  switch (xmlTextReaderNodeType(reader))
  {
  case XML_READER_TYPE_ELEMENT :
  {
    const int name = tokenizer(reinterpret_cast<const char *>(xmlTextReaderConstLocalName(reader)));
    const int ns = tokenizer(reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(reader)) ? reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(reader)) : "");

    if (name > 0)
    {
      newContext = context->element((name | ns));

      if (newContext)
      {
        newContext->startOfElement();
        const bool isEmpty = xmlTextReaderIsEmptyElement(reader);

        if (xmlTextReaderHasAttributes(reader))
        {
          int ret = xmlTextReaderMoveToFirstAttribute(reader);
          while (1 == ret)
          {
            processAttribute(reader, newContext, tokenizer);
            ret = xmlTextReaderMoveToNextAttribute(reader);
          }
        }
        if (newContext)
          newContext->endOfAttributes();

        if (newContext && isEmpty)
        {
          newContext->endOfElement();
        }
      }
      else
      {
        newContext.reset(new DiscardContext());
      }
    }
    break;
  }
  case XML_READER_TYPE_ATTRIBUTE :
    assert(false && "How did i ever got there?");
    processAttribute(reader, newContext, tokenizer);
    break;
  case XML_READER_TYPE_END_ELEMENT :
  {
    context->endOfElement();
    break;
  }
  case XML_READER_TYPE_TEXT :
  {
    xmlChar *const text = xmlTextReaderReadString(reader);
    context->text(reinterpret_cast<char *>(text));
    xmlFree(text);
    break;
  }
  default :
    // ignore other types of XML content
    break;
  }

  return newContext;

}

}

IWORKParser::IWORKParser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, KEYCollector *const collector)
  : m_input(input)
  , m_package(package)
  , m_collector(collector)
{
}

IWORKParser::~IWORKParser()
{
}

bool IWORKParser::parse()
{
  xmlTextReaderPtr reader(xmlReaderForIO(readFromStream, closeStream, m_input.get(), "", 0, 0));
  if (!bool(reader))
    return false;

  TokenizerFunction_t tokenizer = getTokenizer();
  IWORKXMLContextPtr_t currentContext = createDocumentContext();

  int ret = xmlTextReaderRead(reader);

  while ((1 == ret) && currentContext)
  {
    currentContext = processNode(reader, currentContext, tokenizer);
    if (currentContext)
      ret = xmlTextReaderRead(reader);
  }

  xmlTextReaderClose(reader);
  xmlFreeTextReader(reader);


  return true;
}

RVNGInputStreamPtr_t &IWORKParser::getInput()
{
  return m_input;
}

RVNGInputStreamPtr_t IWORKParser::getInput() const
{
  return m_input;
}

RVNGInputStreamPtr_t &IWORKParser::getPackage()
{
  return m_package;
}

RVNGInputStreamPtr_t IWORKParser::getPackage() const
{
  return m_package;
}

KEYCollector *IWORKParser::getCollector() const
{
  return m_collector;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
