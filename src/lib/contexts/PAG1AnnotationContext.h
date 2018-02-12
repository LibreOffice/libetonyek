/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PAG1ANNOTATIONCONTEXT_H_INCLUDED
#define PAG1ANNOTATIONCONTEXT_H_INCLUDED

#include <string>

#include <boost/function.hpp>
#include <boost/optional.hpp>

#include "PAG1XMLContextBase.h"

namespace libetonyek
{

class PAG1AnnotationContext : public PAG1XMLElementContextBase
{
public:
  typedef boost::function<void(const std::string &)> CollectFunction_t;

  PAG1AnnotationContext(PAG1ParserState &state, const CollectFunction_t &collect);

private:
  void startOfElement() override;
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  const CollectFunction_t m_collect;
  boost::optional<std::string> m_target;
};

}

#endif // PAG1ANNOTATIONCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
