/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKParser.h"

#include <cassert>
#include <memory>

#include <libxml/xmlreader.h>

#include <stack>

#include "libetonyek_xml.h"
#include "IWORKTokenizer.h"
#include "IWORKXMLContextBase.h"
#include "IWORKXMLParserState.h"

using std::shared_ptr;
using std::stack;

namespace libetonyek
{

namespace
{

void processAttribute(xmlTextReaderPtr reader, IWORKXMLContextPtr_t context, const IWORKTokenizer &tokenizer)
{
  const int id = tokenizer.getQualifiedId(char_cast(xmlTextReaderConstLocalName(reader)), char_cast(xmlTextReaderConstNamespaceUri(reader)));
  const char *value = char_cast(xmlTextReaderConstValue(reader));
  context->attribute(id, value);
}

}

IWORKParser::IWORKParser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package)
  : m_input(input)
  , m_package(package)
{
}

IWORKParser::~IWORKParser()
{
}

bool IWORKParser::parse()
{
  auto sharedReader = xmlReaderForStream(m_input);
  if (!sharedReader)
    return false;

  xmlTextReaderPtr reader = sharedReader.get();
  assert(reader);

  const IWORKTokenizer &tokenizer = getTokenizer();
  stack<IWORKXMLContextPtr_t> contextStack;

  int ret = xmlTextReaderRead(reader);
  contextStack.push(createDocumentContext());

  bool keynoteDocTypeChecked=false;
  char const *defaultNS=nullptr;
  while ((1 == ret))
  {
    switch (xmlTextReaderNodeType(reader))
    {
    case XML_READER_TYPE_ELEMENT:
    {
      if (!keynoteDocTypeChecked)
      {
        // check for keynote 1 file with doctype node and not a namespace in first node
        keynoteDocTypeChecked=true;
        if (xmlTextReaderNodeType(reader)==XML_READER_TYPE_ELEMENT &&
            xmlTextReaderConstNamespaceUri(reader)==nullptr)
          defaultNS="http://developer.apple.com/schemas/APXL";
      }
      const int id = tokenizer.getQualifiedId(char_cast(xmlTextReaderConstLocalName(reader)),
                                              defaultNS ? defaultNS : char_cast(xmlTextReaderConstNamespaceUri(reader)));

      IWORKXMLContextPtr_t newContext = contextStack.top()->element(id);

      if (!newContext)
        newContext = createDiscardContext();

      const bool isEmpty = xmlTextReaderIsEmptyElement(reader);

      newContext->startOfElement();
      if (xmlTextReaderHasAttributes(reader))
      {
        ret = xmlTextReaderMoveToFirstAttribute(reader);
        while (1 == ret)
        {
          processAttribute(reader, newContext, tokenizer);
          ret = xmlTextReaderMoveToNextAttribute(reader);
        }
      }

      if (isEmpty)
        newContext->endOfElement();
      else
        contextStack.push(newContext);

      break;
    }
    case XML_READER_TYPE_END_ELEMENT:
    {
      contextStack.top()->endOfElement();
      contextStack.pop();
      break;
    }
    case XML_READER_TYPE_CDATA :
    {
      const xmlChar *text = xmlTextReaderConstValue(reader);
      if (text) contextStack.top()->CDATA(char_cast(text));
      break;
    }
    case XML_READER_TYPE_TEXT :
    {
      xmlChar *const text = xmlTextReaderReadString(reader);
      contextStack.top()->text(char_cast(text));
      xmlFree(text);
      break;
    }
    default:
      break;
    }

    ret = xmlTextReaderRead(reader);

  }

  while (!contextStack.empty()) // finish parsing in case of broken XML
  {
    contextStack.top()->endOfElement();
    contextStack.pop();
  }
  xmlTextReaderClose(reader);

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

void IWORKParser::setInput(const RVNGInputStreamPtr_t &input)
{
  m_input = input;
}

RVNGInputStreamPtr_t &IWORKParser::getPackage()
{
  return m_package;
}

RVNGInputStreamPtr_t IWORKParser::getPackage() const
{
  return m_package;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
