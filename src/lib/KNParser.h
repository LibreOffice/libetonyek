/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNPARSER_H_INCLUDED
#define KNPARSER_H_INCLUDED

class WPXInputStream;

namespace libkeynote
{

class KNCollector;

class KNParser
{
  // -Weffc++
  KNParser(const KNParser &);
  KNParser &operator=(const KNParser &);

public:
  KNParser(WPXInputStream *input, KNCollector *collector);
  virtual ~KNParser();
  bool parse();

private:
  WPXInputStream *m_input;
  KNCollector *m_collector;
};

} // namespace libkeynote

#endif //  KNPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
