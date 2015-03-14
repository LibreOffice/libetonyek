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

#include <stack>

#include "libetonyek_xml.h"
#include "IWORKXMLContextBase.h"
#include "IWORKXMLParserState.h"
#include "IWORKXMLReader.h"
#include "KEYCollector.h"

using std::stack;

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

void processAttribute(xmlTextReaderPtr reader, IWORKXMLContextPtr_t context, const TokenizerFunction_t &tokenizer)
{
  const int name = tokenizer(reinterpret_cast<const char *>(xmlTextReaderConstLocalName(reader)));
  const int ns = tokenizer(reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(reader)) ? reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(reader)) : "");
  const char *value = reinterpret_cast<const char *>(xmlTextReaderConstValue(reader));
  context->attribute((name | ns), value);
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
  IWORKXMLContextPtr_t context = createDocumentContext();
  stack <IWORKXMLContextPtr_t> contextStack;

  int ret = xmlTextReaderRead(reader);
  contextStack.push(context);

  while ((1 == ret))
  {
    const int name = tokenizer(reinterpret_cast<const char *>(xmlTextReaderConstLocalName(reader)));
    const int ns = tokenizer(reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(reader)) ? reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(reader)) : "");

    IWORKXMLContextPtr_t newContext = contextStack.top()->element((name | ns));

    if (newContext)
    {
      contextStack.push(newContext);
      newContext->startOfElement();
      if (xmlTextReaderHasAttributes(reader))
      {
        int ret1 = xmlTextReaderMoveToFirstAttribute(reader);
        while (1 == ret1)
        {
          processAttribute(reader, newContext, tokenizer);
          ret1 = xmlTextReaderMoveToNextAttribute(reader);
        }
      }
      newContext->endOfAttributes();
    }
    else
    {
      if (contextStack.size() > 1)
      {
        contextStack.top()->endOfElement();
        contextStack.pop();
      }
    }
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
