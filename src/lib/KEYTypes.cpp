/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <librevenge/librevenge.h>

#include "libetonyek_utils.h"
#include "IWORKText.h"
#include "IWORKTransformation.h"
#include "KEYStyles.h"
#include "KEYTypes.h"

namespace libetonyek
{

KEYLayer::KEYLayer()
  : type()
  , objects()
{
}

KEYPlaceholder::KEYPlaceholder()
  : title()
  , empty()
  , style()
  , geometry()
  , text()
{
}

KEYStickyNote::KEYStickyNote()
  : geometry()
  , text()
{
}

KEYStickyNote::KEYStickyNote(const IWORKGeometryPtr_t &geometry_, const IWORKTextPtr_t &text_)
  : geometry(geometry_)
  , text(text_)
{
}

namespace
{

class PlaceholderObject : public IWORKObject
{
public:
  PlaceholderObject(const KEYPlaceholderPtr_t &body, const IWORKTransformation &trafo);

private:
  virtual void draw(librevenge::RVNGPresentationInterface *painter);

private:
  const KEYPlaceholderPtr_t m_body;
  const IWORKTransformation m_trafo;
};

PlaceholderObject::PlaceholderObject(const KEYPlaceholderPtr_t &body, const IWORKTransformation &trafo)
  : m_body(body)
  , m_trafo(trafo)
{
}

void PlaceholderObject::draw(librevenge::RVNGPresentationInterface *const painter)
{
  if (bool(m_body) && bool(m_body->style) && bool(m_body->text))
    makeObject(m_body->text, m_trafo)->draw(painter);
}

}

IWORKObjectPtr_t makeObject(const KEYPlaceholderPtr_t &body, const IWORKTransformation &trafo)
{
  const IWORKObjectPtr_t object(new PlaceholderObject(body, trafo));
  return object;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
