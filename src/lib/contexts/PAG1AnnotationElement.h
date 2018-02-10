/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PAG1ANNOTATIONELEMENT_H_INCLUDED
#define PAG1ANNOTATIONELEMENT_H_INCLUDED

#include <boost/optional.hpp>

#include "PAG1XMLContextBase.h"

namespace libetonyek
{
class PAG1AnnotationElement : public PAG1XMLElementContextBase
{
public:
  PAG1AnnotationElement(PAG1ParserState &state, IWORKXMLContext &container, bool isRef=false);

protected:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
  virtual void text(const char *value);

private:
  IWORKXMLContext &m_container;
  bool m_isRef;
  boost::optional<ID_t> m_id;
};

}

#endif // PAG1ANNOTATIONELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
