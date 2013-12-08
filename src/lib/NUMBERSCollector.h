/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef NUMBERSCOLLECTOR_H_INCLUDED
#define NUMBERSCOLLECTOR_H_INCLUDED

#include <librevenge/librevenge.h>

namespace libetonyek
{

class NUMBERSCollector
{
public:
  explicit NUMBERSCollector(librevenge::RVNGSpreadsheetInterface *document);

  // collector functions

  // helper functions
  void startDocument();
  void endDocument();

private:
  librevenge::RVNGSpreadsheetInterface *m_document;
};

} // namespace libetonyek

#endif //  NUMBERSCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
