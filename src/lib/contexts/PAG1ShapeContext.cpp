/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAG1ShapeContext.h"

#include <cassert>

#include <boost/optional.hpp>

#include "IWORKDictionary.h"
#include "IWORKGeometryElement.h"
#include "IWORKPathElement.h"
#include "IWORKRefContext.h"
#include "IWORKTextElement.h"
#include "IWORKToken.h"

#include "PAGCollector.h"
#include "PAG1ParserState.h"
#include "PAG1TextStorageElement.h"

namespace libetonyek
{

namespace
{
class TextElement : public PAG1XMLContextBase<IWORKTextElement>
{
public:
  explicit TextElement(PAG1ParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};


TextElement::TextElement(PAG1ParserState &state)
  : PAG1XMLContextBase<IWORKTextElement>(state)
{
}

IWORKXMLContextPtr_t TextElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::text_storage :
    return makeContext<PAG1TextStorageElement>(getState(), PAG_TEXTSTORAGE_KIND_TEXTBOX);
  default:
    break;
  }

  return PAG1XMLContextBase<IWORKTextElement>::element(name);
}
}


PAG1ShapeContext::PAG1ShapeContext(PAG1ParserState &state)
  : PAG1XMLContextBase<IWORKShapeContext>(state)
{
}

IWORKXMLContextPtr_t PAG1ShapeContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::text :
    return makeContext<TextElement>(getState());
  default:
    break;
  }

  return PAG1XMLContextBase<IWORKShapeContext>::element(name);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
