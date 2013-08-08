/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNXMLATTRIBUTEITERATOR_H_INCLUDED
#define KNXMLATTRIBUTEITERATOR_H_INCLUDED

#include <libxml/xmlreader.h>

namespace libkeynote
{

struct KNXMLAttribute
{
  const xmlChar *name;
  const xmlChar *ns;
  const xmlChar *value;

  KNXMLAttribute();
};

class KNXMLAttributeIterator
{
public:
  explicit KNXMLAttributeIterator(xmlTextReaderPtr reader);

  bool next();
  bool last() const;

  const KNXMLAttribute &operator*() const;
  const KNXMLAttribute *operator->() const;

private:
  bool move();
  void read();
  bool test();

private:
  xmlTextReaderPtr m_reader;
  KNXMLAttribute m_current;
  bool m_first;
  bool m_last;
};

}

#endif //  KNXMLATTRIBUTEITERATOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
